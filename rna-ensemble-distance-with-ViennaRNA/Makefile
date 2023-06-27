.PHONY: all clean build-ViennaRNA-1.8.5

# adjust paths to installation directory of ViennaRNA (tested with ViennaRNA-1.8.5)
VIENNA     = ./install-ViennaRNA-1.8.5
VIENNA_INC = $(VIENNA)/include/ViennaRNA
VIENNA_LIB = $(VIENNA)/lib

CC      = gcc
CFLAGS  = -std=c89 -pedantic -Wall -Wextra -Wno-unused
CFLAGS += -ggdb3 -O2

ALLPROGS = rna-ensemble-distance

all: $(VIENNA_LIB)/libRNA.a $(ALLPROGS)

rna-ensemble-distance: rna-ensemble-distance.c
	$(CC) $(CFLAGS) -I$(VIENNA_INC) -c rna-ensemble-distance.c
	$(CC) -o rna-ensemble-distance rna-ensemble-distance.o $(VIENNA_LIB)/libRNA.a -lm

clean:
	rm -f *.o $(ALLPROGS)
	rm -rf ./ViennaRNA-1.8.5
	rm -rf ./install-ViennaRNA-1.8.5

$(VIENNA_LIB)/libRNA.a:
	./build-ViennaRNA-1.8.5.bash
