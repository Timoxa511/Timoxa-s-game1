#include "TXlib.h"

#define that
#define ever (;;)

//{    ���������

struct Vector
    {
    double x, y;
    };

struct Hero
    {
    bool Active;

    Vector pos;
    Vector v;
    int XLen;
    int YLen;

    HDC Texture;
    int AnimationNumber;

    int Type;
    bool InFlock;

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

struct Button
    {
    Rect pos;
    Vector Len;
    HDC Texture;
    };

enum {LOSE, WIN, CONTINUE};

const COLORREF MAP_ROAD         = RGB (  0, 255, 0);

const COLORREF MAP_FILLINGPOINT = RGB (255, 255, 0);

const double DT = 1;

const int LIFE  = 40;

const int FlockSize = 22;

enum {NPC, CHARACTER};

void Talk (int x, int y, const char words [], int sleep = 1000);
void PlsStopSpammingKeysGetYourHandsAwayFromTheKeyBoard_Arrrrrrrr ();
void StartCom ();
HDC SuperLoadImage (const char FilePictureName []);


void Animation      (const Hero *object, int Active);
void SuperAnimation (const Hero *object, int Active);
void FonDraw        (const Fon  *object);
double Gipotenooza  (double xA, double yA, double xB, double yB);
void HeroDraw       (const Hero *object, int radius);
bool InsideSquareArea   (const Hero *object, const Rect *Square);
bool ClrGreenTest (const COLORREF *Clr);

void DrawButton (const Button *TheButton);
bool InsideButton (const Hero *Mouse, const Button *TheButton);

void CreateFlock (Hero TheDucks [], int start, int end, Vector pos, Vector v);
void FlockMove   (Hero TheDucks [], int Size, int t, Hero *LosharikHero);
//void AngryDuckAnimation (const Hero *object, int Active, HDC AngryDuckTexture);
bool AnyDuckOfAFlockHasBumpedTheHero  (Hero object, Hero TheDucks [], int Size, double VarOfTheDistanceBetweenTheDuckAndTheHeroObject);
bool IsInFlock        (Hero TheDucks [], int ArrayDucksNum, int ADuck, double DucksAttracttionToEachOther);
void IsInFlockWrapper (Hero TheDucks [], int ArrayDucksNum);
void DuckLogic (Hero TheDucks [], int ArrayDucksNum);


void FillingProc   (Hero *object);
void HitPointsDraw (Hero object, int x, int y);
int  HitPoints     (Hero *object, COLORREF PointClr, COLORREF PrevClr);
void GamePhysics   (Hero *object, double dt, const Rect *Square1 = NULL, const Rect *Square2 = NULL, Hero *LosharikHero = NULL);
void ObjectControl (Hero *object,
                    int KeyUp = 'W', int KeyDown = 'S', int KeyLeft = 'A', int KeyRight = 'D',
                    int KeyStopMove = VK_SPACE);
bool GameControl (int KeyQuitAndPause);


bool ClrCenter (Hero *object, COLORREF PointClr, COLORREF PrevClr);
void MidPower        (Hero *object, const Rect *Square1, const Rect *Square2, Hero *LosharikHero);
Vector AngryDuckMode (Hero *object, Hero *AngryDuck);

void LoadData (const char FileName [], Hero *LosharikHero, Hero TheDucks [], int  nDucks);
void SaveData (const char FileName [], const Hero *LosharikHero, const Hero TheDucks [], int  nDucks);
void GameProcces (HDC FrontFonTexture, HDC BackFonTexture, HDC Losharik, HDC SharpBird);
int  GameMenu    (HDC ButtonTexture);
//}

int main()
    {
    StartCom ();

    HDC  ButtonTexture   = SuperLoadImage ("Button.bmp");
    HDC  SharpBird       = SuperLoadImage ("SharpBird.bmp");
    HDC  Losharik        = SuperLoadImage ("Sharik.bmp");
    HDC  FrontFonTexture = SuperLoadImage ("GameFon.bmp");
    HDC  BackFonTexture  = SuperLoadImage ("GameBackFon.bmp");

    if (GameMenu    (ButtonTexture) == 1)
        GameProcces (FrontFonTexture, BackFonTexture, Losharik, SharpBird);

    txDeleteDC (FrontFonTexture);
    txDeleteDC (Losharik);
    txDeleteDC (BackFonTexture);
    txDeleteDC (SharpBird);
    txDeleteDC (ButtonTexture);

    txEnd ();
    PlsStopSpammingKeysGetYourHandsAwayFromTheKeyBoard_Arrrrrrrr ();
    }

//-----------------------------------------------------------------------------
int GameMenu (HDC ButtonTexture)
    {
    txSetFillColor (TX_WHITE);
    txRectangle (0, 0, txGetExtentX(), txGetExtentY());
    Button TheButton = {{{txGetExtentX()/2 - 100, txGetExtentY()/2 - 100},
                         {txGetExtentX()/2 + 100, txGetExtentY()/2 + 100}}, {200, 200}, ButtonTexture};
    for ever
        {
        Hero Mouse  = {true, txMouseX (), txMouseY ()};
        if (InsideButton (&Mouse, &TheButton)) return 1;
        txSleep (0);
        }
    return 0;
    }

//{-----------------------------------------------------------------------------
bool InsideButton (const Hero *Mouse, const Button *TheButton)
    {
    DrawButton (TheButton);
    return (InsideSquareArea (Mouse, &TheButton->pos) && txMouseButtons () == 1);
    }

//-----------------------------------------------------------------------------
void DrawButton (const Button *TheButton)
    {
    //Win32::RoundRect (txDC(), ButtonArea.A.x, ButtonArea.A.y, ButtonArea.B.x, ButtonArea.B.y, 70, 70);
    txBitBlt (txDC (), TheButton->pos.A.x, TheButton->pos.A.y, TheButton->Len.x, TheButton->Len.y, TheButton->Texture);
    }
//}
//-----------------------------------------------------------------------------

void GameProcces (HDC FrontFonTexture, HDC BackFonTexture, HDC Losharik, HDC SharpBird)
    {
    Rect Square1 = {{1050, 0}, {1800, 500}};
    Rect Square2 = {{0, 500}, {300, 800}};

    Fon FrontFon = {{1800/2, 1000/2}, FrontFonTexture, 1800, 1000};
    Fon BackFon  = {{1800/2, 1000/2}, BackFonTexture,  1800, 1000};

    Hero LosharikHero = {true, {900, 850},  {0, 0}, 110,  80, Losharik, 0, CHARACTER, false, 100, LIFE};

    Hero TheDucks[FlockSize] = { {true, { }, { }, 114, 85, SharpBird, 0, NPC} };
    CreateFlock (TheDucks, 0,           FlockSize/2, {400, 400}, {5, -1.5});
    CreateFlock (TheDucks, FlockSize/2, FlockSize,   {400, 400}, {3, 3.5 });


// ���� �������� � ����� [666666666] �� ���� ����� ������� �������� 100� ���������� � ������� � ������ � ��� � ����,
//�� �� ������� ����� (��� ����� ���� ����� ����)

    COLORREF PrevClr = -1;
    //int Time1 = GetTickCount ();
    int t = 0;


    LoadData ("dataSharikGame.Andrusha", &LosharikHero, TheDucks, FlockSize);
    //SaveData ("datasharikgame2", &LosharikHero, TheDucks, FlockSize);
    unlink ("dataSharikGame.Andrusha");

    while (!GetAsyncKeyState (VK_ESCAPE))
        {

        txLock ();
        FonDraw (&BackFon);
        COLORREF PointClr = txGetPixel (LosharikHero.pos.x, LosharikHero.pos.y);
        FonDraw (&FrontFon);
        txUnlock ();


        GamePhysics (&LosharikHero, DT, &Square1, &Square2, &LosharikHero);
        Animation (&LosharikHero, t/6%2);



        if (AnyDuckOfAFlockHasBumpedTheHero (LosharikHero, TheDucks, FlockSize, 50) == false) LosharikHero.Life -=1;
        IsInFlockWrapper (TheDucks, FlockSize);
        DuckLogic (TheDucks, FlockSize);
        FlockMove   (TheDucks, FlockSize, t, &LosharikHero);


        if (ClrCenter (&LosharikHero, PointClr, PrevClr) == false) break;
        PrevClr = PointClr;


        //int Time2 = GetTickCount ();
        //printf ("         ����� %d        \r", Time2 - Time1);

        if (GameControl (VK_RETURN))
            {
            SaveData ("dataSharikGame.Andrusha", &LosharikHero, TheDucks, FlockSize);
            break;
            }

        txSleep (20);
        t++;
        }

    }

//-----------------------------------------------------------------------------

void SaveData (const char FileName [], const Hero *LosharikHero, const Hero TheDucks [], int  nDucks)
    {
    FILE* SaveF = fopen (FileName, "w");
    fprintf (SaveF, "LosharikHero { Vector pos: %.1lf %.1lf; Vector v: %.2lf %.2lf; AnimationNumber: %d; FuelAmount: %lf; Life: %d }\n",
                     LosharikHero->pos.x,  LosharikHero->pos.y, LosharikHero->v.x, LosharikHero->v.y,  LosharikHero->AnimationNumber,
                     LosharikHero->FuelAmount, LosharikHero->Life);

    for (int i = 0; i < nDucks; i++)
        {
        fprintf (SaveF, "TheDucks [%2d] { Active: %d; Vector pos: %6.1lf %6.1lf; Vector v: %5.2lf %5.2lf; AnimationNumber: %d; InFlock: %d; Life: %2d }\n",
                     i, TheDucks[i].Active, TheDucks[i].pos.x, TheDucks[i].pos.y, TheDucks[i].v.x, TheDucks[i].v.y, TheDucks[i].AnimationNumber,
                     TheDucks[i].InFlock, TheDucks[i].Life);
        txSleep (0);
        }

    fclose (SaveF);
    }

//-----------------------------------------------------------------------------

void LoadData (const char FileName [], Hero *LosharikHero, Hero TheDucks [], int  nDucks)
    {
    FILE* SaveF = fopen (FileName, "r");
    int jet = fscanf (SaveF, "LosharikHero { Vector pos: %lg %lg; Vector v: %lg %lg; AnimationNumber: %d; FuelAmount: %lg; Life: %d }",
                     &LosharikHero->pos.x,  &LosharikHero->pos.y, &LosharikHero->v.x, &LosharikHero->v.y,  &LosharikHero->AnimationNumber,
                     &LosharikHero->FuelAmount, &LosharikHero->Life);
    //printf ("jet = %d\n\n", jet);
    //assert (jet == 7);

    for (int i = 0; i < nDucks; i++)
        {
        int Active  = TheDucks[i].Active;
        int InFlock = TheDucks[i].InFlock;

        int fghjk = fscanf (SaveF, " TheDucks [%*d] { Active: %d; Vector pos: %lg %lg; Vector v: %lg %lg; AnimationNumber: %d; InFlock: %d; Life: %d }",
                        &Active, &TheDucks[i].pos.x, &TheDucks[i].pos.y, &TheDucks[i].v.x, &TheDucks[i].v.y,
                        &TheDucks[i].AnimationNumber, &InFlock, &TheDucks[i].Life);
                        // %*d ������ ��� ��������� �����(d) �����, * ������ ��� �� ��� �� ���������, � ����������� � ������)))

        //printf ("i = %d fghjk = %d\n",i , fghjk);
        //assert (fghjk == 8);

        TheDucks[i].Active  = (bool) Active;
        TheDucks[i].InFlock = (bool) InFlock;
        txSleep (0);
        }

    fclose (SaveF);
    }
//-----------------------------------------------------------------------------
bool ClrCenter (Hero *object, COLORREF PointClr, COLORREF PrevClr)
    {
    if (HitPoints (object, PointClr, PrevClr) == 1) return false;
    HitPointsDraw (*object, 100, 100);

    if (ClrGreenTest (&PointClr) == true && PointClr != MAP_ROAD) FillingProc (object);

    return true;
    }
//-----------------------------------------------------------------------------
void MidPower (Hero *object, const Rect *Square1, const Rect *Square2, Hero *LosharikHero) // ��������� ������. ������������ ��++ � ����������� �������
    {
    Vector resF = {0, 0};

    if (object->Type != NPC)
        {
        resF.x += -0.3;
        resF.y +=  0.2;
        }
    if (object->Type != NPC)
        if (Square1 != NULL && Square2 != NULL)  // if (Square1 && Square2) ���� ����� �����
            if (InsideSquareArea (object, Square1)  ||
                InsideSquareArea (object, Square2))
                {
                resF.x += -0.2;
                resF.y +=  0.3;
                }
    if (object->Type != NPC) ObjectControl (object);




    if (object->Type == NPC && !object->InFlock)
        {
        Vector promezshytochnoe_nazvanue = AngryDuckMode (LosharikHero, object);
        resF.x += promezshytochnoe_nazvanue.x;
        resF.y += promezshytochnoe_nazvanue.y;

        /*txSetColor (TX_BLACK, 4);
        if (object->Active)
            txLine (object->pos.x, object->pos.y, object->pos.x + promezshytochnoe_nazvanue.x*100,
             object->pos.y + promezshytochnoe_nazvanue.y*100);*/
        }

    object->v.x += resF.x/1 * DT;       //m = 1
    object->v.y += resF.y/1 * DT;


    //object->vx = (object->vx > +7)? +7 : object->vx               ���� ������� �� ����  "�������"
    if (object->v.x > +7) object->v.x = +7;
    if (object->v.x < -7) object->v.x = -7;

    if (object->v.y > +3.7) object->v.y = +3.7;
    if (object->v.y < -5.8) object->v.y = -5.8;
    }

//=============================================================================
void CreateFlock (Hero TheDucks [], int start, int end, Vector pos, Vector v)
    {

    for (int i = start, n = 0; i < end; i++, n++)
        {
        assert (start <= i && i < end);                         /*   �������� �� ������ �� � ����� ������ ��� ��������� � ������, � ���� ������, ...
                                         �� ������ �������� - ��������� ����� ��������� ������� � ���������� ����/
                                         (����, � ������ � ������ � ������ �����) ��������� undefined behavior*/

        TheDucks[i] = TheDucks [0];
        TheDucks[i].pos = pos;
        TheDucks[i].v = v;

        TheDucks[i].pos.y += 50*n;

        if (n < (end - start)/2) TheDucks[i].pos.x += 50*(n+1);
        else                     TheDucks[i].pos.x -= 50*(n - (end - start));

        }

    }

//-----------------------------------------------------------------------------
void FlockMove (Hero TheDucks [], int Size, int t, Hero *LosharikHero)
    {
    for (int i = 0; i < Size; i++)
        {
        assert (0 <= i && i < Size);
        GamePhysics (&TheDucks [i], DT, NULL, NULL, LosharikHero);
        if (TheDucks[i].Active) SuperAnimation (&TheDucks [i], (TheDucks[i].InFlock)? t/3%3 : t


        %3); //else txCircle (TheDucks[i].pos.x, TheDucks[i].pos.y, 2);
        }
    }

//-----------------------------------------------------------------------------
bool AnyDuckOfAFlockHasBumpedTheHero (Hero object, Hero TheDucks [], int Size, double VarOfTheDistanceBetweenTheDuckAndTheHeroObject)
    {
    for (int i = 0; i < Size; i++)
        {
        if (TheDucks[i].Active == false)
            continue;

        double result = Gipotenooza (object.pos.x, object.pos.y, TheDucks[i].pos.x, TheDucks[i].pos.y);

        if (result <= VarOfTheDistanceBetweenTheDuckAndTheHeroObject)
            {
            TheDucks[i].Active = false;
            return false;
            }
        }

    return true;
    }

/*bool IsInFlockWorstSDFGHJKJVC_ploxo_otchen (Hero TheDucks [], int ArrayDucksNum, int ADuck)
    {
    if (ADuck >= 1                   && TheDucks[ADuck - 1].Active) return true;
    if (ADuck <= ArrayDucksNum-1 - 1 && TheDucks[ADuck + 1].Active) return true;

    return false;
    }  */

//-----------------------------------------------------------------------------
bool IsInFlock (Hero TheDucks [], int ArrayDucksNum, int ADuck, double DucksAttracttionToEachOther)
    {
    for (int i = 0; i < ArrayDucksNum; i++)
        {
        if (i == ADuck || TheDucks[i].Active == false)
            continue;

        double Duck_sEyeZone = Gipotenooza (TheDucks[ADuck].pos.x, TheDucks[ADuck].pos.y,
                                            TheDucks[i].pos.x,     TheDucks[i].pos.y);

        if (Duck_sEyeZone < DucksAttracttionToEachOther)
            return true;
        }


    return false;
    }
//-----------------------------------------------------------------------------
void IsInFlockWrapper (Hero TheDucks [], int ArrayDucksNum)
    {
    for (int ADuck = 0; ADuck < ArrayDucksNum; ADuck++)
        {
        TheDucks[ADuck].InFlock = IsInFlock (TheDucks, ArrayDucksNum, ADuck, 100);
        }

    }

//-----------------------------------------------------------------------------
Vector AngryDuckMode (Hero *object, Hero *AngryDuck)
    {


    Vector ChasingForce = {(object->pos.x - AngryDuck->pos.x)/150,
                           (object->pos.y - AngryDuck->pos.y)/150};
    return ChasingForce;
    }

//-----------------------------------------------------------------------------
/*void AngryDuckAnimation (const Hero *object, int Active, HDC AngryDuckTexture)
    {
    if (object->InFlock == true)
        {
        SuperAnimation (object, Active);
        }
    else
        {
        int ActiveMirror = that (object->v.x >= 0); // ����� ���� �����, ��� vx > 0. ���� ������ �� 1 ���� ����� �� 0

        txAlphaBlend (txDC (), object->pos.x - object->XLen/2, object->pos.y - object->YLen/2,
                          object->XLen, object->YLen, AngryDuckTexture,
                          object->XLen * Active, object->YLen * ActiveMirror);
        }
    }

//=============================================================================
 */


void HitPointsDraw (Hero object, int x, int y)
        {
        for (int i = 0; i < object.Life; i++)
            {
            Fon LifeBallon = {x + (i%5 * 0.5*object.XLen), y + (i/5 * object.YLen), object.Texture, object.XLen, object.YLen};
            FonDraw (&LifeBallon);
            }


        /*while (object.Life > 0)
            {
            Fon LifeBallon = {};Fon LifeBallon = {};
            if ( (x + (double) object.Life*object.XLen) < 1700)
                LifeBallon      = {x + (double) object.Life*object.XLen, (double) y, object.Texture, object.XLen, object.YLen};
                else LifeBallon = {x + (double) object.Life*object.XLen, (double) y * (int) ((x + object.Life*object.XLen) / 1700),
                                                                                         object.Texture, object.XLen, object.YLen};

            object.Life --;
            }
            if ((x + object.Life*110.0) > 1800)
                {
                x -= object.Life*110.0;
                y += 120;
                }
            }*/

        }
//-----------------------------------------------------------------------------

int HitPoints (Hero *object, COLORREF PointClr, COLORREF PrevClr)
    {
    if (ClrGreenTest (&PointClr) == false &&
        ClrGreenTest (&PointClr) != ClrGreenTest (&PrevClr) &&
        PrevClr != (COLORREF) -1 )                             // �� ����� � ����

        {
        object->Life -= 1;
        object->Time = GetTickCount ();
        }
    /*  ���������� ��������
    �� ������� ����� � ���� - ������ 1 �� � �������� ����� ��������� � ����

       �� ��� ��������� � ���� - �������� ������� ����� � ���������
                ���� ������� ���� ������ ������ 4 ������, �� -1 ��         */

    if (ClrGreenTest (&PointClr) == false)
        {
        if (GetTickCount () - object->Time >= 2000)
            {
            object->Life -= 1;
            object->Time = GetTickCount ();
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


//-----------------------------------------------------------------------------
void DuckLogic (Hero TheDucks [], int ArrayDucksNum)
    {
    for (int i = 0; i < ArrayDucksNum; i++)
        {
        TheDucks[i].AnimationNumber = (TheDucks[i].InFlock)? 0 : 1;
        }


    //default: printf ("��������� ������, ����������� ������� %d, � ����� ������������ ��� ���� ����� ��������������", object->Type); abort ();
    }
//--------------------------------------------------------------------
void GamePhysics (Hero *object , double dt, const Rect *Square1, const Rect *Square2, Hero *LosharikHero)
    {

    MidPower (object, Square1, Square2, LosharikHero);

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

    //if (GetAsyncKeyState ( KeyUp    ))   if (object->FuelAmount >0) object->vy-- ; else object->vy -= 0.3;     I ������� ������� ������ �������
    //if (GetAsyncKeyState ( KeyDown  ))   object->vy += (object->FuelAmount >0)? 1 :  0.3;                      II   ������� ������� ������ �������

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
bool GameControl (int KeyQuitAndPause)
    {
    return (GetAsyncKeyState (KeyQuitAndPause));
    }
//-----------------------------------------------------------------------------
bool ClrGreenTest (const COLORREF *Clr)
    {
    COLORREF AmountOfGreenInColor = txExtractColor (*Clr, TX_GREEN);
    return (AmountOfGreenInColor == 255);
    }


//-----------------------------------------------------------------------------
//{  slaves

void Animation (const Hero *object, int Active)
    {
    int ActiveMirror = 0;
    //if (object.v.x >= 0) ActiveMirror = 1; else ActiveMirror = 0;

    txTransparentBlt (txDC (), object->pos.x - object->XLen/2, object->pos.y - object->YLen/2,
                      object->XLen, object->YLen, object->Texture,
                      object->XLen * Active, object->YLen * ActiveMirror, RGB (0, 0, 0));
    }
//-----------------------------------------------------------------------------
void SuperAnimation (const Hero *object, int Active)
    {
    int ActiveMirror = 0;
    if (object->v.x >= 0) ActiveMirror = 1; else ActiveMirror = 0;

    txAlphaBlend (txDC (), object->pos.x - object->XLen/2, object->pos.y - object->YLen/2,
                      object->XLen, object->YLen, object->Texture,
                      object->XLen * Active, object->YLen *(object->AnimationNumber * 2 + ActiveMirror));

    txSetFillColor ((object->InFlock)? TX_GREEN : TX_RED);

    //txCircle (object->pos.x, object->pos.y, 20);
    }
//-----------------------------------------------------------------------------

void HeroDraw (const Hero *object, int radius)
    {
    txSetFillColor (TX_GREEN);
    txCircle (object->pos.x, object->pos.y, radius);
    txTransparentBlt (txDC (), object->pos.x - object->XLen/2, object->pos.y - object->YLen/2, object->XLen, object->YLen,
                      object->Texture, 0, 0, RGB (0, 0, 0));
    }
//-----------------------------------------------------------------------------

void FonDraw (const Fon *object)
    {
    txTransparentBlt (txDC (), object->pos.x - object->FonXLen/2, object->pos.y - object->FonYLen/2, object->FonXLen, object->FonYLen,
                      object->Texture, 0, 0, RGB (0, 0, 0));
    }

//-----------------------------------------------------------------------------
bool InsideSquareArea (const Hero *object, const Rect *Square)
    {
    return ((Square->A.x <= object->pos.x) && (object->pos.x <= Square->B.x) &&
            (Square->A.y <= object->pos.y) && (object->pos.y <= Square->B.y));
    }
//-----------------------------------------------------------------------------

HDC SuperLoadImage (const char FilePictureName [])
    {
    HDC Load = txLoadImage (FilePictureName);
    if (Load == NULL)
        {
        //txPlaySound ("Sorry.wav");
        char Message [999] = "";
        sprintf (Message, "���, %s �� �����������", FilePictureName);
        txMessageBox (Message, "��������� �� ��������", MB_ICONEXCLAMATION);;
        }
    return Load;
    }
//-----------------------------------------------------------------------------

double Gipotenooza (double xA, double yA, double xB, double yB)
    {
    return sqrt ( (xB - xA)*(xB - xA) + (yB - yA)*(yB - yA) );
    }
//-----------------------------------------------------------------------------

void StartCom ()
    {
    txCreateWindow (1800, 1000);

    txSetFillColor     (RGB (255, 255, 255));
    txClear ();
    txSetColor         (RGB(0, 0, 0));

    txBegin ();
    txDisableAutoPause();
    }
//-----------------------------------------------------------------------------

//}
//-----------------------------------------------------------------------------

void PlsStopSpammingKeysGetYourHandsAwayFromTheKeyBoard_Arrrrrrrr ()
    {
    while (GetAsyncKeyState ('A') || GetAsyncKeyState ('S') || GetAsyncKeyState ('W') || GetAsyncKeyState ('D'))
        Sleep (0);

    }



