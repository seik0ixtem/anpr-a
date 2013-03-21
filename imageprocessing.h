#include "recognition.h"

#ifndef IMAGEPROCESSING_INCLUDED
#define IMAGEPROCESSING_INCLUDED

//tothink: для рансака попробовать подбирать точки адаптивным алгоритмом!

class PlateValue
{
public:
	PlateValue():vheight(0),vmaxVPpeak(0),
		varea(0), vproportion(0), vSummary(0){}
	~PlateValue() {}

	double vheight;
	double vmaxVPpeak;
	double varea;
	double vproportion;
	double vSummary;
};

class ImageProcessor
{
public:
	ImageProcessor();
	~ImageProcessor();

	static const int MAXBANDS=5;
	static const int MAXPLATE=4;
	static const int INDENT=10;
	
	static const int HOUGHWIDTH=100; //частота решётки разбиения хаффа по ширине
	static const int HOUGHHEIGHT=100;//частота решётки разбиения хаффа по высоте
	//static const int HOUGHSTEP=1000;  //earчастота решётки интегрирования
	static const int REC_HOR_BORDER_M=10; //решетка для убивания черных полос сверху и снизу
	static const int REC_VERT_BORDER_M=10; //решетка для убивания белых полос слева и справа
	static const int IMPROVE_BORDER=150; //константа для статистики разбиения, меньшя - условие мягче


	static const int RANSACINTENSITYTHRESHOLD=50;
	static const int RANSACSAMPLES=3000;
	static const double RANSACVICINITY;

	static const double REC_HOR_BORDER_MF; // модификатор границы ч/б
	static const double REC_VERT_BORDER_MF; // модификатор границы ч/б
	static const double IMPR2_DIV_CONSTANT; //в процедуре улучшения разбиения 
																					//отвечает за выбор пиков

	static double YBC;
	static const double XBC;
	static double MINYBC;

	static const int BEST=3;
	static const int MINHEIGHT=11;

	static const double DIVISION_MODIFIER;



	//горизонтальное размыти, k - половина длины решётки
	static void HorizontalRankFilter(TIL::GrayImage &imSourceImage,
		TIL::GrayImage &imDestinationImage, int KLength);
	static void VerticalRankFilter(TIL::GrayImage &imSourceImage, 
		TIL::GrayImage &imDestinationImage, int KLength);

	//градиент вертикали
	static void VerticalEdgeDetection(TIL::GrayImage &imSourceImage,
		TIL::GrayImage &imDestionationImage);

	//градиент горизонтали
	static void HorizontalEdgeDetection(TIL::GrayImage &imSourceImage,
		TIL::GrayImage &imDestinationImage);

	//усреднение значения проекции
	// nWidth - ширина усреднения
	static void Average(
		const TCL::Array<int> &aSrc,
		TCL::Array<int> &aRz,
		int nWidth);

	

	//убиваем пики на краях
	//массив - изменяется
	static void PeakRecession(TCL::Array<int> &a);

	//проекция на вертикальную ось
	static void VerticalProjection(TIL::GrayImage &im,
		TCL::Array<int> &a,
		TWL::Rect &r);

	//вертикальная проекция
	static void VerticalProjection(TIL::GrayImage &imImage,
		TCL::Array<int> &aPoints);

	// VerticalProjection + Average + PeakRecession
	static void SmoothVerticalProjection(
		TIL::GrayImage &imImage,
		TCL::Array<int> &aPoints);

	//проекция на горизонтальную ось
	static void HorizontalProjection(TIL::GrayImage &im,
		TCL::Array<int> &a,
		TWL::Rect &r);

	//тож самое, только всего изображения
	static void HorizontalProjection(TIL::GrayImage &imImage,
		TCL::Array<int> &aPoints);

	//HorizontalProjection + Average
	static void SmoothHorizontalProjection(
		TIL::GrayImage &imImage,
		TCL::Array<int> &aPoints);

	static void LocateBands(TCL::Array<int> &aPoints,
		TCL::Array<TGL::Point<int,2>> &aBoundries);

	static  void HBandProjection(TIL::GrayImage &imSource,
		TGL::Point<int,2> &pBand,
		TCL::Array<int> &aResult);

	//nHeight-высота банда, для отсеивания заведомо неверных вариантов
	static void LocatePlateGradient(TCL::Array<int> &aPoints,
		TCL::Array<TGL::Point<int,2>> &pResultBand,
		int nHeight);

	static void HoughLineTransform(TIL::GrayImage &image,
		TCL::Array<TCL::Array<int>> &aHough, TWL::Rect &r);

	static void HoughResult(TCL::Array<TCL::Array<int>> &aH,
		TWL::Bitmap &bmpDst, int width, int height);
	
	

	//imDestination - изображение с размерами исходного, 
	 // выпрямленная табличка копируется на то же место (TWL::Rect &r )
	 // этого изображения
	static TGL::Line<double> RansacDeskew(
		TIL::GrayImage &imSource,
		TGL::AffineTransform<double,2,2> &aShear,
		TCL::Array<TGL::Point<double,2>> &aPoints,
		TWL::Rect &r);


	//imSource - исходное изображение, к его часть с 
	 // границами r будет применено преобразование aShear, 
   // а результат скопирован в imDestination с 
	 // границами также r
	 // в imPlate будет скопирован результа поворота
	 // с обрезкой по вертикали
	 // в imDeskewed - без обрезки
	static void MakeImage(
		const TIL::GrayImage &imSource,
		TIL::GrayImage &imDestination,		
		 TGL::AffineTransform<double,2,2> &aShear,
		TWL::Rect &r,
		TIL::GrayImage &imPlate,
		TIL::GrayImage &imDeskewed,
		TCL::Array<int> &aVP);

	

	static double EstimateValue(
		TIL::GrayImage &imVerticalEdge,
		TIL::GrayImage &imHorizontalEdge,
		TWL::Rect &r);

	static double EstimateWeight(
		TIL::GrayImage &im,
		PlateValue &v);

	//сузим область потенциальной позиции номера
	static void ImprovePlatePosition(TIL::GrayImage &imSrc,
		TIL::GrayImage &imDst,
		TCL::Array<double> &aDerv);

	//нормализация яркости и контраста
	static void Normalization(TIL::GrayImage &im);
	
	//находим коэффициент разделения на чёрное и белое
	static int AdaptiveThreshold(TIL::GrayImage &im, TCL::Array<int> &aH, int &p1, int &p2);
	static int OtsuBinarizationThreshold(TIL::GrayImage &im);
	static void BernsenBinarization(TIL::GrayImage &imSrc, 
		TIL::GrayImage &imDst);
	static void NiblekBinarization(TIL::GrayImage &imSrc,
		TIL::GrayImage &imDst);

	//находим медиану как среднее значение яркости
	static void AverageThreshold(TIL::GrayImage &imSrc,
		TIL::GrayImage &imDst);

	static void MedianThreshold(TIL::GrayImage &imSrc,
		TIL::GrayImage &imDst);

	//контрастируем изображение на чёрное и белое согласно границе
	static void ContrastGrayImage(TIL::GrayImage &imSrc, int nThreshold);

	//получаем разбиение изображения
	static void GetDivision(TIL::GrayImage &imSrc,
		TCL::Array<int> &aDivs,		
		TCL::Array<int> &aHist);

	//разобьём по полученному разбиению
	static void DivideIntoBlocks(TIL::GrayImage &imSrc,
		TCL::Array<int> &aDivs,
		TCL::Array<TIL::GrayImage> &aParts);

	//уберём тёмные полосы снизу и сверху
	static void RecessHorizontalBorders(TIL::GrayImage &imSrc,
		TIL::GrayImage &imDst);

	//убираем светлые полосы слева и справа
	static void RecessVerticalBorders(TIL::GrayImage &imSrc,
		TIL::GrayImage &imDst);

	//убивает белый цвет, образовавшийся
	//как результат сужения границ
	static void KillWhite(TIL::GrayImage &im);

	//RecessHorizontalBorders
	//RecessVerticalBorders
	//KillWhite
	static void RecessBorders(TIL::GrayImage &imSrc,
		TIL::GrayImage &imDst);

	//получим интенсивность на разрезах
	static void GetIntensityOnDivisions(TIL::GrayImage &imSrc,
		TCL::Array<int> &aDivs,
		TCL::Array<TCL::Array<int>> &aProj);

	//улучшим разбиение
	//за счёт удаления граней, отстоящих друг от друга
	//на статистически меньших расстояних
	static void ImproveDivision1(TIL::GrayImage &im,
		TCL::Array<int> &aDiv);

	//улучшим разбиение - попытаемся разбить сдвоенные символы
	//за счет анализа яркостных характеристик между 
	//разделителями и заодно захватим
	//символы на краях
	static void ImproveDivision2(TIL::GrayImage &im,
		TCL::Array<int> &aDiv);

	//берёт чб имейж и вытаскивает из него компоненты связности
	//т.е. циферки и мусор
	//в r помещаем прямоугольники, содержащие нужный символ
	static void GetComponents(TIL::GrayImage &im,
		TCL::Array<TIL::GrayImage> &aRez,
		TCL::Array<TWL::Rect> &r);

	//Выбирает из мусора между двумя разделителями
	//компоненту связности, которая и есть символ
	static void ChooseSymbol(TCL::Array<TIL::GrayImage> &aSrc,
		TIL::GrayImage &imRez,
		TCL::Array<TWL::Rect> &aR,
		TWL::Rect &r);

	static void Skeletonize(TIL::GrayImage &imSrc,
		TIL::GrayImage &imRez);

	

private:

	//собирает из массива пикселей изображение
	static void MakeImageFromPoints(TIL::GrayImage &im, TCL::Array<TWL::Point> &a,
		int nP, TWL::Rect &r);


	
	template <class PixelType>
	static TILINLINE void VerticalEdgeConvolve(
		PixelType &rDestination,
		const PixelType *pSource,
		int nWidth,
		void *pData) {

		const PixelType *p;
		PixelType::Result t;

		for(int i=0;i<PixelType::ChannelsNumber;++i) {
			p=pSource;
			t=-p[0][i];
			p+=2*nWidth;            
			t+=p[0][i];
			rDestination[i]=TML::Abs(t);
		}
	}

	//-1 0 1//
	template <class PixelType>
	static TILINLINE void HorizontalEdgeConvolve(
		PixelType &rDestination,
		const PixelType *pSource,
		int nWidth,
		void *pData) {

		const PixelType *p;
		PixelType::Result t;

		for(int i=0;i<PixelType::ChannelsNumber;++i) {
			p=pSource;
			t=-p[0][i]+p[2][i];
			rDestination[i]=TML::Abs(t);
		}
	}

	template <class PixelType>
	static TILINLINE void SobelVerticalConvolve(
		PixelType &rDestination,
		const PixelType *pSource,
		int nWidth,
		void *pData) {

		const PixelType *p;
		PixelType::Result t;

		for(int i=0;i<PixelType::ChannelsNumber;++i) {
			p=pSource;
			t=-p[0][i]-2*p[1][i]-p[2][i];
			p+=2*nWidth;            
			t+=p[0][i]+2*p[1][i]+p[2][i];
			rDestination[i]=TML::Abs(t)/8;
		}
	}

  // | 2, 8,16, 8, 2 |
	template <class PixelType>
	static TILINLINE void HorizontalBlur(
		PixelType &rDestination,
		const PixelType *pSource,
		int nWidth,
		void *pData)
	{
		const PixelType *p;
		PixelType::Result t;

		for(int i=0;i<PixelType::ChannelsNumber;++i) {
			p=pSource;
			int nNumber=31;
			t=0;
			for (int j=0; j<nNumber; j++)
			{
				t+=p[j][i];
			}
			rDestination[i]=t/nNumber;
		}		
	}

	class RansacDescriptor
	{
	public:
		static bool Sample(
			TGL::Line<double> &rLine,
			const TGL::Point<double,2> *pPoints,
			double &rScale,
			double &rVicinity,
			void *pData);

		static double Distance(
			TGL::Line<double> &rLine,
			const TGL::Point<double,2> &rPoint);
			



	private:


	};



};


#endif