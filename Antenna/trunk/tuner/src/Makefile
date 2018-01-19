tunerservice:TunerService.o
	QCC -Vgcc_ntoarmv7le -o tunerservice TunerService.o -lpps -lsocket
TunerService.o:TunerService.c
	QCC -Vgcc_ntoarmv7le -c TunerService.c
clean:
	rm tunerservice TunerService.o

