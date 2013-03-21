#include "recognition.h"

#ifndef HELPWINDOW_INCLUDED
#define HELPWINDOW_INCLUDED

class HelpWindow: public TWL::ImageView
{
public:
	HelpWindow();
	~HelpWindow();

	LRESULT OnPaint(WPARAM wParam, LPARAM lParam);
	LRESULT OnMouseMove(WPARAM wParam, LPARAM lParam);
		


private:


};


#endif