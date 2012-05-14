.PHONY: all install uninstall clean doc

all:
	make -C src

install:
	make -C src install

uninstall:
	make -C src uninstall

clean:
	make -C src clean

doc:
	make -C src doc

