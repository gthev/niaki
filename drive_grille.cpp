#include "drive_grille.hpp"

template<typename S>
auto select_random(const S &s, size_t n) {
  auto it = std::begin(s);
  // 'advance' the iterator n times
  std::advance(it,n);
  return it;
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