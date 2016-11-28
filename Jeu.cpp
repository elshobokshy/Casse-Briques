#include "Jeu.h"

Jeu::Jeu() {
    window = 0;
    renderer = 0;
}

Jeu::~Jeu() {

}

bool Jeu::Init() {
    SDL_Init(SDL_INIT_VIDEO);

    // Create window
    window = SDL_CreateWindow("Casse Briques - Projet Prog Avancé",
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              1200, 600, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
    if (!window) {
        std::cout << "Error creating window:" << SDL_GetError() << std::endl;
        return false;
    }

    // Create renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cout << "Error creating renderer:" << SDL_GetError() << std::endl;
        return false;
    }

    //background image
    SDL_Surface* bgImage = IMG_Load("bg.png");
    SDL_Texture* showBG = SDL_CreateTextureFromSurface(renderer, bgImage);
    SDL_FreeSurface(bgImage);

    // Initialisation du timer
    lasttick = SDL_GetTicks();
    fpstick = lasttick;
    fps = 0;
    framecount = 0;

    testx = 0;
    testy = 0;

    return true;
}

void Jeu::Clean() {
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

void Jeu::Run() {
    table = new Table(renderer);
    raquette = new Raquette(renderer);
    balle = new Balle(renderer);

    NewGame();

    // Main loop
    while (1) {
        // Handler events
        SDL_Event e;
        if (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                break;
            }
        }

        // Calcule delta et fps
        unsigned int curtick = SDL_GetTicks();
        float delta = (curtick - lasttick) / 1000.0f;
        if (curtick - fpstick >= FPS_DELAY) {
            fps = framecount * (1000.0f / (curtick - fpstick));
            fpstick = curtick;
            framecount = 0;
            char buf[100];
            snprintf(buf, 100, "Casse Briques - Projet Prog Avancé (fps: %u)", fps);
            SDL_SetWindowTitle(window, buf);
        } else {
            framecount++;
        }
        lasttick = curtick;

        Update(delta);
        Render(delta);
    }

    delete table;
    delete raquette;
    delete balle;

    Clean();

    SDL_Quit();
}

// nouvelle partie
void Jeu::NewGame() {
    table->CreateLevel();
    ResetRaquette();
}

// quand la balle tombe ou quand la partie se termine
void Jeu::ResetRaquette() {
    raquettebaton = true;
    StickBalle();
}


// balle coller a la raquette
void Jeu::StickBalle() {
    balle->x = raquette->x + raquette->width/2 - balle->width/2;
    balle->y = raquette->y - balle->height;
}

// la logique de la game
void Jeu::Update(float delta) {

    int mx, my;
    Uint8 mstate = SDL_GetMouseState(&mx, &my);
    SetRaquetteX(mx - raquette->width/2.0f);

    if (mstate&SDL_BUTTON(1)) {
        if (raquettebaton) {
            raquettebaton = false;
            balle->SetDirection(1, -1);
        }
    }

    if (raquettebaton) {
        StickBalle();
    }

    CheckTableCollisions();
    CheckRaquetteCollisions();
    CheckBriquesCollisions2();

    if (GetBriquesCount() == 0) {
        NewGame();
    }

    table->Update(delta);
    raquette->Update(delta);

    if (!raquettebaton) {
        balle->Update(delta);
    }
}

void Jeu::SetRaquetteX(float x) {
    float newx;
    if (x < table->x) {
        // upper bounce
        newx = table->x;
    } else if (x + raquette->width > table->x + table->width) {
        // lower bounce
        newx = table->x + table->width - raquette->width;
    } else {
        newx = x;
    }
    raquette->x = newx;
}

void Jeu::CheckTableCollisions() {
    // collisions top & bot
    if (balle->y < table->y) {
        // top
        // empeche la balle de sortir de la table
        balle->y = table->y;
        balle->diry *= -1;
    } else if (balle->y + balle->height > table->y + table->height) {
        // bot

        // quand la balle tombe
        ResetRaquette();
    }

    // collisions gauche et droite
    if (balle->x <= table->x) {
        // gauche
        // empeche la balle de sortir de la table
        balle->x = table->x;
        balle->dirx *= -1;
    } else if (balle->x + balle->width >= table->x + table->width) {
        // droit
        // empeche la balle de sortir de la table
        balle->x = table->x + table->width - balle->width;
        balle->dirx *= -1;
    }
}

float Jeu::GetReflection(float hitx) {
    // s'assurer que hitx est dans la largeur de la raquette
    if (hitx < 0) {
        hitx = 0;
    } else if (hitx > raquette->width) {
        hitx = raquette->width;
    }
    //tout a gauche du centre de la raquette et mis a gauche, idem pour la droite
    hitx -= raquette->width / 2.0f;

    // la balle tombe entre -2.0f et 2.0f
    return 2.0f * (hitx / (raquette->width / 2.0f));
}


void Jeu::CheckRaquetteCollisions() {
    // ballecenterx continent les coordonées x de la ball
    float ballecenterx = balle->x + balle->width / 2.0f;

    // si il y a une collision entre la balle et la raquette
    if (balle->Collisions(raquette)) {
        balle->y = raquette->y - balle->height;
        balle->SetDirection(GetReflection(ballecenterx - raquette->x), -1);
    }
}

void Jeu::CheckBriquesCollisions() {
    for (int i=0; i<BOARD_WIDTH; i++) {
        for (int j=0; j<BOARD_HEIGHT; j++) {
            Briques brique = table->brique[i][j];
            // si la brique est là
            if (brique.state) {
                // les coordonées x et y des briques
                float briquesx = table->briquesoffsetx + table->x + i*BOARD_BRWIDTH;
                float briquesy = table->briquesoffsety + table->y + j*BOARD_BRHEIGHT;
                // regarder si il y a une collisions entre la balle et la brique
                // les collisions sont détérminer en utilisant la moitié des deux rectangles
                float w = 0.5f * (balle->width + BOARD_BRWIDTH);
                float h = 0.5f * (balle->height + BOARD_BRHEIGHT);
                float dx = (balle->x + 0.5f*balle->width) - (briquesx + 0.5f*BOARD_BRWIDTH);
                float dy = (balle->y + 0.5f*balle->height) - (briquesy + 0.5f*BOARD_BRHEIGHT);

                if (fabs(dx) <= w && fabs(dy) <= h) {
                    // si il y a une collisions
                    table->brique[i][j].state = false;

                    float wy = w * dy;
                    float hx = h * dx;

                    if (wy > hx) {
                        if (wy > -hx) {
                            // bas
                            BalleBriquesResponse(3);
                        } else {
                            // gauche
                            BalleBriquesResponse(0);
                        }
                    } else {
                        if (wy > -hx) {
                            // droite
                            BalleBriquesResponse(2);
                        } else {
                            // haut
                            BalleBriquesResponse(1);
                        }
                    }
                    return;
                }
            }
        }
    }
}

void Jeu::CheckBriquesCollisions2() {
    for (int i=0; i<BOARD_WIDTH; i++) {
        for (int j=0; j<BOARD_HEIGHT; j++) {
            Briques brique = table->brique[i][j];
            // si y a une brique
            if (brique.state) {
                // coordonées x et y des briques
                float briquesx = table->briquesoffsetx + table->x + i*BOARD_BRWIDTH;
                float briquesy = table->briquesoffsety + table->y + j*BOARD_BRHEIGHT;
                // coordonées x et y du centre de la balle
                float ballecenterx = balle->x + 0.5f*balle->width;
                float ballecentery = balle->y + 0.5f*balle->height;
                // coordonées x et y du centre de la brique
                float briquescenterx = briquesx + 0.5f*BOARD_BRWIDTH;
                float briquescentery = briquesy + 0.5f*BOARD_BRHEIGHT;

                if (balle->x <= briquesx + BOARD_BRWIDTH && balle->x+balle->width >= briquesx && balle->y <= briquesy + BOARD_BRHEIGHT && balle->y + balle->height >= briquesy) {
                    // y a une collisions, enlève la brique
                    table->brique[i][j].state = false;
                    // calcule de ysize
                    float ymin = 0;
                    if (briquesy > balle->y) {
                        ymin = briquesy;
                    } else {
                        ymin = balle->y;
                    }

                    float ymax = 0;
                    if (briquesy+BOARD_BRHEIGHT < balle->y+balle->height) {
                        ymax = briquesy+BOARD_BRHEIGHT;
                    } else {
                        ymax = balle->y+balle->height;
                    }

                    float ysize = ymax - ymin;

                    // calcule de xsize
                    float xmin = 0;
                    if (briquesx > balle->x) {
                        xmin = briquesx;
                    } else {
                        xmin = balle->x;
                    }

                    float xmax = 0;
                    if (briquesx+BOARD_BRWIDTH < balle->x+balle->width) {
                        xmax = briquesx+BOARD_BRWIDTH;
                    } else {
                        xmax = balle->x+balle->width;
                    }

                    float xsize = xmax - xmin;

                    // le point d'origine se trouve en haut a gauche de l'écran...
                    // création de la "réponse" des collisions
                    if (xsize > ysize) {
                        if (ballecentery > briquescentery) {
                            // bas
                            balle->y += ysize + 0.01f;
                            BalleBriquesResponse(3);
                        } else {
                            // haut
                            balle->y -= ysize + 0.01f;
                            BalleBriquesResponse(1);
                        }
                    } else {
                        if (ballecenterx < briquescenterx) {
                            // gauche
                            balle->x -= xsize + 0.01f;
                            BalleBriquesResponse(0);
                        } else {
                            // droit
                            balle->x += xsize + 0.01f;
                            BalleBriquesResponse(2);
                        }
                    }

                    return;
                }
            }
        }
    }
}

void Jeu::BalleBriquesResponse(int dirindex) {
    // dirindex 0: Left, 1: Top, 2: Right, 3: Bottom

    //les facteurs de direction
    int mulx = 1;
    int muly = 1;

    if (balle->dirx > 0) {
        // si la balle bouge dans les x positives
        if (balle->diry > 0) {
            // si la balle bouge dans les y positives (+1 +1)
            if (dirindex == 0 || dirindex == 3) {
                mulx = -1;
            } else {
                muly = -1;
            }
        } else if (balle->diry < 0) {
            // si la balle bouge dans les y negatives (+1 -1)
            if (dirindex == 0 || dirindex == 1) {
                mulx = -1;
            } else {
                muly = -1;
            }
        }
    } else if (balle->dirx < 0) {
        // si la balle bouge dans les x negatives
        if (balle->diry > 0) {
            // si la balle bouge dans les y positives (-1 +1)
            if (dirindex == 2 || dirindex == 3) {
                mulx = -1;
            } else {
                muly = -1;
            }
        } else if (balle->diry < 0) {
            // si la balle bouge dans les y negatives (-1 -1)
            if (dirindex == 1 || dirindex == 2) {
                mulx = -1;
            } else {
                muly = -1;
            }
        }
    }
    // calcule de la nouvelle position de la balle en multipliants l'ancienne direction avec le facteur de directions correspondant
    balle->SetDirection(mulx*balle->dirx, muly*balle->diry);
}

int Jeu::GetBriquesCount() {
    int briquescount = 0;
    for (int i=0; i<BOARD_WIDTH; i++) {
        for (int j=0; j<BOARD_HEIGHT; j++) {
            Briques brique = table->brique[i][j];
            if (brique.state) {
                briquescount++;
            }
        }
    }

    return briquescount;
}

void Jeu::Render(float delta) {
    SDL_RenderClear(renderer);

    table->Render(delta);
    raquette->Render(delta);
    balle->Render(delta);

    SDL_RenderPresent(renderer);
}
