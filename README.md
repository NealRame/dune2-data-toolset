PAK file extractor
==================

Build
-----

```shell
> cd pak-extractor
> mkdir Build
> cd Build
> cmake -DCMAKE_INSTALL_PREFIX="/path/where/to/install/binary" ..
> make
> make install
```

Usage
-----

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
