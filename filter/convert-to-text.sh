#!/bin/bash

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
if [ "$temp_dir" != "" ]; then
  if [ -f "$temp_dir" ]; then
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
fi

mimeType=$(file -b --mime-type "$file")

case "$mimeType" in
*/pdf)
  pdftotext -enc UTF-8 "$file" -
  ;;

*/msword)
  catdoc -d utf-8 "$file"
  ;;

*document.text)
  pandoc -t plain "$file"
  ;;

*/html)
  html2text -utf8 -nometa "$file"
  ;;

text/plain)
  cat "$file"
  ;;

text/csv)
  cat "$file"
  ;;

message/rfc822)
  cmd="$(dirname "$0")"/cat-mime-message.sh
  "$cmd" "$file" "$temp_dir"
  ;;

*)
  echo "Cannot convert $mimeType" >&2
  ;;
esac
