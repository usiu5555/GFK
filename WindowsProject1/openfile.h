#pragma once
#include <wx/wx.h>
#include <wx/event.h>

class Openfile : public wxFrame
{
public:
	Openfile(const wxString& title);

	void OnOpen(wxCommandEvent& event);

private:
	wxScrolledWindow* m_scrolledWindow;
	wxImage imageOrg;
	wxImage imageCpy;
	wxImage* hexa1;
	wxWindow* hexaWindow;

	void m_scrolledWindow_update(wxUpdateUIEvent& event)
	{
		Repaint();
	}
	void OnScroll(wxScrollWinEvent& event)
	{
		Repaint();
	}
	void Repaint();

	void InitHexa();

};
