Host: Host.o
	g++ -o Host Host.o -L../build -llogsvccpp

Host.o: Host.cpp ../logsvccpp/Host.h
	g++ -c -o Host.o Host.cpp -I..

Log: Log.o
	g++ -o Log Log.o -L../build -llogsvccpp
Log.o: Log.cpp ../logsvccpp/Log.h ../logsvccpp/Host.h
	g++ -c -o Log.o Log.cpp -I..

OutStream: OutStream.o
	g++ -o OutStream OutStream.o -L../build -llogsvccpp
OutStream.o: OutStream.cpp ../logsvccpp/Log.h ../logsvccpp/Host.h ../logsvccpp/OutStream.h
	g++ -c -o OutStream.o OutStream.cpp -I..

logtofile: logtofile.o
	g++ -o logtofile logtofile.o -L../build -llogsvccpp
logtofile.o: logtofile.cpp ../logsvccpp/Log.h ../logsvccpp/Host.h ../logsvccpp/OutStream.h
	g++ -c -o logtofile.o logtofile.cpp -I..
