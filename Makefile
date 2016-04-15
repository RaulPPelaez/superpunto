
all:
	$(MAKE) -C src/ all
	mv src/spunto bin/
clean:
	rm -f bin/spunto
	$(MAKE) -C src/ clean
redo:
	$(MAKE) -C src/ redo 
