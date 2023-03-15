#!/bin/bash

BASE=$(realpath "$(dirname "$0")")

sourceDir=$BASE/src
binDir=$(realpath "$BASE/../bin/test")

if [[ ! -d "$sourceDir" ]]; then
  echo "Source directory not found: $sourceDir"
  exit
fi

if [[ ! -d "$binDir" ]]; then
  echo "Create $binDir"
  mkdir -p "$binDir"
fi

cd "$BASE"
for file in "$sourceDir"/*.c; do
  echo "Build $file"
  gcc -Wall -o "$binDir/$(basename "$file")" "$file"
done

#gcc -Wall -o "$binDir/tokenizer" "$sourceDir/tokenizer.c"
