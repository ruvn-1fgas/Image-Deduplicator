@echo off
set CXX=g++
set CXX_FLAGS=-Wall -Wextra -std=c++17 -ggdb -IC:/msys64/mingw64/bin/../include/gtk-4.0 -IC:/msys64/mingw64/bin/../include/pango-1.0 -IC:/msys64/mingw64/bin/../include -IC:/msys64/mingw64/bin/../include/gdiplus -IC:/msys64/mingw64/bin/../include/glib-2.0 -IC:/msys64/mingw64/bin/../lib/glib-2.0/include -IC:/msys64/mingw64/bin/../include/harfbuzz -IC:/msys64/mingw64/bin/../include/freetype2 -IC:/msys64/mingw64/bin/../include/libpng16 -IC:/msys64/mingw64/bin/../include/fribidi -IC:/msys64/mingw64/bin/../include/cairo -IC:/msys64/mingw64/bin/../include/lzo -IC:/msys64/mingw64/bin/../include/pixman-1 -IC:/msys64/mingw64/bin/../include/gdk-pixbuf-2.0 -IC:/msys64/mingw64/bin/../include/graphene-1.0 -IC:/msys64/mingw64/bin/../lib/graphene-1.0/include -IC:/msys64/mingw64/bin/../include/SDL2 -Ofast -static-libstdc++ -static-libgcc
set BIN=bin
set SRC=src
set INCLUDE=include
set LIB=lib
set EXECUTABLE=Image_Deduplicator
@REM -mwindows
if not exist %BIN% mkdir %BIN%

echo Compiling the C++ program...
%CXX% %CXX_FLAGS% %SRC%\main.cpp -o %BIN%\%EXECUTABLE% -LC:/msys64/mingw64/lib -lgtk-4 -lgraphene-1.0 -lgdiplus -lpangowin32-1.0 -lpangocairo-1.0 -lpango-1.0 -lharfbuzz -lgdk_pixbuf-2.0 -lcairo-gobject -lcairo -lgio-2.0 -lgobject-2.0 -lglib-2.0 -lintl -lpng -ljpeg
echo Done.