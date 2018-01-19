objects = server.o gps_decoder.o
libs = -lsocket
includes = -I.
libPath =

all: $(objects) 
	QCC -Vgcc_ntoarmv7le -o socket_server_test $(objects) $(libs) $(libPath)

server.o: server.c
	QCC -Vgcc_ntoarmv7le -c server.c $(includes)

gps_decoder.o: gps_decoder.c
	QCC -Vgcc_ntoarmv7le -c gps_decoder.c $(includes)

clean:
	rm -f socket_server_test $(objects)
