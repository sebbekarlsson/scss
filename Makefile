exec = scss.out
sources = $(wildcard src/*.c) $(wildcard src/hooks/*.c)
objects = $(sources:.c=.o)
flags = -I$$HOME/.local/include -lm -ldl -fPIC -rdynamic -L$$HOME/.local/lib

objects_no_main = $(filter-out src/main.o, $(objects))

jsfiles = $(wildcard src/js/*.js)
jsheaders = $(jsfiles:.js=.js.h)

ifdef DEBUG
flags += -D DEBUG -pg -Wall -g
endif


$(exec): $(jsheaders) $(objects)
	gcc $(objects) $(flags) -g -o $(exec)

libscss.a: $(objects_no_main)
	ar rcs $@ $^

%.o: %.c include/%.h
	gcc -c $(flags) $< -o $@


install:
	make
	make libscss.a
	mkdir -p $(HOME)/.local/include/scss
	cp -r ./src/include/* $(HOME)/.local/include/scss/.
	cp ./libscss.a $(HOME)/.local/lib/.
	cp ./scss.out $(HOME)/.local/bin/scss

clean:
	-rm *.out
	-rm *.o
	-rm *.a
	-rm src/*.o
	-rm -rf .tmp

lint:
	clang-tidy src/*.c src/include/*.h
