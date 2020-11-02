# Target library
lib := libuthread.a 
targets := queue_tester queue_tester.o queue.o

objs := queue_tester.o queue.o

CC	:= gcc
CFLAGS := -Wall -Wextra -Werror -MMD
CFLAGS  += -g



ifneq ($(V),1)
Q = @
endif

libthread.a : $(objs)
	ar rcs $(lib) $(objs)

all: $(targets)
deps := $(patsubst %.o, %.d, $(objs))
-include $(deps)

queue_tester: $(objs)
	@echo "CC $@"
	$(Q)$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	@echo "CC $@"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

clean:
	@echo "clean"
	$(Q) rm -f $(targets) $(objs) $(deps)