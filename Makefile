#this is the makefile

PROGRAMS = myshell matmult_p matmult_t matformatter multiply
LIBRARIES =
CCFLAGS = -g

all: $(LIBRARIES) $(PROGRAMS)
	@echo "Done!"

myshell: myshell.c
	gcc $(CCFLAGS) $< -o $@

matformatter: matformatter.c helper.c
	gcc -o $@ $^

matmult_p: multiply
	gcc -o $@ matmult_p.c helper.c

matmult_t: multiply
	gcc -o $@ matmult_t.c helper.c

multiply: multiply.c
	gcc -o $@ $^

clean:
	rm -f *.o $(PROGRAMS)
