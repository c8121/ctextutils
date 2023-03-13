#!/bin/bash

BASE=$(realpath "$(dirname "$0")")

urls="https://de.wikipedia.org/wiki/Memmingen https://de.wikipedia.org/wiki/Kempten_(Allg%C3%A4u) https://de.wikipedia.org/wiki/ECDC_Memmingen"
for url in $urls; do
  echo "Get tokens from '$url'"
  wget -O - "$url" |
    "$BASE"/../bin/htmlstriptags |
    "$BASE"/../bin/tokenizer -d $' .,;:()[]{}\\/-=<>"+?!%_|$&@\r\n\t\v\f'
done
