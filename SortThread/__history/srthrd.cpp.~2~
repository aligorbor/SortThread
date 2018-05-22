//---------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
//---------------------------------------------------------------------
USEFORM("srtthrdMain.CPP", SDIAppForm);
USEFORM("ABOUT.CPP", AboutBox);
//---------------------------------------------------------------------------
int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
	Application->Initialize();
	Application->CreateForm(__classid(TSDIAppForm), &SDIAppForm);
		Application->CreateForm(__classid(TAboutBox), &AboutBox);
		Application->Run();

	return 0;
}
//---------------------------------------------------------------------
