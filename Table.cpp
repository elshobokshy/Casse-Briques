#include "Table.h"

Table::Table(SDL_Renderer* renderer): Objet(renderer) {
    SDL_Surface* surface = IMG_Load("briques.png");
    briquetexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    srand(time(0));

    x = 16;
    y = 0;
    width = 1200;
    height = 600;

    briquesoffsetx = 0;
    briquesoffsety = 16;
}

Table::~Table() {
    SDL_DestroyTexture(briquetexture);
}

void Table::Update(float delta) {

}

void Table::Render(float delta) {
    // generer les briques
    for (int i=0; i<BOARD_WIDTH; i++) {
        for (int j=0; j<BOARD_HEIGHT; j++) {
            Briques briques = brique[i][j];

            // si la brique existe
            if (!briques.state)
                continue;

            SDL_Rect srcrect;

            srcrect.x = (briques.type % 2) * BOARD_BRWIDTH;
            srcrect.y = (briques.type / 2) * BOARD_BRHEIGHT;
            srcrect.w = BOARD_BRWIDTH;
            srcrect.h = BOARD_BRHEIGHT;
            // pour les cotés...
            SDL_Rect dstrect;
            dstrect.x = briquesoffsetx + x + i * BOARD_BRWIDTH;
            dstrect.y = briquesoffsety + y + j * BOARD_BRHEIGHT;
            dstrect.w = BOARD_BRWIDTH;
            dstrect.h = BOARD_BRHEIGHT;

            SDL_RenderCopy(renderer, briquetexture, &srcrect, &dstrect);
        }
    }

}

void Table::CreateLevel() {
    for (int i=0; i<BOARD_WIDTH; i++) {
        for (int j=0; j<BOARD_HEIGHT; j++) {
            Briques briques;

            briques.type = rand() % 6;    // couleur aléatoire
            briques.state = true;         // brique existe
            brique[i][j] = briques;
        }
    }
}
