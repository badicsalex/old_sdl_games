ALL_DATA := $(wildcard */data/*.*)
LDFLAGS = -lSDL -lm -lSDL_image -lSDL_mixer

.PHONY: all clean
all: asteroid/asteroid breakout/breakout duckhunt/duckhunt

clean:
	rm -f asteroid/asteroid breakout/breakout duckhunt/duckhunt
	rm -f */*.o

asteroid/asteroid: asteroid/main.o
	$(CC) $(LDFLAGS) -o $@ $(LOADLIBES) $(LDLIBS) $^

breakout/breakout: breakout/main.o
	$(CC) $(LDFLAGS) -o $@ $(LOADLIBES) $(LDLIBS) $^

duckhunt/duckhunt: duckhunt/main.o
	$(CC) $(LDFLAGS) -o $@ $(LOADLIBES) $(LDLIBS) $^


