#include "recognition.h"

#ifndef ANALYZIMAGE_INCLUDED
#define ANALYZIMAGE_INCLUDED

class AnalyzImage:public HelpWindow
{
public:
	AnalyzImage();
	~AnalyzImage();

	virtual void Draw(TWL::DeviceContext &rContext,void *pData=0);

	LRESULT OnMouseMove( WPARAM wParam, LPARAM lParam );

private:

	void BuildGraph(TWL::DeviceContext &dc, TCL::Array<int> &a, 
		TWL::Rect &r, TWL::Color &c);


};


#endif