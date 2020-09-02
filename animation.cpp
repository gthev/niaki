#include "animation.hpp"


position AnimationManager::coordToOnScreenPosition(coord coordonnees) {
    position ret;

    ret.first = m_drawingZone.x + m_size_block * coordonnees.first;
    ret.second = m_drawingZone.y + m_size_block * coordonnees.second;

    return ret;
}

static position computeDelta(coord depart_c, coord arrive_c, position depart, position arrive, float time_per_move) {
    enum direction dir;
    if(depart_c.first == arrive_c.first) /*so x identical : up or down*/ dir = (depart_c.second > arrive_c.second)? DIR_UP : DIR_DOWN;
    else dir = (depart_c.first > arrive_c.first)? DIR_LEFT : DIR_RIGHT;

    std::pair<short, short> delta_unit = dir_to_delta(dir);
    float speed = (dir == DIR_UP || dir == DIR_DOWN)? abs(depart.second - arrive.second) : abs(depart.first - arrive.first);

    if(time_per_move <= 0) {
        std::cout  << "time_per_move bizarre : " << time_per_move << std::endl;
        exit(1);
    }

    speed /= time_per_move;

    position delta(delta_unit.first * speed, delta_unit.second * speed);


    return delta;

}

AnimationManager::AnimationManager(std::map<coord, coord> *deplacements, float time_per_move, box drawing_zone, float size_block, Grille *grille)
: m_drawingZone(drawing_zone), m_size_block(size_block), m_grid(grille)
{
    std::map<coord, coord>::iterator it;
    for(it = deplacements->begin(); it != deplacements->end(); it++) {
        struct cell new_cell;
        new_cell.pos_orig = it->first;
        new_cell.depart = coordToOnScreenPosition(it->first);
        new_cell.current = new_cell.depart;
        new_cell.arrive = coordToOnScreenPosition(it->second);
        new_cell.hasArrived = false;
        new_cell.delta = computeDelta(it->first, it->second, new_cell.depart, new_cell.arrive, time_per_move);

        m_cells.push_back(new_cell);
    }
}

AnimationManager::~AnimationManager()
{
}

void AnimationManager::updatePosition(float time_elapsed) {

    //std::cerr << "updating with " << time_elapsed << " seconds\n";

    for(auto it = m_cells.begin(); it != m_cells.end(); it++) {

        if(it->hasArrived) continue;

        //std::cerr << it->depart.first << " " << it->depart.second << " " << it->arrive.first << " " << it->arrive.second << " " << it->current.first << " " << it->current.second << "\n";

        it->current.first += time_elapsed * it->delta.first;
        it->current.second += time_elapsed * it->delta.second;

        float x_min, x_max, y_min, y_max;
        if(it->depart.first < it->current.first) {
            x_min = it->depart.first;
            x_max = it->current.first;
        } else {
            x_min = it->current.first;
            x_max = it->depart.first;
        }

        if(it->depart.second < it->current.second) {
            y_min = it->depart.second;
            y_max = it->current.second;
        } else {
            y_min = it->current.second;
            y_max = it->depart.second;
        }
        Rectangle rec_hitbox = {x_min - EPSILON, y_min - EPSILON, x_max - x_min + 2*EPSILON, y_max - y_min + 2*EPSILON};

        Vector2 pos_arr = {it->arrive.first, it->arrive.second};

        if(CheckCollisionPointRec(pos_arr, rec_hitbox)) {
            //std::cerr << "arrived !\n";
            it->hasArrived = true;
        }
    }

}

bool AnimationManager::hasFinished() {
    for(auto &it : m_cells) {
        if(!it.hasArrived) return false;
    }
    return true;
}

bool AnimationManager::isMoving(coord pos) {
    for(auto &it : m_cells) {
        if(it.pos_orig == pos) return true;
    }
    return false;
}

void AnimationManager::drawCells() {
    for(auto it = m_cells.begin(); it != m_cells.end(); it++) {
        unsigned short i = it->pos_orig.first, j = it->pos_orig.second;
        DrawRectangle(it->current.first ,it->current.second , SIZE_BLOCK, SIZE_BLOCK, getCouleur(m_grid->getCc(i,j)->couleur));
        //drawCell(it->pos_orig.first, it->pos_orig.second, m_grid);
    }
}