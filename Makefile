CC = gcc
LDFLAGS = -lm
OBJS = main.o \
       x2device.o

x2app: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	-rm -rf $(OBJS)

