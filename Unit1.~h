//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
#include <ImgList.hpp>
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
    TImageList *ImageList1;
    TImage *imgCue;
    TImage *imgBall;
    TTimer *MainTimer;
    TImage *FrontGround;
    TImage *BackGround;
    TLabel *Label1;
    void __fastcall MainTimerTimer(TObject *Sender);
    void __fastcall FrontGroundMouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y);
    void __fastcall FrontGroundClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
 