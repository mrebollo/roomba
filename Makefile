CC = gcc
CFLAGS = -Wall -O2 -std=c99

SRCS = main.c simula_core.c simula_map.c simula_robot.c simula_stats.c
OBJS = $(SRCS:.c=.o)
HEADERS = simula_core.h simula_map.h simula_robot.h simula_stats.h simula_types.h

TARGET = simula

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) -lm

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
