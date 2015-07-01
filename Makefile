all: clean
	mkdir build
	cd ./src; make
	
clean:
	rm -rf build

fullclean: clean
	find . -type f -name '*.o' -delete
