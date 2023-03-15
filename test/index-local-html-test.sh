#!/bin/bash

BASE=$(realpath "$(dirname "$0")")

source="$1"
if [ "$source" == "" ]; then
  echo "Usage: $0 <dirname>"
  exit
fi

doc_id=1

find "$source" -type f -name "*.htm*" -print0 | while IFS= read -r -d '' file; do

  echo "Add $file"

  cat "$file" |
    "$BASE"/../bin/htmlstriptags |
    "$BASE"/../bin/htmlentitiesdecode |
    "$BASE"/../bin/tokenizer -d $' .,;:()[]{}\\/-=<>"+?!%_|$&@\r\n\t\v\f' |
    "$BASE"/../bin/indexer "$doc_id"

  doc_id=$(expr $doc_id + 1)
done
