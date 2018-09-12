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
	//MyFrame *frame = new MyFrame();
	//frame->Show(true);
	Openfile *open = new Openfile(wxT("Openfile"));
	open->Show(true);	

	return true;
}
//MyFrame::MyFrame()
//	: wxFrame(NULL, wxID_ANY, "Hello World")
//{
//
//	Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);
//}
//void MyFrame::OnExit(wxCommandEvent& event)
//{
//	Close(true);
//}