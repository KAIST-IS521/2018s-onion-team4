CCFLAGS=-std=c++11 -Wall
pgp:
	g++ $(CCFLAGS) -c pgp_test.cc -o pgp_test.o
	g++ $(CCFLAGS) -c ../src/Utils/PGP.cc -o PGP.o
	g++ $(CCFLAGS) -g pgp_test.o PGP.o `gpgme-config --cflags --libs` -o pgp_test

clean:
	rm *.o pgp_test

