Dune II data toolset
====================

This project aims to extract data from the __Dune II__ game and convert it into
a format usable by the project [Dune2 Online](https://github.com/NealRame/dune2-online).


Configure options
-----------------

- `CMAKE_INSTALL_PREFIX` - `*.json.gz` files will be installed at `${CMAKE_INSTALL_PREFIX}/public/assets`
- `DUNE2_DATA_ARCHIVE` - the path to orginal Dune 2 game archive

Generate data
-------------

```shell
> cd dune2-data-toolset
> mkdir Build
> cd Build
> cmake \
    -G"Unix Makefiles" \
    -DCMAKE_INSTALL_PREFIX="/path/where/to/install/binary" \
    -DDUNE2_DATA_ARCHIVE="/path/to/dune2/archive.zip" \
    ..
> make
> make install
```
