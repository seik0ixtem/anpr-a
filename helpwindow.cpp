#include "recognition.h"

HelpWindow::HelpWindow()
{
	//SetHandler(WM_PAINT, &HelpWindow::OnPaint);
	SetHandler(WM_MOUSEMOVE, &HelpWindow::OnMouseMove);
}

HelpWindow::~HelpWindow()
{
}

LRESULT HelpWindow::OnPaint( WPARAM wParam, LPARAM lParam )
{
	TWL::PaintContext pc(*this);

	TWL::Rect cr,dr;
	ClientRect(cr);
	pc.Fill(0,0,cr.right, cr.bottom,TWL::Color::Green());

	
	//int t=cr.Width()/4;
	//dr.left=t;
	//dr.right=cr.right-t;
	//dr.top=t;
	//dr.bottom=cr.bottom-t;


	//MainWindow &pMW=RecognitionApp::Instance().cMainWindow;

	//if (pMW.flVerticalProjection)
	//{

	//	pMW.grVerticalProjection.SetDrawRect(dr);

	//	double minx=0.0, miny=0.0,
	//		maxx=0.0,maxy=0.0;
	//	for (int i=0; i<pMW.VerticalView.Points.Size(); i++)
	//	{
	//		if (pMW.VerticalView.Points[i].C(0)>maxx) maxx=pMW.VerticalView.Points[i].C(0);
	//		if (pMW.VerticalView.Points[i].C(1)>maxy) maxy=pMW.VerticalView.Points[i].C(1);
	//	}

	//	pMW.grVerticalProjection.SetRangeX(minx,maxx,5,5);
	//	pMW.grVerticalProjection.SetRangeY(miny,maxy,5,5);

	//	TWL::Chart::Axis xa,ya;
	//	TWL::Chart::Frame fr;
	//	TWL::Chart::Background background;	



	//	pMW.grVerticalProjection.BeginDraw(pc);

	//	pMW.grVerticalProjection.DrawAxisX(xa);
	//	pMW.grVerticalProjection.DrawAxisY(ya);
	//	pMW.grVerticalProjection.DrawBackground(background);
	//	pMW.grVerticalProjection.DrawFrame(fr);
	//	pMW.grVerticalProjection.DrawGraph(pMW.VerticalView);

	//	pMW.grVerticalProjection.EndDraw();
	//}


	return 0;
}
LRESULT HelpWindow::OnMouseMove( WPARAM wParam, LPARAM lParam )
{
	MainWindow &pMW=RecognitionApp::Instance().cMainWindow;

	int x=GET_X_LPARAM(lParam),
		y=GET_Y_LPARAM(lParam);

	TWL::Bitmap bmp;

	GetBitmap(bmp);

	TWL::DeviceContext dc;
	dc.CreateCompatible();

	dc.Select((HBITMAP)bmp.Handle());

	this->ClientToImage(x,y);

	TWL::Color clr(::GetPixel(dc.Handle(), x, y));

	int r=clr.R(),g=clr.G(),b=clr.B();

	int i=-1;
	if ((x>=0)&&(x<pMW.imGrayCurrentImage.Width())
		&&(y>=0)&&(y<pMW.imGrayCurrentImage.Height()))
	{
		TIL::GrayPixel p=pMW.imGrayCurrentImage.Pixel(x,y);
		i=p.I();
	}

	TWL::String str;

	str=TEXT("X: ");
	str.AppendInteger(x);
	pMW.cStatusBar.SetPartText(str.Pointer(),1);

	str=TEXT("Y: ");
	str.AppendInteger(y);
	pMW.cStatusBar.SetPartText(str.Pointer(),2);

	str=TEXT("R: ");
	str.AppendInteger(r);
	pMW.cStatusBar.SetPartText(str.Pointer(),3);

	str=TEXT("G: ");
	str.AppendInteger(g);
	pMW.cStatusBar.SetPartText(str.Pointer(),4);
	
	str=TEXT("B: ");
	str.AppendInteger(b);
	pMW.cStatusBar.SetPartText(str.Pointer(),5);
	
	str=TEXT("I: ");
	str.AppendInteger(i);
	pMW.cStatusBar.SetPartText(str.Pointer(),6);
		
			
	return NextHandler(wParam, lParam);
}