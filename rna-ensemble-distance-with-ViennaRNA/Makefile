.PHONY: all clean

# adjust paths to installation directory of ViennaRNA (tested with ViennaRNA-1.8.5)
VIENNA     = $(HOME)/app/x86_64-opensuse-11.1/pkg/vienna-rna-1.8.5/
VIENNA_INC = $(VIENNA)/include/ViennaRNA
VIENNA_LIB = $(VIENNA)/lib

CC      = gcc
CFLAGS  = -std=c89 -pedantic -Wall -Wextra -Wno-unused
CFLAGS += -ggdb3 -O2

ALLPROGS = rna-ensemble-distance

all: $(ALLPROGS)

rna-ensemble-distance: rna-ensemble-distance.c
	$(CC) $(CFLAGS) -I$(VIENNA_INC) -c rna-ensemble-distance.c
	$(CC) -L$(VIENNA_LIB) -lRNA -lm rna-ensemble-distance.o $(VIENNA_LIB)/libRNA.a -o rna-ensemble-distance

clean:
	rm -f *.o $(ALLPROGS)

