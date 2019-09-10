CFLAGS=-c -Wall -O2 -I../src
LIBS = -lm -lpthread

all: libbb_spi_lcd.a

libbb_spi_lcd.a: bb_spi_lcd.o
	ar -rc libbb_spi_lcd.a bb_spi_lcd.o ;\
	sudo cp libbb_spi_lcd.a /usr/local/lib ;\
	sudo cp ../src/bb_spi_lcd.h /usr/local/include

bb_spi_lcd.o: ../src/bb_spi_lcd.cpp
	$(CC) $(CFLAGS) -D_LINUX_ ../src/bb_spi_lcd.cpp

clean:
	rm *.o libbb_spi_lcd.a
