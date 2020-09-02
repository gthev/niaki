#include "history.hpp"

std::stack<Grille*> history;

bool isHistoryEmpty() {
    return history.empty();
}

void clean_history() {
    Grille *current;
    while(!history.empty()) {
        current = history.top();
        history.pop();
        delete current;
    }
}

void empile_grille(Grille *grille) {
    history.push(grille);
}

Grille *depile_grille() {
    Grille *grille = history.top();
    history.pop();

    return grille;
}