all: clean
	mkdir build
	cd ./src; make

server: clean
	mkdir build
	cd ./src; make server
	
clean:
	rm -rf build

fullclean: clean
	find . -type f -name '*.o' -delete