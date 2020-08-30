#include <iostream>
#include <set>
#include <map>
#include <stack>
#include <vector>
#include <queue>
#include <iterator>
#include <algorithm>

#define DEFAULT_HEIGHT_GRID     8
#define DEFAULT_WIDTH_GRID      6

#define MAX_HEIGHT 1000
#define MAX_WIDTH  1000

#define COORD(x,y, height)              ((x)*(height)+(y))

enum couleur {
    COLOR_RED,
    COLOR_YELLOW,
    COLOR_PINK,
    COLOR_BLUE,
    COLOR_GREEN,
    COLOR_ORANGE,
    NUMBER_COLOR
};

enum direction {
    DIR_UP,
    DIR_LEFT,
    DIR_DOWN,
    DIR_RIGHT
};

/*
-------------------> x
|
|
|
|
|
v
y
*/

typedef std::pair<unsigned short, unsigned short> coord;

/* cc pour "composante connexe" */
struct cc {
    int size;
    enum couleur couleur;
    std::set<coord> elements;
};


class Grille
{
private:
    struct cc **m_grid;
    unsigned short m_height, m_width;
    std::set<struct cc*> m_list_cc;

    void addElement(struct cc *c, unsigned short x, unsigned short y);
    void remElement(struct cc *c, unsigned short x, unsigned short y);
    struct cc *newCc(enum couleur couleur);
    void swap_cells(coord p1, coord p2);
    std::queue<coord> *remplissage_file(struct cc *c, enum direction dir);

public:
    Grille();
    Grille(unsigned short width, unsigned short height);
    Grille(const Grille &grille);
    bool isInCc(struct cc *c, unsigned short x, unsigned short y);      //test whether a cell is in a particular cc
    struct cc *getCc(unsigned short x, unsigned short y);
    struct cc *doUnion(struct cc *c1, struct cc *c2);
    bool decoupeConnex(struct cc *c);                                   //renvoie si la cc était connexe au départ au non
    void newCc(unsigned short x, unsigned short y, enum couleur couleur);

    std::pair<coord, coord> min_max_cc(struct cc *c);

    bool is_in_grid(coord const &coord);

    //(m_width, m_height)
    coord getDimGrid();

    /*
    Fait le déplacement d'un bloc.
    Renvoie une structure contenant:
    - un code de retour 
        -> RET_OK : le bloc est bougé
        -> RET_NOTHING : aucun candidat potentiel
        -> RET_SPLITTING : on coupe forcément un bloc trop gros
    - En cas de RET_OK, un pointeur vers la nouvelle grille (on devra libérer l'une des grilles)
    - En cas de RET_OK, une grille allouée contenant des coordonnées contenant les nouvelles coord de chaque point par rapport à la première grille
        -> devrait être libéré après
    */
    struct retMoveBlock moveBlock(unsigned short x, unsigned short y, enum direction direction); //gros wrapper pour bouger un bloc.
    ~Grille();
};


struct retMoveBlock {

    enum retType {
        RET_OK,
        RET_NOTHING,
        RET_SPLITTING
    } rettype;

    Grille *new_grille;
    //permet d'avoir la trace des déplacements effectués
    std::map<coord,coord> *deplacements;
};

void printGrid(Grille &grille);