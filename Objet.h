#ifndef ENTITY_H_
#define ENTITY_H_

#include <SDL.h>
#include <SDL_image.h>

class Objet {
public:
    Objet(SDL_Renderer* renderer);
    virtual ~Objet();

    float x, y, width, height;

    virtual void Update(float delta);
    virtual void Render(float delta);

    bool Collisions(Objet* other);
protected:
    SDL_Renderer* renderer;

};

#endif
