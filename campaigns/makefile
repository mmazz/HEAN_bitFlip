all: clean bitFlip


small: utils small.cpp
	g++ small.cpp -o small utils.o -L/usr/local/lib ../lib/libHEAAN.a -lntl -lgmp -lm -std=c++11 -O2 -I/../src -pthread


bitFlip: bitFlip_encode bitFlip_encrypt

bitFlip_encode: utils bitFlip_encode.cpp
	g++ bitFlip_encode.cpp -o bitFlip_encode  utils.o -L/usr/local/lib ../lib/libHEAAN.a -lntl -lgmp -lm -std=c++11 -O2 -I/../src -pthread

bitFlip_encrypt: utils bitFlip_encrypt.cpp
	g++ bitFlip_encrypt.cpp -o bitFlip_encrypt -L/usr/local/lib ../lib/libHEAAN.a -lntl -lgmp -lm -std=c++11 -O2 -I/../src -pthread

test: utils test.cpp
	g++ test.cpp -o test -L/usr/local/lib ../lib/libHEAAN.a -lntl -lgmp -lm -std=c++11 -O2 -I/../src -pthread

utils:
	g++ utils.cpp -c utils.o

clean:
	rm -rf bitFlip small

run:
	./bitFlip_encode
