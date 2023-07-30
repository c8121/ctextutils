# ctextutils

Text related utils.

- **tokenizer**  
  Split text to words.
- **soundex**  
  Soundex implemetation.
- **htmlstriptags**  
  Very simple utility to extract plain text from html.
- **htmlentitiesdecode**  
  Replace html entities (&...)
- **mailparser**  
  Extract parts from mime-messages.
- **mailmetadata**  
  Read mime-message file and create JSON Object containing metadata (From, To, Subject...)
- **indexer**  
  Tokenize text from stdin (split at whitespace), write words to fulltext-index database.  
  Word will be related to given document id.
- **finder**  
  Find documents in fulltext-index database containing given words.
 


## Install dependencies

    apt install gcc
    apt install libmysqlclient-dev
    apt install libmailutils-dev

    apt install html2text
    apt install catdoc
    apt install pandoc

## Download & build

    git clone https://github.com/c8121/ctextutils.git
    cd ctextutils
    ./pull-dependencies.sh
    ./build.sh
