main: bigden.o biglag.o calfun.o main.o newuoa.o newuob.o trsapp.o update.o
	gcc -Df2cFortran -O -o newuoa-C bigden.o biglag.o calfun.o main.o trsapp.o update.o newuoa.o newuob.o -lg2c -lm
bigden.o: bigden.f
	gcc -O  -c bigden.f
biglag.o: biglag.f
	gcc -O  -c biglag.f
calfun.o: calfun.f
	gcc -O  -c calfun.f
main.o: main.c
	gcc -O  -Df2cFortran -c main.c
trsapp.o: trsapp.f
	gcc -O  -c trsapp.f
newuoa.o: newuoa.f
	gcc -O  -c newuoa.f
newuob.o: newuob.f
	gcc -O  -c newuob.f
update.o: update.f
	gcc -O  -c update.f
clean:
	rm -f *.o

