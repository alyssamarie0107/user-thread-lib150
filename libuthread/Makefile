# Target library
# library archive must be the default target of Makefile
default_target := libuthread.a
targets := libuthread.a
all_objs:= queue.o uthread.o context.o preempt.o sem.o

CC	:= gcc
CFLAGS := -Wall -Wextra -Werror -MMD
CFLAGS  += -g

ifneq ($(V),1)
Q = @
endif

all: $(targets)
deps := $(patsubst %.o, %.d, $(all_objs))
-include $(deps)

$(default_target): $(all_objs)
	ar rcs $(default_target) $(all_objs)

%.o: %.c
	@echo "CC $@"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<
	
clean:
	@echo "clean"
	$(Q) rm -f $(targets) $(all_objs) $(deps)