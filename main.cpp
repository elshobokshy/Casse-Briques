#include "main.h"

int main(int argc, char* argv[]) {
    Jeu* jeu = new Jeu();

    if (jeu->Init()) {
        jeu->Run();
    }

    delete jeu;
    return 0;
}
