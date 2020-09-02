#include "grille.hpp"

void display_elements(std::set<coord> &elem) {
    std::set<coord>::iterator it = elem.begin();
    while(it != elem.end()) {
        std::cout << "(" << it->first << "," << it->second << ") ";
        it++;
    }

    std::cout << std::endl;
}

std::pair<short, short> dir_to_delta(enum direction dir) {
    switch (dir)
    {
    case DIR_UP:
        return coord(0,-1);

    case DIR_LEFT:
        return coord(-1,0);

    case DIR_DOWN:
        return coord(0,1);

    case DIR_RIGHT:
        return coord(1,0);
    
    default:
        return coord(0,1); //....
    }
}

void Grille::addElement(struct cc *c, unsigned short x, unsigned short y) {
    if(isInCc(c, x, y)) 
        return;
    c->size++;
    c->elements.insert(coord(x,y));
    m_grid[COORD(x,y,m_height)] = c;
}

void Grille::remElement(struct cc *c, unsigned short x, unsigned short y) {
    if(c->size <= 0) return;
    if(!isInCc(c, x, y)) return;

    c->size--;
    c->elements.erase(coord(x,y));
    m_grid[COORD(x,y,m_height)] = nullptr;
}

struct cc* Grille::newCc(enum couleur couleur) {
    struct cc *c = new struct cc;
    c->size = 0;
    c->couleur = couleur;
    m_list_cc.insert(c);
    return c;
}

void Grille::swap_cells(coord p1, coord p2) {

    if(!is_in_grid(p1) || !is_in_grid(p2)) return;

    unsigned short x1 = p1.first, y1 = p1.second, x2 = p2.first, y2 = p2.second;

    struct cc *c1 = getCc(x1, y1);
    struct cc *c2 = getCc(x2, y2);

    remElement(c1, x1, y1);
    remElement(c2, x2, y2);
    addElement(c1, x2, y2);
    addElement(c2, x1, y1);
}

bool Grille::isInCc(struct cc *c, unsigned short x, unsigned short y) {
    if(x < 0 || x >= m_width || y < 0 || y>=m_height) return false;
    bool ret = (m_grid[COORD(x,y,m_height)] == c);
    return ret;
}

void Grille::newCc(unsigned short x, unsigned short y, enum couleur couleur) {
    struct cc *c = newCc(couleur);
    addElement(c, x, y);
}

Grille::Grille() 
: m_height(DEFAULT_HEIGHT_GRID), m_width(DEFAULT_WIDTH_GRID)
{
    m_grid = new struct cc*[m_height * m_width];

    for(int i=0; i<m_width; i++) {
        for(int j=0; j<m_height; j++) {
            m_grid[COORD(i,j,m_height)] = nullptr;
        }
    }
}

Grille::Grille(unsigned short x, unsigned short y) 
: m_width(x), m_height(y)
{
    if(x < 0 || y < 0 || x > MAX_WIDTH || y > MAX_HEIGHT) {
        std::cout << "Constructeur grille (x,y) : valeurs bizarres (x,y) : " << x << " " << y << std::endl;
        exit(1);
    }
    m_grid = new struct cc*[m_height * m_width];

    for(int i=0; i<m_width; i++) {
        for(int j=0; j<m_height; j++) {
            m_grid[COORD(i,j,m_height)] = nullptr;
        }
    }

}

Grille::Grille(const Grille &grille) 
: m_height(grille.m_height), m_width(grille.m_width)
{
    std::set<struct cc*> deja_vu;
    std::map<struct cc*, struct cc*> correspondances;
    m_grid = new struct cc*[m_height * m_width];
    //on parcourt les cases de l'ancienne case
    for(unsigned short i=0; i<m_width; i++) {
        for(unsigned short j=0; j<m_height; j++) {
            //si c'est une nouvelle...
            //std::cout << "doing " << i << " " << j << std::endl; 

            if(deja_vu.find(grille.m_grid[COORD(i,j,m_height)]) == deja_vu.end()) {

                newCc(i,j, grille.m_grid[COORD(i,j,grille.m_height)]->couleur);

                deja_vu.insert(grille.m_grid[COORD(i,j,m_height)]);
                correspondances.insert(std::pair<struct cc*, struct cc*>(grille.m_grid[COORD(i,j,m_height)], m_grid[COORD(i,j,m_height)]));

            } 
            //sinon
            else {
                addElement(correspondances[grille.m_grid[COORD(i,j,m_height)]], i, j);
            }
        }
    }
}

struct cc *Grille::getCc(unsigned short x, unsigned short y) {
    if(x < 0 || x >= m_width || y < 0 || y >= m_height) {
        std::cout << "Probleme de getCc: coordonnées bizarres (x,y) " << x << " " << y << std::endl;
        return nullptr;
    }
    return m_grid[COORD(x,y,m_height)];
}

//fait une union simple. isConnex devrait être appelé juste après
struct cc *Grille::doUnion(struct cc *c1, struct cc *c2) {

    //std::cout << "union between " << c1 << " and " << c2 << std::endl;

    //on parcourt les éléments de la seconde cc, et on les rajoute à la première cc
    for(auto &it : c2->elements) {

        //std::cout << "adding " << it.first << " " << it.second << std::endl;

        unsigned short x = it.first;
        unsigned short y = it.second;

        addElement(c1, x, y);
    }

    if(c1 != c2) {
        m_list_cc.erase(c2);
        delete c2;
    }

    return c1;
}

bool Grille::decoupeConnex(struct cc *c) {
    //on copie les éléments de la cc
    std::set<std::pair<unsigned short, unsigned short>> a_visiter = c->elements;
    std::stack<std::pair<unsigned short, unsigned short>> todo;

    if(a_visiter.empty()) return true;

    std::pair<unsigned short, unsigned short> current;
    //on enlève le premier élément
    current = *a_visiter.begin();
    a_visiter.erase(current);
    todo.push(current);

    while(!todo.empty()) {
        current = todo.top();
        todo.pop();

        unsigned short x = current.first, y = current.second;

        std::vector<std::pair<unsigned short, unsigned short>> candidats = {
            std::pair<unsigned short, unsigned short>(x+1,y),
            std::pair<unsigned short, unsigned short>(x-1,y),
            std::pair<unsigned short, unsigned short>(x,y+1),
            std::pair<unsigned short, unsigned short>(x,y-1),
        };

        //dans chaque direction
        for(auto &candidat : candidats) {
            if(a_visiter.erase(candidat) > 0) todo.push(candidat);
        }
    }

    //return a_visiter.empty();
    //Si c'est vide : on retourne true; sinon, on crée une nvlle cc pour ceux qui restent, et on fait un appel récursif
    if(a_visiter.empty()) {
        return true;
    } else {
        
        struct cc *new_cc = newCc(c->couleur);

        for(auto &restant: a_visiter) {
            remElement(c, restant.first, restant.second);
            addElement(new_cc, restant.first, restant.second);
        }

        decoupeConnex(new_cc);

        return false;
    }
}


std::pair<coord,coord> Grille::min_max_cc(struct cc *c) {
    unsigned short min_x = m_width, max_x = 0, min_y = m_height, max_y = 0;

    for(auto &elem : c->elements) {
        unsigned short x = elem.first, y = elem.second;

        if(x < min_x) min_x = x;
        if(x > max_x) max_x = x;
        if(y < min_y) min_y = y;
        if(y > max_y) max_y = y;
    }

    //on renvoie ((min_x, min_y), (max_x, max_y))
    return std::pair<coord, coord> (coord (min_x, min_y), coord (max_x, max_y));
}

bool Grille::is_in_grid(coord const &coord) {
    return (coord.first >=0 && coord.first < m_width && coord.second >= 0 && coord.second < m_height);
}

std::queue<coord> *Grille::remplissage_file(struct cc *c, enum direction dir) {
    std::queue<coord> *todo_file = new std::queue<coord>;
    std::pair<coord, coord> boite = min_max_cc(c);

    switch (dir)
    {
    case DIR_UP:
        for(short x = boite.first.first; x <= boite.second.first; x++) {
            for(short y = boite.first.second; y <=boite.second.second; y++) {
                if(isInCc(c, x, y)) todo_file->push(coord(x,y));
            }
        }
        break;

    case DIR_DOWN:
        for(short x = boite.first.first; x <= boite.second.first; x++) {
            for(short y = boite.second.second; y >=boite.first.second; y--) {
                if(isInCc(c, x, y)) todo_file->push(coord(x,y));
            }
        }
        break;
    
    case DIR_LEFT:
        for(short y = boite.first.second; y <= boite.second.second; y++) {
            for(short x = boite.first.first; x <=boite.second.first; x++) {
                if(isInCc(c, x, y)) todo_file->push(coord(x,y));
            }
        }
        break;

    default:
        for(short y = boite.first.second; y <= boite.second.second; y++) {
            for(short x = boite.second.first; x >=boite.first.first; x--) {
                if(isInCc(c, x, y)) todo_file->push(coord(x,y));
            }
        }
        break;
    }


    return todo_file;
}



struct retMoveBlock Grille::moveBlock(unsigned short x, unsigned short y, enum direction direction) {

    //std::cout << "here we are\n";

    struct retMoveBlock ret;

    //ret.deplacements = new std::map<coord,coord>;

    //1ère étape : trouver les prochaines positions possibles
    //std::vector<coord> candidats;
    //std::vector<coord> candidats_place;
    coord current_cell(x,y);
    bool isOk = false;
    coord but;
    struct cc *c_orig = getCc(x,y);

    coord delta = dir_to_delta(direction);

    current_cell.first += delta.first;
    current_cell.second += delta.second;

    if(!is_in_grid(current_cell)) {
        ret.rettype = retMoveBlock::RET_NOTHING;
        ret.deplacements = nullptr;
        ret.new_grille = nullptr;
        return ret;
    }

    struct cc *c_neighbour = getCc(current_cell.first, current_cell.second);

    //todo changer tout ça : on ne doit sauter qu'une CC !
    while(is_in_grid(current_cell) && isInCc(c_neighbour, current_cell.first, current_cell.second)) {
        unsigned short x = current_cell.first, y = current_cell.second ;

        //std::cout << "current_cell (debug while) : "<< x << " " << y << std::endl;

        std::vector<coord> voisins = {
            coord(x+1,y),
            coord(x-1,y),
            coord(x,y+1),
            coord(x,y-1),
        };

        for(auto &voisin: voisins) {
            //Si c'est un candidat potentiel...
            if(is_in_grid(voisin) && getCc(voisin.first, voisin.second)->couleur == c_orig->couleur && !isInCc(c_orig, voisin.first, voisin.second)) {
                //s'il n'y est pas déjà...
                /* if(std::find(candidats.begin(), candidats.end(), voisin) == candidats.end()) {
                    candidats.push_back(voisin);
                    if(std::find(candidats_place.begin(), candidats_place.end(), current_cell) == candidats_place.end()) candidats_place.push_back(current_cell);
            
                } */

                but = current_cell;
                current_cell = voisin;
                isOk = true;
                break;
            }
        }

        if(!isOk) {
            current_cell.first += delta.first;
            current_cell.second += delta.second;
        }
    }

    //debug
    /* std::cout << "candidats ok ";
    for(auto &voisin: candidats) {
        std::cout << "( " << voisin.first << "," << voisin.second << ") "; 
    }
    std::cout << std::endl;
    for(auto &voisin: candidats_place) {
        std::cout << "( " << voisin.first << "," << voisin.second << ") "; 
    }
    std::cout << std::endl; */
    //fin debug

    /* if(candidats.empty()) { */
    if(!isOk) {
        ret.rettype = retMoveBlock::RET_NOTHING;
        ret.new_grille = nullptr;
        ret.deplacements = nullptr;
        return ret;
    }

    
    Grille *grid_copy;

    //std::cout << "current_cell : " << current_cell.first << " " << current_cell.second << std::endl;

    //ret.deplacements->clear();

    /* current_cell = candidats.back();
        candidats.pop_back();

        but = candidats_place.back();
        candidats_place.pop_back(); */

    //Deuxième étape : copier la grille, et faire le déplacement du bloc

    grid_copy = new Grille(*this);

    struct cc *c = grid_copy->getCc(x, y);

    //std::cout << "ok new grille\n";
    //printGrid(*grid_copy);

    //std::cout << "Calling remplissage cc of size " << c->size << " ## " << c->elements.size() << std::endl;

    std::queue<coord> *todo_file = grid_copy->remplissage_file(c, direction);

    //std::cout << "ok file, nr elements : " << todo_file->size() << std::endl;

    //bien penser à libérer après remplissage de la file !

    //nombres de swaps à faire
    unsigned short n = (direction == DIR_UP || direction == DIR_DOWN) ? abs(but.second - y) : abs(but.first - x);

    std::set<coord> modified_cells;

    Permutation<coord> permut_cases;

    coord cell;
    while (!todo_file->empty())
    {
        cell = todo_file->front();
        todo_file->pop();

        //std::cout << "moving cell (" << cell.first << "," << cell.second << ")\n";

        coord cell_orig = cell;

        modified_cells.insert(cell);

        //faire n swaps et mettre à jour la cellule à chaque fois
        for (unsigned short i = 0; i < n; i++)
        {
            coord next_cell = coord(cell.first + delta.first, cell.second + delta.second);

            modified_cells.insert(next_cell);

            //std::cout << "swaping (" << cell.first << "," << cell.second << ") and (" << next_cell.first << "," << next_cell.second << ")\n";

            grid_copy->swap_cells(cell, next_cell);

            //TODO : pas bon, on devrait considérer des PERMUTATIONS ici
            /* (*ret.deplacements)[cell] = next_cell;
            (*ret.deplacements)[next_cell] = cell; */
            permut_cases.applyTransposition(cell, next_cell);

            //display_elements(grid_copy->getCc(cell.first, cell.second)->elements);
            //display_elements(grid_copy->getCc(next_cell.first, next_cell.second)->elements);

            cell = next_cell;
        }
    }

    ret.deplacements = new std::map<coord,coord>(permut_cases.getFunction());

    //std::cout << "ok swap\n";

    //on fait l'union des deux cc
    struct cc *new_c = grid_copy->doUnion(c, grid_copy->getCc(current_cell.first, current_cell.second));

    //std::cout << "ok dounion\n";

    int new_size = new_c->size;

    //std::cout << "new_size : " << new_size << std::endl;

    //Troisième étape : faire des tests pour voir si on ne casse pas des cc trop grosses

    isOk = true;

    for (auto &current_cc : grid_copy->m_list_cc)
    {

        int old_size = current_cc->size;

        if (!grid_copy->decoupeConnex(current_cc) && old_size >= new_size)
        {
            //pas bon...
            ret.rettype = retMoveBlock::RET_SPLITTING;
            ret.new_grille = nullptr;
            ret.deplacements = nullptr;
            isOk = false;
            break;
        }
    }

    if (isOk)
    {
        //tout va bien !

        //Il faut parcourir toutes les cc pour les étendre le + possible
        //ou alors, seulement faire les tests pour celles qui ont été modifiées
        //TODO : l'optimisation susnommée
        std::set<coord>::iterator it = modified_cells.begin();

        while (it != modified_cells.end())
        {

            unsigned short x = it->first, y = it->second;

            // std::cout << "testing x " << x << " y " << y <<" of color " << grid_copy->getCc(x,y)->couleur << std::endl;

            std::vector<coord> voisins = {
                coord(x + 1, y),
                coord(x - 1, y),
                coord(x, y + 1),
                coord(x, y - 1),
            };

            //printGrid(*grid_copy); std::cout << std::endl;

            for (auto &voisin : voisins)
            {

                unsigned short x_voisin = voisin.first, y_voisin = voisin.second;

                //std::cout << "looking for " << x_voisin << " " << y_voisin << "...\n";

                //si elle est à côté d'une case de la même couleur...
                if (grid_copy->is_in_grid(voisin) && grid_copy->getCc(x, y)->couleur == grid_copy->getCc(x_voisin, y_voisin)->couleur)
                {

                    //std::cout << "reu trouvée ! " << x_voisin << " " << y_voisin << std::endl;

                    grid_copy->doUnion(grid_copy->getCc(x_voisin, y_voisin), grid_copy->getCc(x, y));
                }

                //printGrid(*grid_copy); std::cout << std::endl;
            }

            it++;
        }

        ret.rettype = retMoveBlock::RET_OK;
        ret.new_grille = grid_copy;
    }
    else
    {
        delete grid_copy;
    }

    delete todo_file;

    return  ret;
}

coord Grille::getDimGrid() {
    return coord(m_width, m_height);
}


Grille::~Grille() 
{
    std::set<struct cc*> deja_vu;
    for(unsigned short i = 0; i<m_width; i++) {
        for(unsigned short j=0; j<m_height; j++) {
            struct cc* c = m_grid[COORD(i,j,m_height)];
            if(deja_vu.find(c) != deja_vu.end()) {
                delete c;
                deja_vu.insert(c);
            }
        }
    }

    delete[] m_grid;
}

void printGrid(Grille &grille) {
    coord dim_grid = grille.getDimGrid();
    for(int j=0; j<dim_grid.second; j++) {
        for(int i=0; i<dim_grid.first; i++) {
            std::cout << grille.getCc(i,j)->couleur << " ";
        }

        //debug
        /* std::cout << "\t";
        for(int i=0; i<dim_grid.first; i++) {
            std::cout << grille.getCc(i,j) << " ";
        } */
        std::cout << std::endl;
    }
}

unsigned short Grille::get_score() {
    unsigned short score_max = m_height * m_width - NUMBER_COLOR;

    std::vector<unsigned short> comptes(NUMBER_COLOR);
    std::vector<unsigned short> nr_cc(NUMBER_COLOR);

    //std::cerr << "getscore...\n";

    for(int i=0; i<NUMBER_COLOR; i++) {
        comptes[i] = 0;
        nr_cc[i] = 0;
    }

    for(auto &cc: m_list_cc) {
        int couleur = cc->couleur;
        comptes[couleur] += cc->size;
        nr_cc[couleur]++;
    }

    unsigned short score = 0;

    for(int i=0; i<NUMBER_COLOR; i++) {
        //std::cerr << "color " << i << ": ";
        //std::cerr << comptes[i] - nr_cc[i] << "\n";
        score += comptes[i] - nr_cc[i];
    }

    return (unsigned short)(((float)(score)/((float)score_max))*100.f);
}