default:
	$(MAKE) host
	$(MAKE) virus
	$(MAKE) seed
	$(MAKE) wrappers
	$(MAKE) test-wrappers

host: host.c
	mkdir -p build
	gcc -std=c99 -o host host.c

virus: virus.c
	mkdir -p build
	gcc -std=c99 -o virus virus.c
	
seed: host virus
	mkdir -p build
	touch build/seed
	cat build/virus >> build/seed
	cat deadbeef >> build/seed
	cat build/host >> build/seed
	chmod a+x build/seed

wrappers: open.c close.c 
	mkdir -p build
	gcc -c open.c
	gcc -c close.c
test-wrappers: tstWrappers
	gcc -o tstWrappers tstWrappers.c
clean:
	rm tstWrappers seed virus host open.o close.o
