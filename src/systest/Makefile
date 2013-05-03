Host: Host.o
	g++ -o Host Host.o -L../build -llogsvccpp

Host.o: Host.cpp
	g++ -c -o Host.o Host.cpp -I..

Log: Log.o
	g++ -o Log Log.o -L../build -llogsvccpp
Log.o: Log.cpp
	g++ -c -o Log.o Log.cpp -I..

OutStream: OutStream.o
	g++ -o OutStream OutStream.o -L../build -llogsvccpp
OutStream.o: OutStream.cpp
	g++ -c -o OutStream.o OutStream.cpp -I..
