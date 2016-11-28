#ifndef GAME_H_
#define GAME_H_

#include "SDL.h"
#include "SDL_image.h"
#include <iostream>
#include <stdio.h>

#include "Table.h"
#include "Raquette.h"
#include "Balle.h"

#define FPS_DELAY 500

class Jeu {
public:
    Jeu();
    ~Jeu();

    bool Init();
    void Run();

private:
    SDL_Window* window;
    SDL_Renderer* renderer;

    SDL_Texture* texture;

    // timing
    unsigned int lasttick, fpstick, fps, framecount;

    // test
    float testx, testy;

    Table* table;
    Raquette* raquette;
    Balle* balle;
    bool raquettebaton;

    void Clean();
    void Update(float delta);
    void Render(float delta);

    void NewGame();
    void ResetRaquette();
    void StickBalle();

    void SetRaquetteX(float x);
    void CheckTableCollisions();
    float GetReflection(float hitx);
    void CheckRaquetteCollisions();
    void CheckBriquesCollisions();
    void CheckBriquesCollisions2();
    void BalleBriquesResponse(int dirindex);
    int GetBriquesCount();
};

#endif
