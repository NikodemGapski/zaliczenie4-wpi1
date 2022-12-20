
main:
	g++ @opcjeCpp main.cpp kol.cpp -o main.e

_main:
	g++ @opcjeCpp _main.cpp _kol.cpp -o _main.e

clear:
	rm -f *.e