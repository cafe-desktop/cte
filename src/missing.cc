/*
 * Copyright © 2020 Christian Persch
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "config.h"

#ifdef HAVE_SYS_RESOURCE_H
#include <sys/resource.h>
#endif

#include <glib-unix.h>
#include <gio/gio.h>

#ifdef __linux__
#include <sys/syscall.h>  /* for syscall and SYS_getdents64 */
#endif

#include "missing.hh"

/* BEGIN copied from glib
 *
 * Code for fdwalk copied from glib/glib/gspawn.c, there under LGPL2.1+,
 * and used here under LGPL3+.
 *
 * Copyright 2000 Red Hat, Inc.
 */

#ifndef HAVE_FDWALK

#ifdef __linux__

struct linux_dirent64
{
  guint64        d_ino;    /* 64-bit inode number */
  guint64        d_off;    /* 64-bit offset to next structure */
  unsigned short d_reclen; /* Size of this dirent */
  char           d_name[]; /* Filename (null-terminated) */
};

/* This function is called between fork and execve/_exit and so must be
 * async-signal-safe; see man:signal-safety(7).
 */
static int
filename_to_fd (const char *p)
{
  char c;
  int fd = 0;
  const int cutoff = G_MAXINT / 10;
  const int cutlim = G_MAXINT % 10;

  if (*p == '\0')
    return -1;

  while ((c = *p++) != '\0')
    {
      if (c < '0' || c > '9')
        return -1;
      c -= '0';

      /* Check for overflow. */
      if (fd > cutoff || (fd == cutoff && c > cutlim))
        return -1;

      fd = fd * 10 + c;
    }

  return fd;
}

#endif /* __linux__ */

/* This function is called between fork and execve/_exit and so must be
 * async-signal-safe; see man:signal-safety(7).
 */
static rlim_t
getrlimit_NOFILE_max(void)
{
#ifdef HAVE_SYS_RESOURCE_H
#ifdef __linux__
{
        struct rlimit rlim;

        if (prlimit(0 /* this PID */, RLIMIT_NOFILE, nullptr, &rlim) == 0)
                return rlim.rlim_max;

        return RLIM_INFINITY;
}
#endif /* __linux__ */

#ifdef __GLIBC__
{
        struct rlimit rlim;

        /* Use getrlimit() function provided by the system if it is known to be
         * async-signal safe.
         *
         * According to the glibc manual, getrlimit is AS-safe.
         */
        if (getrlimit(RLIMIT_NOFILE, &rlim) == 0)
                return rlim.rlim_max;
}

        /* fallback */
#endif /* __GLIBC__ */

#endif /* HAVE_SYS_RESOURCE_H */

#if defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__APPLE__)
        /* Use sysconf() function provided by the system if it is known to be
         * async-signal safe.
         */
        auto const r = sysconf(_SC_OPEN_MAX);
        if (r != -1)
                return r;

        /* fallback */
#endif

        /* couldn't determine, so potentially infinite */
        return RLIM_INFINITY;
}

/* This function is called between fork and execve/_exit and so must be
 * async-signal-safe; see man:signal-safety(7).
 */
int
fdwalk(int (*cb)(void *data, int fd),
       void *data)
{
  /* Fallback implementation of fdwalk. It should be async-signal safe, but it
   * may be slow on non-Linux operating systems, especially on systems allowing
   * very high number of open file descriptors.
   */
  int fd;
  int res = 0;

#ifdef __linux__
  /* Avoid use of opendir/closedir since these are not async-signal-safe. */
  int dir_fd = open ("/proc/self/fd", O_RDONLY | O_DIRECTORY | O_CLOEXEC);
  if (dir_fd >= 0)
    {
      char buf[4096];
      int pos, nread;
      struct linux_dirent64 *de;

      while ((nread = syscall (SYS_getdents64, dir_fd, buf, sizeof(buf))) > 0)
        {
          for (pos = 0; pos < nread; pos += de->d_reclen)
            {
              de = (struct linux_dirent64 *)(buf + pos);

              fd = filename_to_fd (de->d_name);
              if (fd < 0 || fd == dir_fd)
                  continue;

              if ((res = cb (data, fd)) != 0)
                  break;
            }
        }

      close (dir_fd);
      return res;
    }

  /* If /proc is not mounted or not accessible we fall back to the old
   * rlimit trick */

#endif

  auto const open_max = getrlimit_NOFILE_max();
  if (open_max == RLIM_INFINITY || open_max > G_MAXINT) {
    /* We cannot close infinitely many FDs, but we also must not
     * leak any FDs. Return an error.
     */
    errno = ENFILE;
    return -1;
  }

  for (fd = 0; fd < int(open_max); fd++)
      if ((res = cb (data, fd)) != 0)
          break;

  return res;
}
#endif /* !HAVE_FDWALK */

#ifndef HAVE_STRCHRNUL
/* Copied from glib */
char*
strchrnul(char const* s,
          int c)
{
        char *p = (char *) s;
        while (*p && (*p != c))
                ++p;

        return p;
}
#endif /* !HAVE_STRCHRNUL */

/* END copied from glib */
