#ifndef BALL_H_
#define BALL_H_

#include "Objet.h"

#include <math.h>

// vitesse de la balle
const float BALL_SPEED = 900;

class Balle: public Objet {
public:
    Balle(SDL_Renderer* renderer);
    ~Balle();

    void Update(float delta);
    void Render(float delta);

    void SetDirection(float dirx, float diry);

    float dirx, diry;

private:
    SDL_Texture* texture;

};

#endif
