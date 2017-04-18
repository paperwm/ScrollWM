.DEFAULT_GOAL := all

CC = gcc

PKGS = clutter-wayland-compositor-1.0 clutter-x11-1.0 clutter-cogl-1.0 gdk-pixbuf-2.0
PKGS += gjs-1.0 gobject-introspection-1.0 gmodule-2.0

CFLAGS = $(shell pkg-config --cflags $(PKGS))
LDFLAGS = $(shell pkg-config --libs $(PKGS))

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

all: wayland-compositor 

clean: 
	rm -f build/*.o
	rm -f wayland-compositor
