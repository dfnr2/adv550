# Something quick and dirty... you must put things where you want...
#

# Installation paths (can be overridden on command line)
EXE_INSTALL_PATH ?= /usr/local/bin
DB_INSTALL_PATH ?= /usr/local/share/adventure

all:
	mkdir -p build
	cd src/kio ; make
	cd src/munge ; make
	cd src/adv ; make
	src/munge/munge < comcave
	cp src/adv/adv build/
	cp adv.key adv.rec build/
	@echo "All done. Files copied to ./build/"

lint:
	cd src/kio ; make lint
	cd src/munge ; make lint
	cd src/adv ; make lint

print:
	cd src/kio ; make print
	cd src/munge ; make print
	cd src/adv ; make print

test:
	cd src/kio && make test
	cd src/munge && make test
	cd src/adv && make test

test-path:
	cd src/kio && make test-path

install:
	install -d $(EXE_INSTALL_PATH)
	install -d $(DB_INSTALL_PATH)
	install -m 755 build/adv $(EXE_INSTALL_PATH)/
	install -m 644 build/adv.key build/adv.rec $(DB_INSTALL_PATH)/
	@echo "Installed: adv to $(EXE_INSTALL_PATH), database files to $(DB_INSTALL_PATH)"

clean:
	cd src/kio ; rm -f klib.a *.o test_path
	cd src/munge ; rm -f munge mlib.a *.o adv.key adv.rec
	cd src/adv ; rm -f adv alib.a *.o
	rm -f adv.key adv.rec
	rm -rf build