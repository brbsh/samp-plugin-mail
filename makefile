GPP = g++
GCC = gcc

OUTFILE = "mail.so"

COMPILE_FLAGS = -c -m32 -O3 -fPIC -w -DLINUX -Wall -I./SDK/amx/

all:
	$(GCC) $(COMPILE_FLAGS) ./SDK/amx/*.c
	$(GPP) $(COMPILE_FLAGS) ./jwsmtp/*.cpp
	$(GPP) $(COMPILE_FLAGS) ./SDK/*.cpp
	$(GPP) $(COMPILE_FLAGS) *.cpp
	$(GPP) -O2 -fshort-wchar -shared -o $(OUTFILE) *.o -lpthread
	rm -f *.o
