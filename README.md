Dune II data toolset
====================

Provides command line tools to manipulate data from game __Dune II__.

Build
-----

```shell
> cd dune2-data-toolset
> mkdir Build
> cd Build
> cmake -DCMAKE_INSTALL_PREFIX="/path/where/to/install/binary" ..
> make
> make install
```

PAK extractor
-------------

Extract data from a _.PAK_ file.

### Synopsis
```
Usage: pak-extractor [OPTIONS] input-pak-file
```

### Options

* `-h`, `--help`

  Display help message and exit.

* `-l`, `--list`

  List file in the PAK.

* `-v`, `--verbose`

  Enable verbosity.

* `-d arg`, `--output-dir=arg`

  Set output directory.
