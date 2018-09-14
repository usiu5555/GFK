#pragma once
#include <wx/wx.h>
#include <wx/event.h>

//man GUI frame with all images and controls
class ColorCorrGUIFrame : public wxFrame
{
public:
	ColorCorrGUIFrame(const wxString& title);

	void OnOpen(wxCommandEvent& event);

private:
	//scrolled window for image import
	wxScrolledWindow* imageScrolledWindow;
	
	//Original image storage 
	wxImage imageOrg;

	//Modified image storage
	wxImage imageCpy;

	//color hexagon image storage
	wxImage colorHexagonImage;
	
	//modified color hexagon image storage
	wxImage colorHexagonImageCpy;

	//GUI with hexagons and sliders
	wxWindow* GUIWindow;

	//text field indicating color chosen on image
	wxStaticText* chosenColorTextField;

	//point indicating position on hexagon of color chosen on image 
	wxPoint* chosenColorPointOnHex = NULL;

	//point chosen on hex
	wxPoint* pointOnHex = NULL;

	//button used to refresh window
	wxButton* execute;

	//slider used to change proportional coefficient 
	wxSlider* sliderProportional;

	//slider used to change orginal image fraction
	wxSlider* sliderOrgImgFrac;

	//slider used to change hue rotation
	wxSlider* sliderHue;

	//slider used to modify saturation
	wxSlider* sliderSaturation;

	//hue rotation [-1,1]
	double rotHue = 0;

	//saturation increment [-1, 1]
	double incSaturation = 0;

	//proportional coefficient [1:255]
	double k = 125;

	//orginal image fraction
	double orgFrac = 0;

	//color chosen on image
	wxColor onImg;

	//color chosen on exagon
	wxColor onHexa;

	//handler for choosing colors from image 
	void OnMouseDown(wxMouseEvent &event);

	//handler for choosing color on color hexagon
	void OnMouseDownHexa(wxMouseEvent &event);

	//check if point is inside hexagon
	bool isOnHexa(const wxPoint& p);

	//handler for any UI update event - refresh window
	void windowUpdate(wxUpdateUIEvent& event)
	{
		Repaint();
	}

	//handler for sliders
	void OnScroll(wxScrollWinEvent& event)
	{
		Repaint();
	}

	//method for refreshing canvas
	void Repaint();

	//draw color hexagon and store in olorHexagonImage and olorHexagonImageCpy
	void InitHexa();

	//find specified color on hexagon, brightness is set as max by scaling
	void MarkHexaColor(const wxColor& color);

	//event for updating color correction
	void ChangeColors(wxCommandEvent& event);

	//handlers for sliders
	void proportionalHandler(wxScrollEvent& event);
	void oldImgSliderHandler(wxScrollEvent & event);
	void hueSliderHandler(wxScrollEvent & event);
	void saturationSliderHandler(wxScrollEvent & event);

	//combine with original image using orgFrac
	void combineWithOrginal(wxImage & cpy, const wxImage & org);

	//adjust saturation
	void adjustSaturation(wxImage & cpy);

	//scaling proportionally to inverse
	void proportionalScaling(wxImage & cpy, const wxImage & org);
};
