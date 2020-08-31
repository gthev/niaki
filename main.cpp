#include "drive_grille.hpp"
#include "raylib.h"

#include "math.h"
#include <sstream>

#define WIDTH_BLOCKS    6
#define HEIGHT_BLOCKS   8

#define OFFSET_SCORE    150

#define SIZE_BLOCK  50
#define W_WIDTH     ((WIDTH_BLOCKS) * (SIZE_BLOCK))         //20 * 6
#define W_HEIGHT    ((HEIGHT_BLOCKS) * (SIZE_BLOCK) + (OFFSET_SCORE))         //20 * 8

Rectangle hintRectangle = {10, OFFSET_SCORE - SIZE_BLOCK - 10, 2*SIZE_BLOCK, SIZE_BLOCK};
Rectangle undoRectangle = {W_WIDTH - 2*SIZE_BLOCK - 15, OFFSET_SCORE - SIZE_BLOCK - 10, 2.1*SIZE_BLOCK, SIZE_BLOCK};

std::stack<Grille*> history;

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
        return PURPLE;

    case COLOR_GREEN:
        return GREEN;

    case COLOR_BLUE:
        return BLUE;
    
    default:
        break;
    }
}

int main() {
    srand(time(NULL));

    Grille *grille = gen_new_grille(DEFAULT_WIDTH_GRID, DEFAULT_HEIGHT_GRID);

    //printGrid(*grille);

    unsigned short x, y, direction;
    struct retMoveBlock ret;

    /* while(1) {

        std::cout << "x: ";
        std::cin >> x;
        std::cout << "y: ";
        std::cin >> y;
        std::cout << "dir (0: up, 1: left, 2: down, 3: right): ";
        std::cin >> direction;

        ret = grille->moveBlock(x, y, (enum direction)direction);

        std::cout << "retour : " << ret.rettype << std::endl;

        if(ret.rettype == retMoveBlock::RET_OK) {
            printGrid(*ret.new_grille);
            *grille = *ret.new_grille;
        }

    } */

     InitWindow(W_WIDTH, W_HEIGHT, "Niaki");

    SetTargetFPS(60);

    bool isCellSelected = false;
    coord selectedCell(0,0);
    Vector2 mousePosition;
    coord hint(0,0);
    struct retCheckGO retGO;
    bool shouldDisplayHint = false;
    unsigned int score = grille->get_score();

    while(!WindowShouldClose()) {

        //GetMousePosition donne bien la position par rapport à la fenêtre !

        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            mousePosition = GetMousePosition();
            coord new_selectedCell;
            float x_mouse = mousePosition.x, y_mouse = mousePosition.y;
            if(y_mouse >= OFFSET_SCORE) {

                new_selectedCell.first = (int)floor(mousePosition.x) / SIZE_BLOCK;
                new_selectedCell.second = ((int)floor(mousePosition.y) - OFFSET_SCORE) / SIZE_BLOCK;

                //std::cerr << "new position : " << new_selectedCell.first << " " << new_selectedCell.second << " ; " << isCellSelected << std::endl;

                if(isCellSelected && new_selectedCell == selectedCell) {
                    isCellSelected = false;
                }
                else if(isCellSelected) {
                    selectedCell = new_selectedCell;
                } else {
                    selectedCell = new_selectedCell;
                    isCellSelected = true;
                }
            }

            else if(CheckCollisionPointRec(mousePosition, undoRectangle)) {
                if(!history.empty()) {
                    //delete grille;
                    grille = history.top();
                    history.pop();
                }
            }

            else if(CheckCollisionPointRec(mousePosition, hintRectangle)) {
                shouldDisplayHint = true;
            }
        }

        bool mustMove = false;
        enum direction dir;

        if(IsKeyPressed(KEY_UP)) {
            dir = DIR_UP;
            mustMove = true;
        }
        if(IsKeyPressed(KEY_DOWN)) {
            dir = DIR_DOWN;
            mustMove = true;
        }
        if(IsKeyPressed(KEY_LEFT)) {
            dir = DIR_LEFT;
            mustMove = true;
        }
        if(IsKeyPressed(KEY_RIGHT)) {
            dir = DIR_RIGHT;
            mustMove = true;
        }

        if(isCellSelected && mustMove) {
            struct retMoveBlock ret;

            ret = grille->moveBlock(selectedCell.first, selectedCell.second, dir);

            if(ret.rettype == retMoveBlock::RET_OK) {
                history.push(grille);
                grille = new Grille(*(ret.new_grille));
            }

            isCellSelected = false;
            shouldDisplayHint = false;

            score = grille->get_score();

            //test nouvelles possibilitées
            retGO = checkIfGameOver(grille);
            if(retGO.retcode == retCheckGO::RET_OK) {
                hint = retGO.hint;
            } else {
                //Game over...
                //TODO : faire un truc mieux
                if(score >= 100) {
                    std::cout << "Victoire !\n";
                } else {
                    std::cout << "Game over, score = " << score << std::endl;
                }
                exit(0);
            }
        }


        //après moveBlock : isCellSelected = false

        std::ostringstream sscore;

        sscore << score << "%";

        std::string score_text(sscore.str());

        //Si une case est sélectionnée, on l'affiche
        Rectangle rec_around_cell = {(float)selectedCell.first * SIZE_BLOCK, (float)selectedCell.second * SIZE_BLOCK + OFFSET_SCORE, SIZE_BLOCK, SIZE_BLOCK};

        Rectangle rec_around_hint = {(float)hint.first * SIZE_BLOCK, (float)hint.second * SIZE_BLOCK + OFFSET_SCORE, SIZE_BLOCK, SIZE_BLOCK};

        BeginDrawing();

        ClearBackground(RAYWHITE);

        //tracer la grille
        for(unsigned short i = 0; i<WIDTH_BLOCKS; i++) {
            for(unsigned short j = 0; j<HEIGHT_BLOCKS; j++) {
                DrawRectangle(SIZE_BLOCK * i,OFFSET_SCORE + SIZE_BLOCK * j , SIZE_BLOCK, SIZE_BLOCK, getCouleur(grille->getCc(i,j)->couleur));
            }
        }

        if(shouldDisplayHint) {
            DrawRectangleLinesEx(rec_around_hint, 5, GRAY);
        }

        if(isCellSelected) {
            DrawRectangleLinesEx(rec_around_cell, 5, BLACK);
        }

        DrawText("Score : ", 30, 20, 30, BLACK);
        DrawText(score_text.data(), 150, 20, 30, BLACK);
        //Draw Hint Button
        DrawRectangleRoundedLines(hintRectangle, 0.3, 4, 5, BLACK);
        DrawText("Hint", 20, OFFSET_SCORE - SIZE_BLOCK - 5, 40, BLACK);

        DrawRectangleRoundedLines(undoRectangle, 0.3, 4, 5, BLACK);
        DrawText("Undo", W_WIDTH - 2*SIZE_BLOCK - 10, OFFSET_SCORE - SIZE_BLOCK - 5, 40, BLACK);

        EndDrawing();

        //ici, on pourrait attendre qu'une touche soit pressée ?

    }
    return 0;
}