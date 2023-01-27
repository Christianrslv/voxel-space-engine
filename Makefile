build:
	gcc *.c `sd12-config --libs --cflags` -lGLEW -framework OpenGL -lpthread -o voxel

run:
	./voxel
