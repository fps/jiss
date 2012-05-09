.PHONY: all install clean doc

all:
	make -C src

install:
	make -C src install

clean:
	make -C src clean

doc:
	make -C src doc

