#!/bin/bash
cat <<EOF >> /etc/pacman.conf

[mingw64]
Server = https://repo.msys2.org/mingw/mingw64
SigLevel = Never
EOF
pacman -Syyu --noconfirm mingw-w64-x86_64-libpng mingw-w64-gcc
x86_64-w64-mingw32-g++ -I/mingw64/include -L/mingw64/lib readmap.cpp -static-libgcc -static-libstdc++ -static -lpng -lz -lssp -o readmap
