#ifndef BOARD_H_
#define BOARD_H_

#include "Objet.h"
#include <stdlib.h>
#include <time.h>

// dimmensions de la table et des briques
#define BOARD_WIDTH 18
#define BOARD_HEIGHT 12
#define BOARD_BRWIDTH 64
#define BOARD_BRHEIGHT 24

class Briques {
public:
    int type;
    bool state;
};

class Table: public Objet {
public:
    Table(SDL_Renderer* renderer);
    ~Table();

    void Update(float delta);
    void Render(float delta);
    void CreateLevel();

    float briquesoffsetx, briquesoffsety;

    // tab a 2 dimensions du tableau
    Briques brique[BOARD_WIDTH][BOARD_HEIGHT];

private:
    SDL_Texture* briquetexture;
    SDL_Texture* sidetexture;
};

#endif
