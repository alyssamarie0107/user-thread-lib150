# Target library
# library archive must be the default target of Makefile according to prompt
targets := libuthread.a
objs := queue_tester.o queue.o uthread.o context.o

CC	:= gcc
CFLAGS := -Wall -Wextra -Werror -MMD
CFLAGS  += -g

ifneq ($(V),1)
Q = @
endif

all: $(targets)
deps := $(patsubst %.o, %.d, $(objs))
-include $(deps)

$(targets): $(objs)
	@echo "CC $@"
	#$(Q)$(CC) $(CFLAGS) -o $@ $^
	ar rcs $(targets) $(objs)

%.o: %.c
	@echo "CC $@"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<
	
clean:
	@echo "clean"
	$(Q) rm -f $(targets) $(objs) $(deps)