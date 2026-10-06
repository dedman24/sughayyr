#!/bin/bash

# this script builds everything from scratch.
if [ $# -lt 1 ]; then
  echo "ERROR: too few arguments." >&2
elif [[ "$1" == "help" || "$1" == "--help" || "$1" == "-h" ]]; then
  echo "build script for sughayyr. usage is:"
  echo "$0 'path to final executable'"
else
  if [[ $# == 2 && "$2" == "debug" ]]; then
    gcc -g src/sughayyr.c src/renderer/renderer.c src/linking/vfastarr/vfastarr.c -o $1 -fsanitize=address -lglfw -lGL
  else
    gcc src/sughayyr.c src/renderer/renderer.c src/linking/vfastarr/vfastarr.c -o $1 -O3 -lglfw -lGL
  fi
fi
