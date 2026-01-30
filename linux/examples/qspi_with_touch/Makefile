CFLAGS=-D__LINUX__ -Wall -O2 -I../../
LIBS=-lbb_spi_lcd -lbb_captouch -lgpiod 

all: qspi

qspi: main.o
	$(CXX) main.o $(LIBS) -o qspi

main.o: main.cpp
	$(CXX) $(CFLAGS) -c main.cpp

clean:
	rm -rf *.o qspi
