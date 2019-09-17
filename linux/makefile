CFLAGS=-c -Wall -O2 -I../src
LIBS = -lm -lpthread

all: libbb_spi_lcd.a

libbb_spi_lcd.a: bb_spi_lcd.o
	ar -rc libbb_spi_lcd.a bb_spi_lcd.o ;\
	sudo cp libbb_spi_lcd.a /usr/local/lib ;\
	sudo cp ../src/bb_spi_lcd.h /usr/local/include

bb_spi_lcd.o: bb_spi_lcd.c ../src/bb_spi_lcd.cpp ../src/bb_spi_lcd.h
	$(CC) $(CFLAGS) -D_LINUX_ bb_spi_lcd.c

clean:
	rm *.o libbb_spi_lcd.a
