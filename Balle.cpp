#include "Balle.h"

Balle::Balle(SDL_Renderer* renderer): Objet(renderer) {
    SDL_Surface* surface = IMG_Load("balle.png");
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    x = 0;
    y = 0;
    width = 24;
    height = 24;

    SetDirection(1, 1);
}

Balle::~Balle() {
    SDL_DestroyTexture(texture);
}

void Balle::Update(float delta) {
    x += dirx * delta;
    y += diry * delta;
}

void Balle::Render(float delta) {
    SDL_Rect rect;
    rect.x = (int)(x + 0.5f);
    rect.y = (int)(y + 0.5f);
    rect.w = width;
    rect.h = height;
    SDL_RenderCopy(renderer, texture, 0, &rect);
}

void Balle::SetDirection(float dirx, float diry) {
    // Direction de la ballee et la vitesse avec BALL_SPEED
    float length = sqrt(dirx * dirx + diry * diry);
    this->dirx = BALL_SPEED * (dirx / length);
    this->diry = BALL_SPEED * (diry / length);
}

