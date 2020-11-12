# Target library
# library archive must be the default target of Makefile
default_target := libuthread.a
targets := libuthread.a queue_tester uthread sem test_preempt
objs1 := queue_tester.o queue.o
objs2 := test_preempt.o uthread.o preempt.o 
all_objs:= queue_tester.o queue.o uthread.o context.o preempt.o sem.o test_preempt.o
#all_objs:= queue_tester.o queue.o

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

test_preempt: $(all_objs)
	@echo "CC $@"
	$(Q)$(CC) $(CFLAGS) -o $@ $^

uthread: $(all_objs)
	@echo "CC $@"
	$(Q)$(CC) $(CFLAGS) -o $@ $^

sem: $(all_objs)
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