CFLAGS=-c -Wall -O2 -D__LINUX__
LIBS = -lbb_spi_lcd -lgpiod -lm -lpthread

all: hello_cpp

hello_cpp: main.o
	$(CC) main.o $(LIBS) -o hello_cpp

main.o: main.cpp
	$(CXX) $(CFLAGS) main.cpp

clean:
	rm *.o hello_cpp
