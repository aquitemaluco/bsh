CC := gcc
CFLAGS := -c -Wall

HEADERS := $(shell ls *.h)
SOURCES := bsh.c
OBJECTS := $(SOURCES:.c=.o)
OUT := ../bsh


all: $(OUT)

$(OUT): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $(OUT)

.c.o: $(HEADERS)
	$(CC) $(INCLUDES) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(OUT)  

