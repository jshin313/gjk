CC=clang

CC_WEB=emcc
FLAGS_WEB=-s USE_SDL=2 -s USE_SDL_GFX=2 --bind

CFLAGS=-I$(IDIR) -Wall -Wextra -Wno-missing-braces
IDIR =src
SDIR=src
ODIR=obj
BINDIR=bin

LIBS=-lSDL2 -lSDL2_gfx

_DEPS = vector.h gjk.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o vector.o gjk.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	@mkdir -p $(@D)
	$(CC) -c -o $@ $< $(CFLAGS)

$(BINDIR)/main: $(OBJ)
	@mkdir -p $(@D)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -rf $(ODIR) $(BINDIR) *~ core $(INCDIR)/*~

# # Web version
# web: $(OBJ)
# 	$(CC_WEB) $^ -o index.html $(FLAGS_WEB)
