#include "recognition.h"

#ifndef MAINWINDOW_INCLUDED
#define MAINWINDOW_INCLUDED

#define MAXIMAGES 321


struct Plate
{
	TWL::Rect r; // ������������� �� �����������
	int nHProjection; // ����� �������������� ��������
	double value;
};

struct _p
{
	int p1,p2;

};

class MainWindow:public	TWL::ApplicationWindow<1>
{
	friend class AnalyzImage;
	friend class HelpWindow;

	static const int ADDCANDIDATEWIDTH=10;

public:
	MainWindow();
	~MainWindow();


//����������
	AnalyzImage cImage;

//��������
	LRESULT OnCreate( WPARAM wParam, LPARAM lParam );
	LRESULT OnClose ( WPARAM wParam, LPARAM lParam );
	LRESULT OnCommand ( WPARAM wParam, LPARAM lParam );
private:

	virtual void OnClientSize(TWL::WindowDocker &rDocker,int nLeft,int nTop,int nRight,int nBottom);

	void OnImageLoad();
	void OnVEdge();
	void OnHEdge();
	void OnVProjection();
	void OnHProjection();
	void OnMakeImageList();
	void OnNextImage();
	void OnPrevImage();

	void LoadBitmap(TWL::Bitmap &bmp);
	void LoadImageList();
	void BuildProjections();
	void LocatePlates();

	//������ ����� � ������ � �����
	void MakePlates();

	void Hough();
	void Deskew(); 
	//�������� ������ ������� �����������-����������
	//void BuildCandidates();

	//������� ��������
	void ImprovePlates();

	//"�������" ����������
	void Value();

	//����������� ���������� �� ��������� ( �� ������� � ������� )
	void SortCandidates();

	void CollectBest();

	//������� ���������� �����-������
	void CandidatesToBW();

	//��������� �� �������!
	void Divide();

	//����������� ������� � ��������
	void NormBrightnessContrast();

	//����� ������ �����
	void KillBorder();

	//������ ������������� �� ��������
	void CollectDivisionIntensity();

	void Gaussian();

	void SymbolsWork();

	//���������� ������� ������� ���������
	//� ���������� �� ����������� �������� ��������
	void FilterBySymSize();

	//���� �� ��������� ������ � ������� ����������� �������� 
	//�������, ������ �� ��������, � �����.
	void GetGraySymbols();



	static bool SaveString(const TWL::String &sString, TWL::XMLElement &xmlElement);
	static bool LoadString(TWL::String &sString, TWL::XMLElement &xmlElement);

	TWL::StripContainer cStrip;
	HelpWindow cHelpWindow;

	TIL::RGBImage imCurrentImage;
	TIL::GrayImage imGrayCurrentImage, imVerticalEdge,
		imHorizontalEdge, Deskewed;

	TWL::Bitmap bmpCurrentBitmap,bmpGrayCurrent;
	TWL::String sCurrentBitmap;

	TCL::Array<TWL::String> aImages;
	TCL::Array<int> aHProjection, aVProjection;
	int nImagesIndex;


	TCL::Array<TCL::Array<int>> aHPr;
	TCL::Array<TGL::Point<int,2>> aBandBounds;
	TCL::Array<TCL::Array<TGL::Point<int,2>>> aPlateBounds;
	TCL::Array<Plate> aPlates;
	TCL::Array<TCL::Array<TCL::Array<int>>> aHoughs;
	TCL::Array<TWL::Bitmap> abmpH;
	TCL::Array<TGL::Line<double>> aLines;
	TCL::Array<TCL::Array<TGL::Point<double,2>>> aMPoints;
	TCL::Array<TIL::GrayImage> aPlateIms;
	TCL::Array<TIL::GrayImage> aImprPlates;
	TCL::Array<TCL::Array<double>> aDervs;
	TCL::Array<TCL::Array<int>> aVProjs;
	TCL::Array<TIL::GrayImage> aDesk;
	TCL::Array<PlateValue> aValues;	
	TCL::Array<TCL::Array<int>> aBWThres;
	TCL::Array<TIL::GrayImage> aDaBest;
	TCL::Array<_p> aP;
	TCL::Array<TCL::Array<int>> aDivs;
	TCL::Array<TCL::Array<TIL::GrayImage>> aParts;
	//�������������� � ����������� ��������� aParts, �
	//������� ��������� ����� ������ ������
	TCL::Array<TCL::Array<TWL::Rect>> aPRects;
	TCL::Array<TCL::Array<int>> aBestHisto;
	TCL::Array<TCL::Array<TCL::Array<int>>> aDivProj;
	TCL::Array<TCL::Array<TCL::Array<TIL::GrayImage>>> aComps;
	TCL::Array<TCL::Array<TIL::GrayImage>> aSyms;

	//������ ������ ����������, ����� �� ������������ ��� ���������
	TCL::Array<bool> aBad;


	TWL::Graph grVerticalProjection;
	TWL::Graph::View VerticalView, HorizontalView;
	bool flVerticalProjection, flHorizontalProjection;

	//��������� ��� ��������� FilterBySymSize;
	double symsratio;

	//����������� ���������� ���������� ��������
	int nSyms;

	int ShowState;


};


#endif