# GNU Makefile
# - wildcard
# - patsubst
# - functions

.PHONY: all clean check

CC      = cc
CFLAGS += -std=c99
CFLAGS += -O2
CFLAGS += -pedantic -Wall -Wextra -Wshadow
CFLAGS += -Werror
CFLAGS += -Wno-unused-parameter -Wno-unused-variable -Wno-absolute-value
CFLAGS += -Wstrict-prototypes

VALGRIND := valgrind -q

DSS_OPT_DIR := ..
CFLAGS += -I$(DSS_OPT_DIR)
LDFLAGS = -L$(DSS_OPT_DIR) -Wl,-rpath=$(DSS_OPT_DIR) -Wl,--enable-new-dtags -ldssopt
LIBDSSOPT = $(DSS_OPT_DIR)/libdssopt.so

TEST_PROGRAMS_SRC = $(wildcard test-*.c)
TEST_PROGRAMS = $(patsubst %.c,run-%,$(TEST_PROGRAMS_SRC))

UNITY_DIR = ./Unity/src
CFLAGS += -I$(UNITY_DIR)
UNITY_SRC = ./Unity/src/unity.c
SRCS = $(TEST_PROGRAMS_SRC) $(UNITY_SRC)
OBJS = $(SRCS:.c=.o)
UNITY_OBJ = $(UNITY_SRC:.c=.o)

all: $(TEST_PROGRAMS) check

# We use the Unity unit testing framework
Unity:
	git clone --depth 1 https://github.com/ThrowTheSwitch/Unity/

$(LIBDSSOPT):
	make -C .. lib

$(TEST_PROGRAMS): run-%: $(LIBDSSOPT) %.o $(UNITY_OBJ)
	@echo -n "[LINK   ]  "
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

%.o: %.c Unity
	@echo -n "[COMPILE]  "
	$(CC) $(CFLAGS) -c $< -o $@

check: $(TEST_PROGRAMS)
	@echo "###################"
	@echo "# Running unittests"
	@echo "###################"
	./all-test-runner.bash

check-valgrind: $(TEST_PROGRAMS)
	@echo "#################################"
	@echo "# Running unittests with valgrind"
	@echo "#################################"
	./all-test-runner.bash $(VALGRIND)

clean:
	rm -f $(TEST_PROGRAMS) $(OBJS)

