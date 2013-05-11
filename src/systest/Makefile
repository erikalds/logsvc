LIBS=-llogsvccpp -llog -lboost_system-mt -lboost_filesystem-mt
CPPFLAGS=-Wall -std=gnu++11

Host: Host.o
	g++ -o Host Host.o -L../build $(LIBS)

Host.o: Host.cpp ../logsvccpp/Host.h
	g++ -c -o Host.o Host.cpp $(CPPFLAGS) -I..

Log: Log.o
	g++ -o Log Log.o -L../build $(LIBS)
Log.o: Log.cpp ../logsvccpp/Log.h ../logsvccpp/Host.h
	g++ -c -o Log.o Log.cpp $(CPPFLAGS) -I..

OutStream: OutStream.o
	g++ -o OutStream OutStream.o -L../build $(LIBS)
OutStream.o: OutStream.cpp ../logsvccpp/Log.h ../logsvccpp/Host.h ../logsvccpp/OutStream.h
	g++ -c -o OutStream.o OutStream.cpp $(CPPFLAGS) -I..

logtofile: logtofile.o
	g++ -o logtofile logtofile.o -L../build $(LIBS)
logtofile.o: logtofile.cpp ../logsvccpp/Log.h ../logsvccpp/Host.h ../logsvccpp/OutStream.h
	g++ -c -o logtofile.o logtofile.cpp $(CPPFLAGS) -I..

clean:
	rm -f Host.o Log.o OutStream.o logtofile.o Host Log OutStream logtofile
