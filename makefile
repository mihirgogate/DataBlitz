all:main
binDirectory = Debug
jsonCPPLibDirectory = dev/jsonCPP/libs/

main :	dir
	g++ dev/*.cpp -o Debug/DataBlitz  $(jsonCPPLibDirectory)libjson_linux-gcc-4.6_libmt.a

dir:	
	mkdir -p $(binDirectory)

clean:
	rm -rf $(binDirectory)
