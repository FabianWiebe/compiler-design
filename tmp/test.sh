tar xzf ass1-int.tgz
cd ass1-int
make
./int testcase.lua
# Output from interpreter to stdout, dot-format parse tree into the text file.
dot parse.txt -Oparse.svg -Tsvg 

