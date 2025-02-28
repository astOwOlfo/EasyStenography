compile:
	clang++ src/stenography.cpp -o stenography -std=c++17 -lX11 -lXi -lXtst `pkg-config --cflags --libs glib-2.0 gdk-pixbuf-2.0 libnotify`

run: compile
	./stenography run stenography-abbreviations