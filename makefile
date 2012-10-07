all:main
binDirectory = Debug

main :	dir
	g++ dev/*.cpp -o Debug/DataBlitz

dir:	
	mkdir -p $(binDirectory)

