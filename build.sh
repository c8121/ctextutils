#!/bin/bash

BASE=$(realpath "$(dirname "$0")")

sourceDir=$BASE/src
binDir=$BASE/bin
dependenciesDir=$BASE/dep

if [[ ! -d "$sourceDir" ]]; then
  echo "Source directory not found: $sourceDir"
  exit
fi

if [[ ! -d "$dependenciesDir" ]]; then
  echo "Dependencies directory not found, please pull dependencies first: ./pull-dependencies.sh"
  exit
fi

if [[ ! -d "$binDir" ]]; then
  echo "Create $binDir"
  mkdir -p "$binDir"
fi

INCLUDE_PATH=$BASE/dep

cd "$BASE" || exit
gcc -Wall -I"$INCLUDE_PATH" -o "$binDir/tokenizer" "$sourceDir/tokenizer.c"
gcc -Wall -I"$INCLUDE_PATH" -o "$binDir/htmlstriptags" "$sourceDir/htmlstriptags.c"
gcc -Wall -I"$INCLUDE_PATH" -o "$binDir/htmlentitiesdecode" "$sourceDir/htmlentitiesdecode.c"
gcc -Wall -I"$INCLUDE_PATH" -o "$binDir/mailparser" "$sourceDir/mailparser.c"
gcc -Wall -I"$INCLUDE_PATH" -o "$binDir/mailmetadata" "$sourceDir/mailmetadata.c" -lmailutils
gcc -Wall -I"$INCLUDE_PATH" -o "$binDir/indexer" "$sourceDir/indexer.c" -lmysqlclient -lm
gcc -Wall -I"$INCLUDE_PATH" -o "$binDir/finder" "$sourceDir/finder.c" -lmysqlclient -lm
