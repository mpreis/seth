all: cryptopp clean
	mkdir build
	cd ./src; make
	
clean:
	rm -rf build

fullclean: clean
	find . -type f -name '*.o' -delete

cryptopp:
	cd ./src/libraries/cryptopp562/; make static
	cd ../../../
	