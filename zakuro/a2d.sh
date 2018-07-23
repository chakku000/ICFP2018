#!/bin/bash

clang++ -std=c++1z a2d.cpp -o a2d
mkdir -p out

for f in `ls -1 $1` 
do
  echo $f
  ./a2d $1/$f out/${f/FA/FD}
done

rm -f a2d