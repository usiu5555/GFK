#include "openfile.h"
#include <cmath>
#include <vector>

Openfile::Openfile(const wxString & title)
	: wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(1024, 720))
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

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer(wxHORIZONTAL);

	m_scrolledWindow = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxSize(300,300), wxHSCROLL | wxVSCROLL);
	m_scrolledWindow->SetScrollRate(5, 5);
	m_scrolledWindow->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(Openfile::m_scrolledWindow_update), NULL, this);
	m_scrolledWindow->Connect(wxEVT_SCROLLWIN_THUMBTRACK, wxScrollWinEventHandler(Openfile::OnScroll), NULL, this);
	bSizer1->Add(m_scrolledWindow, 1, wxEXPAND | wxALL, 5);

	hexaWindow = new wxWindow(this, wxID_ANY, wxDefaultPosition, wxSize(300,300));

	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer(wxVERTICAL);
	bSizer2->SetMinSize(wxSize(300,300));
	bSizer2->Add(hexaWindow, 0, wxALL , 5);
	bSizer1->Add(bSizer2, 0, wxALL, 5);

	this->SetSizer(bSizer1);
	this->Layout();
	this->Centre(wxBOTH);
	Center();
	InitHexa();

}


void Openfile::OnOpen(wxCommandEvent& event)
{

	wxFileDialog * openFileDialog = new wxFileDialog(this);

	if (openFileDialog->ShowModal() == wxID_OK) {
		wxString fileName = openFileDialog->GetPath();
		if (imageOrg.LoadFile(fileName))
		{
			imageCpy = imageOrg.Copy();
			m_scrolledWindow->SetVirtualSize(imageOrg.GetSize());
			Repaint();
		}
	}
}

void Openfile::Repaint()
{
	wxBitmap bitmap(imageCpy);          // Tworzymy tymczasowa bitmape na podstawie Img_Cpy
	wxBitmap hexabit(*hexa1);          
	wxClientDC dc(m_scrolledWindow);   // Pobieramy kontekst okna
	wxClientDC dc2(hexaWindow);
	m_scrolledWindow->DoPrepareDC(dc); // Musimy wywolac w przypadku wxScrolledWindow, zeby suwaki prawidlowo dzialaly
	dc.Clear();
	dc2.Clear();
	dc.DrawBitmap(bitmap, 0, 0, false); // Rysujemy bitmape na kontekscie urzadzenia
	dc2.DrawBitmap(hexabit, 0, 0, false);
}

class Line
{
public:
	Line(int posx1, int posy1, int posx2, int posy2)
		: startx{posx1}, starty{posy1}, endx{ posx2 }, endy{ posy2 }
	{
		alpha = std::atan(static_cast<double>(endy - endx) / static_cast<double>(endy - starty));
	}

	bool isUnder(int x, int y) 
	{
		if (x < startx || x > endx) return false;
		int yonline = (x - startx)*std::tan(alpha);
		if (y < yonline) return true;
		return false;
	}

	bool isOnRight(int x, int y)
	{
		if (y < starty || y > endy) return false;
		int xonline = (endy - y)*std::tan(M_PI/2. -alpha);
		if (xonline > x) return true;
		return false;
	}

public:
	int startx;
	int starty;
	int endx;
	int endy;
	double alpha;
};

void Openfile::InitHexa()
{

}
