#include "history.hpp"

#define EPSILON 4.f

typedef std::pair<float, float> position;
typedef Rectangle box;

struct cell
{
    coord pos_orig;
    position depart;
    position arrive;
    position current;
    position delta; //contains the speed of the moving cell
    bool hasArrived;
};

class AnimationManager
{
private:

    std::vector<struct cell>   m_cells;
    Grille                    *m_grid;
    box                        m_drawingZone;
    float                      m_size_block;

    position                   coordToOnScreenPosition(coord coordonnees);

public:
    AnimationManager(std::map<coord, coord> *deplacements, float time_per_move, box drawing_zone, float size_block, Grille *grille);
    ~AnimationManager();
    void updatePosition(float time_elapsed);
    void drawCells();
    bool hasFinished();
    bool isMoving(coord position);
};


