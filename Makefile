CFLAGS=-I/usr/include/libxml2

all: bzl

bzl: bzl.c
	$(CC) $(CFLAGS) -o bzl bzl.c -lxml2
