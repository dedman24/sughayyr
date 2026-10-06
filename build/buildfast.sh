#!/bin/bash

# this script builds the program quickly by splitting it into its various parts.
if [[ "$1" == "all" ]]; then
  if [[ $# != 2 ]]; then
    echo "amount of arguments for option $1 is incorrect."
  fi

  ./buildall.sh $2
elif [[ "$1" == "some" ]]; then                     # compiles only minor libs.
  if [[ $# != 1 ]]; then
    echo "amount of arguments for option $1 is incorrect."
  fi

  echo "this feature is currently not implemented."
elif [[ "$1" == "main" ]]; then
  if [[ $# != 2 ]]; then
    echo "amount of arguments for option $1 is incorrect."
  fi

  echo "this feature is currently not implemented."
else
  echo "unknown option $1"
fi
