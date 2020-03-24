#!/bin/bash
echo "mp3: Cleaning auto-deps and compiled imgs ..."
make -s clean
echo "mp3: Generating dependencies ..."
make -s dep
echo "mp3: Compiling kernel ..."
make -s