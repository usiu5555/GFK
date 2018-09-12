#pragma once
#include <wx/wx.h>
#include <wx/event.h>

class Openfile : public wxFrame
{
public:
	Openfile(const wxString& title);

	void OnOpen(wxCommandEvent& event);
	wxScrolledWindow* m_scrolledWindow;
	wxImage imageOrg;
	wxImage imageCpy;

private:
	void m_scrolledWindow_update(wxUpdateUIEvent& event)
	{
		Repaint();
	}
	void OnScroll(wxScrollWinEvent& event)
	{
		Repaint();
	}
	void Repaint();
};
