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
	using linePointType = double;
	Line(linePointType posx1, linePointType posy1, linePointType posx2, linePointType posy2)
		: startx{posx1}, starty{posy1}, endx{ posx2 }, endy{ posy2 }
	{
		alpha = std::atan(static_cast<double>(endy - starty) / static_cast<double>(endx - startx));
	}

	bool isUnder(linePointType x, linePointType y) 
	{
		if (x < startx || x > endx) return false;
		linePointType yonline = (x - startx)*std::tan(alpha) + starty;
		if (y < yonline) return true;
		return false;
	}

	bool isOver(linePointType x, linePointType y)
	{
		if (x < startx || x > endx) return false;
		linePointType yonline = (x - startx)*std::tan(alpha) + starty;
		if (y > yonline) return true;
		return false;
	}

	bool isOnRight(linePointType x, linePointType y)
	{
		if (y < starty || y > endy) return false;
		linePointType xonline = (endy - y)*std::tan(M_PI/2. -alpha) + startx;
		if (xonline > x) return true;
		return false;
	}

	bool isOnLeft(linePointType x, linePointType y)
	{
		if (y < starty || y > endy) return false;
		linePointType xonline = (endy - y)*std::tan(M_PI / 2. - alpha) + startx;
		if (xonline < x) return true;
		return false;
	}

	linePointType yVertDist(linePointType x, linePointType y)
	{
		linePointType yonline = (x - startx)*std::tan(alpha) + starty;
		return std::abs(yonline - y);
	}

	linePointType xHorDist(linePointType x, linePointType y)
	{
		linePointType xonline = (endy - y)*std::tan(M_PI / 2. - alpha) + startx;
		return std::abs(xonline - x);
	}

	linePointType dist(linePointType x, linePointType y)
	{
		wxPoint a(endx - startx, endy - starty);
		wxPoint b(x - startx, y - starty);
		double area = std::abs(a.x*b.y - a.y*b.x);
		return area/std::sqrt(a.x*a.x + a.y*a.y);
	}

private:
	linePointType startx;
	linePointType starty;
	linePointType endx;
	linePointType endy;
	double alpha;

};

void Openfile::InitHexa()
{
	double a = 125;
	int ysize = 2*a;
	int xsize = a*std::sqrt(3);
	hexa1 = new wxImage(wxSize(xsize, ysize));
	unsigned char * imageData = hexa1->GetData();

	int ny = hexa1->GetHeight();
	int nx = hexa1->GetWidth();
	int x0 = nx / 2;
	int y0 = ny / 2;

	int y1 = (nx / 2)*std::tan(M_PI / 6.);
	int y2 = y1 + a;
	int y3 = ny - 1;
	
	Line botRight(x0, 0, nx - 1, y1);
	Line botLeft(0, y1, x0, 0);
	Line left(0, y1, 0, y2);
	Line right(nx - 1, y1, nx - 1, y2);
	Line mid(x0, 0, x0, y0);
	Line midLeft(0, y2, x0, y0);
	Line midRight(x0, y0, nx - 1, y2);
	Line topLeft(0, y2, x0, y3);
	Line topRight(x0, y3, nx - 1, y2);

	//init colors
	for (int i = 0; i < nx; i++)
	{
		for (int j = 0; j < ny; j++)
		{
			if (midRight.isUnder(i, j))
			{
				imageData[1 + 3 * i + (ny-1-j) * nx * 3] = 255;
				double ydist = midRight.yVertDist(i, j);
				imageData[0 + 3 * i + (ny - 1 - j) * nx * 3] = 255*(1-ydist/(ysize/2.));
				double xdist = right.xHorDist(i, j);
				imageData[2 + 3 * i + (ny - 1 - j) * nx * 3] = 255 * (xdist / (xsize/2.));
			}
			else if (midLeft.isUnder(i, j))
			{
				imageData[2 + 3 * i + (ny - 1 - j) * nx * 3] = 255;
				double ydist = midLeft.yVertDist(i, j);
				imageData[0 + 3 * i + (ny - 1 - j) * nx * 3] = 255 * (1 - ydist / (ysize/2.));
				double xdist = left.xHorDist(i, j);
				imageData[1 + 3 * i + (ny - 1 - j) * nx * 3] = 255 * (xdist / (xsize/2.));
			}
			else
			{
				imageData[0 + 3 * i + (ny - 1 - j) * nx * 3] = 255;
				double green = topLeft.dist(i, j);
				imageData[1 + 3 * i + (ny - 1 - j) * nx * 3] = 255 *(green / (xsize / 2.));
				double blue = topRight.dist(i, j);
				imageData[2 + 3 * i + (ny - 1 - j) * nx * 3] = 255 *(blue / (xsize / 2.));
			}
		}
	}

	//white outside of hexa
	for (int i = 0; i < nx; i++)
	{
		for (int j = 0; j < ny; j++)
		{
			if (botRight.isUnder(i, j) || botLeft.isUnder(i, j) || topRight.isOver(i, j) || topLeft.isOver(i, j))
			{
				imageData[0 + 3 * i + (ny-1-j) * nx * 3] = 255;
				imageData[1 + 3 * i + (ny-1-j) * nx * 3] = 255;
				imageData[2 + 3 * i + (ny - 1 - j) * nx * 3] = 255;
			}
		}
	}
}
