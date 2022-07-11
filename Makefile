all: mk printersim printsched printersim.dSYM

mk:
	mkdir exe obj

printersim: src/printersim.c
	gcc src/printersim.c inc/gfx.c -o exe/printersim -Wall -g -pthread -lX11 -lm

dll.o: inc/dll.c
	gcc -o obj/dll.o -c inc/dll.c -Wall -Wextra -Werror

printsched: dll.o src/printsched.c 
	gcc -pthread -Wall -Wextra -Werror obj/dll.o src/printsched.c -o exe/printsched

printersim.dSYM:
	rm -rf exe/printersim.dSYM

clean:
	rm -rf exe
	rm -rf obj
