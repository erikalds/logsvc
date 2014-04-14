LIBS=-llogsvccpp -llog -lnetwork -lboost_system -lboost_filesystem -lpthread
CPPFLAGS=-Wall -std=gnu++11
DEPLIBS=../build/liblogsvccpp.so ../build/liblog.so ../build/libnetwork.so

CLIBS=-llogsvcc
CFLAGS=-Wall
CDEPLIBS=../build/liblogsvcc.so

Host: Host.o $(DEPLIBS)
	g++ -o Host Host.o -L../build $(LIBS)

Host.o: Host.cpp ../logsvccpp/Host.h
	g++ -c -o Host.o Host.cpp $(CPPFLAGS) -I..

Log: Log.o $(DEPLIBS)
	g++ -o Log Log.o -L../build $(LIBS)
Log.o: Log.cpp ../logsvccpp/Log.h ../logsvccpp/Host.h
	g++ -c -o Log.o Log.cpp $(CPPFLAGS) -I..

OutStream: OutStream.o $(DEPLIBS)
	g++ -o OutStream OutStream.o -L../build $(LIBS)
OutStream.o: OutStream.cpp ../logsvccpp/Log.h ../logsvccpp/Host.h ../logsvccpp/OutStream.h
	g++ -c -o OutStream.o OutStream.cpp $(CPPFLAGS) -I..

logviastream: logviastream.o $(DEPLIBS)
	g++ -o logviastream logviastream.o -L../build $(LIBS)
logviastream.o: logviastream.cpp ../logsvccpp/Log.h ../logsvccpp/Host.h ../logsvccpp/OutStream.h
	g++ -c -o logviastream.o logviastream.cpp $(CPPFLAGS) -I..

loglinetofile: loglinetofile.o $(DEPLIBS)
	g++ -o loglinetofile loglinetofile.o -L../build $(LIBS)
loglinetofile.o: loglinetofile.cpp ../logsvccpp/Log.h ../logsvccpp/Host.h ../logsvccpp/OutStream.h
	g++ -c -o loglinetofile.o loglinetofile.cpp $(CPPFLAGS) -I..

cloglinetofile: cloglinetofile.o $(CDEPLIBS)
	gcc -o cloglinetofile cloglinetofile.o -L../build $(CLIBS)
cloglinetofile.o: cloglinetofile.c ../logsvcc/Log.h ../logsvcc/Host.h
	gcc -c -o cloglinetofile.o cloglinetofile.c $(CFLAGS) -I..

clean:
	rm -f Host.o Log.o OutStream.o logviastream.o Host Log OutStream logviastream loglinetofile.o loglinetofile.cpp
