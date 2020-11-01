# Target library
lib := main main.o queue.o

objs := main.o queue.o

CC	:= gcc
CFLAGS := -Wall -Wextra -Werror -MMD
CFLAGS  += -g

ifneq ($(V),1)
Q = @
endif


all: $(lib)
deps := $(patsubset %.o, %.d, $(objs))
-include $(deps)

main: $(objs)
	@echo "CC $@"
	$(Q)$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	@echo "CC $@"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

clean:
	@echo "clean"
	$(Q) rm -f $(targets) $(objs) $(deps)