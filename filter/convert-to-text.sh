#!/bin/bash

file="$1"

if [ "$file" == "" ]; then
  echo "Usage: $0 <filename>" >&2
  exit
fi

if [ ! -f "$file" ]; then
  echo "File not found" >&2
  exit
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

*)
  echo "Cannot convert $mimeType" >&2
  ;;
esac
