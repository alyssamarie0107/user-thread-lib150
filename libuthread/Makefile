# Target library
lib := libuthread.a 
targets := libs queue_tester
objs := queue_tester.o queue.o

CC	:= gcc
CFLAGS := -Wall -Wextra -Werror -MMD
CFLAGS  += -g

ifneq ($(V),1)
Q = @
endif

all: $(targets)
deps := $(patsubst %.o, %.d, $(objs))
-include $(deps)

queue_tester: $(objs)
	@echo "CC $@"
	$(Q)$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	@echo "CC $@"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<


libuthread.a: $(objs)
	ar rcs $(lib) $(objs)

libs: libuthread.a
	
clean:
	@echo "clean"
	$(Q) rm -f $(targets) $(objs) $(deps)