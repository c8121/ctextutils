#!/bin/bash

BASE=$(realpath "$(dirname "$0")")
PARSER_COMMAND="$BASE/../bin/mailparser"
CONVERT_COMMAND="$BASE/convert-to-text.sh"

function usage_message() {
  echo "Usage: $0 <filename> [temp dir]" >&2
}

file="$1"

if [ "$file" == "" ]; then
  usage_message
  exit
fi

if [ ! -f "$file" ]; then
  echo "File not found" >&2
  usage_message
  exit
fi

temp_dir="$2"
if [ "$temp_dir" == "" ]; then
  temp_dir=$(mktemp -d)
elif [ -f "$temp_dir" ]; then
  echo "Temp dir is a file" >&2
  usage_message
  exit
elif [ ! -d "$temp_dir" ]; then
  echo "Temp dir does not exist" >&2
  usage_message
  exit
elif [ "$(ls -A "$temp_dir")" ]; then
  echo "Temp dir must be empty" >&2
  usage_message
  exit
fi


$PARSER_COMMAND "$temp_dir" -ignore image/ <"$file" >&2
find "$temp_dir" -type f -print0 | while IFS= read -r -d '' content_file; do

  #ls -l "$content_file"
  $CONVERT_COMMAND "$content_file"

done

rm -r "$temp_dir"
