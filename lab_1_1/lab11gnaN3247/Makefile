W_COMPILE_FLAGS = -Wall -Wextra -Werror
OPTIMIZATION_FLAGS = -O3

all: lab11gnaN3247

lab11gnaN3247: file_process.o file_traversal.o lab11gnaN3247.o
	gcc $(W_COMPILE_FLAGS) $(OPTIMIZATION_FLAGS) -o lab11gnaN3247 file_process.o file_traversal.o lab11gnaN3247.o

file_process.o: file_process.c
	gcc $(W_COMPILE_FLAGS) $(OPTIMIZATION_FLAGS) -c file_process.c

file_traversal.o: file_traversal.c file_process.h
	gcc $(W_COMPILE_FLAGS) $(OPTIMIZATION_FLAGS) -c file_traversal.c

lab11gnaN3247.o: lab11gnaN3247.c file_traversal.h
	gcc $(W_COMPILE_FLAGS) $(OPTIMIZATION_FLAGS) -c lab11gnaN3247.c

clean:
	rm -rf *.o lab11gnaN3247
