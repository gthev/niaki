#include "grille.hpp"

struct retCheckGO {
    enum rettype {
        RET_OK,
        RET_STUCK
    };

    enum rettype retcode;
    coord hint;

};

Grille *gen_new_grille(unsigned short width, unsigned short height);

struct retCheckGO checkIfGameOver(Grille *grille);
