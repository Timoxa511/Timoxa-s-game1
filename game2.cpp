



//TODO сделать функцию ColorAnalysis :
//TODO надо что бы она решала какую изменяющую функцию вызывать :
//TODO добавить/отнять  топливо/хп
//TODO если поменялся цвет(кол-во зеленого в цвете или цвет не равен дороге или цвет поменялся и не равен дороге…)
//TODO что бы все функции игровой логики(зависящие от цвета карты) вызывались в ней
//TODO txAlphaBlend
//TODO txLib установить




#include "TXlib.h"


struct Vector
    {
    double x, y;
    };

struct Hero
    {
    Vector pos;
    Vector v;
    int XLen;
    int YLen;

    HDC Texture;

    double FuelAmount;
    int Life;

    int Time;
    };

struct Fon
    {
    Vector pos;
    HDC Texture;
    int FonXLen;
    int FonYLen;
    };

struct Rect
    {
    Vector A;
    Vector B;
    };



const int LOSE     = 0,
          WIN      = 2,
          CONTINUE = 1;

const COLORREF MAP_ROAD         = RGB (  0, 255, 0);

const COLORREF MAP_FILLINGPOINT = RGB (255, 255, 0);

const double DT = 1;

const int LIFE  = 3;


void Talk (int x, int y, const char words [], int sleep = 1000);
void StartCom ();
HDC SuperLoadImage (const char FilePictureName []);


void Animation    (Hero object, int Active);
void FonDraw      (Fon object);
void HeroDraw     (Hero object, int radius);
bool InsideArea   (Hero object, Rect Square);
bool ClrGreenTest (COLORREF object);


void FillingProc   (Hero *object);
void HitPointsDraw (Hero  object, int x, int y);
int HitPoints      (Hero *object, COLORREF PointClr, COLORREF PrevClr);
void GamePhysics   (Hero *object, double dt, Rect Square1, Rect Square2);
void ObjectControl (Hero *object,
                    int KeyUp = 'W', int KeyDown = 'S', int KeyLeft = 'A', int KeyRight = 'D',
                    int KeyStopMove = VK_SPACE);

bool ClrCenter (Hero *object, COLORREF PointClr, COLORREF PrevClr);

void MidPower  (Hero *object, Rect Square1, Rect Square2);

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
    Fon FrontFon = {{1800/2, 1000/2}, FrontFonTexture, 1800, 1000};
    Fon BackFon  = {{1800/2, 1000/2}, BackFonTexture,  1800, 1000};
    Hero LosharikHero = {{900, 500}, {0, 0}, 110, 80, Losharik, 100, LIFE};
    Rect Square1 = {{1050, 0}, {1800, 500}};
    Rect Square2 = {{0, 500}, {300, 800}};

    COLORREF PrevClr = -1;
    int Time1 = GetTickCount ();
    int t = 0;

    while (!GetAsyncKeyState (VK_ESCAPE))
        {

        txLock ();
        FonDraw (BackFon);
        COLORREF PointClr = txGetPixel (LosharikHero.pos.x, LosharikHero.pos.y);
        FonDraw (FrontFon);
        txUnlock ();


        GamePhysics (&LosharikHero, DT, Square1, Square2);

        //printf ("                                        гелий %lg      \r", LosharikHero.FuelAmount);

        Animation (LosharikHero, t/6%2);

        if (ClrCenter (&LosharikHero, PointClr, PrevClr) == false) break;

        //if (HitPoints (&LosharikHero, PointClr, PrevClr) == 1) break;
        //HitPointsDraw (LosharikHero, 100, 100);
        //FillingCenter (*LosharikHero, Life)

        PrevClr = PointClr;


        int Time2 = GetTickCount ();
        //printf ("         время %d        \r", Time2 - Time1);

        txSleep (20);
        t++;
        }
    }

//-----------------------------------------------------------------------------
bool ClrCenter (Hero *object, COLORREF PointClr, COLORREF PrevClr)
    {
    if (HitPoints (object, PointClr, PrevClr) == 1) return false;
    HitPointsDraw (*object, 100, 100);

    if (ClrGreenTest (PointClr) == true && PointClr != MAP_ROAD) FillingProc (object);

    return true;
    }
//-----------------------------------------------------------------------------
void MidPower (Hero *object, Rect Square1, Rect Square2)
    {
    Vector resF = {0, 0};

    resF.x += -0.3;
    resF.y +=  0.2;





    if (InsideArea (*object, Square1)  ||
        InsideArea (*object, Square2))
        {
        resF.x += -0.2;
        resF.y +=  0.3;
        }

    ObjectControl (object);



    object->v.x += resF.x/1 * DT;       //m = 1
    object->v.y += resF.y/1 * DT;


    //object->vx = (object->vx > +7)? +7 : object->vx               Илья говорит не надо  "странно"
    if (object->v.x > +7) object->v.x = +7;
    if (object->v.x < -7) object->v.x = -7;

    if (object->v.y > +3.7) object->v.y = +3.7;
    if (object->v.y < -5.8) object->v.y = -5.8;

    }
//----------------------------------------------------------------------------
void HitPointsDraw (Hero object, int x, int y)
        {
        while (object.Life > 0)
            {
            Fon LifeBallon = {x + object.Life*110.0, (double) y, object.Texture, 110, 80};
            FonDraw (LifeBallon);
            object.Life --;

            if ((x + object.Life*110.0) > 1800) y += 120;
            }

        }
//-----------------------------------------------------------------------------

int HitPoints (Hero *object, COLORREF PointClr, COLORREF PrevClr)
    {
    if (ClrGreenTest (PointClr) == false &&
        ClrGreenTest (PointClr) != ClrGreenTest (PrevClr) &&
        PrevClr != (COLORREF) -1 )                             // мы вошли в зону

        {
        object->Life -= 1;
        object->Time = GetTickCount ();
        printf ("PointClr 0x%06X PrevClr 0x%06X \n", PointClr, PrevClr);

        }
    /* мы впервые вошли в зону - отнять 1 хп и замерить время вхождения в зону

       мы уже находимся в зоне - замерить текущее время и проверить
                если разница двух времен больше 4 секунд, то -1 хп         */

    if (ClrGreenTest (PointClr) == false)
        {
        if (GetTickCount () - object->Time >= 2000)
            {
            object->Life -= 1;
            object->Time = GetTickCount ();
            printf ("*******************************************************************************************\n");
            }
        }

    if (object->Life < 0) object->Life = 0;
    if (object->Life <= 0) return 1;

    return 0;
    }

//-----------------------------------------------------------------------------
void FillingProc (Hero *object)
    {
    if (object->Life < LIFE) object->Life += 1;
    if (object->FuelAmount < 100) object->FuelAmount += 1;
    }
//--------------------------------------------------------------------
void GamePhysics (Hero *object , double dt, Rect Square1, Rect Square2)
    {

    MidPower (object, Square1, Square2);

    object->pos.x += object->v.x * dt;
    object->pos.y += object->v.y * dt;


    if (object->pos.x > 1800)
        {
        object->v.x = -object->v.x;
        object->pos.x = 1800;
        }
    if (object->pos.y > 1000)
        {
        object->v.y = -object->v.y;
        object->pos.y  = 1000;
        }
    if (object->pos.x < 0)
        {
        object->v.x = -object->v.x;
        object->pos.x = 0;
        }
    if (object->pos.y < 0)
        {
        object->v.y = -object->v.y;
        object->pos.y = 0;
        }


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
        object->v.y -= DeltaV;
        object->FuelAmount -= FuelRateY;
        }
    if (GetAsyncKeyState ( KeyDown  ))
        {
        object->v.y += DeltaV;
        object->FuelAmount -= FuelRateY;
        }
    if (GetAsyncKeyState ( KeyLeft  ))
        {
        object->v.x -= DeltaV;
        object->FuelAmount -= FuelRateX;
        }
    if (GetAsyncKeyState ( KeyRight ))
        {
        object->v.x += DeltaV;
        object->FuelAmount -= FuelRateX;
        }

    if (object->FuelAmount < 0) object->FuelAmount = 0;

    if (GetAsyncKeyState ( KeyStopMove ))
        object->v.x = object->v.y = 0;
    }
//-----------------------------------------------------------------------------
bool ClrGreenTest (COLORREF Clr)
    {
    COLORREF AmountOfGreenInColor = txExtractColor (Clr, TX_GREEN);
    return (AmountOfGreenInColor == 255);
    }


//-----------------------------------------------------------------------------
//{  slaves

void Animation (Hero object, int Active)
    {
    int ActiveMirror = 0;
    //if (object.vx >= 0) ActiveMirror = 1; else ActiveMirror = 0;

    txTransparentBlt (txDC (), object.pos.x - object.XLen/2, object.pos.y - object.YLen/2,
                      object.XLen, object.YLen, object.Texture,
                      object.XLen * Active, object.YLen * ActiveMirror, RGB (0, 0, 0));
    }
//-----------------------------------------------------------------------------

void HeroDraw (Hero object, int radius)
    {
    txSetFillColor (TX_GREEN);
    txCircle (object.pos.x, object.pos.y, radius);
    txTransparentBlt (txDC (), object.pos.x - object.XLen/2, object.pos.y - object.YLen/2, object.XLen, object.YLen,
                      object.Texture, 0, 0, RGB (0, 0, 0));
    }
//-----------------------------------------------------------------------------

void FonDraw (Fon object)
    {
    txTransparentBlt (txDC (), object.pos.x - object.FonXLen/2, object.pos.y - object.FonYLen/2, object.FonXLen, object.FonYLen,
                      object.Texture, 0, 0, RGB (0, 0, 0));
    }

//-----------------------------------------------------------------------------
bool InsideArea (Hero object, Rect Square)
    {
    return ((Square.A.x <= object.pos.x) && (object.pos.x <= Square.B.x) &&
            (Square.A.y <= object.pos.y) && (object.pos.y <= Square.B.y));
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



