all:
	meson compile -C build

setup:
	meson setup --reconfigure build

run: all
	build/learn-opengl

.PHONY: setup all run
