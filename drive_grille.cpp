#include "drive_grille.hpp"

template<typename S>
auto select_random(const S &s, size_t n) {
  auto it = std::begin(s);
  // 'advance' the iterator n times
  std::advance(it,n);
  return it;
}

Color getCouleur(enum couleur couleur) {
    if(couleur < 0 || couleur > NUMBER_COLOR) return RAYWHITE;

    switch (couleur)
    {
    case COLOR_RED:
        return RED;
        break;
    case COLOR_YELLOW:
        return YELLOW;
        
    case COLOR_ORANGE:
        return ORANGE;

    case COLOR_PINK:
        return VIOLET;

    case COLOR_GREEN:
        return GREEN;

    case COLOR_BLUE:
        return BLUE;
    
    default:
        return BLUE;
    }
}

Grille *gen_new_grille(unsigned short width, unsigned short height) {
    Grille *grille = new Grille(width, height);

    for(unsigned short x = 0; x < width; x++) {
        for(unsigned short y = 0; y < height; y++) {
            //ici, on rajoute la nvelle cc en faisant attention à ne pas prendre de couleur adjacente
            std::vector<coord> voisins;
            if(x > 0) voisins.push_back(coord(x-1, y));
            if(y > 0) voisins.push_back(coord(x, y-1));

            std::set<enum couleur> potentielles;
            for(int couleur = 0; couleur < NUMBER_COLOR; couleur++) {
                potentielles.insert((enum couleur)couleur);
            }

            for(auto &voisin : voisins) {
                potentielles.erase(grille->getCc(voisin.first, voisin.second)->couleur);
            }

            grille->newCc(x, y, *select_random(potentielles, rand() % potentielles.size()));

        }
    }

    return grille;
}

struct retCheckGO checkIfGameOver(Grille *grille) {
    
    coord dim_grille = grille->getDimGrid();
    struct retCheckGO res;
    
    for(unsigned short x = 0; x < dim_grille.first; x++) {
        for(unsigned short y = 0; y < dim_grille.second; y++) {

            std::set<enum direction> candidats = {DIR_UP, DIR_LEFT, DIR_DOWN, DIR_RIGHT};
            if(x == 0) candidats.erase(DIR_LEFT);
            if(y == 0) candidats.erase(DIR_UP);
            if(x == dim_grille.first - 1) candidats.erase(DIR_RIGHT);
            if(y == dim_grille.second - 1) candidats.erase(DIR_DOWN);

            std::set<enum direction>::iterator it = candidats.begin();

            while(it != candidats.end()) {
                struct retMoveBlock ret;
                ret = grille->moveBlock(x,y,*it);
                if(ret.rettype == retMoveBlock::RET_OK) {
                    res.retcode = retCheckGO::RET_OK;
                    res.hint = coord(x,y);
                    goto out;
                }
                it++;
            }
        }
    }

    res.retcode = retCheckGO::RET_STUCK;
    res.hint = coord(0,0);

out:
    return res;
}

void drawCell(unsigned i, unsigned j, Grille *grille) {
    DrawRectangle(SIZE_BLOCK * i,OFFSET_SCORE + SIZE_BLOCK * j , SIZE_BLOCK, SIZE_BLOCK, getCouleur(grille->getCc(i,j)->couleur));
}
