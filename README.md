# ReadMap

## Converts Minecraft's map files to images

The program will take in the uncompressed NBT data and convert it to a PPM format.

You can install ImageMagick to convert the PNG stream to a PNG.

Rrequirements:
 - g++
 - gzip
 - ImageMagick (optional)

Example:

```bash
g++ readmap.cpp -o readmap
gzip -dc map_73.dat | ./readmap | pnmtopng  > test.png
```
Licenced under the MIT Licence or GPL2.0
