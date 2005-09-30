# Powell open source

LIBNAME=libpowell.a
EXENAME=newuoa-C
OBJS=biglag.o calfun.o newuoa.o newuob.o trsapp.o update.o bigden.o

main: $(OBJS) main.o
	gcc -Df2cFortran -O -o $(newuoa-C) $(OBJS) main.o -lg2c -lm

liblinux: $(OBJS)
	libtool --mode=link $(OBJS) -o libpowell.a

libmac: $(OBJS)
	libtool -static -o $(LIBNAME) *.o
	ranlib $(LIBNAME)

clean:
	rm -f *.o
	rm -f $(LIBNAME)
	rm -f $(EXENAME)

bigden.o: bigden.f
	g77 -O  -c bigden.f
biglag.o: biglag.f
	g77 -O  -c biglag.f
calfun.o: calfun.f
	g77 -O  -c calfun.f
main.o: main.c
	gcc -O  -Df2cFortran -c main.c
trsapp.o: trsapp.f
	g77 -O  -c trsapp.f
newuoa.o: newuoa.f
	g77 -O  -c newuoa.f
newuob.o: newuob.f
	g77 -O  -c newuob.f
update.o: update.f
	g77 -O  -c update.f

