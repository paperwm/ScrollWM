.DEFAULT_GOAL := all

CC = gcc

PKGS = clutter-wayland-compositor-1.0 clutter-x11-1.0 clutter-cogl-1.0 gdk-pixbuf-2.0
PKGS += gjs-1.0 gobject-introspection-1.0 gmodule-2.0

CFLAGS = $(shell pkg-config --cflags $(PKGS))
LDFLAGS = $(shell pkg-config --libs $(PKGS))

# Environment variables is automatically available as Makefile variables
DEFINES = -DGI_TYPELIB_PATH=$(GI_TYPELIB_PATH):.

SOURCES = $(wildcard *.c)

OBJS = $(addprefix build/,$(subst .c,.o,$(SOURCES)))

build-dir:
	@mkdir -p build

$(OBJS): | build-dir

build/%.o: %.c
	@$(CC) $(DEFINES) $(CFLAGS) -c $< -o $@
	@echo "CC $<"

wayland-compositor: $(OBJS)
	@$(CC) -o $@ $(OBJS) $(LDFLAGS)
	@echo Linking $@

# We should probably use autotools for the gir/typelib stuff..
# https://wiki.gnome.org/Projects/GObjectIntrospection/AutotoolsIntegration
Scroll-0.1.gir: scroll-global.c | wayland-compositor
	g-ir-scanner scroll-global.[ch] \
	         --program=./wayland-compositor \
	         `pkg-config --cflags gobject-introspection-1.0` \
	         --include=GObject-2.0 \
	         --include=Clutter-1.0 \
	         --namespace=Scroll \
	         --nsversion=0.1 \
	         --output=Scroll-0.1.gir \
	         --warn-all
%.typelib: %.gir
	g-ir-compiler $< -o $@

all: wayland-compositor Scroll-0.1.typelib

clean: 
	rm -f build/*.o
	rm -f wayland-compositor
	rm -f *.gir
	rm -f *.typelib
