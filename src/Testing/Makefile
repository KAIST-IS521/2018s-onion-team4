pgp:
	g++ -c pgp_test.cc -o pgp_test.o
	g++ -c ../PGP.cc -o PGP.o
	g++ pgp_test.o PGP.o `gpgme-config --cflags --libs` -o pgp_test

clean:
	rm *.o pgp_test

