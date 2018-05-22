/*
This example demonstrates the use and the need for managing processing of threads competing for the same resource. This example manages three sorting routines in three separate threads using the following mechanisms:

1. The TThread Synchronize method executes the procedure it is handed in the main thread. This is required for executing VCL from a thread. It also can be used to avoid contension over shared resources (in this case, WaitForPaint).

2. TThread Queue can be used to queue access to a shared resource. Queue cannot be used to execute VCL tasks.

3. A mutex can be used to hold the access of a resource.

This example allows you to turn off the use of Synchronise, Queue and the mutex. Use any one of these to avoid conflicts. Turning them all off results in conflicts.
*/

// From SortThds.h:

#define ARRAY_SIZE 512
typedef int TSortArray[ARRAY_SIZE];

//---------------------------------------------------------------------------

class TSortThread: public TThread
{
__published:	// IDE-managed Components
private: // User declarations
	TPaintBox *FBox;
	TSortArray FSortArray;
	int FSize;
	int FA; int FB; int FI; int FJ;
	void __fastcall DoVisualSwap();
	void __fastcall WaitBeforePaint();
	void __fastcall ConflictMsg();
protected:  // User declarations
	void __fastcall Execute();
	void __fastcall VisualSwap(int A, int B, int I, int J);
	virtual void __fastcall Sort(TSortArray &A);
public:  // User declarations
	__fastcall TSortThread(TPaintBox *Box, TSortArray &SortArray);
};
//---------------------------------------------------------------------------


/* TBubbleSort */
//---------------------------------------------------------------------------

 class TBubbleSort: public TSortThread
{
__published:	// IDE-managed Components
protected:  // User declarations
	void __fastcall Sort( TSortArray &A);
public:
	__fastcall TBubbleSort(TPaintBox *Box,  TSortArray &SortArray);
};
//---------------------------------------------------------------------------


/* TSelectionSort */
//---------------------------------------------------------------------------

 class TSelectionSort: public TSortThread
{
__published:	// IDE-managed Components
protected:  // User declarations
	void __fastcall Sort( TSortArray &A);
public:
	__fastcall TSelectionSort(TPaintBox *Box,  TSortArray &SortArray);
};
//---------------------------------------------------------------------------


/* TQuickSort */
//---------------------------------------------------------------------------

 class TQuickSort: public TSortThread
{
__published:	// IDE-managed Components
protected:  // User declarations
	void __fastcall Sort( TSortArray &A);
	void __fastcall QuickSort(TSortArray &A, int iLo, int iHi);
public:
	__fastcall TQuickSort(TPaintBox *Box,  TSortArray &SortArray);
};
//---------------------------------------------------------------------------


extern PACKAGE void __fastcall PaintLine(TCanvas *Canvas, int I, int Len);
extern PACKAGE int ASize(TSortArray &A);
extern bool UseSynchronize;
extern bool UseQueue;
extern bool useMutex;
extern int mysleep, conflict;
extern TMutex *myMutex;// From SortThds.cpp:

#include "SortThds.h"
#include "ThSort.h"

//---------------------------------------------------------------------------

bool UseSynchronize;
bool UseQueue;
bool useMutex;
int mysleep;
TMutex *myMutex;

/*
Notice that it is handy to have WaitBeforePaint be a member of the thread base class TSortThread.  This allows Synchronize calls to ConflictMsg from inside of WaitBeforePaint, something that would not be possible for a proc outside of the thread classes.  This means that there are three instances of WaitBeforePaint running, but we still get the contention we are looking for over the global counter mysleep.
*/

void __fastcall TSortThread::WaitBeforePaint()
{
  int supersleep;
  if (mysleep != 0)
  {
	Synchronize(ConflictMsg);
	Terminate();
	return;
  };

  mysleep = 50000;
  while (mysleep > 0) // use a process intensive loop here.  We are trying to block.
  {
	supersleep  = mysleep*mysleep;
	mysleep  = mysleep - 1;
  };
};

void __fastcall PaintLine(TCanvas *Canvas, int i, int len)
{
  Canvas->MoveTo(1, int(i/2.22)+1);
  Canvas->LineTo(int(len/2.92)+1, int(i/2.22)+1);
}

//---------------------------------------------------------------------------

int ASize(TSortArray &A)
{
  return sizeof(A)/sizeof(int);
}

/* TSortThread */

//---------------------------------------------------------------------------

void __fastcall TSortThread::Sort(TSortArray &)
{
}

//---------------------------------------------------------------------------

__fastcall TSortThread::TSortThread(TPaintBox *Box, TSortArray &SortArray)
		: TThread(false)
{
  FBox = Box;
  FSize = ASize(SortArray);
  for(int i=0; i<FSize; ++i) {
	  FSortArray[i] = SortArray[i];
  }
  FreeOnTerminate = true;
}

/* Since DoVisualSwap uses a VCL component (i.e., the TPaintBox) it should never be called directly by this thread.  DoVisualSwap should be called by passing it to the Synchronize method which causes DoVisualSwap to be executed by the main VCL thread, avoiding multi-thread conflicts. */

//---------------------------------------------------------------------------

void __fastcall TSortThread::ConflictMsg()
{
	MessageDlg(Format("Conflict! mysleep = %d.",
	  ARRAYOFCONST((mysleep))), mtError, TMsgDlgButtons() << mbOK, 0);
}

void __fastcall TSortThread::DoVisualSwap()
{
  FBox->Canvas->Pen->Color = clBtnFace;
  PaintLine(FBox->Canvas, FI, FA);
  PaintLine(FBox->Canvas, FJ, FB);
  FBox->Canvas->Pen->Color = clRed;
  PaintLine(FBox->Canvas, FI, FB);
  PaintLine(FBox->Canvas, FJ, FA);
}

/* VisusalSwap is a wrapper on DoVisualSwap, used to decide how to call the shared non-VCL resource inside of Synchronize, Queue or the mutex.
*/

//---------------------------------------------------------------------------

void __fastcall TSortThread::VisualSwap(int a, int b, int i, int j)
{
  FA = a;
  FB = b;
  FI = i;
  FJ = j;

  // shared resource that is not VCL
   if (useMutex)
	myMutex->Acquire();
  if (UseSynchronize) Synchronize(WaitBeforePaint);
  else
  {
	if (UseQueue) Queue(WaitBeforePaint);
	else WaitBeforePaint();
  }
  if (useMutex) myMutex->Release();

  Synchronize(DoVisualSwap);
}

/* The Execute method is called when the thread starts */
//---------------------------------------------------------------------------

void __fastcall TSortThread::Execute()
{
  TThread::NameThreadForDebugging((AnsiString)ClassName());
  Sort(&FSortArray);
}

/* TBubbleSort */
//---------------------------------------------------------------------------

__fastcall TBubbleSort::TBubbleSort(TPaintBox *Box, TSortArray &SortArray)

		:TSortThread(Box,SortArray)
{
}

//---------------------------------------------------------------------------

void __fastcall TBubbleSort::Sort(TSortArray &A)
{
  for(int i = ASize(A)-1; i > 0; --i)
	for(int j = 0; j < ASize(A) - 1; ++j)
	  if( A[j] > A[j + 1] )

	  {
		VisualSwap(A[j], A[j + 1], j, j + 1);
		int t = A[j];
		A[j] = A[j + 1];
		A[j + 1] = t;
		if( Terminated ) return;
	  }
}

/* TSelectionSort */
//---------------------------------------------------------------------------

__fastcall TSelectionSort::TSelectionSort(TPaintBox *Box, TSortArray &SortArray)

		:TSortThread(Box,SortArray)
{
}

//---------------------------------------------------------------------------

void __fastcall TSelectionSort::Sort(TSortArray &A)
{
  for(int i = 0; i < ASize(A) - 2; i++)
	for(int j = ASize(A)-1; j > i + 1; j--)
	  if( A[i] > A[j] )

	  {
		VisualSwap(A[i], A[j], i, j);
		int t = A[i];
		A[i] = A[j];
		A[j] = t;
		if( Terminated ) return;
	  }
}

/* TQuickSort */
//---------------------------------------------------------------------------

__fastcall TQuickSort::TQuickSort(TPaintBox *Box, TSortArray &SortArray)

		:TSortThread(Box,SortArray)
{
}

//---------------------------------------------------------------------------

  void __fastcall TQuickSort::QuickSort(TSortArray &A, int iLo, int iHi)
{
	int Lo = iLo;
	int Hi = iHi;
	int Mid = A[(Lo + Hi) / 2];
	do {

	  while( A[Lo] < Mid ) Lo++;
	  while( A[Hi] > Mid ) Hi--;
	  if( Lo <= Hi )

	  {
		VisualSwap(A[Lo], A[Hi], Lo, Hi);
		int t = A[Lo];
		A[Lo] = A[Hi];
		A[Hi] = t;
		Lo++;
		Hi--;
	  }
	} while(Lo <= Hi);
	if( Hi > iLo ) QuickSort(A, iLo, Hi);
	if( Lo < iHi ) QuickSort(A, Lo, iHi);
	if( Terminated ) return;
  }

//---------------------------------------------------------------------------
void __fastcall TQuickSort::Sort(TSortArray &A)
{

  QuickSort(A, 0, ASize(A)-1);

}
