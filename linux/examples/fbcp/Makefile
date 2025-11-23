CFLAGS=-D__LINUX__ -Wall -O2 -I/usr/include/drm
LIBS=-lbb_spi_lcd -lgpiod -ldrm -L/usr/lib/aarch64-linux-gnu/

all: fbcp

fbcp: main.o
	$(CXX) main.o $(LIBS) -o fbcp

main.o: main.cpp
	$(CXX) $(CFLAGS) -c main.cpp

clean:
	rm -rf *.o fbcp
