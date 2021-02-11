# scss
> CLI and library for transpiling `scss` to `css`.

## Install
> First clone down the repo, then run:
```bash
make
make install
```

## Usage
### CLI
> To use the CLI:
```bash
scss <filename>
```

### Library
> To use the library:
```C
#include <scss/scss.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
  // initialize
  init_scss();

  // something to transpile
  char contents[] = ".mydiv { color: red; .somechild { color: green; } }";

  // transpile it
  char* out = scss(contents, 0);

  // print it?
  printf("%s\n", out);

  return 0;
}
```
> Also, remember to compile it with `-lscss`.
> For example, one could compile it like this:
```bash
gcc ./main.c -I$HOME/.local/include -L$HOME/.local/lib -lscss
```
> After you run this program, you should see this:
```css
.mydiv {color:red;}
.mydiv .somechild {color:green;}

```
