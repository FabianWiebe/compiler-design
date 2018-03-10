comp: lex.yy.c binary.tab.o main.cc Node.hh Environment.hh Value.hh Value.cpp
	g++ -std=c++11 -g -ocomp binary.tab.o lex.yy.c main.cc Value.cpp
binary.tab.o: binary.tab.cc Node.hh Environment.hh Value.hh
	g++ -std=c++11 -g -c binary.tab.cc
binary.tab.cc: binary.yy
	bison -v --report-file=report.txt binary.yy
lex.yy.c: binary.ll binary.tab.cc
	flex binary.ll
clean: 
	rm -f binary.tab.* lex.yy.c* comp report.txt *.svg cfg.dot
