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
	wxImage hexa1;
	wxImage hexaCpy;
	wxWindow* hexaWindow;
	wxStaticText* debugTextField;
	wxPoint* circPoint = NULL;
	wxPoint* circOnHexPoint = NULL;
	int mouseX = 0;
	int mouseY = 0;

	void OnMouseDown(wxMouseEvent &event);

	void OnMouseDownHexa(wxMouseEvent &event);

	bool isOnHexa(const wxPoint& p);

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

	void MarkHexaColor(const wxColor& color);

};
