main: main.o getSource.o compile.o
	gcc -o main main.o getSource.o compile.o

main.o: main.c
	gcc -c main.c

getSource.o: getSource.c
	gcc -c getSource.c

compile.o: compile.c
	gcc -c compile.c