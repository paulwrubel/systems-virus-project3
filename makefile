default:
	$(MAKE) host
	$(MAKE) virus
	$(MAKE) seed
	$(MAKE) wrappers
	$(MAKE) test-wrappers

host: host.c
	mkdir -p build
	gcc -std=c99 -o build/host host.c

virus: virus.c
	mkdir -p build
	gcc -std=c99 -o build/virus virus.c
	
seed: host virus
	mkdir -p build
	touch build/seed
	cat build/virus >> build/seed
	cat deadbeef >> build/seed
	cat build/host >> build/seed
	chmod a+x build/seed

wrappers: useOpen.c useClose.c open.c close.c 
	mkdir -p build
	gcc -c open.c
	gcc -o build/useOpen useOpen.c open.c
	gcc -c close.c
	gcc -o build/useClose useClose.c close.c
test-wrappers:

clean:
	rm -r build
