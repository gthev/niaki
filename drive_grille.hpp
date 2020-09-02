#include "grille.hpp"

#define WIDTH_BLOCKS    6
#define HEIGHT_BLOCKS   8

#define OFFSET_SCORE    180

#define SIZE_BLOCK  50
#define W_WIDTH     ((WIDTH_BLOCKS) * (SIZE_BLOCK))         //20 * 6
#define W_HEIGHT    ((HEIGHT_BLOCKS) * (SIZE_BLOCK) + (OFFSET_SCORE))         //20 * 8

struct retCheckGO {
    enum rettype {
        RET_OK,
        RET_STUCK
    };

    enum rettype retcode;
    coord hint;

};

Color getCouleur(enum couleur couleur);

Grille *gen_new_grille(unsigned short width, unsigned short height);

struct retCheckGO checkIfGameOver(Grille *grille);

void drawCell(unsigned short x, unsigned short y, Grille *grille);
