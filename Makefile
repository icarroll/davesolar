davesolar: davesolar.cc
	g++ -m32 davesolar.cc -I/mingw32/include/SDL2 -I/mingw32/include/cairo -I/mingw32/include/chipmunk -L/mingw32/lib -Wl,-subsystem,windows -lmingw32 -lSDL2main -lSDL2 -lcairo -lchipmunk -mwindows -o davesolar.exe
