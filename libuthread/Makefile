# Target library
# library archive must be the default target of libuthread Makefile
target := libuthread.a
objs:= queue_tester.o queue.o uthread.o context.o preempt.o sem.o

CC	:= gcc
CFLAGS := -Wall -Wextra -Werror -MMD
CFLAGS  += -g

ifneq ($(V),1)
Q = @
endif

all: $(target)
deps := $(patsubst %.o, %.d, $(all_objs))
-include $(deps)

$(target): $(objs)
	ar rcs $(target) $(objs)

%.o:%.c
	@echo "CC $@"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<
	
clean:
	@echo "clean"
	$(Q) rm -f $(target) $(objs) $(deps)