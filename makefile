all:main
binDirectory = Debug

main :	dir
	g++ dev/*.cpp -o Debug/DataBlitz

dir:	
	mkdir -p $(binDirectory)

clean:
	rm -rf $(binDirectory)
