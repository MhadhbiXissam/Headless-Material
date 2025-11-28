

build : 
	gcc main.c material.c renderer.c   -o material_app -lGLESv2 -lEGL -ldl -lm -o main