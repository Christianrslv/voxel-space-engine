build:
	gcc *.c `sdl2-config --libs --cflags` -lGLEW -lGL -lm -ldl -lpthread -o voxel

run:
	./voxel
