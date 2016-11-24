#include "Objet.h"

Objet::Objet(SDL_Renderer* renderer) {
    this->renderer = renderer;

    x = 0;
    y = 0;
    width = 1;
    height = 1;
}

Objet::~Objet() {
}

void Objet::Update(float delta) {
}

void Objet::Render(float delta) {
}

bool Objet::Collisions(Objet* other) {
    if (x + width > other->x && x < other->x + other->width &&
        y + height > other->y && y < other->y + other->height) {
        return true;
    }
    return false;
}

