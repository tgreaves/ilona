# Makefile.

CC = gcc
CFLAGS = -g -Wall -pedantic

default: ilona

ilona: config.o logging.o bot.o main.o servers.o users.o parsing.o
	$(CC) $(CFLAGS) -o ilona config.o logging.o bot.o main.o servers.o users.o parsing.o

config.o:   config.c bot.h config.h main.h parsing.h users.h
	$(CC) $(CFLAGS) -c config.c

logging.o:  logging.c bot.h config.h logging.h main.h users.h
	$(CC) $(CFLAGS) -c logging.c

bot.o:      bot.c config.h bot.h logging.h main.h servers.h users.h
	$(CC) $(CFLAGS) -c bot.c

main.o:     main.c bot.h bot.h config.h main.h logging.h servers.h parsing.h users.h
	$(CC) $(CFLAGS) -c main.c

servers.o:  servers.c servers.h config.h bot.h logging.h main.h users.h
	$(CC) $(CFLAGS) -c servers.c

users.o:  users.c config.h bot.h logging.h users.h main.h
	$(CC) $(CFLAGS) -c users.c

parsing.o:  parsing.c config.h bot.h logging.h users.h main.h
	$(CC) $(CFLAGS) -c parsing.c

style:
	astyle --style=allman *.c *.h

clean:
	rm -f *.o *.orig

