// wxWidgets "Hello World" Program
// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include "openfile.h"
class MyApp : public wxApp
{
public:
	virtual bool OnInit();
};

enum
{
	ID_Hello = 1
};

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
	ColorCorrGUIFrame *open = new ColorCorrGUIFrame(wxT("Color Correction"));
	open->Show(true);	

	return true;
}