#ifndef PADDLE_H_
#define PADDLE_H_

#include "Objet.h"

class Raquette: public Objet {
public:
    Raquette(SDL_Renderer* renderer);
    ~Raquette();

    void Update(float delta);
    void Render(float delta);

private:
    SDL_Texture* texture;
};

#endif
