.PHONY: all lib clean

CC      = cc
CFLAGS += -std=c99
CFLAGS += -O2 -ggdb3
CFLAGS += -pedantic -Wall -Wextra -Wshadow
CFLAGS += -Wno-unused-parameter -Wno-unused-variable -Wno-absolute-value
CPPFLAGS += -I.

#CFLAGS += -DNDEBUG

LDFLAGS = -lm
LDFLAGS_GSL = -lgsl -lgslcblas -lm

PROGS_OPT  = opt-md opt-sd eval-score eval-dGdp
PROGS_MISC = eval-useq eval-pseq random-seq random-vienna
PROGS_GSL  = opt-sd-gsl
ALLPROGS   = $(PROGS_OPT) $(PROGS_MISC) $(PROGS_GSL)

OBJS_COMMON = na.o nn_param.o $(OBJS_CTOOLS)
OBJS_OPT    = nj_param.o dss.o helpers-for-main.o helpers-for-main-opt.o $(OBJS_MD)
OBJS_CTOOLS = ctools/dary.o ctools/libctools.o ctools/num_utils.o ctools/random.o
OBJS_MD     = md/md.o
ALLOBJS     = $(OBJS_COMMON) $(OBJS_OPT) $(OBJS_CTOOLS) $(OBJS_MD)

LIB_FILE_EXT = so
LIBDSSOPT    = libdssopt.$(LIB_FILE_EXT)
ALLOBJS_SRC := $(subst .o,.c, $(ALLOBJS))

all: $(ALLPROGS)

lib: $(LIBDSSOPT)

clean:
	rm -f $(ALLPROGS) $(ALLOBJS) $(LIBDSSOPT)

$(PROGS_MISC): %: main-%.c $(OBJS_COMMON)
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(PROGS_OPT): %: main-%.c $(OBJS_COMMON) $(OBJS_OPT)
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(PROGS_GSL): %: main-%.c $(OBJS_COMMON) $(OBJS_OPT)
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@ $(LDFLAGS_GSL)

$(LIBDSSOPT): $(ALLOBJS_SRC)
	$(CC) -shared -fPIC $(CPPFLAGS) $(CFLAGS) $^ -o $@ $(LDFLAGS)
