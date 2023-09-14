CC 	 := gcc
LDFLAGS  := -ldl -lpthread
CFLAGS := -Wall -g
TARGET   := mainApp

OBJS = $(patsubst %.c, %.o, $(wildcard *.c))

$(TARGET):$(OBJS)
	$(CC) -o $@ $(OBJS) $(CFLAGS) $(LDFLAGS)
	$(MAKE) clean

%.o:%.cpp
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY:clean cleanall
cleanall:clean
	@rm $(TARGET)
clean:
	@rm ./*.o 
