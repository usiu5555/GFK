#include "openfile.h"

Openfile::Openfile(const wxString & title)
	: wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(300, 200))
{

	wxMenuBar *menubar = new wxMenuBar;
	wxMenu *file = new wxMenu;
	file->Append(wxID_OPEN, wxT("&Open"));
	menubar->Append(file, wxT("&File"));
	SetMenuBar(menubar);

	Connect(wxID_OPEN, wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler(Openfile::OnOpen));

	wxImage::AddHandler(new wxJPEGHandler);           // Dodajemy handlery do formatow
	wxImage::AddHandler(new wxPNGHandler);            // z jakich bedziemy korzytsac

	m_scrolledWindow = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL | wxVSCROLL);
	m_scrolledWindow->SetScrollRate(5, 5);
	m_scrolledWindow->SetVirtualSize(wxSize(2000, 2000));
	m_scrolledWindow->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(Openfile::m_scrolledWindow_update), NULL, this);
	m_scrolledWindow->Connect(wxEVT_SCROLLWIN_THUMBTRACK, wxScrollWinEventHandler(Openfile::OnScroll), NULL, this);
	Center();
}


void Openfile::OnOpen(wxCommandEvent& event)
{

	wxFileDialog * openFileDialog = new wxFileDialog(this);

	if (openFileDialog->ShowModal() == wxID_OK) {
		wxString fileName = openFileDialog->GetPath();
		if (imageOrg.LoadFile(fileName))
		{
			imageCpy = imageOrg.Copy();
			Repaint();
		}
	}
}

void Openfile::Repaint()
{
	wxBitmap bitmap(imageCpy);          // Tworzymy tymczasowa bitmape na podstawie Img_Cpy
	wxClientDC dc(m_scrolledWindow);   // Pobieramy kontekst okna
	m_scrolledWindow->DoPrepareDC(dc); // Musimy wywolac w przypadku wxScrolledWindow, zeby suwaki prawidlowo dzialaly
	dc.Clear();
	dc.DrawBitmap(bitmap, 0, 0, true); // Rysujemy bitmape na kontekscie urzadzenia
}