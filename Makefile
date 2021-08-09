all : builder

libraries:
	cd ./lib/json && make clean
	cd ./lib/json && make build
	
builder :
	cd ./src/builder && make build

clean:
	cd ./lib/json && make clean
