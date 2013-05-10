GPP=g++
GCC=gcc

OUTFILE="Pawnmail.so"

COMPILE_FLAGS=-c -std=c++11 -O2 -w -DLINUX -I./SDK/amx/

all:
	$(GCC) $(COMPILE_FLAGS) ./SDK/amx/*.c
	$(GPP) $(COMPILE_FLAGS) ./jwsmtp/*.cpp
	$(GPP) $(COMPILE_FLAGS) ./SDK/*.cpp
	$(GPP) $(COMPILE_FLAGS) *.cpp
	$(GPP) -O2 -fshort-wchar -shared -o $(OUTFILE) *.o -static -lboost_regex
	rm -f *.o
