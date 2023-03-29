#!/bin/bash

BASE=$(realpath "$(dirname "$0")")

sourceDir=$BASE/src
binDir=$(realpath "$BASE/../bin/test")
dependenciesDir=$BASE/../dep

if [[ ! -d "$sourceDir" ]]; then
  echo "Source directory not found: $sourceDir"
  exit
fi

if [[ ! -d "$dependenciesDir" ]]; then
  echo "Dependencies directory not found, please pull dependencies first: ../pull-dependencies.sh"
  exit
fi

if [[ ! -d "$binDir" ]]; then
  echo "Create $binDir"
  mkdir -p "$binDir"
fi

cd "$BASE" || exit
for file in "$sourceDir"/*.c; do
  echo "Build $file"
  gcc -Wall -I"$dependenciesDir" -o "$binDir/$(basename "$file")" "$file"
done