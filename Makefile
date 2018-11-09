main: main.o getSource.o compile.o table.o codegen.o
	gcc -o main main.o getSource.o compile.o table.o codegen.o 

main.o: main.c
	gcc -c main.c

getSource.o: getSource.c
	gcc -c getSource.c

compile.o: compile.c
	gcc -c compile.c

table.o: table.c
	gcc -c table.c

codegen.o: codegen.c
	gcc -c codegen.c


gdb:
	gcc -g -o main main.c getSource.c compile.c table.c codegen.c

clean:
	rm -f main.o getSource.o compile.o table.o codegen.o
	