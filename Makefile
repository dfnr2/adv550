# Something quick and dirty... you must put things where you want...
#
all:
	cd src/kio ; make
	cd src/munge ; make
	cd src/adv ; make
	src/munge/munge < comcave
	@echo "All done."

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

clean:
	cd src/kio ; rm -f klib.a *.o test_path
	cd src/munge ; rm -f munge mlib.a *.o
	cd src/adv ; rm -f adv alib.a *.o
	rm -f adv.key adv.rec
