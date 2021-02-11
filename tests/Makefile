exec = test.out
sources = $(wildcard src/*.c)
objects = $(sources:.c=.o)
flags = -I$$HOME/.local/include -I../src/include -g -Wall -lm -Wl,--no-as-needed -ldl -fPIC -rdynamic -L$$HOME/.local/lib -lscss

$(exec): $(objects) libscss.a
	gcc $(objects) $(flags) -o $(exec)

libscss.a:
	cd .. && make clean; make install && mv libscss.a tests/.
	ar rcs $@ $^

%.o: %.c include/%.h
	gcc -c $(flags) $< -o $@

clean:
	-rm *.out
	-rm *.o
	-rm *.a
	-rm src/*.o

lint:
	clang-tidy src/*.c src/include/*.h

