#include "ColorCorr.h"
#include <cmath>
#include <vector>
#include <list>

ColorCorrGUIFrame::ColorCorrGUIFrame(const wxString & title)
	: wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(1024, 720))
{
	wxMenuBar *menubar = new wxMenuBar;
	wxMenu *file = new wxMenu;
	file->Append(wxID_OPEN, wxT("&Open"));
	menubar->Append(file, wxT("&File"));
	SetMenuBar(menubar);

	Connect(wxID_OPEN, wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler(ColorCorrGUIFrame::OnOpen));

	wxImage::AddHandler(new wxJPEGHandler);
	wxImage::AddHandler(new wxPNGHandler);
	wxImage::AddHandler(new wxBMPHandler);

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer(wxHORIZONTAL);

	imageScrolledWindow = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxSize(300, 300), wxHSCROLL | wxVSCROLL);
	imageScrolledWindow->SetScrollRate(5, 5);
	bSizer1->Add(imageScrolledWindow, 1, wxEXPAND | wxALL, 5);
	imageScrolledWindow->SetBackgroundColour(backgroundColor);

	GUIWindow = new wxWindow(this, wxID_ANY, wxDefaultPosition, wxSize(300, 700));
	GUIWindow->SetBackgroundColour(backgroundColor);

	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer(wxVERTICAL);
	bSizer2->SetMinSize(wxSize(300, 300));

	chosenColorTextField = new wxStaticText(this, wxID_ANY, _("Choose image (File->Open)."), wxDefaultPosition, wxDefaultSize, 0);
	chosenColorTextField->Wrap(-1);
	bSizer2->Add(chosenColorTextField, 0, wxALL, 5);

	execute = new wxButton(this, wxID_ANY, _("Execute"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer2->Add(execute, 0, wxALL | wxEXPAND, 5);

	bSizer2->Add(GUIWindow, 0, wxALL, 5);
	sliderProportional = new wxSlider(this, wxID_ANY | wxSL_LABELS | wxSL_AUTOTICKS, 1, 1, 100);
	sliderOrgImgFrac = new wxSlider(this, wxID_ANY | wxSL_LABELS, 0, 0, 100);
	sliderSaturation = new wxSlider(this, wxID_ANY | wxSL_LABELS, 0, -100, 100);
	sliderBrightness = new wxSlider(this, wxID_ANY | wxSL_LABELS, 0, 0, 255);

	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer(wxVERTICAL);
	bSizer3->SetMinSize(wxSize(100, 100));

	bSizer3->Add(new wxStaticText(this, wxID_ANY, _("Proportionality coefficient [1,2]"), wxDefaultPosition, wxDefaultSize, 0), 0, wxALL, 5);
	bSizer3->Add(sliderProportional, 0, wxALL, 5);
	bSizer3->Add(new wxStaticText(this, wxID_ANY, _("Orginal image fraction [0,1]"), wxDefaultPosition, wxDefaultSize, 0), 0, wxALL, 5);
	bSizer3->Add(sliderOrgImgFrac, 0, wxALL, 5);
	bSizer3->Add(new wxStaticText(this, wxID_ANY, _("Saturation increment [-1,1]"), wxDefaultPosition, wxDefaultSize, 0), 0, wxALL, 5);
	bSizer3->Add(sliderSaturation, 0, wxALL, 5);
	bSizer3->Add(new wxStaticText(this, wxID_ANY, _("Brightness increment [0,255]"), wxDefaultPosition, wxDefaultSize, 0), 0, wxALL, 5);
	bSizer3->Add(sliderBrightness, 0, wxALL, 5);
	bSizer1->Add(bSizer2, 0, wxALL, 5);
	bSizer1->Add(bSizer3, 0, wxALL, 5);
	this->SetSizer(bSizer1);
	this->Layout();
	this->Centre(wxBOTH);
	this->SetBackgroundColour(backgroundColor);
	Center();
	InitHexa();

	imageScrolledWindow->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(ColorCorrGUIFrame::windowUpdate), NULL, this);
	imageScrolledWindow->Connect(wxEVT_SCROLLWIN_THUMBTRACK, wxScrollWinEventHandler(ColorCorrGUIFrame::OnScroll), NULL, this);
	imageScrolledWindow->Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(ColorCorrGUIFrame::OnMouseDown), NULL, this);
	GUIWindow->Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(ColorCorrGUIFrame::OnMouseDownHexa), NULL, this);
	execute->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ColorCorrGUIFrame::ChangeColors), NULL, this);
	sliderProportional->Connect(wxEVT_SLIDER, wxScrollEventHandler(ColorCorrGUIFrame::proportionalHandler), NULL, this);
	sliderOrgImgFrac->Connect(wxEVT_SLIDER, wxScrollEventHandler(ColorCorrGUIFrame::oldImgSliderHandler), NULL, this);
	sliderSaturation->Connect(wxEVT_SLIDER, wxScrollEventHandler(ColorCorrGUIFrame::saturationSliderHandler), NULL, this);
	sliderBrightness->Connect(wxEVT_SLIDER, wxScrollEventHandler(ColorCorrGUIFrame::brightnessSliderHandler), NULL, this);
}

void ColorCorrGUIFrame::proportionalHandler(wxScrollEvent& event) {
	k = sliderProportional->GetValue()/100.*45 + 5;
	wxCommandEvent evt;
	ChangeColors(evt);
	Repaint();
}

void ColorCorrGUIFrame::oldImgSliderHandler(wxScrollEvent& event) {
	orgFrac = sliderOrgImgFrac->GetValue() / 100.;
	wxCommandEvent evt;
	ChangeColors(evt);
	Repaint();
}

void ColorCorrGUIFrame::saturationSliderHandler(wxScrollEvent& event) {
	incSaturation = sliderSaturation->GetValue() / 100.;
	wxCommandEvent evt;
	ChangeColors(evt);
	Repaint();
}

void ColorCorrGUIFrame::brightnessSliderHandler(wxScrollEvent& event) {
	incBrightness = sliderBrightness->GetValue();
	wxCommandEvent evt;
	ChangeColors(evt);
	Repaint();
}

void ColorCorrGUIFrame::OnOpen(wxCommandEvent& event)
{

	wxFileDialog * openFileDialog = new wxFileDialog(this);

	if (openFileDialog->ShowModal() == wxID_OK) {
		wxString fileName = openFileDialog->GetPath();
		if (imageOrg.LoadFile(fileName))
		{
			imageCpy = imageOrg.Copy();
			imageScrolledWindow->SetVirtualSize(imageOrg.GetSize());
			wxClientDC(imageScrolledWindow).Clear();
			Repaint();
		}
		chosenColorTextField->SetLabel("Choose points on image and upper hexagon, then execute.");
	}
}

const int hexaOffsetx = 0;
const int hexaOffsety = 0;

void ColorCorrGUIFrame::Repaint()
{
	if (imageCpy.IsOk())
	{
		wxBitmap bitmap(imageCpy);
		wxClientDC dc(imageScrolledWindow);
		imageScrolledWindow->DoPrepareDC(dc);
		dc.DrawBitmap(bitmap, 0, 0, false);
	}
	if (colorHexagonImage.IsOk() && colorHexagonImageCpy.IsOk())
	{
		wxBitmap hexabit(colorHexagonImage);
		wxBitmap hexabit2(colorHexagonImageCpy);
		wxClientDC dc2(GUIWindow);
		dc2.DrawBitmap(hexabit, hexaOffsetx, hexaOffsety, false);
		dc2.DrawBitmap(hexabit2, 0, 305, false);
		dc2.SetPen(wxPen(wxColor(0, 0, 0), 3));
		if (chosenColorPointOnHex) dc2.DrawCircle(*chosenColorPointOnHex, 5);
		dc2.SetPen(wxPen(wxColor(255, 0, 0), 3));
		if (pointOnHex) dc2.DrawCircle(*pointOnHex, 5);
	}
}

//helper class for color hexagon initialisation
class Line
{
public:
	using linePointType = double;
	Line(linePointType posx1, linePointType posy1, linePointType posx2, linePointType posy2)
		: startx{ posx1 }, starty{ posy1 }, endx{ posx2 }, endy{ posy2 }
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
		linePointType xonline = (endy - y)*std::tan(M_PI / 2. - alpha) + startx;
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
		return area / std::sqrt(a.x*a.x + a.y*a.y);
	}

private:
	linePointType startx;
	linePointType starty;
	linePointType endx;
	linePointType endy;
	double alpha;

};

void ColorCorrGUIFrame::BackgroundOutsideOfHexa(wxImage& hex, const wxColor& bgColor)
{
	int nx = hex.GetWidth();
	int ny = hex.GetHeight();
	int x0 = nx / 2;
	int y0 = ny / 2;

	double a = 125;
	int y1 = (nx / 2)*std::tan(M_PI / 6.);
	int y2 = y1 + a;
	int y3 = ny - 1;
	Line botRight(x0, 0, nx - 1, y1);
	Line botLeft(0, y1, x0, 0);
	Line topLeft(0, y2, x0, y3);
	Line topRight(x0, y3, nx - 1, y2);

	auto R = bgColor.Red();
	auto G = bgColor.Green();
	auto B = bgColor.Blue();

	unsigned char * imageData = hex.GetData();
	for (int i = 0; i < nx; i++)
	{
		for (int j = 0; j < ny; j++)
		{
			if (botRight.isUnder(i, j) || botLeft.isUnder(i, j) || topRight.isOver(i, j) || topLeft.isOver(i, j))
			{
				imageData[0 + 3 * i + (ny - 1 - j) * nx * 3] = R;
				imageData[1 + 3 * i + (ny - 1 - j) * nx * 3] = G;
				imageData[2 + 3 * i + (ny - 1 - j) * nx * 3] = B;
			}
		}
	}
}

void ColorCorrGUIFrame::InitHexa()
{
	double a = 125;
	int ysize = 2 * a;
	int xsize = a * std::sqrt(3);
	colorHexagonImage = wxImage(wxSize(xsize, ysize));
	unsigned char * imageData = colorHexagonImage.GetData();

	int nx = colorHexagonImage.GetWidth();
	int ny = colorHexagonImage.GetHeight();
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
				imageData[1 + 3 * i + (ny - 1 - j) * nx * 3] = 255;
				double ydist = midRight.yVertDist(i, j);
				imageData[0 + 3 * i + (ny - 1 - j) * nx * 3] = 255 * (1 - ydist / (ysize / 2.));
				double xdist = right.xHorDist(i, j);
				imageData[2 + 3 * i + (ny - 1 - j) * nx * 3] = 255 * (xdist / (xsize / 2.));
			}
			else if (midLeft.isUnder(i, j))
			{
				imageData[2 + 3 * i + (ny - 1 - j) * nx * 3] = 255;
				double ydist = midLeft.yVertDist(i, j);
				imageData[0 + 3 * i + (ny - 1 - j) * nx * 3] = 255 * (1 - ydist / (ysize / 2.));
				double xdist = left.xHorDist(i, j);
				imageData[1 + 3 * i + (ny - 1 - j) * nx * 3] = 255 * (xdist / (xsize / 2.));
			}
			else
			{
				imageData[0 + 3 * i + (ny - 1 - j) * nx * 3] = 255;
				double green = topLeft.dist(i, j);
				imageData[1 + 3 * i + (ny - 1 - j) * nx * 3] = 255 * (green / (xsize / 2.));
				double blue = topRight.dist(i, j);
				imageData[2 + 3 * i + (ny - 1 - j) * nx * 3] = 255 * (blue / (xsize / 2.));
			}
		}
	}
	BackgroundOutsideOfHexa(colorHexagonImage, backgroundColor);
	colorHexagonImageCpy = colorHexagonImage.Copy();
}

void ColorCorrGUIFrame::MarkHexaColor(const wxColor& color)
{
	int R = color.Red();
	int G = color.Green();
	int B = color.Blue();
	int maxColor = std::max(std::max(R, G), B);
	int diff = 255 - maxColor;
	R += diff;
	G += diff;
	B += diff;
	onImg = wxColor(R, G, B);

	wxPoint colorPosOnHex;

	if (R > 243 && G > 243 && B > 243)
	{
		colorPosOnHex.x = colorHexagonImage.GetWidth() / 2;
		colorPosOnHex.y = colorHexagonImage.GetHeight() / 2;
	}
	else
	{
		int nx = colorHexagonImage.GetWidth();
		int ny = colorHexagonImage.GetHeight();
		int tol = 2;
		for (int i = 0; i < nx; i++)
		{
			for (int j = 0; j < ny; j++)
			{
				int diffR = std::abs(colorHexagonImage.GetRed(i, j) - R);
				int diffG = std::abs(colorHexagonImage.GetGreen(i, j) - G);
				int diffB = std::abs(colorHexagonImage.GetBlue(i, j) - B);
				if (diffR < tol && diffG < tol && diffB < tol)
				{
					colorPosOnHex.x = i;
					colorPosOnHex.y = j;
				}
			}
		}
	}
	if (chosenColorPointOnHex) delete chosenColorPointOnHex;
	chosenColorPointOnHex = new wxPoint(colorPosOnHex.x, colorPosOnHex.y);
	onImg = color;
}

bool ColorCorrGUIFrame::isOnHexa(const wxPoint& p)
{
	double a = 125;
	int ny = colorHexagonImage.GetHeight();
	int nx = colorHexagonImage.GetWidth();
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

	int i = p.x;
	int j = p.y;
	if (botRight.isUnder(i, j) || botLeft.isUnder(i, j) || topRight.isOver(i, j) || topLeft.isOver(i, j))
	{
		return false;
	}
	return true;

}

void ColorCorrGUIFrame::OnMouseDown(wxMouseEvent &event)
{
	wxPoint pt = wxGetMousePosition();
	wxClientDC dc(imageScrolledWindow);
	pt = event.GetLogicalPosition(dc);
	pt = imageScrolledWindow->CalcUnscrolledPosition(pt);

	onImgOrg = wxColor(wxColor(imageOrg.GetRed(pt.x, pt.y), imageOrg.GetGreen(pt.x, pt.y), imageOrg.GetBlue(pt.x, pt.y)));
	MarkHexaColor(onImgOrg);
}

void ColorCorrGUIFrame::OnMouseDownHexa(wxMouseEvent &event)
{
	wxPoint pt = wxGetMousePosition();
	wxClientDC dc(imageScrolledWindow);
	pt = event.GetLogicalPosition(dc);
	if (isOnHexa(pt))
	{
		if (pointOnHex) delete pointOnHex;
		pointOnHex = new wxPoint(pt);
		onHexa = wxColor(colorHexagonImage.GetRed(pt.x, pt.y), colorHexagonImage.GetGreen(pt.x, pt.y), colorHexagonImage.GetBlue(pt.x, pt.y));
	}
}

void ColorCorrGUIFrame::combineWithOrginal(wxImage & cpy, const wxImage & org)
{
	auto nx = cpy.GetWidth();
	auto ny = cpy.GetHeight();
	const unsigned char* orgData = org.GetData();
	unsigned char* imageData = cpy.GetData();
	for (int i = 0; i < nx; i++)
	{
		for (int j = 0; j < ny; j++)
		{
			auto R = orgData[0 + 3 * i + j * nx * 3];
			auto G = orgData[1 + 3 * i + j * nx * 3];
			auto B = orgData[2 + 3 * i + j * nx * 3];
			imageData[0 + 3 * i + j * nx * 3] = orgFrac * R + (1 - orgFrac)*imageData[0 + 3 * i + j * nx * 3];
			imageData[1 + 3 * i + j * nx * 3] = orgFrac * G + (1 - orgFrac)*imageData[1 + 3 * i + j * nx * 3];
			imageData[2 + 3 * i + j * nx * 3] = orgFrac * B + (1 - orgFrac)*imageData[2 + 3 * i + j * nx * 3];
		}
	}
}

void ColorCorrGUIFrame::adjustBrightness(wxImage & cpy)
{
	auto nx = cpy.GetWidth();
	auto ny = cpy.GetHeight();
	unsigned char* imageData = cpy.GetData();
	for (int i = 0; i < nx; i++)
	{
		for (int j = 0; j < ny; j++)
		{
			if (static_cast<int>(imageData[0 + 3 * i + j * nx * 3]) + incBrightness <= 255)
				imageData[0 + 3 * i + j * nx * 3] += incBrightness;
			else
				imageData[0 + 3 * i + j * nx * 3] = 255;

			if (static_cast<int>(imageData[1 + 3 * i + j * nx * 3]) + incBrightness <= 255)
				imageData[1 + 3 * i + j * nx * 3] += incBrightness;
			else
				imageData[1 + 3 * i + j * nx * 3] = 255;

			if (static_cast<int>(imageData[2 + 3 * i + j * nx * 3]) + incBrightness <= 255)
				imageData[2 + 3 * i + j * nx * 3] += incBrightness;
			else
				imageData[2 + 3 * i + j * nx * 3] = 255;

		}
	}
}

void ColorCorrGUIFrame::adjustSaturation(wxImage & cpy)
{
	wxImage::HSVValue tempHSV;
	wxImage::RGBValue tempRGB;
	auto nx = cpy.GetWidth();
	auto ny = cpy.GetHeight();
	unsigned char* imageData = cpy.GetData();
	for (int i = 0; i < nx; i++)
	{
		for (int j = 0; j < ny; j++)
		{
			tempRGB.red = imageData[0 + 3 * i + j * nx * 3];
			tempRGB.green = imageData[1 + 3 * i + j * nx * 3];
			tempRGB.blue = imageData[2 + 3 * i + j * nx * 3];
			tempHSV = wxImage::RGBtoHSV(tempRGB);
			tempHSV.saturation += incSaturation;
			tempHSV.saturation = std::min(tempHSV.saturation, 1.);
			tempHSV.saturation = std::max(tempHSV.saturation, 0.);
			tempRGB = wxImage::HSVtoRGB(tempHSV);
			imageData[0 + 3 * i + j * nx * 3] = tempRGB.red;
			imageData[1 + 3 * i + j * nx * 3] = tempRGB.green;
			imageData[2 + 3 * i + j * nx * 3] = tempRGB.blue;
		}
	}
}

void ColorCorrGUIFrame::proportionalScaling(wxImage & cpy, const wxImage & org, const wxColor & orginal)
{
	int nx = cpy.GetWidth();
	int ny = cpy.GetHeight();
	auto * imageData = cpy.GetData();
	const auto * orgData = org.GetData();
	auto OrgR = orginal.Red();
	auto OrgG = orginal.Green();
	auto OrgB = orginal.Blue();

	auto RefR = onHexa.Red();
	auto RefG = onHexa.Green();
	auto RefB = onHexa.Blue();

	auto transferFun = [&](double ref, double org, double chosen, double dist)
	{
		//auto dist = std::pow(std::abs(chosen - org), 1);
		auto fun1 = [](double x)
		{
			return 1. / x;
		};
		auto fun2 = [&](double x)
		{
			return 1. / (std::exp(2.5 * (x - k) / k) + 1);
		};

		if (dist > 0)
		return static_cast<unsigned short>(chosen*(1. - fun2(dist)) + fun2(dist)*ref);
		else 
		return static_cast<unsigned short>(ref);
	};

	for (int i = 0; i < nx; i++)
	{
		for (int j = 0; j < ny; j++)
		{
			auto R = orgData[0 + 3 * i + j * nx * 3];
			auto G = orgData[1 + 3 * i + j * nx * 3];
			auto B = orgData[2 + 3 * i + j * nx * 3];

			auto dist = (std::sqrt(std::pow(B - OrgB, 2) + std::pow(G - OrgG, 2) + std::pow(R - OrgR, 2)));

			imageData[2 + 3 * i + j * nx * 3] = transferFun(RefB, OrgB, B, dist);
			imageData[1 + 3 * i + j * nx * 3] = transferFun(RefG ,OrgG, G, dist);
			imageData[0 + 3 * i + j * nx * 3] = transferFun(RefR, OrgR, R, dist);


			//if (R == onImg.Red() && G == onImg.Green() && B == onImg.Blue())
			//{
			//	imageData[0 + 3 * i + j * nx * 3] = onHexa.Red();
			//	imageData[1 + 3 * i + j * nx * 3] = onHexa.Green();
			//	imageData[2 + 3 * i + j * nx * 3] = onHexa.Blue();
			//}
		}
	}
}

void ColorCorrGUIFrame::ChangeColors(wxCommandEvent& event)
{
	if (pointOnHex && chosenColorPointOnHex)
	{
		imageCpy = imageOrg.Copy();
		proportionalScaling(imageCpy, imageOrg, onImg);
		adjustSaturation(imageCpy);
		adjustBrightness(imageCpy);
		combineWithOrginal(imageCpy, imageOrg);
		colorHexagonImageCpy = colorHexagonImage.Copy();
		proportionalScaling(colorHexagonImageCpy, colorHexagonImage, onImg);
		adjustSaturation(colorHexagonImageCpy);
		adjustBrightness(colorHexagonImageCpy);
		combineWithOrginal(colorHexagonImageCpy, colorHexagonImage);
		BackgroundOutsideOfHexa(colorHexagonImageCpy, backgroundColor);
	}
}