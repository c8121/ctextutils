#!/bin/bash

BASE=$(realpath "$(dirname "$0")")

source="$1"
if [ "$source" == "" ]; then
  echo "Usage: $0 <dirname or filename>"
  exit
fi

doc_id=1

### Functions ###

function index_document() {
  file="$1"
  if [ ! -f "$file" ]; then
    echo "File not found: $file"
    return
  fi
  file=$(realpath "$file")

  echo "Add $file (id=$doc_id)"
  "$BASE"/../filter/convert-to-text.sh "$file" |
    "$BASE"/../bin/tokenizer -d $' .,;:()[]{}\\/-=<>"+*?!%_|$&@\r\n\t\v\f' |
    "$BASE"/../bin/indexer "$doc_id"

  doc_id=$(expr $doc_id + 1)
}

### Main ###

if [ -d "$source" ]; then
  find "$source" -type f -name "*.htm*" -print0 | while IFS= read -r -d '' file; do
    index_document "$file"
  done

elif [ -f "$source" ]; then
  index_document "$source"
else
  echo "Not found: $source"
fi
