#!/bin/bash

BASE=$(realpath "$(dirname "$0")")

doc_id=1

urls="https://de.wikipedia.org/wiki/Memmingen https://de.wikipedia.org/wiki/Kempten_(Allg%C3%A4u) https://de.wikipedia.org/wiki/ECDC_Memmingen https://de.wikipedia.org/wiki/Liste_bedeutender_Schiffsversenkungen"
for url in $urls; do
  echo "Get tokens from '$url'"
  wget -O - "$url" |
    "$BASE"/../bin/htmlstriptags |
    "$BASE"/../bin/htmlentitiesdecode |
    "$BASE"/../bin/tokenizer -d $' .,;:()[]{}\\/-=<>"+?!%_|$&@\r\n\t\v\f' |
    "$BASE"/../bin/indexer "$doc_id"

  doc_id=$(expr $doc_id + 1)
done
