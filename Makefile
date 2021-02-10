exec = scss.out
sources = $(wildcard src/*.c) $(wildcard src/hooks/*.c)
objects = $(sources:.c=.o)
flags = -I$$HOME/.local/include -lm -ldl -fPIC -rdynamic -L$$HOME/.local/lib

objects_no_main = $(filter-out src/main.o, $(objects))

cssfiles = $(wildcard src/css/*.css)
cssheaders = $(cssfiles:.css=.css.h)


ifdef DEBUG
flags += -D DEBUG -pg -Wall -g
endif


$(exec): $(cssheaders) $(objects)
	gcc $(objects) $(flags) -g -o $(exec)

libscss.a: $(objects_no_main)
	ar rcs $@ $^

%.o: %.c include/%.h
	gcc -c $(flags) $< -o $@

%.css.h: %.css
	mkdir -p .tmp
	mkdir -p ./src/include/css
	cat ./src/css/$(notdir $^) | tr -d \\n > .tmp/$(notdir $^) ; xxd -i .tmp/$(notdir $^) | sed 's/\([0-9a-f]\)$$/\0, 0x00/' > ./src/include/css/$(notdir $^.h)

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
