comp: lex.yy.c binary.tab.o main.cc Environment.hh Value.hh Value.cpp labTrees.hh labTrees.cpp
	g++ -std=c++11 -g -ocomp binary.tab.o lex.yy.c main.cc Value.cpp labTrees.cpp
binary.tab.o: binary.tab.cc Environment.hh Value.hh
	g++ -std=c++11 -g -c binary.tab.cc
binary.tab.cc: binary.yy
	bison -v --report-file=report.txt binary.yy
lex.yy.c: binary.ll binary.tab.cc
	flex binary.ll
target: target.cc
	g++ -std=c++14 -g -otarget target.cc
clean: 
	rm -f binary.tab.* lex.yy.c* comp report.txt *.svg cfg.dot target*
