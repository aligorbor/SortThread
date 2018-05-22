//---------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "srtthrdMain.h"
#include "About.h"
#include "thrdUnit.h"
//---------------------------------------------------------------------
#pragma resource "*.dfm"
TSDIAppForm *SDIAppForm;

TSortArray A;
TBubbleSort * BubbleSort;
TSelectionSort * SelectionSort;
TQuickSort * QuickSort;

//---------------------------------------------------------------------
__fastcall TSDIAppForm::TSDIAppForm(TComponent *AOwner)
	: TForm(AOwner)
{
}
//---------------------------------------------------------------------

void __fastcall TSDIAppForm::FileNew1Execute(TObject *Sender)
{
  // Do nothing        
}
//---------------------------------------------------------------------------

void __fastcall TSDIAppForm::FileOpen1Execute(TObject *Sender)
{
  OpenDialog->Execute();        
}
//---------------------------------------------------------------------------

void __fastcall TSDIAppForm::FileSave1Execute(TObject *Sender)
{
  SaveDialog->Execute();        
}
//---------------------------------------------------------------------------


void __fastcall TSDIAppForm::FileExit1Execute(TObject *Sender)
{
  Close();        
}
//---------------------------------------------------------------------------

void __fastcall TSDIAppForm::HelpAbout1Execute(TObject *Sender)
{
  AboutBox->ShowModal();        
}
//---------------------------------------------------------------------------

void __fastcall TSDIAppForm::btnArrayClick(TObject *Sender)
{
	// Generate random data
	for (int i = 0; i < ARRAY_SIZE; i++)
	  A[i] = Random(ARRAY_SIZE - 1);

}
//---------------------------------------------------------------------------

void __fastcall TSDIAppForm::rgSynchrClick(TObject *Sender)
{
  switch (rgSynchr->ItemIndex) {
	case 0:   {
		UseSynchronize = true;
		UseQueue = false;
		UseMutex = false;
	}
	  break;
	case 1:   {
		UseSynchronize = false;
		UseQueue = true;
		UseMutex = false;
	}
	  break;
	case 2:   {
		UseSynchronize = false;
		UseQueue = false;
		UseMutex = true;
	}
	  break;
	default: {
		UseSynchronize = true;
		UseQueue = false;
		UseMutex = false;
	}
  }
}
//---------------------------------------------------------------------------

void __fastcall TSDIAppForm::btnStartClick(TObject *Sender)
{
  const int maxThrNumber = 3;
  THandle Handles[maxThrNumber];
  TSortThread *Threads[maxThrNumber];
  int i, ThrNumber = 0;
  if (UseMutex) myMutex = new TMutex(false);
  if (cbBubbleSort->Checked )  {
	Threads[ThrNumber] = new TBubbleSort(pbSort, A, true);
	Handles[ThrNumber] = Threads[ThrNumber]->Handle;
	ThrNumber++;
  }
  if (cbSelectionSort->Checked )  {
	Threads[ThrNumber] = new TSelectionSort(pbSort, A, true);
	Handles[ThrNumber] = Threads[ThrNumber]->Handle;
	ThrNumber++;
  }
  if (cbQuickSort->Checked )  {
	Threads[ThrNumber] = new TQuickSort(pbSort, A, true);
	Handles[ThrNumber] = Threads[ThrNumber]->Handle;
	ThrNumber++;
  }
  for (i = 0; i < ThrNumber; i++) {
	 Threads[i]->Start();
  }

  /*WaitForMultipleObjects(ThrNumber, (void* const *)&Handles, true, INFINITE);

  for (i = 0; i < ThrNumber; i++) {
	 delete Threads[i];
  }   */
  if (UseMutex) {
	delete myMutex;
  }
  }
//---------------------------------------------------------------------------

