#!/bin/bash

BASE=$(realpath "$(dirname "$0")")
PARSER_COMMAND="$BASE/../bin/mailparser"
CONVERT_COMMAND="$BASE/convert-to-text.sh"

file="$1"

if [ "$file" == "" ]; then
  echo "Usage: $0 <filename>" >&2
  exit
fi

if [ ! -f "$file" ]; then
  echo "File not found" >&2
  exit
fi

temp_dir=$(mktemp -d)

$PARSER_COMMAND "$temp_dir" <"$file" >&2
find "$temp_dir" -type f -print0 | while IFS= read -r -d '' content_file; do

  #ls -l "$content_file"
  $CONVERT_COMMAND "$content_file"

done

rm -r "$temp_dir"
