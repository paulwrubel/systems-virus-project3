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

wrappers:

test-wrappers:

clean:
	rm -r build
