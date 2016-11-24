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

        // Calculate delta and fps
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

void Jeu::NewGame() {
    table->CreateLevel();
    ResetRaquette();
}

void Jeu::ResetRaquette() {
    raquettebaton = true;
    StickBalle();
}

void Jeu::StickBalle() {
    balle->x = raquette->x + raquette->width/2 - balle->width/2;
    balle->y = raquette->y - balle->height;
}

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
    if (hitx < 0) {
        hitx = 0;
    } else if (hitx > raquette->width) {
        hitx = raquette->width;
    }
    hitx -= raquette->width / 2.0f;

    return 2.0f * (hitx / (raquette->width / 2.0f));
}


void Jeu::CheckRaquetteCollisions() {
    float ballecenterx = balle->x + balle->width / 2.0f;

    if (balle->Collisions(raquette)) {
        balle->y = raquette->y - balle->height;
        balle->SetDirection(GetReflection(ballecenterx - raquette->x), -1);
    }
}

void Jeu::CheckBriquesCollisions() {
    for (int i=0; i<BOARD_WIDTH; i++) {
        for (int j=0; j<BOARD_HEIGHT; j++) {
            Briques brique = table->brique[i][j];

            if (brique.state) {
                float briquesx = table->briquesoffsetx + table->x + i*BOARD_BRWIDTH;
                float briquesy = table->briquesoffsety + table->y + j*BOARD_BRHEIGHT;

                float w = 0.5f * (balle->width + BOARD_BRWIDTH);
                float h = 0.5f * (balle->height + BOARD_BRHEIGHT);
                float dx = (balle->x + 0.5f*balle->width) - (briquesx + 0.5f*BOARD_BRWIDTH);
                float dy = (balle->y + 0.5f*balle->height) - (briquesy + 0.5f*BOARD_BRHEIGHT);

                if (fabs(dx) <= w && fabs(dy) <= h) {
                    table->brique[i][j].state = false;

                    float wy = w * dy;
                    float hx = h * dx;

                    if (wy > hx) {
                        if (wy > -hx) {
                            BalleBriquesResponse(3);
                        } else {
                            BalleBriquesResponse(0);
                        }
                    } else {
                        if (wy > -hx) {
                            BalleBriquesResponse(2);
                        } else {
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

            if (brique.state) {
                float briquesx = table->briquesoffsetx + table->x + i*BOARD_BRWIDTH;
                float briquesy = table->briquesoffsety + table->y + j*BOARD_BRHEIGHT;

                float ballecenterx = balle->x + 0.5f*balle->width;
                float ballecentery = balle->y + 0.5f*balle->height;

                float briquescenterx = briquesx + 0.5f*BOARD_BRWIDTH;
                float briquescentery = briquesy + 0.5f*BOARD_BRHEIGHT;

                if (balle->x <= briquesx + BOARD_BRWIDTH && balle->x+balle->width >= briquesx && balle->y <= briquesy + BOARD_BRHEIGHT && balle->y + balle->height >= briquesy) {
                    table->brique[i][j].state = false;
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

                    // Calculate xsize
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

                    if (xsize > ysize) {
                        if (ballecentery > briquescentery) {
                            balle->y += ysize + 0.01f;
                            BalleBriquesResponse(3);
                        } else {
                            balle->y -= ysize + 0.01f;
                            BalleBriquesResponse(1);
                        }
                    } else {
                        if (ballecenterx < briquescenterx) {
                            balle->x -= xsize + 0.01f;
                            BalleBriquesResponse(0);
                        } else {
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
    int mulx = 1;
    int muly = 1;

    if (balle->dirx > 0) {
        if (balle->diry > 0) {
            if (dirindex == 0 || dirindex == 3) {
                mulx = -1;
            } else {
                muly = -1;
            }
        } else if (balle->diry < 0) {
            if (dirindex == 0 || dirindex == 1) {
                mulx = -1;
            } else {
                muly = -1;
            }
        }
    } else if (balle->dirx < 0) {
        if (balle->diry > 0) {
            if (dirindex == 2 || dirindex == 3) {
                mulx = -1;
            } else {
                muly = -1;
            }
        } else if (balle->diry < 0) {
            if (dirindex == 1 || dirindex == 2) {
                mulx = -1;
            } else {
                muly = -1;
            }
        }
    }

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
