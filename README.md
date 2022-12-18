# ReadMap

## Converts Minecraft's map files to images

The program will take in the uncompressed NBT data and convert it to a PNG format.

```
$ readmap -h
Usage:
readmap (map.dat)
readmap (map.dat) [output.png]
readmap -h
Run with no inputs for a prompt.
```
To compile

For Linux:
```bash
$ g++  readmap.cpp --static -lm -lz -lpng -o readmap
```

For Windows on Linux:
```bash
$ ./make_release.sh
```

Licenced under the MIT Licence or GPL2.0
