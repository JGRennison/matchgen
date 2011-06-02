gccver:=$(shell gcc -dumpmachine)
ifeq (mingw, $(findstring mingw,$(gccver)))
	POSTFIX:=.exe
	CFLAGS:=-s -static
else
	POSTFIX:=
	CFLAGS:=-s
endif

readme.txt: matchgen$(POSTFIX)
	matchgen$(POSTFIX) --help > readme.txt

matchgen$(POSTFIX): matchgen.cpp SimpleOpt.h
	g++ -O3 -o matchgen$(POSTFIX) matchgen.cpp $(CFLAGS)
