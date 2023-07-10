.PHONY: all lib clean check check-valgrind unittest unittest-valgrind

CC      = cc
CFLAGS += -std=c99
CFLAGS += -O3
#CFLAGS += -O2 -ggdb3
CFLAGS += -pedantic -Wall -Wextra -Wshadow
CFLAGS += -Werror
CFLAGS += -Wno-unused-parameter -Wno-unused-variable -Wno-absolute-value
CFLAGS += -Wstrict-prototypes
# CFLAGS += -Wmissing-prototypes
CPPFLAGS += -I.

#CFLAGS += -DNDEBUG

LDFLAGS = -lm
LDFLAGS_GSL = -lgsl -lgslcblas -lm

PROGS_OPT  = opt-md opt-sd eval-score eval-dGdp
PROGS_MISC = eval-useq eval-pseq random-seq random-vienna
PROGS_GSL  = opt-sd-gsl
ALLPROGS   = $(PROGS_OPT) $(PROGS_MISC) $(PROGS_GSL)

OBJS_COMMON = na.o nn_param.o $(OBJS_CTOOLS)
OBJS_OPT    = opt.o nj_param.o dss.o helpers-for-main.o helpers-for-main-opt.o $(OBJS_MD)
OBJS_CTOOLS = ctools/dary.o ctools/libctools.o ctools/num_utils.o ctools/random.o
OBJS_MD     = md/md.o
ALLOBJS     = $(OBJS_COMMON) $(OBJS_OPT) $(OBJS_CTOOLS) $(OBJS_MD)

LIB_FILE_EXT = so
LIBDSSOPT    = libdssopt.$(LIB_FILE_EXT)
ALLOBJS_SRC := $(subst .o,.c, $(ALLOBJS))

all: $(ALLPROGS) lib

lib: $(LIBDSSOPT)

clean:
	rm -f $(ALLPROGS) $(ALLOBJS) $(LIBDSSOPT)
	make -C unittest clean

check: all unittest
	@echo "#######################"
	@echo "# Running program tests"
	@echo "#######################"
	./tests/test-executables.bash

check-valgrind: all unittest-valgrind
	@echo "#####################################"
	@echo "# Running program tests with valgrind"
	@echo "#####################################"
	./tests/test-executables.bash --valgrind

unittest: lib
	make -C unittest check

unittest-valgrind: lib
	make -C unittest check-valgrind

$(PROGS_MISC): %: main-%.c $(OBJS_COMMON)
	@echo -n "[LINK     ]  "
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(PROGS_OPT): %: main-%.c $(OBJS_COMMON) $(OBJS_OPT)
	@echo -n "[LINK     ]  "
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(PROGS_GSL): %: main-%.c $(OBJS_COMMON) $(OBJS_OPT)
	@echo -n "[LINK     ]  "
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@ $(LDFLAGS_GSL)

$(LIBDSSOPT): $(ALLOBJS_SRC)
	@echo -n "[SHAREDLIB]  "
	$(CC) -shared -fPIC $(CPPFLAGS) $(CFLAGS) $^ -o $@ $(LDFLAGS)
