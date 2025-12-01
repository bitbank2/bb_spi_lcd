CFLAGS=-c -ggdb -Wall -O2 -D__LINUX__ -I../src
LIBS = -lm -lpthread

all: libbb_spi_lcd.a

libbb_spi_lcd.a: bb_spi_lcd.o bb_parallel.o
	ar -rc libbb_spi_lcd.a bb_spi_lcd.o bb_parallel.o ;\
	sudo cp libbb_spi_lcd.a /usr/local/lib ;\
	sudo cp ../src/bb_spi_lcd.h /usr/local/include

bb_spi_lcd.o: ../src/bb_spi_lcd.cpp ../src/linux_io.inl ../src/bb_spi_lcd.h
	$(CXX) $(CFLAGS) ../src/bb_spi_lcd.cpp

bb_parallel.o: ../src/bb_parallel.cpp ../src/bb_spi_lcd.h
	$(CXX) $(CFLAGS) ../src/bb_parallel.cpp

clean:
	rm *.o libbb_spi_lcd.a
