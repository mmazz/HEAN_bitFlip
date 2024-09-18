# This sample should be run after make libHEAAN.a file in lib folder
# All Target
all: clean HEAAN run


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
	g++ utils.h -c utils.o

# Tool invocations
HEAAN:
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++ run.cpp -o HEAAN -L/usr/local/lib ../lib/libHEAAN.a -lntl -lgmp -lm -std=c++11 -O2 -I/../src -pthread
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	rm -rf HEAAN bitFlip small

run:
	./HEAAN
