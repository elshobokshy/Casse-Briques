# A propos du projet Casse-Briques
## Plus d'informations
Ce projet est un projet pour nous faire découvrir le C++ et essayer d'approfondir nos connaissances en SDL2 avec un niveau plus au moins professionnel. Il a été crée en 3 mois donc il montre un jeu de casse-briques basiques. Le code est mis en entier en open-source, pour n'importe quelle personne qui voudrait un exemple d'un jeu de casse-briques.
# Comment faire tourner le jeu ?
## Code::Blocks 13.12
Important avant de commencer à lire : En faisant un import du projet chez vous (clone project), vous aurez déjà le jeu qui s'exécute si vous avez la version de Code::Blocks qui contient le compilateur mingw. On a fait un commit les propriétés du projet.

On va vous expliquer comment initialiser Code::Blocks pour faire fonctionner le projet chez vous, en prenant en compte que vous avez déjà Code::Blocks installer. <br />
Ici, on utilise mingw comme compilateur. Si vous ne l'avez pas, il faudra soit l'installer à part soit installer la version de Code::Blocks qui le contient directement de leur site : http://www.codeblocks.org/downloads/26 (codeblocks-16.01mingw-setup.exe) <br />
Aussi on prend en compte que vous avez déjà installer SDL2 ! <br />

1) Project -> Properties -> Project build options... (en bas a droite)
2) Dans Search Directories -> Compiler -> Add, ajouter l'endroit où se trouve votre bibliothèque SDL2. <br />
  ex: C:\mingw_dev_lib\include\SDL2
  Attention : Si vous avez une erreur qui vous dit qu'il n'arrive pas à trouver SDL.h, c'est probablement que vous avez mal fait cette étape.
3) Dans Search Directories -> Linker -> Add, ajouter l'emplacement du dossier lib de votre compilateur mingw. <br />
  ex: C:\mingw_dev_lib\lib
  Attention : Si vous avez une erreur qui vous dit qu'il n'arrive pas à trouver -lSDL2 ou -lSDL2main, c'est probablement que vous avez mal fait cette étape.
4) Dans Linker settings -> Other linker options, ajouter "-lmingw32 -lSDL2main -lSDL" <br />
  Attention : Si vous avez une erreur qui vous dit qu'il y a des "undefined references", c'est probablement que vous avez mal fait cette étape.
