CPP = g++
MSWCPP = i386-mingw32msvc-g++

VERSION = 4.4
MSVERSION = 4_4

FLAGS = -Wall -pipe -O3 -funroll-loops -Wall -DVERSION="\"$(VERSION)\"" -DCVERSION="\"`date +%Y%m%d`\""

DEFAULT: gtk

####################################################### gtk ###########################################

gtk: Sand.o MainFrame.o Canvas.o DownloadFileDialog.o
	$(CPP) $(FLAGS) -ggdb Sand.o MainFrame.o Canvas.o DownloadFileDialog.o `wx-config --libs --cxxflags` -o sand 

Sand.o: Sand.cpp Sand.h
	$(CPP) -c $(FLAGS) -ggdb Sand.cpp `wx-config --cxxflags` -o Sand.o 

MainFrame.o: MainFrame.cpp MainFrame.h pause.xpm refresh.xpm 
	$(CPP) -c $(FLAGS) -ggdb MainFrame.cpp `wx-config --cxxflags` -o MainFrame.o 

Canvas.o: Canvas.cpp Canvas.h
	$(CPP) -c $(FLAGS) -ggdb Canvas.cpp `wx-config --cxxflags` -o Canvas.o

DownloadFileDialog.o: DownloadFileDialog.cpp DownloadFileDialog.h
	$(CPP) -c $(FLAGS) -ggdb DownloadFileDialog.cpp `wx-config --cxxflags` -o DownloadFileDialog.o

####################################################### msw ###########################################


msw: Sand.mswo MainFrame.mswo Canvas.mswo DownloadFileDialog.mswo
	$(MSWCPP) $(FLAGS) -ggdb Sand.mswo wxres.o MainFrame.mswo Canvas.mswo DownloadFileDialog.mswo `~/opt/cross-compile/bin/wx-config --libs --cxxflags` -o sand.exe

Sand.mswo: Sand.cpp Sand.h
	$(MSWCPP) -c $(FLAGS) -ggdb Sand.cpp `~/opt/cross-compile/bin/wx-config --cxxflags` -o Sand.mswo 

MainFrame.mswo: MainFrame.cpp MainFrame.h pause.xpm refresh.xpm 
	$(MSWCPP) -c $(FLAGS) -ggdb MainFrame.cpp `~/opt/cross-compile/bin/wx-config --cxxflags` -o MainFrame.mswo 

Canvas.mswo: Canvas.cpp Canvas.h
	$(MSWCPP) -c $(FLAGS) -ggdb Canvas.cpp `~/opt/cross-compile/bin/wx-config --cxxflags` -o Canvas.mswo

DownloadFileDialog.mswo: DownloadFileDialog.cpp DownloadFileDialog.h
	$(MSWCPP) -c $(FLAGS) -ggdb DownloadFileDialog.cpp `~/opt/cross-compile/bin/wx-config --cxxflags` -o DownloadFileDialog.mswo


#######################################################  ###########################################

dist: gtk msw
	i386-mingw32msvc-strip sand.exe
	strip sand
	cp -f sand.exe fsg-$(MSVERSION)-`date +%Y%m%d`.exe
	cp -f sand fsg-$(VERSION)-`date +%Y%m%d`
	scp sand-* piettes.com:piettes.com/fallingsandgame/

make distsrc:
	rm -rf ../fsg-$(VERSION)
	rm -f ../fsg-src-$(VERSION).tar.gz
	mkdir ../fsg-$(VERSION)
	cp *.cpp *.h *.xpm makefile ../fsg-$(VERSION)
	tar -cf ../fsg-src-$(VERSION).tar ../fsg-$(VERSION)
	gzip ../fsg-src-$(VERSION).tar
	scp ../fsg-src-$(VERSION).tar.gz piettes.com:piettes.com/fallingsandgame/

