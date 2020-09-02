all:
	g++ main.cpp animation.cpp history.cpp drive_grille.cpp grille.cpp permutation.cpp -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o niaki
