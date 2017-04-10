
test: src/xdg-foreign.c src/main.c src/xdg-shell.c
	gcc -g -o test src/main.c src/xdg-foreign.c src/xdg-shell.c `pkg-config --libs mutter-clutter-1.0 --cflags glib-2.0`
