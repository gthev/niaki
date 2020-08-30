all:
	g++ -o niaki main.cpp drive_grille.cpp grille.cpp -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
