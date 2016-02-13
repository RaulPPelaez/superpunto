
all:
	$(MAKE) -C src/ all
	mv src/spunto bin/
clean:
	rm bin/spunto
	$(MAKE) -C src/ clean
redo:
	$(MAKE) -C src/ redo 
