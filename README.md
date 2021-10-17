Basic TErminal
================

BTE provides a basic terminal widget for CTK applications.

Installation
------------

```
$ git clone https://github.com/cafe-desktop/bte # Get the source code of BTE
$ cd bte                                        # Change to the toplevel directory
$ meson _build                                  # Run the configure script
$ ninja -C _build                               # Build BTE
[ Optional ]
$ ninja -C _build install                       # Install BTE to default `/usr/local`
```

* By default, BTE will install under `/usr/local`. You can customize the
prefix directory by `--prefix` option, e.g. If you want to install BTE under
`~/foobar`, you should run `meson _build --prefix=~/foobar`. If you already
run the configure script before, you should also pass `--reconfigure` option to it.

* You may need to execute `ninja -C _build install` as root
(i.e. `sudo ninja -C _build install`) if installing to system directories.

* If you wish to test BTE before installing it, you may execute it directly from
its build directory. As `_build` directory, it should be `_build/src/app/bte-[version]`.

* You can pass `-Ddebugg=true` option to meson if you wish to enable debug function.


Debugging
---------

After installing BTE with `-Ddebugg=true` flag, you can use `BTE_DEBUG` variable to control
BTE to print out the debug information

```
# You should change bte-[2.91] to the version you build
$ BTE_DEBUG=selection ./_build/src/app/bte-2.91

# Or, you can mixup with multiple logging level
$ BTE_DEBUG=selection,draw,cell ./_build/src/app/bte-2.91

$ Or, you can use `all` to print out all logging message
$ BTE_DEBUG=all ./_build/src/app/bte-2.91
```

For logging level information, please refer to enum [BteDebugFlags](src/debug.h).


Contributing
------------

Bugs should be filed here: https://github.com/cafe-desktop/bte/issues
Please note that this is *not a support forum*; if you are a end user,
always file bugs in your distribution's bug tracker, or use their
support forums.

If you want to provide a patch, please attach them to an issue in
github, in the format output by the git format-patch command.
