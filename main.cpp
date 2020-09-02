#include "animation.hpp"

#include "math.h"
#include <sstream>

Rectangle hintRectangle = {10, OFFSET_SCORE - SIZE_BLOCK - 10, 2*SIZE_BLOCK, SIZE_BLOCK};
Rectangle undoRectangle = {W_WIDTH - 2*SIZE_BLOCK - 15, OFFSET_SCORE - SIZE_BLOCK - 10, 2.1*SIZE_BLOCK, SIZE_BLOCK};
Rectangle restartRectangle = {W_WIDTH / 2 - 80, OFFSET_SCORE / 2 - 30, 2.9*SIZE_BLOCK, 0.7*SIZE_BLOCK};

bool play_game() {

    coord hint(0,0);
    struct retCheckGO retGO;
    retGO.retcode = retCheckGO::RET_STUCK;
    Grille *grille;
    do {
        grille = gen_new_grille(DEFAULT_WIDTH_GRID, DEFAULT_HEIGHT_GRID);
        retGO = checkIfGameOver(grille);
        hint = retGO.hint;
    } while(retGO.retcode == retCheckGO::RET_STUCK);
    //printGrid(*grille);

    std::cout << "new game\n";

    unsigned short x, y, direction;

    bool isCellSelected = false;
    coord selectedCell(0,0);
    Vector2 mousePosition;
    
    
    bool shouldDisplayHint = false;
    unsigned int score = grille->get_score();
    bool GameOver = false;
    bool restart = false;
    bool isMoving = false;
    AnimationManager *animation;
    struct retMoveBlock ret;

    while(!WindowShouldClose() && !restart) {

        //GetMousePosition donne bien la position par rapport à la fenêtre !

        if(!isMoving && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
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
                if(!isHistoryEmpty()) {
                    //delete grille;
                    grille = depile_grille();
                    retGO = checkIfGameOver(grille);
                    //normalement, pas besoin, mais bon...
                    if(retGO.retcode == retCheckGO::RET_OK) {
                        hint = retGO.hint;
                    }
                }
            }

            else if(CheckCollisionPointRec(mousePosition, hintRectangle)) {
                shouldDisplayHint = true;
            }

            else if(CheckCollisionPointRec(mousePosition, restartRectangle)) {
                //TODO : clean ?
                /* std::cout << "mouse button pressed\n";
                while(IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                    std::cout << IsMouseButtonDown(MOUSE_LEFT_BUTTON) << IsMouseButtonPressed(MOUSE_LEFT_BUTTON) << IsMouseButtonUp(MOUSE_LEFT_BUTTON) << IsMouseButtonReleased(MOUSE_LEFT_BUTTON) << std::endl;
                }
                std::cout << "returning restart\n"; */
                restart = true;
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

        if(!isMoving && !GameOver && isCellSelected && mustMove && grille->is_in_grid(selectedCell)) {

            ret = grille->moveBlock(selectedCell.first, selectedCell.second, dir);

            if(ret.rettype == retMoveBlock::RET_OK) {
                empile_grille(grille);
                animation = new AnimationManager(ret.deplacements, 0.3, {0, OFFSET_SCORE, W_WIDTH - OFFSET_SCORE, W_HEIGHT}, SIZE_BLOCK, grille);
                
                delete ret.deplacements;
                isMoving = true;
                //grille = new Grille(*(ret.new_grille));
            }

            isCellSelected = false;
            shouldDisplayHint = false;

        }

        if(isMoving) {
            if(animation->hasFinished()) {
                isMoving = false;
                delete animation;
                grille = new Grille(*(ret.new_grille));
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
                    GameOver = true;
                }

            } else {
                animation->updatePosition(GetFrameTime());
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
                //if isMoving => animation->isMoving(i,j)... 
                if(!isMoving || !animation->isMoving(coord(i,j))) DrawRectangle(SIZE_BLOCK * i,OFFSET_SCORE + SIZE_BLOCK * j , SIZE_BLOCK, SIZE_BLOCK, getCouleur(grille->getCc(i,j)->couleur));
            }
        }

        DrawText("Score : ", 30, 20, 30, BLACK);
        DrawText(score_text.data(), 150, 20, 30, BLACK);

        DrawRectangleRoundedLines(restartRectangle, 0.3, 4, 5, BLACK);
        DrawText("Restart", W_WIDTH/2 - 65, OFFSET_SCORE/2 - SIZE_BLOCK/2 - 2, 30, BLACK);

        if(isMoving) {
            animation->drawCells();
        }

        if(!GameOver) {

            if(shouldDisplayHint) {
                DrawRectangleLinesEx(rec_around_hint, 5, GRAY);
            }

            if(isCellSelected) {
                DrawRectangleLinesEx(rec_around_cell, 5, BLACK);
            }

            
            //Draw Hint Button
            DrawRectangleRoundedLines(hintRectangle, 0.3, 4, 5, BLACK);
            DrawText("Hint", 20, OFFSET_SCORE - SIZE_BLOCK - 5, 40, BLACK);

            DrawRectangleRoundedLines(undoRectangle, 0.3, 4, 5, BLACK);
            DrawText("Undo", W_WIDTH - 2*SIZE_BLOCK - 10, OFFSET_SCORE - SIZE_BLOCK - 5, 40, BLACK);

        } else {

            DrawText("Game Over !", (W_WIDTH / 2) - 90, OFFSET_SCORE - SIZE_BLOCK, 30, BLACK);

        }

        EndDrawing();

        //ici, on pourrait attendre qu'une touche soit pressée ?

    }

    clean_history();

    return restart;
}

int main() {
    srand(time(NULL));

    InitWindow(W_WIDTH, W_HEIGHT, "Niaki");

    SetTargetFPS(60);

    /* while(play_game()) {
        std::cerr << "restart\n";
    } */

    while(play_game());
    
    CloseWindow();
    return 0;
}