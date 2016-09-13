#include "TXlib.h"


struct Hero
    {
    double x, y;
    double vx, vy;
    HDC Texture;
    int PokemonXLen;
    int PokemonYLen;
    int DeltaCircle;
    };

struct Fon
    {
    double x, y;
    HDC Texture;
    int FonXLen;
    int FonYLen;
    };

const int LOSE     = 0,
          WIN      = 2,
          CONTINUE = 1;

void Talk (int x, int y, const char words [], int sleep = 1000);
void StartCom ();
double Gipotenooza (double xA, double yA, double xB, double yB);
HDC SuperLoadImage (const char FilePictureName []);














