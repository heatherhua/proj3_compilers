#! /bin/sh

./glc < samples/$1.glsl > test

diff -w test samples/$1.out
