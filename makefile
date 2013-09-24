gccver:=$(shell gcc -dumpmachine)
ifeq (mingw, $(findstring mingw,$(gccver)))
	POSTFIX:=.exe
	CFLAGS:=-s -static
else
	POSTFIX:=
	CFLAGS:=-s
endif

CFLAGS2:=-Wall -O3

readme.txt: matchgen$(POSTFIX)
	./matchgen$(POSTFIX) --help > readme.txt

matchgen$(POSTFIX): matchgen.cpp SimpleOpt.h
	g++ -o matchgen$(POSTFIX) matchgen.cpp $(CFLAGS) $(CFLAGS2)
