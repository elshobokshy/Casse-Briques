#include "Raquette.h"

Raquette::Raquette(SDL_Renderer* renderer): Objet(renderer) {
    SDL_Surface* surface = IMG_Load("raquette.png");
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    width = 128;
    height = 12;

    y = 565;
}

Raquette::~Raquette() {
    SDL_DestroyTexture(texture);
}


void Raquette::Update(float delta) {

}

void Raquette::Render(float delta) {
    SDL_Rect rect;
    rect.x = (int)(x + 0.5f);
    rect.y = (int)(y + 0.5f);
    rect.w = width;
    rect.h = height;
    SDL_RenderCopy(renderer, texture, 0, &rect);
}
