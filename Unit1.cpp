//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

// ������������� �������� ���������� Math.h, ������ ��� ������
#include <math.h>

#include "Unit1.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
// ��������� � ������� �������� ���������� ���� �����, ����� �������
// �������� ������ ������� ����, �� ������� ��� ������ �����������
struct linesCrd {
    int x1, y1; // ������ �����
    int x2, y2; // � ������
};

// ���������� ������ ��������, �� ����� �� ������ �������
linesCrd lineTop;    // ������
linesCrd lineBottom; // �����
linesCrd lineLeft;   // �����
linesCrd lineRight;  // � ������


int cue_swing = 0; // �������� ���

int index = 0;
int index2 = 0;

bool animateCue = false;  // ��� ������� �������� ���, ������� �� ��������
bool moveBall = false;   // ���. �������, ���� �� ����������� ���
bool canHitBall = true;  // ���. �������, ����� �� "�������" ���
bool calcAngle = true;   // ���. �������, ���� �� ��������� ���� ������ ����

double ballEnergy = 10;  // ������������ ������� ����. ���������� ��������, ��
                         // ������� ������������ ��� ����� �����. ����������� �� ��������
double angle = 0;  // ��������� ����. ���������� � ��������

int mPosX=0, mPosY=0;  // ��������� ������� ����
int destX =0, destY=0;  // ������ � ���� �������� ��������� ������� ����, ���������� �� ���������� ����������

//---------------------------------------------------------------------------
void MoveImage(TImage *img, int destX, int destY, int step, bool &needMove);

// �����������, � ������� ����� ���������� �����-�� ��������
// �����������, ����� �������� �����.
//    ����� �������� �����, ��������� � ��������� ����������, ��
//    ����������� ���������� ������, ������� � �� ������ ���������� �
//    ��� "�� ���������" ����������� �����
__fastcall TForm1::TForm1(TComponent* Owner)
    : TForm(Owner)
{
    Form1->DoubleBuffered = true;  // ������� ���������� ���. �����. ��������� �������� ����������� ��� ���� ����� �������

    // �������� ������� ���� �� ������ ����, ��� �� �� ��� ��� ���
    BackGround->SendToBack();

    // ����������� "������� �����������" ������ ���� ��������� ��������

    FrontGround->BringToFront();

    // ����������� ��������� ������ ����
    // ��������  ������������ ��������
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

//    � ��������� ������� �������� ����. ����� ��������
//    ������� �������� "animateCue" �� "true"

void DrawImage(TImageList *imgList, int &index, TImage *img, double angle)
{
    if(index>=imgList->Count)
        index = 0;

    TRect rect(0,0,img->Width, img->Height);
    img->Canvas->Brush->Color=clBlack;
    img->Canvas->FillRect(rect);

    //1 ��������� ������ �������
    XFORM savetagx;
    SetGraphicsMode(img->Canvas->Handle, GM_ADVANCED);// ��������� ������������ ������
    GetWorldTransform(img->Canvas->Handle, &savetagx);

    //���������� ����� �������
    XFORM xform;
    //ZeroMemory(&xform, sizeof(xform));
    xform.eM11 = cos(angle);
    xform.eM12 = sin(angle);
    xform.eM21 = -sin(angle);
    xform.eM22 = cos(angle);
    xform.eDx  = img->Width/2;
    xform.eDy  = img->Height/2;

    ModifyWorldTransform(img->Canvas->Handle, &xform, MWT_LEFTMULTIPLY);

    //���������� ��������
    imgList->Draw(img->Canvas,-img->Width/2,-img->Height/2, index);

    //������� ������ �������
    SetWorldTransform(img->Canvas->Handle, &savetagx);
    //img->Invalidate();
}

//   �������� ������, � ������� �������� ��� �������� ��������
void __fastcall TForm1::MainTimerTimer(TObject *Sender)
{
    if (animateCue) // ���������� ��� ����� ����������
    {

        double angle = atan2(mPosY - imgCue->Top - imgCue->Height/2, mPosX - imgCue->Left-imgCue->Width/2);
        DrawImage(ImageList1, index2, imgCue, angle);
    }
    //------------
    if (moveBall) // �������� ����
    {
        bool needWalk; // ����������, ���������� �� ��, �������� �� ���
        ballEnergy -= 0.05; // ���������� ������������ ������� ���� � ������� �������
        MoveImage(imgBall, destX, destY, ballEnergy, needWalk); // ����� ������� �������� ����

        //Timer3->Enabled = needWalk; // �����������
    }
    //Form1->Canvas->Rectangle(100,100,103,103); // ��� �����������
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FrontGroundMouseMove(TObject *Sender, TShiftState Shift,
      int X, int Y)
{
    // ��������� ��������� ������� ���� � ���� ������ "��������� ����"
    mPosX = X;
    mPosY = Y;
}
//---------------------------------------------------------------------------
// ��� ��������� �����������, ��������� "FrontGround'��" ������������ ��� ���
// ��� ����� �� ���� ����� ��������� ���, � � ������ ����� ������������� ��� "�����������" ���������
void __fastcall TForm1::FrontGroundClick(TObject *Sender)
{
    if (canHitBall) // ���� ��� ��������, �� ��� ����� ����� ��������� � ��������
    {
        canHitBall = false; // �� ��������� ������ ���, �� ��� ���, ���� �� �� ����������� ���

        destX = mPosX; // ��������� ��������� ����� ������������ ����, � ������� �����
        destY = mPosY; // ��������� ���, � ������, ������������ ������� ����� ����������� ����
                       // �� �������� ����� ��� ���������
        moveBall = true;
    }
}
//---------------------------------------------------------------------------
int Distance(linesCrd Line, int x0, int y0) // ��������� � ���� ���������
{
    // ������������ ���������� �� ������ ���� �� ������� ����

    int x21 = Line.x2 - Line.x1;
    int y21 = Line.y2 - Line.y1;

    int res1 = abs(y21*x0 - x21*y0 + Line.x2*Line.y1 - Line.y2*Line.x1);
    int res2 = sqrt(y21*y21 + x21*x21);

    return res1/res2; //  ���������� ����� �� �������
}


void MoveImage(TImage *img, int destX, int destY, int step, bool &needMove)
{
    // ��������� ���� ������ ����
    // ����������� ���� ���, ����� �� ���� "���� ����"
    if (calcAngle)
    {
        // ������������� ���������� ���� ������ ���, �����
        // ���� �������� ����� ������
        calcAngle = false;


        int midX = img->Left + img->Width/2;
        int midY = img->Top + img->Height/2;
        int dx = (destX - midX);
        int dy = (destY - midY);

        // �������� ���� ����������� � ��������� ���������� ����������, �������
        // ����� ������������ ��������, �� ��� �� ����� ����� � �������, �����
        // �������� ���� ��� ������.
        angle = atan2(dy, dx);
    }
    // ������� ��������� ����. ����� ������� �� �������, ��� ������ ������������
    if (step <= 0) //abs(dx)<step && abs(dy)<step // �������
    {
        // ������� "�����������" �������� �������� ����������, ��� ��
        // ��� ����� "����" �� �������� ��� ����
        needMove = false;
        ballEnergy = 10;
        moveBall = false;
        canHitBall = true;
        calcAngle = true;

        return; // ����� �� ���� �������
    }
    needMove = true;

    //if ( Distance(lineRight, Form1->imgBall->Left+15, Form1->imgBall->Top+15) <= 15 ) //

    // ��� �������, ���� ��� "��������" ��� ��� ������ ���� �� ������ ����
    if (Distance(lineTop, Form1->imgBall->Left+15, Form1->imgBall->Top+15)<15)
    {
        angle *= -1; // �������� ��� ����� ���
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

    // ���� �������� ����������� ������������ ����, �������, ���������

    img->Left += step*cos(angle);
    img->Top += step*sin(angle);
}

//---------------------------------------------------------------------------


// ������ � ������-�� ����� �� ���������������� :
// "���� �� ������������ ������������ ������ ��� ��, ��� ������ ���������, ��
//                 ������ �� ���������� ����� ��������� �� �����������"
