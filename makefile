default:
	$(MAKE) host
	$(MAKE) virus
	$(MAKE) seed
	$(MAKE) wrappers
	$(MAKE) test-wrappers

host: host.c
	gcc -std=c99 -o host host.c

virus: virus.c
	gcc -std=c99 -o virus virus.c
	
seed: host virus
	touch seed
	cat virus >> seed
	cat deadbeef >> seed
	cat host >> seed
	chmod a+x seed

wrappers: open.c close.c 
	gcc -c open.c
	gcc -c close.c
test-wrappers: tstWrappers.c wrappers
	gcc -o tstWrappers tstWrappers.c open.o close.o
clean:
	rm tstWrappers seed virus host open.o close.o
