//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

// дополнительно подрубил библиотеку Math.h, потому что синусы
#include <math.h>

#include "Unit1.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
// структура в которой хранятся координаты двух точек, через которые
// проходит прямая границы поля, от которой шар должен отскакивать
struct linesCrd {
    int x1, y1; // первая точка
    int x2, y2; // и вторая
};

// обьявление четырёх структур, по одной на каждую границу
linesCrd lineTop;    // сверху
linesCrd lineBottom; // снизу
linesCrd lineLeft;   // слева
linesCrd lineRight;  // и справа


int cue_swing = 0; // смещение кия

int index = 0;
int index2 = 0;

bool animateCue = false;  // для отладки анимации кия, которая не работает
bool moveBall = false;   // доп. условие, надо ли анимировать шар
bool canHitBall = true;  // доп. условие, можно ли "ударить" шар
bool calcAngle = true;   // доп. условие, надо ли высчитать угол старта шара

double ballEnergy = 10;  // кинетическая энергия шара. количество пикселей, на
                         // которое перемещается шар после удара. уменьшается со временем
double angle = 0;  // начальный угол. измеряется в радианах

int mPosX=0, mPosY=0;  // положение курсора мыши
int destX =0, destY=0;  // хранят в себе значение положения курсора мыши, получаемое из предыдущих переменных

//---------------------------------------------------------------------------
void MoveImage(TImage *img, int destX, int destY, int step, bool &needMove);

// конструктор, в котором можно обозначить какие-то действия
// срабатывает, когда создаётся форма.
//    когда создаётся форма, создаются и остальные компоненты, но
//    конструктор вызывается раньше, поэтому в нём нельзя обращаться к
//    ещё "не созданным" компонентам формы
__fastcall TForm1::TForm1(TComponent* Owner)
    : TForm(Owner)
{
    Form1->DoubleBuffered = true;  // функция подключает доп. буфер. уменьшает мерцание изображения или даже вовсе убирает

    // отправка заднего фона на нижний слой, что бы он был как фон
    BackGround->SendToBack();

    // перемещение "обьекта изображения" поверх всех остальных обьектов

    FrontGround->BringToFront();

    // обозначение координат границ поля
    // значения  относительно картинки
    //--------------------------
    lineRight.x1 = 959;
    lineRight.x2 = 959;
    lineRight.y1 = 0;
    lineRight.y2 = Form1->ClientHeight;

    lineLeft.x1 = 199;
    lineLeft.x2 = 199;
    lineLeft.y1 = 0;
    lineLeft.y2 = Form1->ClientHeight;

    lineTop.x1 = 0;
    lineTop.x2 = Form1->ClientWidth;
    lineTop.y1 = 99;
    lineTop.y2 = 99;

    lineBottom.x1 = 0;
    lineBottom.x2 = Form1->ClientWidth;
    lineBottom.y1 = 480;
    lineBottom.y2 = 480;
    //--------------------------
}
//---------------------------------------------------------------------------

//    к программе методом научного тыка. можно включить
//    изменив значение "animateCue" на "true"

void DrawImage(TImageList *imgList, int &index, TImage *img, double angle)
{
    if(index>=imgList->Count)
        index = 0;

    TRect rect(0,0,img->Width, img->Height);
    img->Canvas->Brush->Color=clBlack;
    img->Canvas->FillRect(rect);

    //1 сохранить старую матрицу
    XFORM savetagx;
    SetGraphicsMode(img->Canvas->Handle, GM_ADVANCED);// установка расширенного режима
    GetWorldTransform(img->Canvas->Handle, &savetagx);

    //установить новую матрицу
    XFORM xform;
    //ZeroMemory(&xform, sizeof(xform));
    xform.eM11 = cos(angle);
    xform.eM12 = sin(angle);
    xform.eM21 = -sin(angle);
    xform.eM22 = cos(angle);
    xform.eDx  = img->Width/2;
    xform.eDy  = img->Height/2;

    ModifyWorldTransform(img->Canvas->Handle, &xform, MWT_LEFTMULTIPLY);

    //отрисовать картинку
    imgList->Draw(img->Canvas,-img->Width/2,-img->Height/2, index);

    //вернуть старую матрицу
    SetWorldTransform(img->Canvas->Handle, &savetagx);
    //img->Invalidate();
}

//   основной таймер, в котором крутятся все основные действия
void __fastcall TForm1::MainTimerTimer(TObject *Sender)
{
    if (animateCue) // обновление кия когда необходимо
    {

        double angle = atan2(mPosY - imgCue->Top - imgCue->Height/2, mPosX - imgCue->Left-imgCue->Width/2);
        DrawImage(ImageList1, index2, imgCue, angle);
    }
    //------------
    if (moveBall) // движение шара
    {
        bool needWalk; // переменная, отвечающая за то, покоится ли шар
        ballEnergy -= 0.05; // уменьшение кинетической энергии шара в еденицу времени
        MoveImage(imgBall, destX, destY, ballEnergy, needWalk); // вызов функции движения шара

        //Timer3->Enabled = needWalk; // комментарий
    }
    //Form1->Canvas->Rectangle(100,100,103,103); // ещё комментарий
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FrontGroundMouseMove(TObject *Sender, TShiftState Shift,
      int X, int Y)
{
    // получение координат курсора мыши с того самого "переднего слоя"
    mPosX = X;
    mPosY = Y;
}
//---------------------------------------------------------------------------
// это невидимое изображение, названное "FrontGround'ом" используется ещё для
// при клике на него можно запустить шар, и в момент клика высчитываются все "необходимые" параметры
void __fastcall TForm1::FrontGroundClick(TObject *Sender)
{
    if (canHitBall) // если шар покоится, то его можно снова отправить в движение
    {
        canHitBall = false; // не разрешает мешать ему, до тех пор, пока он не остановится сам

        destX = mPosX; // получение координат точки относительно мыши, в которую будет
        destY = mPosY; // двигаться шар, а точнее, относительно которых будет вычисляться угол
                       // по которому будет шар двигаться
        moveBall = true;
    }
}
//---------------------------------------------------------------------------
int Distance(linesCrd Line, int x0, int y0) // принимает в себя структуру
{
    // высчитывание расстояния от центра шара до границы поля

    int x21 = Line.x2 - Line.x1;
    int y21 = Line.y2 - Line.y1;

    int res1 = abs(y21*x0 - x21*y0 + Line.x2*Line.y1 - Line.y2*Line.x1);
    int res2 = sqrt(y21*y21 + x21*x21);

    return res1/res2; //  возвращает длину до барьера
}


void MoveImage(TImage *img, int destX, int destY, int step, bool &needMove)
{
    // вычисляет угол старта шара
    // срабатывает один раз, когда по шару "бьют кием"
    if (calcAngle)
    {
        // предотвращает вычисление угла каждый раз, когда
        // цикл начинает новый оборот
        calcAngle = false;


        int midX = img->Left + img->Width/2;
        int midY = img->Top + img->Height/2;
        int dx = (destX - midX);
        int dy = (destY - midY);

        // значение угла сохраняется в отдельную глобальную переменную, которая
        // после используется функцией, но шар всё равно носит в стороны, когда
        // скорость шара уже низкая.
        angle = atan2(dy, dx);
    }
    // условие остановки шара. когда энергии не остаётся, шар должен остановиться
    if (step <= 0) //abs(dx)<step && abs(dy)<step // коммент
    {
        // задание "стандартных" значений основным переменным, что бы
        // при новом "ходе" всё работало как надо
        needMove = false;
        ballEnergy = 10;
        moveBall = false;
        canHitBall = true;
        calcAngle = true;

        return; // выход из всей функции
    }
    needMove = true;

    //if ( Distance(lineRight, Form1->imgBall->Left+15, Form1->imgBall->Top+15) <= 15 ) //

    // это условия, если шар "коснулся" или уже пересёк одну из границ поля
    if (Distance(lineTop, Form1->imgBall->Left+15, Form1->imgBall->Top+15)<15)
    {
        angle *= -1; // работает раз через раз
    }
    else if (Distance(lineBottom, Form1->imgBall->Left+15, Form1->imgBall->Top+15)<15)
    {
        angle *= -1;
    }
    else if (Distance(lineLeft, Form1->imgBall->Left+15, Form1->imgBall->Top+15)<15)
    {
        angle -= 90;
    }
    else if (Distance(lineRight, Form1->imgBall->Left+15, Form1->imgBall->Top+15)<15)
    {
        angle -= 90;
    }
    Form1->Label1->Caption = FloatToStr(angle); 

    // само смещение изображения относительно угла, синусов, косинусов

    img->Left += step*cos(angle);
    img->Top += step*sin(angle);
}

//---------------------------------------------------------------------------


// Цитата с какого-то сайта по программированию :
// "Если бы программисты проектиовали здания так же, как пришут программы, то
//                 первый же залетевший стриж уничтожил бы цивилизацию"
