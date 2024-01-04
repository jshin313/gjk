CC=clang

CC_WEB=emcc
FLAGS_WEB=-s USE_SDL=2 -s USE_SDL_GFX=2 --bind -s WASM=1 -O3
WEBGENDIR=public/gen

CFLAGS=-I$(IDIR) -Wall -Wextra
GJKIDIR=src/gjk
IDIR=src
SDIR=src
ODIR=obj
BINDIR=bin

LIBS=-lSDL2 -lSDL2_gfx

_GJKDEPS = vector.h gjk.h
GJKDEPS = $(patsubst %,$(GJKIDIR)/%,$(_GJKDEPS))

_DEPS = utils.h loop.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o utils.o loop.o vector.o gjk.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(GJKIDIR)/%.c $(GJKDEPS)
	@mkdir -p $(@D)
	$(CC) -c -o $@ $< $(CFLAGS)

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	@mkdir -p $(@D)
	$(CC) -c -o $@ $< $(CFLAGS)


$(BINDIR)/main: $(OBJ)
	@mkdir -p $(@D)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -rf $(ODIR) $(BINDIR) $(WEBGENDIR) *~ core

# WebASM version
wasm:
	@mkdir -p $(WEBGENDIR)
	$(CC_WEB) $(SDIR)/*.c $(GJKIDIR)/*.c -o $(WEBGENDIR)/index.js $(FLAGS_WEB)

ti:
	make -f makefile.ti84pce
