#include "recognition.h"

double ImageProcessor::YBC=0.45;
const double ImageProcessor::XBC=0.50;
double ImageProcessor::MINYBC=0.9;
const double ImageProcessor::RANSACVICINITY=0.36;
const double ImageProcessor::REC_HOR_BORDER_MF=0.4;
const double ImageProcessor::REC_VERT_BORDER_MF=0.4;
const double ImageProcessor::IMPR2_DIV_CONSTANT=1.5;

const double ImageProcessor::DIVISION_MODIFIER=0.7;
ImageProcessor::ImageProcessor()
{
}

ImageProcessor::~ImageProcessor()
{
}

void ImageProcessor::VerticalEdgeDetection( TIL::GrayImage &imSourceImage, 
																			TIL::GrayImage &imDestinationImage )
{
	TIL::GrayImage imTemp;

	//imTemp.Copy(imSourceImage);
	//TIL::ImageApertureTransform<TIL::GrayPixel, TIL::GrayPixel,
	//	HorizontalBlur<TIL::GrayPixel>,31,1,15,0>(
	//	imTemp, imSourceImage);

	//imTemp.FillEdges(TIL::GrayPixel(0),2,0,2,0);
	//imDestinationImage.Copy(imTemp);

	//TIL::ImageSmoothGaussian5x5(imTemp,imSourceImage);
	
	TIL::ImageApertureTransform<TIL::GrayPixel, TIL::GrayPixel,
		VerticalEdgeConvolve<TIL::GrayPixel>,1,3,0,1>(
		imDestinationImage, imSourceImage);

	imDestinationImage.CopyEdges(imSourceImage,0,1,0,1);
	
	//TIL::GrayPixel *p=imDestinationImage.Pointer();
	//for (int i=0; i<imDestinationImage.Width()*imDestinationImage.Height(); i++)
	//{
	//	if (p->I()>=40) p->I()=255;
	//	else p->I()=0;
	//	p++;
	//}
}
void ImageProcessor::HorizontalEdgeDetection(TIL::GrayImage &imSourceImage,
	TIL::GrayImage &imDestinationImage)
{
	TIL::GrayImage imTemp;

	//TIL::ImageSmoothGaussian5x5(imTemp, imSourceImage);

	TIL::ImageApertureTransform<TIL::GrayPixel, TIL::GrayPixel,
		HorizontalEdgeConvolve<TIL::GrayPixel>,3,1,1,0>(
		imDestinationImage, imSourceImage);

	imDestinationImage.CopyEdges(imSourceImage,1,0,1,0);
	
	//TIL::GrayPixel *p=imDestinationImage.Pointer();
	//for (int i=0; i<imDestinationImage.Width()*imDestinationImage.Height(); i++)
	//{
	//	if (p->I()>=40) p->I()=255;
	//	else p->I()=0;
	//	p++;
	//}
}

void ImageProcessor::Average(
	const TCL::Array<int> &aSrc,
	TCL::Array<int> &aRz,
	int nWidth )
{
	int nSize=nWidth,
		nsp=nSize/2;

	aRz.Resize(aSrc.Size());

	for (int i=0; i<aSrc.Size(); i++)
	{
		aRz[i]=0;
		int nCounter=0;
		for (int j=max(0,i-nsp); j<=min(i+nsp, aSrc.Size()-1); j++)
		{
			aRz[i]+=aSrc[j];
			nCounter++;
		}
		aRz[i]=TML::Round(aRz[i]/nCounter);
	}
}

void ImageProcessor::PeakRecession( TCL::Array<int> &a )
{
	for (int i=0; i<a.Size()-1; i++)
	{
		if (a[i+1]<=a[i])
		{
			a[i]=0;
		} else
		{
			break;
		}
	}
	for (int i=a.Size()-1; i>0; i--)
	{
		if (a[i-1]<=a[i])
		{
			a[i]=0;
		} else
		{
			break;
		}
	}
}

void ImageProcessor::VerticalProjection(
 TIL::GrayImage &im,
 TCL::Array<int> &a,
 TWL::Rect &r )
{
	int h=r.Height(),
		w=r.Width();	

	a.Resize(h);

	TIL::GrayPixel *p=im.Pointer();
	p+=r.top*h;

	for (int i=0; i<h; i++)
	{	
		a[i]=0;
		for (int j=r.left; j<r.right; j++)
		{
			a[i]+=(p+j)->I();			
		}
		p+=w;
	}

}

void ImageProcessor::VerticalProjection(
	TIL::GrayImage &imImage,
	TCL::Array<int> &aPoints)
{
	VerticalProjection(imImage, aPoints,
		TWL::Rect(0,0,imImage.Width(),imImage.Height()));

	//aPoints.Resize(imImage.Height());
	//TIL::GrayPixel *p=imImage.Pointer();
	//for (int i=0; i<imImage.Height(); i++)
	//{
	//	int nSumIntensity=0;
	//	for (int j=0; j<imImage.Width(); j++)
	//	{
	//		nSumIntensity+=p->I();
	//		p++;
	//	}
	//	aPoints[i]=nSumIntensity;
	//}
}

void ImageProcessor::SmoothVerticalProjection(
	TIL::GrayImage &imImage,
	TCL::Array<int> &aPoints )
{
	VerticalProjection(imImage, aPoints);

	TCL::Array<int> aAvPoints;
	aAvPoints.Copy(aPoints);

	int nSize=15;

	Average(aAvPoints, aPoints,nSize);

	PeakRecession(aPoints);
}

void ImageProcessor::HorizontalProjection(
	TIL::GrayImage &im,
	TCL::Array<int> &a,
	TWL::Rect &r)
{
	int w=r.Width();	

	a.Resize(w);
	for (int i=0; i<w; i++)
	{
		a[i]=0;
	}	

	TIL::GrayPixel *p=im.Pointer();
	p+=r.top*w;

	for (int i=r.top; i<r.bottom; i++)
	{	
		for (int j=0; j<r.Width(); j++)
		{
			a[j]+=(p+j+r.left)->I();			
		}
		p+=w;
	}
}

void ImageProcessor::HorizontalProjection(
	TIL::GrayImage &imImage,
	TCL::Array<int> &aPoints )
{
	HorizontalProjection(imImage,aPoints,
		TWL::Rect(0,0,imImage.Width(),imImage.Height()));

	//aPoints.Resize(imImage.Width());

	//TIL::GrayPixel *p=imImage.Pointer();

	//for (int i=0; i<imImage.Width(); i++)
	//{
	//	aPoints[i]=0;
	//}
	//for (int i=0; i<imImage.Height(); i++)
	//{
	//	for (int j=0; j<imImage.Width(); j++)
	//	{
	//		aPoints[j]+=p->I();
	//		p++;
	//	}
	//}
}
void ImageProcessor::SmoothHorizontalProjection( TIL::GrayImage &imImage,
																					TCL::Array<int> &aPoints )
{
	HorizontalProjection(imImage, aPoints);

	TCL::Array<int> aAvPoints;
	aAvPoints.Copy(aPoints);

	//усредним

	int nSize=9;
	Average(aAvPoints,aPoints,nSize);
}



void ImageProcessor::LocateBands( TCL::Array<int > &aPoints, TCL::Array<TGL::Point<int,2> > &aBoundaries )
{
	aBoundaries.Resize(MAXBANDS);
	int nSize=0;

	TCL::Array<int> aTemp;
	aTemp.Copy(aPoints);
	//aTemp.Copy(aPoints);

	//находим области с пиками
	for (int i=0; i<MAXBANDS; i++)
	{
		int p=aTemp.MaxIndex(),
			pl,pr;
		if (!aTemp[p]) break;

		for ( pl=p; pl>0; pl--)
		{
			if ((aTemp[pl]<=aTemp[p]*YBC)||
				((aTemp[pl]<aTemp[pl-1])&&(aTemp[pl]<=aTemp[p]*MINYBC)))
			{
				break;
			}
		}

		for (pr=p; pr<aTemp.Size()-1; pr++)
		{
			if ((aTemp[pr]<=aTemp[p]*YBC)||
				((aTemp[pr]<aTemp[pr+1])&&(aTemp[pr]<=aTemp[p]*MINYBC)))
			{
				break;
			}
		}		

		aBoundaries[nSize].C(0)=pl;
		aBoundaries[nSize].C(1)=pr;

		for (int j=pl; j<=pr; j++)
		{
			aTemp[j]=0;
		}
		for (int j=pl-1; j>0; j--)
		{
			if (aTemp[j-1]<aTemp[j])
			{
				aTemp[j]=0;
			} else
			{
				break;
			}
		}
		for (int j=pr+1; j<aTemp.Size()-1; j++)
		{
			if (aTemp[j+1]<aTemp[j])
			{
				aTemp[j]=0;
			} else 
			{
				break;
			}
		}

		nSize++;
	}

	aBoundaries.Resize(nSize);
	
}

void ImageProcessor::HBandProjection(TIL::GrayImage &imSource,
																		 TGL::Point<int,2> &pBand,
																		 TCL::Array<int> &aPoints)
{
	HorizontalProjection(imSource,aPoints,
		TWL::Rect(0,pBand.C(0),imSource.Width(),pBand.C(1)+1));

	TCL::Array<int> aTemp;
	aTemp.Copy(aPoints);

	int nSmooth=61;

	Average(aTemp,aPoints,nSmooth);
	PeakRecession(aPoints);
}

void ImageProcessor::LocatePlateGradient( TCL::Array<int> &aPoints,
																				 TCL::Array<TGL::Point<int,2>> &pResultBand,
																				 int nHeight)
{
	TCL::Array<int> aTemp;
	aTemp.Copy(aPoints);

	for (int i=0; i<INDENT; i++)
	{
		aTemp[i]=0;
		aTemp[aPoints.Size()-1-i]=0;

	}

	pResultBand.Resize(MAXPLATE);

	int c=0;
	while (c<MAXPLATE)
	{
		int p=aTemp.MaxIndex(),
		pl,pr;
		
		if (!p) break;

		for (pl=p; pl>0; pl--)
		{	
			if (aTemp[pl]<aTemp[p]*XBC)
			{
				break;
			}		
		}
		for (pr=p; pr<aTemp.Size()-1; pr++)
		{	
			if (aTemp[pr]<aTemp[p]*XBC)
			{
				break;
			}
		}
		
		for (int j=pl+1; j<pr; j++)
		{
			aTemp[j]=0;
		}
		for (int j=pl; j>0; j--)
		{
			if (aTemp[j]>=aTemp[j-1])
			{
				aTemp[j]=0;
			} else
			{
				break;
			}
		}

		for (int j=pr; j<aTemp.Size()-1; j++)
		{
			if (aTemp[j]>=aTemp[j+1])
			{
				aTemp[j]=0;
			} else
			{
				break;
			}
		}

		//i++
		//предварительные фильтры
		//фильтр высоты/ширины
		
//		if (pr-pl>nHeight)
//		{			
			pResultBand[c].C(0)=pl;
			pResultBand[c].C(1)=pr;
			c++;
//		}
	}
	pResultBand.Resize(c);
}


//x*cos(theta)+y*sin(theta)=R - линия
void ImageProcessor::HoughLineTransform( TIL::GrayImage &image, TCL::Array<TCL::Array<int> > &aHough,
																				TWL::Rect &r)
{

	int w=r.Width(),
		h=r.Height();

	aHough.Resize(HOUGHHEIGHT);
	for (int i=0; i<aHough.Size(); i++)
	{
		aHough[i].Resize(HOUGHWIDTH);
		aHough[i].Fill(0);
	}

	TCL::Array<TCL::Array<int>> aTemp;
	aTemp.Resize(h);
	

	//занесём выделенную область в отдельный массив
	TIL::GrayPixel *p=image.Pointer();
	for (int i=0; i<aTemp.Size(); i++)
	{
		aTemp[i].Resize(w);
		for (int j=0; j<aTemp[i].Size(); j++)
		{
			aTemp[i][j]=p[image.Width()*(r.top+i)+r.left+j].I();
			p++;
		}		
	}

	double wv=-TML::Pi()/2,
		hv=0,
		wh=TML::Pi()/(HOUGHWIDTH-1),
		hh=TML::Sqrt(2.0)/(HOUGHHEIGHT-1);
	

	for (int i=0; i<HOUGHHEIGHT; i++)
	{
		wv=-TML::Pi()/2;
		for (int j=0; j<HOUGHWIDTH; j++)
		{

			for (int k=0; k<w; k++)
			{
				double x=k,
					y=(hv-x*TML::Cos(wv))/TML::Sin(wv);
					
				int ox=TML::Round(x),
					oy=TML::Round(y);

				if ((ox>=0)&&(ox<w)&&(oy>=0)&&(oy<h)) 
				{
					aHough[i][j]+=aTemp[oy][ox];
				}
			}
			wv+=wh;
		}
		hv+=hh;
	}
}

void ImageProcessor::HoughResult(TCL::Array<TCL::Array<int>> &aH,
																 TWL::Bitmap &bmpDst, int width, int height)
{
	TIL::GrayImage imTmp;
	imTmp.Create(aH[0].Size(), aH.Size());


	int h=aH.Size(),
		w=aH[0].Size();
	const int im=255; //максимальная интенсивность

	int m=0;
	for (int i=0; i<aH.Size(); i++)
	{
		for (int j=0; j<aH[i].Size(); j++)
		{
			if (aH[i][j]>m) m=aH[i][j];
		}
	}


	TIL::GrayPixel *p=imTmp.Pointer();
	for (int i=0; i<aH.Size(); i++)
	{
		for (int j=0; j<aH[i].Size(); j++)
		{
			p[i*aH.Size()+j]=TML::Round(aH[i][j]*double(im)/m);
		}
	}

	TWL::Bitmap bmpT;
	bmpT.FromImage(imTmp);
	bmpDst.Copy(bmpT,width,height);

}




bool ImageProcessor::RansacDescriptor::Sample(
	TGL::Line<double> &rLine,
	const TGL::Point<double,2> *pPoints,
	double &rScale,
	double &rVicinity,
	void *pData)
{
	rLine.Estimate(pPoints[0].C(0),pPoints[0].C(1), pPoints[1].C(0), pPoints[1].C(1));

	rScale=1.0;
	rVicinity=RANSACVICINITY;

	return rLine.Valid();
}

double ImageProcessor::RansacDescriptor::Distance(
	TGL::Line<double> &rLine,
	const TGL::Point<double,2> &rPoint)
{
	return rLine.Distance(rPoint);
}



TGL::Line<double> ImageProcessor::RansacDeskew(
		TIL::GrayImage &imSource,
		TGL::AffineTransform<double,2,2> &aShear,
		TCL::Array<TGL::Point<double,2>> &aPoints,
		TWL::Rect &r)
{


	double theta;
	

	imSource.GetPointsArray(aPoints, r.left,r.top,
		r.Width(),r.Height(),RANSACINTENSITYTHRESHOLD);

	TGL::Line<double> result;

	if (!aPoints.Size())
	{
		result.Estimate(r.left,r.top,r.right,r.bottom);
		return result;

	}

	TGL::SingleRobustEstimator<
		ImageProcessor::RansacDescriptor,
		TGL::Line<double>,
		TGL::Point<double,2>,
		double,
		2>
		Estimator(result, aPoints.Pointer(),
			aPoints.Size(), RANSACSAMPLES);

	if (!(result.C(1)&&result.Valid())) goto quit;

	theta=-result.C(0)/result.C(1);

	aShear.M(0)=1;
	aShear.M(1)=0;
	aShear.M(2)=0;
	aShear.M(3)=theta;
	aShear.M(4)=1.0;
	aShear.M(5)=0;

	return result;




quit:
	aShear.M(0)=1;
	//aShear.M(1)=-theta;
	aShear.M(1)=0;
	aShear.M(2)=0;
	aShear.M(3)=0;
	aShear.M(4)=1;
	aShear.M(5)=0;

	return result;
}

void ImageProcessor::MakeImage(
	const TIL::GrayImage &imSource,
	TIL::GrayImage &imDestination,
	TGL::AffineTransform<double,2,2> &aShear,
	TWL::Rect &r,
	TIL::GrayImage &imPlate,
	TIL::GrayImage &imLocal,
	TCL::Array<int> &aVP)
{
	TIL::GrayImage imResult(r.Width(), r.Height()), 
		imTransform(r.Width(), r.Height());


	TIL::GrayPixel *pS, *pD, *pR, *pT;

	pS=imSource.Pointer();
	pT=imTransform.Pointer();

	//выделяем из исходного изображения то, что будем 
	 //преобразовывать
	imTransform.Copy(imSource,0,0,
		r.left,r.top,r.Width(), r.Height());

  //ch - величина изменения высоты
	int w=r.Width(), ch=TML::Abs(TML::Round(w*aShear.M(3))),
		h=r.Height()+ch;
	imResult.Create(w, h);


	TIL::ImageAffineTransform<double, TIL::GrayPixel,3>
		(imResult, imTransform,
		w, h, aShear);


	

	//TIL::GrayImage imLocal;
	imLocal.Create(imResult.Width(),h);
	imLocal.Copy(imResult,0,0,0,0,imResult.Width(),h);


	//натягиваем изображение на пустоты, образовавшиеся в 
	//результате спрямления изображения
	TIL::GrayPixel *pL=imLocal.Pointer();
	for (int i=0; i<imLocal.Height(); i++)
	{
		for (int j=0; j<imLocal.Width(); j++)
		{
			if (pL[i*imLocal.Width()+j].I())
			{
				for (int k=j; k>=0; k--)
				{
					pL[i*imLocal.Width()+k]=pL[i*imLocal.Width()+j];
				}
				break;
			}

		}
		for (int j=imLocal.Width()-1; j>=0; j--)
		{
			if (pL[i*imLocal.Width()+j].I())
			{
				for (int k=j;k<imLocal.Width(); k++)
				{
					pL[i*imLocal.Width()+k]=pL[i*imLocal.Width()+j];
				}
				break;
			}
		}
	}
	
	//локализуем новый бэнд на выпрямленном изображении

	//double tYBC=YBC, tMINYBC=MINYBC;
	//YBC=0.5;
	//MINYBC=0.5;

	TIL::GrayImage imLocalVerticalEdge;
	//TCL::Array<int> aLocalVProjection;
	TCL::Array<TGL::Point<int,2>> aLocalBands;
	
	HorizontalEdgeDetection(imLocal, imLocalVerticalEdge);
	SmoothVerticalProjection(imLocalVerticalEdge, aVP);
	LocateBands(aVP, aLocalBands);

	if (aLocalBands.Size())
	{
		imPlate.Create(imLocal.Width(), aLocalBands[0].C(1)-aLocalBands[0].C(0));
		imPlate.Copy(imLocal,0,0,0,aLocalBands[0].C(0),
			imLocal.Width(), aLocalBands[0].C(1)-aLocalBands[0].C(0));
	} else
	{
		imPlate.Copy(imLocal);	}

	//YBC=tYBC;
	//MINYBC=tMINYBC;

	//это (не) очень нужно для изображения, похожего на исходное, 
	//но с выпрямленными компонентами
	pD=imDestination.Pointer();
	pR=imResult.Pointer();
	for (int i=0; i<r.Height(); i++)
	{
		for (int j=0; j<r.Width(); j++)
		{
			if (aShear.M(3)<=0)
			{

				pD[(i+r.top)*imSource.Width()+r.left+j]=
					pR[(TML::Abs(TML::Round(w*aShear.M(3)))+i)*r.Width()+j];
			} else
			{
				pD[(i+r.top)*imSource.Width()+r.left+j]=
					pR[i*r.Width()+j];
			}

		}
	}


}
double ImageProcessor::EstimateValue(
		TIL::GrayImage &imVerticalEdge,
		TIL::GrayImage &imHorizontalEdge,
		TWL::Rect &r)
{
	double a1=r.Height();
	
	TCL::Array<int> aVertP, aHorP;
	aVertP.Resize(r.Height()+1);
	aHorP.Resize(r.Width()+1);

	for (int i=r.top; i<=r.bottom; i++)
	{
		aVertP[i-r.top]=0;
		for (int j=0; j<=r.Width()-1; j++)
		{
			aVertP[i-r.top]+=imVerticalEdge.Pixel(j,i).I();
		}
	}

	for (int i=r.left; i<=r.right; i++)
	{
		aHorP[i-r.left]=0;
		for (int j=r.top; j<=r.bottom; j++)
		{
			aHorP[i-r.left]+=imHorizontalEdge.Pixel(i,j).I();				
		}
	}

	double m=0,sum=0;

	for (int i=0; i<aVertP.Size(); i++)
	{
		sum+=aVertP[i];
		if (aVertP[i]>m) m=aVertP[i];
	}

	double a2=1/m, a3=1/sum;

	m=0, sum=0;
	for (int i=0; i<aHorP.Size(); i++)
	{
		sum+=aHorP[i];
		if (aHorP[i]>m) m=aHorP[i];
	}

	double a4=1/m, a5=1/sum;

	double a6=TML::Abs(TML::Abs((r.Width())/(r.Height()))-5);


	double a=0.15*a1+0.25*a2+0.4*a3+0.4*a6;	
	return a;
}


void ImageProcessor::ImprovePlatePosition(
		TIL::GrayImage &imSrc,
		TIL::GrayImage &imDst,
		TCL::Array<double> &aDerv)
{
	TCL::Array<int> aHProjection;

	const int H=4;
	int w=imSrc.Width(), h=imSrc.Height();

	aHProjection.Resize(w);
	aDerv.Resize(w);
	for (int i=0; i<w; i++) 
	{
		aHProjection[i]=0;
		aDerv[i]=0;
	}

	TIL::GrayPixel *p=imSrc.Pointer();
	for (int i=0; i<h; i++)
	{
		for (int j=0; j<w; j++)
		{
			aHProjection[j]+=p->I();
			p++;
		}
	}


	double mind=10e9,maxd=-10e9;
	const double c=0.7;

	for (int i=H; i<w; i++)
	{
		double iz=aDerv[i]=(aHProjection[i]-aHProjection[i-H])/double(H);

		if (iz<mind) mind=iz;
		if (iz>maxd) maxd=iz;
	}

	int x0=0, x1=w-1;
	for (int i=0; i<=w-1; i++)
	{
		if (aDerv[i]>=c*maxd)
		{
			x0=i;
			break;
		}
	}
	for (int i=w-1; i>=0; i--)
	{
		if (aDerv[i]<=c*mind)
		{
			x1=i;
			break;
		}
	}

	if (x0>x1) 
	{
		int t=x0;
		x0=x1;
		x1=t;
	}
		
	TIL::GrayPixel pe;
	pe.I()=255;
	imDst.Create(x1-x0+1,h);
	imDst.CopyBlock(imSrc,x0,0,x1-x0+1,h,pe);
}

double ImageProcessor::EstimateWeight(TIL::GrayImage &im,
																						 PlateValue &v)
{
	double a3;

	if (im.Height()<MINHEIGHT)
	{
		v.vSummary=100000000; 
		return v.vSummary;
	}

	v.vheight=im.Height();


	

	TIL::GrayImage imHE;
	TCL::Array<int> aVP;

	HorizontalEdgeDetection(im,imHE);
	VerticalProjection(imHE, aVP);

	v.vmaxVPpeak=1.0/aVP.MaxValue();

	a3=0;
	for (int i=0; i<aVP.Size(); i++)
	{
		a3+=aVP[i];
	}
	v.varea=1/a3;

	v.vproportion=TML::Abs(TML::Abs(double(im.Width())/(im.Height()))-5);

	v.vSummary=0.001*v.vheight+0.25*v.vmaxVPpeak+0.4*v.varea+0.4*v.vproportion;

	//критические ограничения


	return v.vSummary;
}

int ImageProcessor::AdaptiveThreshold(
	TIL::GrayImage &im, TCL::Array<int> &aH,
	int &p1, int &p2)
{
	TCL::Array<int> aT(TIL::GrayPixel::Max()+1);
	aT.Fill(0);

	aH.Resize(aT.Size());
	aH.Fill(0);


	TIL::GrayPixel *p=im.Pointer();
	for (int i=0; i<im.Height(); i++)
	{
		for (int j=0; j<im.Width(); j++)
		{
			aT[p->I()]++;
			p++;
		}
	}

	Average(aT,aH,6);

	aT.Copy(aH);
	//aH.Copy(aT);

	p1=aT.MaxIndex();
	int p1v=aT[p1];
	for (int i=p1; i<aT.Size()-1; i++)
	{
		if (aT[i]>=aT[i+1])
		{
			aT[i]=0;
		} else
		{
			break;
		}
	}
	aT[p1]=p1v;
	for (int i=p1; i>0; i--)
	{
		if (aT[i]>=aT[i-1])
		{
			aT[i]=0;
		} else
		{
			break;
		}
	}

	p2=aT.MaxIndex();

	return (p1+p2)/2;
	
}

int ImageProcessor::OtsuBinarizationThreshold( TIL::GrayImage &im )
{
	int mi=TIL::GrayPixel::Max()+1;
	TCL::Array<int> aT(mi);
	aT.Fill(0);

	int w=im.Width(),
		h=im.Height(),
		s=h*w;

	TIL::GrayPixel *p=im.Pointer();
	for (int i=0; i<h; i++)
	{
		for (int j=0; j<w; j++)
		{
			aT[p->I()]++;
			p++;
		}
	}

	TCL::Array<double> P(mi), q1(mi),q2(mi), m1(mi),m2(mi);
	for (int i=0; i<mi; i++)
	{
		P[i]=double(aT[i])/s;
	}
	double m=0;
	for (int t=0; t<mi; t++)
	{
		m+=t*P[t];
		q2[t]=0;
		for (int i=t+1; i<mi; i++)
		{
			q2[t]+=P[i];
		}
	}

	q1[0]=P[0];
	m1[0]=0;

	for (int t=0; t<mi-1; t++)
	{
		q1[t+1]=q1[t]+P[t+1];

		m1[t+1]=(q1[t]*m1[t]+(t+1)*P[t+1])/q1[t+1];		

		m2[t+1]=(m-q1[t+1]*m1[t+1])/(1-q1[t+1]);
	}

	double smax=-1;
	int tmax=0;
	for (int t=0; t<mi; t++)
	{
		double sigma=q1[t]*(1-q1[t])*TML::Sqr(m1[t]-m2[t]);
		if (sigma>smax) 
		{
			smax=sigma;
			tmax=t;
		}
	}

	return tmax;
}

void ImageProcessor::BernsenBinarization(TIL::GrayImage &imSrc, 
		TIL::GrayImage &imDst)
{
	const int m=2; //размер решётки
	const int th=30; //порог

	int w=imSrc.Width(),
		h=imSrc.Height();

	imDst.Create(w,h);

	for (int i=0; i<h; i++)
	{
		for (int j=0; j<w; j++)
		{
			int bmax=-1, bmin=300;
			for (int k=max(0,i-m); k<=min(h-1,i+m); k++)
			{
				for (int l=max(0,j-m); l<=min(w-1,j+m); l++)
				{
					TIL::GrayPixel &p=imSrc.Pixel(l,k);
					if (p.I()>bmax) bmax=p.I();
					if (p.I()<bmin) bmin=p.I();
				}
			}

			int b=(bmax+bmin)/2;
			if (imSrc.Pixel(j,i).I()>=b)
			{
				imDst.Pixel(j,i)=TIL::GrayPixel::Max();
			} else
			{
				imDst.Pixel(j,i)=0;
			}

		}
	}

}
void ImageProcessor::NiblekBinarization(TIL::GrayImage &imSrc,
																				TIL::GrayImage &imDst)
{
	const int m=5; //размер решётки
	const int th=30; //порог
	const double kk=0.;

	int w=imSrc.Width(),
		h=imSrc.Height();

	imDst.Create(w,h);

	for (int i=0; i<h; i++)
	{
		for (int j=0; j<w; j++)
		{
			int bmax=-1, bmin=300;
			double m1=0,m2=0,d=0;
			int n=0;
			for (int k=max(0,i-m); k<=min(h-1,i+m); k++)
			{
				for (int l=max(0,j-m); l<=min(w-1,j+m); l++)
				{
					TIL::GrayPixel &p=imSrc.Pixel(l,k);
					int t=p.I();

					m1+=t;
					m2+=TML::Sqr(t);			
					n++;

				}
			}

			m1/=n;
			m2/=n;
			d=m2-TML::Sqr(m1);

			int b=TML::Round(m1+kk*d);
			//int b=TML::Round(m1);

			if (imSrc.Pixel(j,i).I()>=b)
			{
				imDst.Pixel(j,i)=TIL::GrayPixel::Max();
			} else
			{
				imDst.Pixel(j,i)=0;
			}

		}
	}	
}
void ImageProcessor::Normalization( TIL::GrayImage &im )
{
	TIL::GrayPixel *p=im.Pointer();
	int I=TIL::GrayPixel::Max();
	
	int Hmax=-1, Hmin=300;

	for (int i=0; i<im.Height(); i++)
	{
		for (int j=0; j<im.Width(); j++)
		{
			int t=p->I();
			if (t>Hmax) Hmax=t;
			if (t<Hmin) Hmin=t;
			p++;
		}
	}
	p=im.Pointer();
	for (int i=0; i<im.Height(); i++)
	{
		for (int j=0; j<im.Width(); j++)
		{			
			p->I()=(p->I()-Hmin)*(I-1)/(Hmax-Hmin);
			p++;
		}
	}
}
void ImageProcessor::ContrastGrayImage(TIL::GrayImage &im, int nT)
{
	TIL::GrayPixel *p=im.Pointer();


	for (int i=0; i<im.Height(); i++)
	{
		for (int j=0; j<im.Width(); j++)
		{
			if (p->I()>=nT)
			{
				p->I()=TIL::GrayPixel::Max();
			} else
			{
				p->I()=0;
			}
			p++;
		}
	}

}
void ImageProcessor::GetDivision(
		TIL::GrayImage &imSrc,
		TCL::Array<int> &aDivs,		
		TCL::Array<int> &aHist)
{
	const double cx=1;
	const double cw=DIVISION_MODIFIER;

	TCL::Array<int> aP;


	HorizontalProjection(imSrc,aHist);
	////сделаем не сумму интенсивностей, а максимум
	//int w=imSrc.Width(),
	//	h=imSrc.Height();
	//TCL::Array<int> MMM(w);
	//MMM.Fill(0);
	//TCL::Array<int> mmm(w);
	//mmm.Fill(TIL::GrayPixel::Max());
	//int m=0;

	//aHist.Resize(w);
	//aHist.Fill(TIL::GrayPixel::Max());
	//for (int i=0; i<w; i++)
	//{
	//	TCL::Array<int> aa(h);
	//	int aaCnt=0;
	//	int N=4;
	//	for (int j=0; j<h; j++)
	//	{
	//		
	//		int ints=imSrc.Pixel(i,j).I();
	//		if (ints==TIL::GrayPixel::Max()) continue;
	//		aa[aaCnt++]=ints;
	//		if (ints<mmm[i]) mmm[i]=ints;
	//		if (ints>MMM[i]) MMM[i]=ints;
	//	}
	//	aa.Resize(aaCnt);
	//	if (aaCnt<N)
	//	{
	//		m=0;
	//	} else
	//	{
	//		TCL::NthElement< int,TCL::Less< int > >(aa,N);
	//		mmm[i]=aa[N];

	//		TCL::NthElement< int,TCL::Greater< int > >(aa,N);
	//		MMM[i]=aa[N];

	//		int mz=MMM[i]-mmm[i];
	//		if (mz>m) m=mz;
	//	}
	//}
	//for (int i=0; i<w; i++)
	//{
	//	if (m) aHist[i]=300*(MMM[i]-mmm[i])/m;
	//	else aHist[i]=1;
	//}
	////вроде нашли

	TCL::Array<int> aT;
	aT.Copy(aHist);
	//for (int i=0; i<aHist.Size(); i++) aT=aHist[i];//инициализация массивом


	Average(aT,aHist,1);
	//PeakRecession(aHist);
	aP.Copy(aHist);

	int vm=aP.MaxValue();
	int mm=0;
	int NormPeak=1000;
	for (int i=0; i<aP.Size(); i++) mm+=aP[i];
	mm/=aP.Size();

	if (vm==0) return ;
	for (int i=0; i<aP.Size(); i++)
	{
		aP[i]=NormPeak*aP[i]/vm;
	}

	TCL::Array<int> &aRez=aDivs;
	int aRCount=0;

	while (1) 
	{
		int xm=aP.MaxIndex(),
			xmv=aP[xm];
		for (int i=xm; i>0; i--)
		{
			if ((aP[i]>=aP[i-1]))
			{
				aP[i]=0;
			} else
			{
				break;
			}
		}
		for (int i=xm+1; i<aP.Size()-1; i++)
		{
			if ((aP[i]>=aP[i+1]))
			{
				aP[i]=0;
			} else
			{
				break;
			}
		}
		aP[xm]=0;

		if (xmv<NormPeak*cw) 
		{
			break;
		}

			if (aRez.Size()<=aRCount)
			{
				aRez.Resize(2*(aRCount+1));
			}
			aRez[aRCount++]=xm;
		

	}
	aRez.Resize(aRCount);

	

	//double cz=0.8;

	//TCL::Array<int> aK(aRez);

	//aRCount=1;
	//aRez[0]=aK[0];

	//int nT=0;
	//for (int i=1; i<aK.Size()-1; i++)
	//{
	//	int av=(aRez[nT]+aK[i+1])/2;
	//	if (aK[i]>av*cz)
	//	{
	//		if (aRCount>=aRez.Size()) aRez.Resize(2*(aRCount+1));

	//		aRez[aRCount++]=aK[i];
	//		nT++;
	//	}
	//}
	//aRez.Resize(aRCount);
}

void ImageProcessor::RecessHorizontalBorders(
	TIL::GrayImage &imSrc, TIL::GrayImage &imDst)
{
	//imDst.Copy(imSrc);
	int w=imDst.Width(),
		h=imDst.Height();

	TIL::GrayPixel *p=imDst.Pointer();

	const int m=REC_HOR_BORDER_M;
	const double mf=REC_HOR_BORDER_MF;

	//сверху
	int tmax=-1;
	for (int i=0; i<h; i++)
	{
		int s=0;
		int av=0;
		for (int j=0; j<w; j++)
		{
			av+=imSrc.Pixel(j,i).I();
			int bmax=TIL::GrayPixel::Min(),
				bmin=TIL::GrayPixel::Max();
			double m1=0,m2=0,d=0;
			int n=0;
			for (int k=max(0,i-m); k<=min(h-1,i+m); k++)
			{
				for (int l=max(0,j-m); l<=min(w-1,j+m); l++)
				{
					TIL::GrayPixel &p=imSrc.Pixel(l,k);
					int t=p.I();

					m1+=t;			
					n++;
				}
			}
			m1/=n;
			int b=TML::Round(m1);

			if (!(imSrc.Pixel(j,i).I()>=b))
			{
				s++;
			}
		}
		av/=w;
		if ((s>TML::Round(w*mf)))
		{
			//for (int j=0; j<w; j++)
			//{
			//	imDst.Pixel(j,i).I()=TIL::GrayPixel::Max();
			//}
			tmax=i;
		} else
		{
			break;
		}
	}	

	//снизу
	int bmax=h;
	for (int i=h-1; i>=0; i--)
	{
		int s=0;
		int av=0;
		for (int j=0; j<w; j++)
		{
			av+=imSrc.Pixel(j,i).I();
			int bmax=TIL::GrayPixel::Min(), bmin=TIL::GrayPixel::Max();
			double m1=0,m2=0,d=0;
			int n=0;
			for (int k=max(0,i-m); k<=min(h-1,i+m); k++)
			{
				for (int l=max(0,j-m); l<=min(w-1,j+m); l++)
				{
					TIL::GrayPixel &p=imSrc.Pixel(l,k);
					int t=p.I();

					m1+=t;			
					n++;
				}
			}
			m1/=n;
			int b=TML::Round(m1);

			if (imSrc.Pixel(j,i).I()<b)
			{
				s++;
			}
		}
		av/=w;
		if ((s>TML::Round(w*mf)))
		{
			//for (int j=0; j<w; j++)
			//{
			//	imDst.Pixel(j,i).I()=TIL::GrayPixel::Max();
			//}
			bmax=i;
		} else
		{
			break;
		}
	}


	TIL::GrayPixel z(TIL::GrayPixel::Max());
	tmax+=2;
	bmax-=2;
	if (bmax-tmax+1>0)
	{
		imDst.CopyBlock(imSrc,0,tmax,w,bmax-tmax+1,z);
	} else
	{
		imDst.Copy(imSrc);
	}
}

void ImageProcessor::RecessVerticalBorders(
		TIL::GrayImage &imSrc,
		TIL::GrayImage &imDst)
{
	imDst.Copy(imSrc);
	int w=imDst.Width(),
		h=imDst.Height();

	TIL::GrayPixel *p=imDst.Pointer();

	const int m=REC_VERT_BORDER_M;
	const double mf=REC_VERT_BORDER_MF;

	//слева

	int lmax=0;
	for (int i=0; i<w; i++)
	{
		int s=0;
		int av=0;
		for (int j=0; j<h; j++)
		{
			av+=imSrc.Pixel(i,j).I();
			int bmax=TIL::GrayPixel::Min(), bmin=TIL::GrayPixel::Max();
			double m1=0,m2=0,d=0;
			int n=0;
			for (int k=max(0,i-m); k<=min(w-1,i+m); k++)
			{
				for (int l=max(0,j-m); l<=min(h-1,j+m); l++)
				{
					TIL::GrayPixel &p=imSrc.Pixel(k,l);
					int t=p.I();

					m1+=t;			
					n++;
				}
			}
			m1/=n;
			int b=TML::Round(m1);

			if (!(imSrc.Pixel(i,j).I()<=b))
			{
				s++;
			}
		}
		av/=h;
		if ((s>TML::Round(h*mf)))
		{
			for (int j=0; j<h; j++)
			{
				imDst.Pixel(i,j).I()=TIL::GrayPixel::Min();

			}
			lmax=i;
		} else
		{
			for (int k=max(0,lmax-1); k<=lmax; k++)
			{
				for (int j=0; j<h; j++)
				{
					imDst.Pixel(k,j).I()=imSrc.Pixel(k,j);
				}
			}

			break;
		}
	}	

	//справа
	int rmax=w-1;
	for (int i=w-1; i>=0; i--)
	{
		int s=0;
		int av=0;
		for (int j=0; j<h; j++)
		{
			av+=imSrc.Pixel(i,j).I();
			int bmax=TIL::GrayPixel::Min(), bmin=TIL::GrayPixel::Max();
			double m1=0,m2=0,d=0;
			int n=0;
			for (int k=max(0,i-m); k<=min(w-1,i+m); k++)
			{
				for (int l=max(0,j-m); l<=min(h-1,j+m); l++)
				{
					TIL::GrayPixel &p=imSrc.Pixel(k,l);
					int t=p.I();

					m1+=t;			
					n++;
				}
			}
			m1/=n;
			int b=TML::Round(m1);

			if (imSrc.Pixel(i,j).I()<b)
			{
				s++;
			}
		}
		av/=h;
		if ((s>TML::Round(w*mf)))
		{
			for (int j=0; j<h; j++)
			{
				imDst.Pixel(i,j).I()=TIL::GrayPixel::Min();
			}
			rmax=i;
		} else
		{
			//засветим полосу за границей белого света
			for (int k=min(rmax+1,w-1); k>=rmax; k--)
			{
				for (int j=0; j<h; j++)
				{
					imDst.Pixel(k,j).I()=imSrc.Pixel(k,j);
				}
			}

			break;
		}
	}
}



void ImageProcessor::DivideIntoBlocks(
	TIL::GrayImage &imSrc,
	TCL::Array<int> &aDivs,
	TCL::Array<TIL::GrayImage> &aParts)
{
	int w=imSrc.Width(),
		h=imSrc.Height();

	////крайние тоже захватим
	//aDivs.InsertFront(0);
	//aDivs.InsertBack(w-1);

	int nCount=0;

	

	for (int i=0; i<aDivs.Size()-1; i++)
	{
		int width=aDivs[i+1]-aDivs[i];

		if (width>4)
		{
			if (aParts.Size()<=nCount) aParts.Resize((nCount+1)*2);


			int ww=aDivs[i+1]-aDivs[i]+1;
			aParts[nCount].Create(ww,h);
			aParts[nCount].Copy(imSrc,0,0,aDivs[i],0,ww,h);
			nCount++;
		}
	}
	aParts.Resize(nCount);
}

void ImageProcessor::GetIntensityOnDivisions(
	TIL::GrayImage &imSrc,
	TCL::Array<int> &aDivs,
	TCL::Array<TCL::Array<int> > &aProj)
{
	int s=aDivs.Size();
	int h=imSrc.Height();
	aProj.Resize(s);
	
	for (int i=0; i<s; i++)
	{
		aProj[i].Resize(h);
		for (int j=0; j<h; j++)
		{
			aProj[i][j]=imSrc.Pixel(aDivs[i],j);
		}
	}
}

void ImageProcessor::ImproveDivision1(
	TIL::GrayImage &im,
	TCL::Array<int> &aDiv)
{
	int nMagicConstant=IMPROVE_BORDER;

	//aDiv.InsertFront(0);
	//aDiv.InsertBack(im.Width()-1);

	TCL::Sort<int,TCL::Less<int>>(aDiv);

	TCL::Array<int> aP;
	HorizontalProjection(im,aP);
	

	while (1)
	{
		if (aDiv.Size()<=7)
		{
			break;
		}
		double av=0;
		int _max=-1,
			nMaxInd=-1,
			_min=999,
			nMinInd=-1;
		for (int i=0; i<aDiv.Size()-1; i++)
		{
			int d=aDiv[i+1]-aDiv[i];
			av+=d;
			//if (d>_max)
			//{
			//	_max=d;
			//	nMaxInd=i;
			//}
			if (d<_min)
			{
				_min=d;
				nMinInd=i;
			}
		}
		av/=(aDiv.Size()-1);

		//отклонение в процентах
		if (av/_min*100>nMagicConstant)
		{
			int t=nMinInd;
			if (aP[aDiv[t]]>aP[aDiv[t+1]])
			{
				t++;
			}
			aDiv.Remove(t);
		} else
		{
			break;
		}
	}

	if (aDiv.Size()>10)
	{
		int nMin=999999999;
		int nInd=-1;
		for (int i=0; i<aDiv.Size(); i++)
		{
			if (aP[aDiv[i]]<nMin)
			{
				nMin=aP[aDiv[i]];
				nInd=i;
			}
		}
		if (nInd!=-1)
		{
			aDiv.Remove(nInd);
			ImproveDivision1(im,aDiv);
		}
	}


	return;
}

void ImageProcessor::ImproveDivision2(
	TIL::GrayImage &im,
	TCL::Array<int> &aDiv)
{

	if (!aDiv.Size()) return;

	double c=IMPR2_DIV_CONSTANT;


	TCL::Array<int> aProj;

	if (aDiv[0]>3) aDiv.InsertFront(0);
	if (im.Width()-aDiv[aDiv.Size()-1]>3) aDiv.InsertBack(im.Width()-1);

	for (int i=0; i<aDiv.Size()-1; i++)
	{
		int p1=aDiv[i], 
			p2=aDiv[i+1],
			w=p2-p1+1,
			h=im.Height();

		TIL::GrayPixel *p=im.Pointer();
		p+=p1;
		aProj.Resize(w);
		aProj.Fill(0);

		int av=0;

		for (int j=0; j<h; j++)
		{
			for (int k=0; k<w; k++)
			{
				aProj[k]+=p->I();
				p++;
			}
			p+=im.Width()-w;			
		}

		for (int j=0; j<w; j++)
		{
			av+=aProj[j];
		}
		av/=w;

		TCL::Array<int> aT;
		aT.Copy(aProj);
		Average(aT,aProj,5);
		aT.Copy(aProj);

		TCL::Array<int> aTop,
										aBottom;
		int nTop=0,
				nBottom=0;

		while (1)
		{
			int nInd=aT.MaxIndex();
			bool flB=false;
			if (aT[nInd]>TML::Round(av*c))
			{
				if (nTop>=aTop.Size()) aTop.Resize((nTop+1)*2);
				aTop[nTop++]=nInd;

				for (int j=nInd; j<w; j++)
				{

					if (aT[j]>=av) 
					{
						aT[j]=av;
					} else
					{
						break;
					}
				}

				for (int j=nInd; j>=0; j--)
				{
					if (aT[j]>=av)
					{
						aT[j]=av;
					} else
					{
						break;
					}
				}
			} else
			{
				flB=true;
			}

			nInd=aT.MinIndex();
			if (aT[nInd]<av)
			{	
				if (nBottom>=aBottom.Size()) aBottom.Resize((nBottom+1)*2);
				aBottom[nBottom++]=nInd;

				for (int j=nInd; j<w; j++)
				{
					if (aT[j]<=av)
					{
						aT[j]=av;
					} else
					{
						break;
					}
				}
				for (int j=nInd; j>=0; j--)
				{
					if (aT[j]<=av)
					{
						aT[j]=av;
					} else
					{
						break;
					}
				}
			} else
			{
				if (flB) break;
			}
		}
		
		aTop.Resize(nTop);

		if (aTop.Size())
		{
			TCL::Sort<int,TCL::Less<int>>(aTop);
		}

		aBottom.Resize(nBottom);
		if (aBottom.Size())
		{
			TCL::Sort<int, TCL::Less<int>>(aBottom);
		}

		aT.Resize(0);		

		double MAJIC_EPS=0.6;

		for (int j=0; j<aBottom.Size()-1; j++)
		{
			for (int k=j+1; k<aBottom.Size(); k++)
			{
				int p1=aBottom[j],
						p2=aBottom[k];


				for (int l=0; l<aTop.Size(); l++)
				{
					int p3=aTop[l];

					int d1=TML::Sqr(p1-p3),
							d2=TML::Sqr(p2-p3);
					double dd=d1/d2;
					double ddd=TML::Sqr(dd-1);


					if ((p3>p1)&&(p3<p2))
					{
						aDiv.Insert(aDiv[i]+p3,i+1);
						goto cycles_exit;
					}
				}
			}
		}

cycles_exit:;

	}
	



}


void ImageProcessor::AverageThreshold(
 TIL::GrayImage &imSrc,
 TIL::GrayImage &imDst )
{
	int w=imSrc.Width(),
		h=imSrc.Height();

	int v=0;

	TIL::GrayPixel *p=imSrc.Pointer();

	for (int i=0; i<h; i++)
	{
		for (int j=0; j<w; j++)
		{
			v+=p->I();
			p++;
		}
	}

	v=TML::Round(double(v)/(w*h));

	imDst.Copy(imSrc);

	ContrastGrayImage(imDst,v);
}

void ImageProcessor::MedianThreshold(
	 TIL::GrayImage &imSrc,
	 TIL::GrayImage &imDst)
{
	int w=imSrc.Width(),
		h=imSrc.Height();
	TCL::Array<int> aI(w*h);

	TIL::GrayPixel *p=imSrc.Pointer();

	int nCount=0;
	for (int i=0; i<h; i++)
	{
		for (int j=0; j<w; j++)
		{
			aI[nCount++]=p->I();
			p++;
		}
	}

	int v=TCL::Median<int,TCL::Less<int>> (aI);

	imDst.Copy(imSrc);
	ContrastGrayImage(imDst,v);
}

void ImageProcessor::GetComponents(
	 TIL::GrayImage &im,
	 TCL::Array<TIL::GrayImage> &aRez,
	 TCL::Array<TWL::Rect> &r)
{

	int nRez=0;

	int h=im.Height(),
			w=im.Width();

	//watched - карта флагов просмотренных пикселей
	//imMap - массив чб точек
	//false - белый, true - черный
	TCL::Array<TCL::Array<bool>> imMap, watched;	
	
	//statck - стэк, не может превосходить размера изображения
	//comp - массив отдельной компоненты связности
	// stop - размер стека, индекс следующего за вершиной элемента
	//nComp - размер компоненты связности
	TCL::Array<TWL::Point> stack(h*w), comp(h*w);
	int stop=0, nComp=0;

	//создаём карты
	imMap.Resize(h);
	watched.Resize(h);
	TIL::GrayPixel *p=im.Pointer();

	//по рядам
	for (int i=0; i<h; i++)
	{
		imMap[i].Resize(w);
		//все - белые
		imMap[i].Fill(false);
		
		watched[i].Resize(w);
		// не смотрели пока ничего
		watched[i].Fill(false);
		
		//по i-му ряду 
		for (int j=0; j<w; j++)
		{
			if (!(p->I()))
			{
				//кроме чёрных
				imMap[i][j]=true;
			}
			p++;
		}
	}

	//до бесконечности 8)
	while (true)
	{
		nComp=0; //нету рисунка

		//ежели не пуст стек
		while (stop)
		{
			TWL::Point p=stack[--stop];

			comp[nComp++]=p;

			if (p.X()>0)
			{
				int x=p.X()-1,
						y=p.Y();

				if (!watched[y][x])
				{
					watched[y][x]=true;
					if (imMap[y][x])
					{
						stack[stop++]=TWL::Point(x,y);
					}
				}
			}

			if (p.X()<w-1)
			{
				int x=p.X()+1,
						y=p.Y();

				if (!watched[y][x])
				{
					watched[y][x]=true;
					if (imMap[y][x])
					{
						stack[stop++]=TWL::Point(x,y);
					}
				}
			}

			if (p.Y()>0)
			{
				int x=p.X(),
						y=p.Y()-1;

				if (!watched[y][x])
				{
					watched[y][x]=true;
					if (imMap[y][x])
					{
						stack[stop++]=TWL::Point(x,y);
					}
				}
			}

			if (p.Y()<h-1)
			{
				int x=p.X(),
						y=p.Y()+1;

				if (!watched[y][x])
				{
					watched[y][x]=true;
					if (imMap[y][x])
					{
						stack[stop++]=TWL::Point(x,y);
					}
				}
			}			
		}

		if (nComp)
		{
			if (nComp)
			{			
				while (nRez>=aRez.Size())
				{
					aRez.Resize((nRez+1)*2);
					r.Resize((nRez+1)*2);
				}

				MakeImageFromPoints(aRez[nRez],comp,nComp,r[nRez]);
				nRez++;

				nComp=0;
			}
		}

		//флаг выхода из бесконечного основного цикла
		bool flOut=true;

		for (int i=0; i<h; i++)
		{
			for (int j=0; j<w; j++)
			{
				if ((!watched[i][j])&&imMap[i][j])
				{
					watched[i][j]=true;
					stack[stop++]=TWL::Point(j,i);
					flOut=false;
					goto cycle_out;
				}
			}
		}
cycle_out:;
		if (flOut)
		{
			break;
		}
	}	
	aRez.Resize(nRez);
	r.Resize(nRez);
}

void ImageProcessor::MakeImageFromPoints(
	TIL::GrayImage &im, 
  TCL::Array<TWL::Point> &a,
  int nP,
	TWL::Rect &r)
{	

	int maxx=0, maxy=0, minx=2000000, miny=2000000;

	for (int i=0; i<nP; i++)
	{
		int x=a[i].X(),
			  y=a[i].Y();

		if (x>maxx) maxx=x;
		if (x<minx) minx=x;
		if (y>maxy) maxy=y;
		if (y<miny) miny=y;
	}

	int w=maxx-minx+1,
			h=maxy-miny+1;

	im.Create(w,h);
	TIL::GrayPixel p(TIL::GrayPixel::Max());
	im.Fill(p);

	for (int i=0; i<nP; i++)
	{
		int x=a[i].X(),
			  y=a[i].Y();

		im.Pixel(x-minx,y-miny).I()=TIL::GrayPixel::Min();
	}
	r.left=minx;
	r.right=maxx;
	r.top=miny;
	r.bottom=maxy;

	
}

void ImageProcessor::ChooseSymbol(
	 TCL::Array<TIL::GrayImage> &aSrc,
	 TIL::GrayImage &imRez,
	 TCL::Array<TWL::Rect> &aR,
	 TWL::Rect &r)
{
	//всё просто как три копейки:
	//из всех компонент одного разбиения выберем ту
	//у которой белых пекселей больше

	int s=aSrc.Size();

	TCL::Array<int> aBlack(s);
	aBlack.Fill(0);


	for (int i=0; i<s; i++)
	{		
		TIL::GrayPixel *p=aSrc[i].Pointer();

		for (int j=0; j<aSrc[i].Height(); j++)
		{
			for (int k=0; k<aSrc[i].Width(); k++)
			{
				if (TIL::GrayPixel::Min()==p->I())
				{
					aBlack[i]++;
				}
				p++;
			}
		}
	}

	int z=aBlack.MaxIndex();
	imRez.Copy(aSrc[z]);
	r=aR[z];
}

void ImageProcessor::Skeletonize(
	 TIL::GrayImage &imSrc, 
	 TIL::GrayImage &imRez)
{
	int w=imSrc.Width(),
		h=imSrc.Height();

	//true - белый, false - чёрный
	//на карте добавлена рамка белого цвета
	TCL::Array<TCL::Array<bool>> aMap(h+2);

	//граница, содержит координаты граничных пикселёв
	TCL::Array<TWL::Point> aB((w+2)*(h+2));
	// размер aB
	int nab=0;

	//содержит индексы элементнов в aB, которые следует уничтожить
	TCL::Array<int> aTD((w+2)*(h+2));
	
	for (int i=0; i<h+2; i++)
	{
		aMap[i].Resize(w+2);		
		for (int j=0; j<w+2; j++)
		{
			if ((!j)||(!i)||(j==w+1)||(i==h+1))
			{
				aMap[i][j]=1;
			} else
			{
				aMap[i][j]=(imSrc.Pixel(j-1,i-1).I()==TIL::GrayPixel::Max())
					?true:false;
			}
		}
	}

	bool work=true;
	while (work)
	{
		work=false;
		nab=0;
		for (int i=1; i<h+1; i++)
		{
			for (int j=1; j<w+1; j++)
			{
				if ((!aMap[i][j])&&(aMap[i][j-1]||aMap[i][j+1]||
					aMap[i-1][j-1]||aMap[i-1][j]||aMap[i-1][j+1]||					
					aMap[i+1][j-1]||aMap[i+1][j]||aMap[i+1][j+1]))
				{
					aB[nab]=TWL::Point(j,i);
					aTD[nab]=false;
					nab++;
				}				
			}
		}
		

		for (int i=0; i<nab; i++)
		{
			int x=aB[i].X(),
				y=aB[i].Y();
			bool ptl=aMap[y-1][x-1], pt=aMap[y-1][x], ptr=aMap[y-1][x+1],
				pl=aMap[y][x-1], p=aMap[y][x], pr=aMap[y][x+1],
				pbl=aMap[y+1][x-1], pb=aMap[y+1][x], pbr=aMap[y+1][x+1];

			int c=0;
			if (ptl) c++;
			if (pt) c++;
			if (ptr) c++;
			if (pl) c++;
			if (pr) c++;
			if (pbl) c++;
			if (pb) c++;
			if (pbr) c++;

			int v=0;
			if ((pt)&&(!ptr)) v++;
			if ((ptr)&&(!pr)) v++;
			if ((pr)&&(!pbr)) v++;
			if ((pbr)&&(!pb)) v++;
			if ((pb)&&(!pbl)) v++;
			if ((pbl)&&(!pl)) v++;
			if ((pl)&&(!ptl)) v++;
			if ((ptl)&&(!pt)) v++;
 
			if (!(pt||pr||pb)) continue;
			if (!(pl||pr||pb)) continue;
			if (!((2<=c)&&(c<=6))) continue;
			if (1!=v) continue;

			aTD[i]=true;
			work=true;
		}
		for (int i=0; i<nab; i++)
		{
			if (aTD[i]) aMap[aB[i].Y()][aB[i].X()]=true;
		}
	}

	imRez.Create(w,h);


	TIL::GrayPixel *p=imRez.Pointer();
	for (int i=0; i<h; i++)
	{
		for (int j=0; j<w; j++)
		{
			p->I()=TIL::GrayPixel::Max();
			if (!aMap[i+1][j+1]) p->I()=TIL::GrayPixel::Min();			
			p++;
		}
	}
}

