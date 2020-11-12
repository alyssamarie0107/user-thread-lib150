# Target library
# library archive must be the default target of Makefile
default_target := libuthread.a
targets := libuthread.a queue_tester uthread sem preempt
objs1 := queue_tester.o queue.o 
objs2 := uthread.o queue.o context.o
objs3 := sem.o queue.o context.o
objs4 := preempt.o uthread.o context.o
all_objs:= queue_tester.o queue.o uthread.o context.o sem.o preempt.o 

CC	:= gcc
CFLAGS := -Wall -Wextra -Werror -MMD
CFLAGS  += -g

ifneq ($(V),1)
Q = @
endif

all: $(targets)
deps := $(patsubst %.o, %.d, $(all_objs))
-include $(deps)

queue_tester: $(objs1)
	@echo "CC $@"
	$(Q)$(CC) $(CFLAGS) -o $@ $^

uthread: $(objs2)
	@echo "CC $@"
	$(Q)$(CC) $(CFLAGS) -o $@ $^

sem: $(objs3)
	@echo "CC $@"
	$(Q)$(CC) $(CFLAGS) -o $@ $^

preempt: $(objs4)
	@echo "CC $@"
	$(Q)$(CC) $(CFLAGS) -o $@ $^

$(default_target): $(all_objs)
	ar rcs $(default_target) $(all_objs)

%.o: %.c
	@echo "CC $@"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<
	
clean:
	@echo "clean"
	$(Q) rm -f $(targets) $(all_objs) $(deps)