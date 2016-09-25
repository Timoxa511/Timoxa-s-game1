#include "TXlib.h"


struct Hero
    {
    double x, y;
    double vx, vy;
    int XLen;
    int YLen;

    HDC Texture;


    double FuelAmount;
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

const int MAP_ROAD = RGB (0, 255, 0);





void Talk (int x, int y, const char words [], int sleep = 1000);
void StartCom ();
HDC SuperLoadImage (const char FilePictureName []);


void Animation (Hero object, int Active);
void FonDraw   (Fon object);
void HeroDraw  (Hero object, int radius);

void HitPointsDraw (double LifePoints, HDC HpTexture, int x, int y);
int HitPoints      (COLORREF PointClr, COLORREF PrevClr, double *LifePoints);
void GamePhysics   (Hero *object, double dt);
void ObjectControl (Hero *object,
                    int KeyUp = 'W', int KeyDown = 'S', int KeyLeft = 'A', int KeyRight = 'D',
                    int KeyStopMove = VK_SPACE);


void GameProcces (HDC FrontFonTexture, HDC BackFonTexture, HDC Losharik);

int main()
    {
    StartCom ();


    HDC  Losharik        = SuperLoadImage ("Sharik.bmp");
    HDC  FrontFonTexture = SuperLoadImage ("GameFon.bmp");
    HDC  BackFonTexture  = SuperLoadImage ("GameBackFon.bmp");

    GameProcces (FrontFonTexture, BackFonTexture,Losharik);

    txDeleteDC (FrontFonTexture);
    txDeleteDC (Losharik);
    txDeleteDC (BackFonTexture);

    txEnd ();
    }


//-----------------------------------------------------------------------------
void GameProcces (HDC FrontFonTexture, HDC BackFonTexture, HDC Losharik)
    {
    Fon FrontFon = {1800/2, 1000/2, FrontFonTexture, 1800, 1000};
    Fon BackFon  = {1800/2, 1000/2, BackFonTexture,  1800, 1000};
    Hero LosharikHero = {900, 500, 0, 0, 110, 80, Losharik, 100};

    double LifePoints = 3;

    COLORREF PrevClr = -1;


    int Time1 = GetTickCount ();
    int t = 0;

    while (!GetAsyncKeyState (VK_ESCAPE))
        {


        txLock ();

        FonDraw (BackFon);
        COLORREF PointClr = txGetPixel (LosharikHero.x, LosharikHero.y);
        FonDraw (FrontFon);

        txUnlock ();

        //HitPoints (Losharik);

        //printf ("                                        гелий %lg      \r", LosharikHero.FuelAmount);


        Animation (LosharikHero, t/6%2);


        GamePhysics (&LosharikHero, 1);
        ObjectControl (&LosharikHero);

        if (HitPoints (PointClr, PrevClr, &LifePoints) == 1) break;
        HitPointsDraw (LifePoints, Losharik, 100, 100);


        PrevClr = PointClr;

        int Time2 = GetTickCount ();
        printf ("         время %d        \r", Time2 - Time1);

        txSleep (20);
        t++;
        }
    }

//----------------------------------------------------------------------------
void HitPointsDraw (double LifePoints, HDC HpTexture, int x, int y)
        {
        int BaloonNumber = (int) LifePoints;
        while (BaloonNumber > 0)
            {
            Fon LifePoint = {x + BaloonNumber*110.0, (double) y, HpTexture, 110, 80};
            FonDraw (LifePoint);
            BaloonNumber --;
            }

        }
//-----------------------------------------------------------------------------

int HitPoints (COLORREF PointClr, COLORREF PrevClr, double *LifePoints)
    {
    if (PointClr != MAP_ROAD) *LifePoints -= 0.01;
    if (*LifePoints < 0) *LifePoints = 0;

    if (PointClr != PrevClr) *LifePoints -= 1;

    if (*LifePoints <= 0) return 1;

    return 0;
    }
//-----------------------------------------------------------------------------

void GamePhysics (Hero *object, double dt)
    {
    double WindX = -0.5, WindY = 0.1;
    object->vx += WindX * dt;
    object->vy += WindY * dt;

    object->x = object->x + object->vx * dt;
    object->y = object->y + object->vy * dt;


    if (object->x > 1800)
        {
        object->vx = -object->vx;
        object->x = 1800;
        }
    if (object->y > 1000)
        {
        object->vy = -object->vy;
        object->y  = 1000;
        }
    if (object->x < 0)
        {
        object->vx = -object->vx;
        object->x = 0;
        }
    if (object->y < 0)
        {
        object->vy = -object->vy;
        object->y = 0;
        }

    //object->vx = (object->vx > +7)? +7 : object->vx               Илья говорит не надо  "странно"
    if (object->vx > +7) object->vx = +7;
    if (object->vx < -7) object->vx = -7;

    if (object->vy > +3.7) object->vy = +3.7;
    if (object->vy < -5.8) object->vy = -5.8;
    }
//-----------------------------------------------------------------------------
void ObjectControl (Hero *object, int KeyUp, int KeyDown, int KeyLeft, int KeyRight, int KeyStopMove)
    {
    double DeltaV = (object->FuelAmount >0)? 1 : 0.35;
    double FuelRateX = 0.22;
    double FuelRateY = 0.035;

    //if (GetAsyncKeyState ( KeyUp    ))   if (object->FuelAmount >0) object->vy-- ; else object->vy -= 0.3;     I вариант условия внутри условия
    //if (GetAsyncKeyState ( KeyDown  ))   object->vy += (object->FuelAmount >0)? 1 :  0.3;                      II   вариант условия внутри условия

    if (GetAsyncKeyState ( KeyUp    ))
        {
        object->vy -= DeltaV;
        object->FuelAmount -= FuelRateY;
        }
    if (GetAsyncKeyState ( KeyDown  ))
        {
        object->vy += DeltaV;
        object->FuelAmount -= FuelRateY;
        }
    if (GetAsyncKeyState ( KeyLeft  ))
        {
        object->vx -= DeltaV;
        object->FuelAmount -= FuelRateX;
        }
    if (GetAsyncKeyState ( KeyRight ))
        {
        object->vx += DeltaV;
        object->FuelAmount -= FuelRateX;
        }

    if (object->FuelAmount < 0) object->FuelAmount = 0;

    if (GetAsyncKeyState ( KeyStopMove ))
        object->vx = object->vy = 0;
    }
//-----------------------------------------------------------------------------
//{  slaves

void Animation (Hero object, int Active)
    {
    int ActiveMirror = 0;
    //if (object.vx >= 0) ActiveMirror = 1; else ActiveMirror = 0;

    txTransparentBlt (txDC (), object.x - object.XLen/2, object.y - object.YLen/2,
                      object.XLen, object.YLen, object.Texture,
                      object.XLen * Active, object.YLen * ActiveMirror, RGB (0, 0, 0));
    }
//-----------------------------------------------------------------------------

void HeroDraw (Hero object, int radius)
    {
    txSetFillColor (TX_GREEN);
    txCircle (object.x, object.y, radius);
    txTransparentBlt (txDC (), object.x - object.XLen/2, object.y - object.YLen/2, object.XLen, object.YLen,
                      object.Texture, 0, 0, RGB (0, 0, 0));
    }
//-----------------------------------------------------------------------------

void FonDraw (Fon object)
    {
    txTransparentBlt (txDC (), object.x - object.FonXLen/2, object.y - object.FonYLen/2, object.FonXLen, object.FonYLen,
                      object.Texture, 0, 0, RGB (0, 0, 0));
    }
//-----------------------------------------------------------------------------

HDC SuperLoadImage (const char FilePictureName [])
    {
    HDC Load = txLoadImage (FilePictureName);
    if (Load == NULL)
        {
        //txPlaySound ("Sorry.wav");
        char Message [999] = "";
        sprintf (Message, "Упс, %s не загружается", FilePictureName);
        txMessageBox (Message, "программа не работает", MB_ICONEXCLAMATION);;
        }
    return Load;
    }

//-----------------------------------------------------------------------------
void StartCom ()
    {
    txCreateWindow (1800, 1000);

    txSetFillColor     (RGB (255, 255, 255));
    txClear ();
    txSetColor         (RGB(0, 0, 0));

    txBegin ();
    //txDisableAutoPause();
    }
//-----------------------------------------------------------------------------

//}
//-----------------------------------------------------------------------------






