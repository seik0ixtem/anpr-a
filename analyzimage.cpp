#include "recognition.h"

AnalyzImage::AnalyzImage()
{
	//SetHandler(WM_MOUSEMOVE, &AnalyzImage::OnMouseMove);

}

AnalyzImage::~AnalyzImage()
{
}

void AnalyzImage::Draw(TWL::DeviceContext &rContext,void *pData)
{

	MainWindow &pMW=RecognitionApp::Instance().cMainWindow;
	//return;


	TWL::Bitmap bmpUser=UserBitmap();	

	TWL::Rect cr(0,0,bmpUser.Width(), bmpUser.Height());



	rContext.Fill(0,0,cr.right, cr.bottom, TWL::Color::Green());



	TCL::Array<TWL::Color> aColArray;
	aColArray.Resize(ImageProcessor::MAXBANDS);
	aColArray[0]=TWL::Color::Green();
	aColArray[1]=TWL::Color::Orange();
	aColArray[2]=TWL::Color::Red();
	aColArray[3]=TWL::Color::Blue();
	aColArray[4]=TWL::Color::Blue();
	if (pMW.ShowState==0)
	{
		TWL::DeviceContext dc;
		dc.CreateCompatible();
		HGDIOBJ hOlb=dc.Select(pMW.bmpGrayCurrent);

		rContext.Copy(dc,0,0,0,0,
			pMW.bmpGrayCurrent.Width(),
			pMW.bmpGrayCurrent.Height());				

		return;
	}

	if (pMW.ShowState==1)
	{
		TWL::DeviceContext dc;
		dc.CreateCompatible();

		TWL::Bitmap bmp;
		bmp.FromImage(pMW.imHorizontalEdge);

		HGDIOBJ hOlb=dc.Select(bmp);

		rContext.Copy(dc,0,0,0,0,
			bmp.Width(),
			bmp.Height());				

		return;
	}

	if (pMW.ShowState==2)
	{
		TWL::DeviceContext dc;
		dc.CreateCompatible();

		TWL::Bitmap bmp;
		bmp.FromImage(pMW.imVerticalEdge);

		HGDIOBJ hOlb=dc.Select(bmp);

		rContext.Copy(dc,0,0,0,0,
			bmp.Width(),
			bmp.Height());				

		return;
	}

	if (pMW.ShowState==3)
	{
		TWL::DeviceContext dc;
		dc.CreateCompatible();

		TWL::Bitmap bmp;
		bmp.FromImage(pMW.imVerticalEdge);

		HGDIOBJ hOlb=dc.Select(bmp);

		rContext.Copy(dc,0,0,0,0,
			bmp.Width(),
			bmp.Height());	

		///горизонтальная проекция
		if (pMW.flHorizontalProjection)
		{
			int nMax=0;
			for (int i=0; i<pMW.aHProjection.Size(); i++)
			{
				if (pMW.aHProjection[i]>nMax) nMax=TML::Round(pMW.aHProjection[i]);
			}

			rContext.MoveTo(TWL::Point(
				0,
				TML::Round((nMax-pMW.aHProjection[0])*double(cr.Height())/nMax)));

			for (int i=1; i<pMW.aHProjection.Size(); i++)
			{
				rContext.LineTo(
					i,
					TML::Round((nMax-pMW.aHProjection[i])*double(cr.Height())/nMax),
					TWL::Color::Red());
			}
		}

		return;
	}

	if (pMW.ShowState==4)
	{

		TWL::DeviceContext dc;
		dc.CreateCompatible();

		TWL::Bitmap bmp;
		bmp.FromImage(pMW.imHorizontalEdge);

		HGDIOBJ hOlb=dc.Select(bmp);

		rContext.Copy(dc,0,0,0,0,
			bmp.Width(),
			bmp.Height());	

		///вертикальная проекция всего изображения

		if (pMW.flVerticalProjection)
		{
			int nMax=0;
			for (int i=0; i<pMW.aVProjection.Size(); i++)
			{
				if (pMW.aVProjection[i]>nMax) nMax=TML::Round(pMW.aVProjection[i]);
			}

			rContext.MoveTo(TWL::Point(
				TML::Round(pMW.aVProjection[0]*double(cr.Width())/nMax),
				0));

			for (int i=1; i<pMW.aVProjection.Size(); i++)
			{
				rContext.LineTo(
					TML::Round(pMW.aVProjection[i]*double(cr.Width())/nMax),
					i, TWL::Color::Red());
			}
		}

		//TWL::Bitmap bmp;
		//bmp.FromImage(pMW.imHorizontalEdge);
		//pMW.cHelpWindow.SetBitmap(bmp);

		

		return;
	}

	if (pMW.ShowState==5)
	{
		TWL::DeviceContext dc;
		dc.CreateCompatible();


		HGDIOBJ hOlb=dc.Select(pMW.bmpGrayCurrent);

		rContext.Copy(dc,0,0,0,0,
			pMW.bmpGrayCurrent.Width(),
			pMW.bmpGrayCurrent.Height());	

		///вертикальная проекция всего изображения

		if (pMW.flVerticalProjection)
		{
			int nMax=0;
			for (int i=0; i<pMW.aVProjection.Size(); i++)
			{
				if (pMW.aVProjection[i]>nMax) nMax=TML::Round(pMW.aVProjection[i]);
			}

			rContext.MoveTo(TWL::Point(
				TML::Round(pMW.aVProjection[0]*double(cr.Width())/nMax),
				0));

			for (int i=1; i<pMW.aVProjection.Size(); i++)
			{
				rContext.LineTo(
					TML::Round(pMW.aVProjection[i]*double(cr.Width())/nMax),
					i, TWL::Color::Red());
			}
		}


		for (int i=0; i<pMW.aBandBounds.Size(); i++)
		{
			rContext.Frame(0,pMW.aBandBounds[i].C(0),
				cr.Width(),pMW.aBandBounds[i].C(1),3,aColArray[i]);
		}
		
		return;
	}

	if (pMW.ShowState==6)
	{
		/// копирования потенциальных областей номера 
		TWL::DeviceContext oDC;
		if (pMW.aPlateBounds.Size()&&(!pMW.bmpCurrentBitmap.Empty()))
		{
			oDC.CreateCompatible();
			oDC.Select(pMW.bmpCurrentBitmap);
		}


		for (int i=0; i<pMW.aPlateBounds.Size(); i++)
		{
			for (int j=0; j<pMW.aPlateBounds[i].Size(); j++)
			{
				rContext.Copy(oDC,pMW.aPlateBounds[i][j].C(0),pMW.aBandBounds[i].C(0),
					pMW.aPlateBounds[i][j].C(0),pMW.aBandBounds[i].C(0),
					pMW.aPlateBounds[i][j].C(1)-pMW.aPlateBounds[i][j].C(0),
					pMW.aBandBounds[i].C(1)-pMW.aBandBounds[i].C(0));
			}
		}
		oDC.Delete();

		return;
	}

	if (pMW.ShowState==7)
	{
		///// снова кажется кандидаты

		//for (int i=0; i<pMW.abmpH.Size(); i++)
		//{
		//	TWL::DeviceContext tDC;
		//	tDC.CreateCompatible();
		//	tDC.Select(pMW.abmpH[i]);

		//	rContext.Copy(tDC,0,0,pMW.aPlates[i].r.left, pMW.aPlates[i].r.right,
		//		pMW.aPlates[i].r.Width(), pMW.aPlates[i].r.Height());

		//}

		/// копирования потенциальных областей номера 
		TWL::DeviceContext oDC;
		if (pMW.aPlateBounds.Size()&&(!pMW.bmpCurrentBitmap.Empty()))
		{
			oDC.CreateCompatible();
			oDC.Select(pMW.bmpCurrentBitmap);
		}


		for (int i=0; i<pMW.aPlateBounds.Size(); i++)
		{
			for (int j=0; j<pMW.aPlateBounds[i].Size(); j++)
			{
				rContext.Copy(oDC,pMW.aPlateBounds[i][j].C(0),pMW.aBandBounds[i].C(0),
					pMW.aPlateBounds[i][j].C(0),pMW.aBandBounds[i].C(0),
					pMW.aPlateBounds[i][j].C(1)-pMW.aPlateBounds[i][j].C(0),
					pMW.aBandBounds[i].C(1)-pMW.aBandBounds[i].C(0));
			}
		}
		oDC.Delete();

		/// просто полоску рисуем - линия поворота
		for (int i=0; i<pMW.aLines.Size(); i++)
		{
			rContext.Line(pMW.aLines[i], pMW.aPlates[i].r,
				TWL::Color::Blue(), 3);
		}

		return;
	}	


	if (pMW.ShowState==8)
	{


		/// выбираем лучшего кандидата
		int nIndex=-1;
		double m=10000000;
		for (int i=0; i<pMW.aPlates.Size(); i++)
		{
			if (pMW.aPlates[i].value<m)
			{
				nIndex=i;
				m=pMW.aPlates[i].value;
			}
		}
			
		if (nIndex>-1)
		{
			TWL::DeviceContext dc;
			dc.CreateCompatible();
			dc.Select(pMW.bmpCurrentBitmap);

			rContext.Copy(dc, pMW.aPlates[nIndex].r.left,pMW.aPlates[nIndex].r.top,
				pMW.aPlates[nIndex].r.left, pMW.aPlates[nIndex].r.top,
				pMW.aPlates[nIndex].r.Width(), pMW.aPlates[nIndex].r.Height());

			
			
		}

		return;
	}	

	if (pMW.ShowState==9)
	{





		/// рисование кандидатов с выводом их на экран в упорядоченном виде
		/// (столбцы)

		TWL::Bitmap cb;
		GetBitmap(cb);
		
		int nHPos=0,nVPos=0;
		int nMWidth=0;

		TCL::Array<TIL::GrayImage> &aIm=pMW.aPlateIms;

		for (int i=0; i<aIm.Size(); i++)
		{

			TWL::Bitmap bmp;

			bmp.FromImage(aIm[i]);

			TWL::DeviceContext dc;
			dc.CreateCompatible();
			dc.Select(bmp);

			rContext.Copy(dc,0,0,nHPos,nVPos,
				bmp.Width(), bmp.Height());

			if (nMWidth<bmp.Width()) nMWidth=bmp.Width();





			

			nVPos+=aIm[i].Height();



			if (i<aIm.Size()-1)
			{
				if (nVPos+aIm[i+1].Height()>cb.Height())
				{
					nHPos+=nMWidth;
					nVPos=0;
					nMWidth=0;
				}
			}
		}	



		return;
	}	

	if (pMW.ShowState==10)
	{
		/// рисование кандидатов с выводом их на экран в упорядоченном виде
		/// (столбцы)

		TWL::Bitmap cb;
		GetBitmap(cb);

		int nHPos=0,nVPos=0;
		int nMWidth=0;

		TCL::Array<TIL::GrayImage> &aIm=pMW.aPlateIms;

		for (int i=0; i<aIm.Size(); i++)
		{

			TWL::Bitmap bmp;

			bmp.FromImage(aIm[i]);

			TWL::DeviceContext dc;
			dc.CreateCompatible();
			dc.Select(bmp);

			rContext.Copy(dc,0,0,nHPos,nVPos,
				bmp.Width(), bmp.Height());

			if (nMWidth<bmp.Width()) nMWidth=bmp.Width();


				//построим график производной
				//вычислим амплитуду
				double ytop=pMW.aDervs[i].MaxValue(),
					ybottom=pMW.aDervs[i].MinValue();

				int w=bmp.Width(), h=bmp.Height();

				int zp=h/2; //ноль		
				
				rContext.Line(nHPos,nVPos+zp,nHPos+w-1,nVPos+zp,TWL::Color::Green());

				for (int j=0; j<pMW.aDervs[i].Size(); j++)
				{
					int yp=nVPos;
					int xp=nHPos+j;
					double y=pMW.aDervs[i][j];
					if (y>=0)
					{
						yp+=TML::Round(zp-y*zp/ytop);
					} else
					{
						yp+=TML::Round(zp+TML::Abs(y*zp/ybottom));
					}
					if (j)
					{
						rContext.LineTo(xp,yp,TWL::Color::Red());
					} else 
					{
						rContext.MoveTo(xp,yp);
					}
				}


			nVPos+=aIm[i].Height();



			if (i<aIm.Size()-1)
			{
				if (nVPos+aIm[i+1].Height()>cb.Height())
				{
					nHPos+=nMWidth;
					nVPos=0;
					nMWidth=0;
				}
			}
		}	



		return;
	}

	if (pMW.ShowState==11)
	{
		rContext.Fill(0,0,cr.right, cr.bottom, TWL::Color::White());
			//уменьшенная область кандидатов
		
			TWL::Bitmap cb;
			GetBitmap(cb);
			int nHPos=0,nVPos=0;
			int nMWidth=0;
		
			TCL::Array<TIL::GrayImage> &aIm=pMW.aImprPlates;
		
		//Выбор лучших
			struct __t
			{
				double val;
				int pos;
			};
		
			TCL::Array<__t> aBest(ImageProcessor::BEST);
		
			int nCount=0;
		
		
			for (int i=0; i<ImageProcessor::BEST; i++)
			{
				int nMin=-1;
				double m=1000000.999; 
				for (int j=0; j<pMW.aValues.Size(); j++)
				{
					if (pMW.aValues[j].vSummary<m) 
					{
						m=pMW.aValues[j].vSummary;
						nMin=j;
					}
				}
				if (nMin>-1)
				{
					aBest[nCount].val=m;
					aBest[nCount].pos=nMin;
					pMW.aValues[nMin].vSummary=10000000.0;
					nCount++;
				}
			}
		
			for (int i=0; i<nCount; i++)
			{
				pMW.aValues[aBest[i].pos].vSummary=aBest[i].val;
			}
		//закончили выбор лучших
		
		
			for (int i=0; i<aIm.Size(); i++)
			{
		
				TWL::Bitmap bmp;
		
				bmp.FromImage(aIm[i]);
		
				TWL::DeviceContext dc;
				dc.CreateCompatible();
				dc.Select(bmp);
		
				rContext.Copy(dc,0,0,nHPos,nVPos,
					bmp.Width(), bmp.Height());
		
		//обведём лучших рамкой
				TCL::Array<TWL::Color> aClr(ImageProcessor::BEST);
				aClr[0]=TWL::Color::Green();
				aClr[1]=TWL::Color::Yellow();
				aClr[2]=TWL::Color::Red();
				int flBest=-1;
				for (int j=0; j<nCount; j++)
				{
					if (aBest[j].pos==i) 
					{
						flBest=j;
						break;
					}
				}
				if (flBest!=-1)
				{
					rContext.Frame(nHPos,nVPos,bmp.Width()+nHPos, bmp.Height()+nVPos,
						2,aClr[flBest]);
				}
		
				if (nMWidth<bmp.Width()) nMWidth=bmp.Width();		
		
				nVPos+=aIm[i].Height();
		
				if (i<aIm.Size()-1)
				{
					if (nVPos+aIm[i+1].Height()>cb.Height())
					{
						nHPos+=nMWidth;
						nVPos=0;
						nMWidth=0;
					}
				}
			}


		return;
	}	

	if (pMW.ShowState==12)
	{
		pMW.ShowState++;
		return;

		//необрезанные кандидаты и их вертикальная проекция
		TWL::Bitmap cb;
		GetBitmap(cb);
		
		int nHPos=0,nVPos=0;
		int nMWidth=0;

		TCL::Array<TIL::GrayImage> &aIm=pMW.aDesk;

		for (int i=0; i<aIm.Size(); i++)
		{

			TWL::Bitmap bmp;

			bmp.FromImage(aIm[i]);

			TWL::DeviceContext dc;
			dc.CreateCompatible();
			dc.Select(bmp);

			rContext.Copy(dc,0,0,nHPos,nVPos,
				bmp.Width(), bmp.Height());

			if (nMWidth<bmp.Width()) nMWidth=bmp.Width();

			//график вертикальной проекции

			int xmax=pMW.aVProjs[i].MaxValue();

			

			for (int j=0; j<pMW.aVProjs[i].Size(); j++)
			{
				int x=nHPos+
					TML::Round(pMW.aPlates[i].r.Width()*pMW.aVProjs[i][j]/double(xmax)),
					y=nVPos+j;
				if (j)
				{
					rContext.LineTo(x,y,TWL::Color::Green());
				} else 
				{
					rContext.MoveTo(x,y);
				}
			}

			

			nVPos+=aIm[i].Height();



			if (i<aIm.Size()-1)
			{
				if (nVPos+aIm[i+1].Height()>cb.Height())
				{
					nHPos+=nMWidth;
					nVPos=0;
					nMWidth=0;
				}
			}
		}	

		return;
	}

	if (pMW.ShowState==13)
	{
		//кандидаты на разбиение, чёрно-белые, доп. инфо

		TWL::Bitmap cb;
		GetBitmap(cb);
		int nHPos=0,nVPos=0;
		int nMWidth=0;

		TCL::Array<TIL::GrayImage> &aIm=pMW.aDaBest;


		for (int i=0; i<aIm.Size(); i++)
		{

			TWL::Bitmap bmp;

			bmp.FromImage(aIm[i]);

			TWL::DeviceContext dc;
			dc.CreateCompatible();
			dc.Select(bmp);

			rContext.Copy(dc,0,0,nHPos,nVPos,
				bmp.Width(), bmp.Height());

			TWL::String s;

			if (nMWidth<bmp.Width()) nMWidth=bmp.Width();	

			//rContext.Line(pMW.aP[i].p1,nVPos,pMW.aP[i].p1,nVPos+256,
			//	TWL::Color::Red());
			//rContext.Line(pMW.aP[i].p2,nVPos,pMW.aP[i].p2,nVPos+256,
			//	TWL::Color::Red());


			//BuildGraph(rContext,pMW.aBWThres[i],
			//	TWL::Rect(nHPos,nVPos,nHPos+256,nVPos+256),
			//	TWL::Color::Green());


			nVPos+=aIm[i].Height()+10;
			//nVPos+=257; //для красивого вывода графика

			if (i<aIm.Size()-1)
			{
				if (nVPos+aIm[i+1].Height()>cb.Height())
				{
					//nHPos+=nMWidth;
					nHPos+=257;
					nVPos=0;
					nMWidth=0;
				}
			}
		}

		return;
	}

	if (pMW.ShowState==14)
	{
		//разбиение на цифры

		TWL::Bitmap cb;
		GetBitmap(cb);

		int nHPos=0,nVPos=0;
		int nMWidth=0;

		TCL::Array<TIL::GrayImage> &aIm=pMW.aDaBest;

		for (int i=0; i<aIm.Size(); i++)
		{

			//if (i>=pMW.aSyms.Size()) continue;
			if (!pMW.aBad.Size()) continue;
			if (pMW.aBad[i]) continue;
			TWL::Bitmap bmp;

			bmp.FromImage(aIm[i]);

			TWL::DeviceContext dc;
			dc.CreateCompatible();
			HGDIOBJ hOlb=dc.Select(bmp);

			rContext.Copy(dc,0,0,nHPos,nVPos,
				bmp.Width(), bmp.Height());		


			//нарисуем гистограмму проекции
			BuildGraph(rContext,pMW.aBestHisto[i],
				TWL::Rect(nHPos,nVPos+bmp.Height(),nHPos+pMW.aBestHisto[i].Size(),nVPos+300),
				TWL::Color::Green());
			//нарисовали

			//нарисуем границы разбиения
			for (int j=0; j<pMW.aDivs[i].Size(); j++)
			{
				rContext.Line(nHPos+pMW.aDivs[i][j],nVPos,
					nHPos+pMW.aDivs[i][j],nVPos+bmp.Height(), TWL::Color::Red());
			}
			//нарисовали


			if (nMWidth<bmp.Width()) nMWidth=bmp.Width();

			//график интенсивности разбиения
			int nP=nMWidth;
			if (pMW.aDivProj.Size())
			{
				for (int j=0; j<pMW.aDivProj[i].Size(); j++)
				{
					BuildGraph(rContext,pMW.aDivProj[i][j],
						TWL::Rect(nP,nVPos,nP+pMW.aDivProj[i][j].Size(),nVPos+300),TWL::Color::Red());
					nP+=pMW.aDivProj[i][j].Size();
					rContext.Line(nP,nVPos,nP,nVPos+300,TWL::Color::Black());
					nP++;
				}
			}
			//нарисовали

			////вывод символов
			int nPX=nMWidth + 5,
				nPY=nVPos;

			//if (i<pMW.aSyms.Size())
			//{
			//	for (int j=0; j<pMW.aSyms[i].Size(); j++)
			//	{

			//		TWL::Bitmap bmp;
			//		bmp.FromImage(pMW.aSyms[i][j]);

			//		dc.Select(bmp.Handle());

			//		rContext.Copy(dc,0,0,nPX,nVPos,bmp.Width(),bmp.Height());
			//		nPX+=bmp.Width()+6;
			//	}
			//}
			//нарисовали


			//nVPos+=aIm[i].Height();
			nVPos=nPY+100;
			if (i<aIm.Size()-1)
			{
				if (nVPos+aIm[i+1].Height()>cb.Height())
				{
					nHPos+=nMWidth;
					nVPos=0;
					nMWidth=0;
				}
			}
		}	

		return;
	}

	//for (int i=0; i<pMW.aBandBounds.Size(); i++)
	//{
	//	rContext.Frame(0,pMW.aBandBounds[i].C(0),
	//		cr.Width(),pMW.aBandBounds[i].C(1),3,aColArray[i]);
	//}



	/////горизонтальная проекция отдельных полос
	//if (pMW.flHorizontalProjection)
	//{
	//	int nMax=0;
	//	for (int i=0; i<pMW.aHProjection.Size(); i++)
	//	{
	//		if (pMW.aHProjection[i]>nMax) nMax=TML::Round(pMW.aHProjection[i]);
	//	}

	//	rContext.MoveTo(TWL::Point(
	//		0,
	//		TML::Round((nMax-pMW.aHProjection[0])*double(cr.Height())/nMax)));

	//	for (int i=1; i<pMW.aHProjection.Size(); i++)
	//	{
	//		rContext.LineTo(
	//			i,
	//			TML::Round((nMax-pMW.aHProjection[i])*double(cr.Height())/nMax),
	//			 TWL::Color::Red());
	//	}
	//}

	///// копирования потенциальных областей номера 
	//TWL::DeviceContext oDC;
	//if (pMW.aPlateBounds.Size()&&(!pMW.bmpCurrentBitmap.Empty()))
	//{
	//	oDC.CreateCompatible();
	//	oDC.Select(pMW.bmpCurrentBitmap);
	//}


	//for (int i=0; i<pMW.aPlateBounds.Size(); i++)
	//{
	//	for (int j=0; j<pMW.aPlateBounds[i].Size(); j++)
	//	{
	//		rContext.Copy(oDC,pMW.aPlateBounds[i][j].C(0),pMW.aBandBounds[i].C(0),
	//			pMW.aPlateBounds[i][j].C(0),pMW.aBandBounds[i].C(0),
	//			pMW.aPlateBounds[i][j].C(1)-pMW.aPlateBounds[i][j].C(0),
	//			pMW.aBandBounds[i].C(1)-pMW.aBandBounds[i].C(0));
	//	}
	//}
	//oDC.Delete();

	/////вертикальная проекция всего изображения

	//if (pMW.flVerticalProjection)
	//{
	//	int nMax=0;
	//	for (int i=0; i<pMW.aVProjection.Size(); i++)
	//	{
	//		if (pMW.aVProjection[i]>nMax) nMax=TML::Round(pMW.aVProjection[i]);
	//	}

	//	rContext.MoveTo(TWL::Point(
	//		TML::Round(pMW.aVProjection[0]*double(cr.Width())/nMax),
	//		0));

	//	for (int i=1; i<pMW.aVProjection.Size(); i++)
	//	{
	//		rContext.LineTo(
	//			TML::Round(pMW.aVProjection[i]*double(cr.Width())/nMax),
	//			i, TWL::Color::Green());
	//	}
	//}


	/////Проекции на отдельных кандидатов
	//for (int j=0; j<pMW.aHPr.Size(); j++)
	//{
	//	int nMax=0;
	//	TWL::Rect mr;
	//	mr.left=cr.left;
	//	mr.right=cr.right;
	//	mr.top=pMW.aBandBounds[j].C(0);
	//	mr.bottom=pMW.aBandBounds[j].C(1);

	//	for (int i=0; i<pMW.aHPr[j].Size(); i++)
	//	{
	//		if (pMW.aHPr[j][i]>nMax) nMax=pMW.aHPr[j][i];
	//	}

	//	rContext.MoveTo(0, mr.bottom-TML::Round(double(pMW.aHPr[j][0])*
	//		mr.Height()/nMax));
	//	for (int i=1; i<pMW.aHPr[j].Size(); i++)
	//	{
	//		rContext.LineTo(i, 
	//			mr.bottom-TML::Round(double(pMW.aHPr[j][i])*mr.Height()/nMax),
	//			TWL::Color::Red());
	//	}

	//}

	///// снова кажется кандидаты

	//for (int i=0; i<pMW.abmpH.Size(); i++)
	//{
	//	TWL::DeviceContext tDC;
	//	tDC.CreateCompatible();
	//	tDC.Select(pMW.abmpH[i]);

	//	rContext.Copy(tDC,0,0,pMW.aPlates[i].left, pMW.aPlates[i].right,
	//		pMW.aPlates[i].Width(), pMW.aPlates[i].Height());

	//}

	///// рисуем точки, использованные в рансаке
	//for (int i=0; i<pMW.aMPoints.Size();i++)
	//{
	//	for (int j=0; j<pMW.aMPoints[i].Size(); j++)
	//	{
	//		::SetPixel(rContext.Handle(),
	//			TML::Round(pMW.aMPoints[i][j].C(0)),
	//			TML::Round(pMW.aMPoints[i][j].C(1)),
	//			TWL::Color::Orange());

	//	}
	//}

	 ///// просто полоску рисуем - линия поворота
	//for (int i=0; i<pMW.aLines.Size(); i++)
	//{
	//	rContext.Line(pMW.aLines[i], pMW.aPlates[i].r,
	//		TWL::Color::Blue(), 1);
	//}


  ///// выбираем лучшего кандидата
	//int nIndex=-1;
	//double m=10000000;
	//for (int i=0; i<pMW.aPlates.Size(); i++)
	//{
	//	if (pMW.aPlates[i].value<m)
	//	{
	//		nIndex=i;
	//		m=pMW.aPlates[i].value;
	//	}
	//}
	//	
	//if (nIndex>-1)
	//{
	//	TWL::DeviceContext dc;
	//	dc.CreateCompatible();
	//	dc.Select(pMW.bmpCurrentBitmap);

	//	rContext.Copy(dc, pMW.aPlates[nIndex].r.left,pMW.aPlates[nIndex].r.top,
	//		pMW.aPlates[nIndex].r.left, pMW.aPlates[nIndex].r.top,
	//		pMW.aPlates[nIndex].r.Width(), pMW.aPlates[nIndex].r.Height());

	//	
	//	
	//}


///------------------------------------------

	int z=0;

	///// рисование кандидатов с выводом их на экран в упорядоченном виде
	///// (столбцы)

	//TWL::Bitmap cb;
	//GetBitmap(cb);
	//
	//int nHPos=0,nVPos=0;
	//int nMWidth=0;

	//TCL::Array<TIL::GrayImage> &aIm=pMW.aPlateIms;

	//for (int i=0; i<aIm.Size(); i++)
	//{

	//	TWL::Bitmap bmp;

	//	bmp.FromImage(aIm[i]);

	//	TWL::DeviceContext dc;
	//	dc.CreateCompatible();
	//	dc.Select(bmp);

	//	rContext.Copy(dc,0,0,nHPos,nVPos,
	//		bmp.Width(), bmp.Height());

	//	if (nMWidth<bmp.Width()) nMWidth=bmp.Width();

	//	//построим график производной
	//	//вычислим амплитуду
	//	double ytop=pMW.aDervs[i].MaxValue(),
	//		ybottom=pMW.aDervs[i].MinValue();

	//	int w=bmp.Width(), h=bmp.Height();

	//	int zp=h/2; //ноль		
	//	
	//	rContext.Line(nHPos,nVPos+zp,nHPos+w-1,nVPos+zp,TWL::Color::Green());

	//	for (int j=0; j<pMW.aDervs[i].Size(); j++)
	//	{
	//		int yp=nVPos;
	//		int xp=nHPos+j;
	//		double y=pMW.aDervs[i][j];
	//		if (y>=0)
	//		{
	//			yp+=TML::Round(zp-y*zp/ytop);
	//		} else
	//		{
	//			yp+=TML::Round(zp+TML::Abs(y*zp/ybottom));
	//		}
	//		if (j)
	//		{
	//			rContext.LineTo(xp,yp,TWL::Color::Red());
	//		} else 
	//		{
	//			rContext.MoveTo(xp,yp);
	//		}
	//	}

	//	

	//	nVPos+=aIm[i].Height();



	//	if (i<aIm.Size()-1)
	//	{
	//		if (nVPos+aIm[i+1].Height()>cb.Height())
	//		{
	//			nHPos+=nMWidth;
	//			nVPos=0;
	//			nMWidth=0;
	//		}
	//	}
	//}	


	int zz=0;
//	///----------------------------
//
//	//уменьшенная область кандидатов
//
//	TWL::Bitmap cb;
//	GetBitmap(cb);
//	int nHPos=0,nVPos=0;
//	int nMWidth=0;
//
//	TCL::Array<TIL::GrayImage> &aIm=pMW.aImprPlates;
//
////Выбор лучших
//	struct __t
//	{
//		double val;
//		int pos;
//	};
//
//	TCL::Array<__t> aBest(ImageProcessor::BEST);
//
//	int nCount=0;
//
//
//	for (int i=0; i<ImageProcessor::BEST; i++)
//	{
//		int nMin=-1;
//		double m=1000000.999; 
//		for (int j=0; j<pMW.aValues.Size(); j++)
//		{
//			if (pMW.aValues[j].vSummary<m) 
//			{
//				m=pMW.aValues[j].vSummary;
//				nMin=j;
//			}
//		}
//		if (nMin>-1)
//		{
//			aBest[nCount].val=m;
//			aBest[nCount].pos=nMin;
//			pMW.aValues[nMin].vSummary=10000000.0;
//			nCount++;
//		}
//	}
//
//	for (int i=0; i<nCount; i++)
//	{
//		pMW.aValues[aBest[i].pos].vSummary=aBest[i].val;
//	}
////закончили выбор лучших
//
//
//	for (int i=0; i<aIm.Size(); i++)
//	{
//
//		TWL::Bitmap bmp;
//
//		bmp.FromImage(aIm[i]);
//
//		TWL::DeviceContext dc;
//		dc.CreateCompatible();
//		dc.Select(bmp);
//
//		rContext.Copy(dc,0,0,nHPos,nVPos,
//			bmp.Width(), bmp.Height());
//
////обведём лучших рамкой
//		TCL::Array<TWL::Color> aClr(ImageProcessor::BEST);
//		aClr[0]=TWL::Color::Green();
//		aClr[1]=TWL::Color::Yellow();
//		aClr[2]=TWL::Color::Red();
//		int flBest=-1;
//		for (int j=0; j<nCount; j++)
//		{
//			if (aBest[j].pos==i) 
//			{
//				flBest=j;
//				break;
//			}
//		}
//		if (flBest!=-1)
//		{
//			rContext.Frame(nHPos,nVPos,bmp.Width()+nHPos, bmp.Height()+nVPos,
//				2,aClr[flBest]);
//		}
//
////вывод информации оценки
//
//		TWL::String s;
//		
//		
//		s.Append(TWL::FloatToString(float(pMW.aValues[i].vheight),3));
//		s+=TEXT(" + ");
//		s.Append(TWL::FloatToString(float(pMW.aValues[i].vmaxVPpeak),3));
//		s+=TEXT(" + ");
//		s.Append(TWL::FloatToString(float(pMW.aValues[i].varea),3));
//		s+=TEXT(" + ");
//		s.Append(TWL::FloatToString(float(pMW.aValues[i].vproportion),3));
//		s+=TEXT(" = ");
//		s.Append(TWL::FloatToString(float(pMW.aValues[i].vSummary),3));
//
//		rContext.Text(nHPos,nVPos,s.Pointer(), TWL::Color::Red());
//
//
////конец вывода информации оценки		
//
//
//
//
//
//
//		if (nMWidth<bmp.Width()) nMWidth=bmp.Width();		
//
//		nVPos+=aIm[i].Height();
//
//		if (i<aIm.Size()-1)
//		{
//			if (nVPos+aIm[i+1].Height()>cb.Height())
//			{
//				nHPos+=nMWidth;
//				nVPos=0;
//				nMWidth=0;
//			}
//		}
//	}
//
//	int zzz=0;

	///----------------------------

	////необрезанные кандидаты и их вертикальная проекция
	//TWL::Bitmap cb;
	//GetBitmap(cb);
	//
	//int nHPos=0,nVPos=0;
	//int nMWidth=0;

	//TCL::Array<TIL::GrayImage> &aIm=pMW.aDesk;

	//for (int i=0; i<aIm.Size(); i++)
	//{

	//	TWL::Bitmap bmp;

	//	bmp.FromImage(aIm[i]);

	//	TWL::DeviceContext dc;
	//	dc.CreateCompatible();
	//	dc.Select(bmp);

	//	rContext.Copy(dc,0,0,nHPos,nVPos,
	//		bmp.Width(), bmp.Height());

	//	if (nMWidth<bmp.Width()) nMWidth=bmp.Width();

	//	//график вертикальной проекции

	//	int xmax=pMW.aVProjs[i].MaxValue();

	//	

	//	for (int j=0; j<pMW.aVProjs[i].Size(); j++)
	//	{
	//		int x=nHPos+
	//			TML::Round(pMW.aPlates[i].r.Width()*pMW.aVProjs[i][j]/double(xmax)),
	//			y=nVPos+j;
	//		if (j)
	//		{
	//			rContext.LineTo(x,y,TWL::Color::Green());
	//		} else 
	//		{
	//			rContext.MoveTo(x,y);
	//		}
	//	}

	//	

	//	nVPos+=aIm[i].Height();



	//	if (i<aIm.Size()-1)
	//	{
	//		if (nVPos+aIm[i+1].Height()>cb.Height())
	//		{
	//			nHPos+=nMWidth;
	//			nVPos=0;
	//			nMWidth=0;
	//		}
	//	}
	//}	

	/////----------------------------

	////кандидаты на разбиение, чёрно-белые, доп. инфо

	//TWL::Bitmap cb;
	//GetBitmap(cb);
	//int nHPos=0,nVPos=0;
	//int nMWidth=0;

	//TCL::Array<TIL::GrayImage> &aIm=pMW.aDaBest;


	//for (int i=0; i<aIm.Size(); i++)
	//{

	//	TWL::Bitmap bmp;

	//	bmp.FromImage(aIm[i]);

	//	TWL::DeviceContext dc;
	//	dc.CreateCompatible();
	//	dc.Select(bmp);

	//	rContext.Copy(dc,0,0,nHPos,nVPos,
	//		bmp.Width(), bmp.Height());

	//	TWL::String s;

	//	if (nMWidth<bmp.Width()) nMWidth=bmp.Width();	

	//	//rContext.Line(pMW.aP[i].p1,nVPos,pMW.aP[i].p1,nVPos+256,
	//	//	TWL::Color::Red());
	//	//rContext.Line(pMW.aP[i].p2,nVPos,pMW.aP[i].p2,nVPos+256,
	//	//	TWL::Color::Red());


	//	//BuildGraph(rContext,pMW.aBWThres[i],
	//	//	TWL::Rect(nHPos,nVPos,nHPos+256,nVPos+256),
	//	//	TWL::Color::Green());


	//	//nVPos+=aIm[i].Height();
	//	nVPos+=257; //для красивого вывода графика

	//	if (i<aIm.Size()-1)
	//	{
	//		if (nVPos+aIm[i+1].Height()>cb.Height())
	//		{
	//			//nHPos+=nMWidth;
	//			nHPos+=257;
	//			nVPos=0;
	//			nMWidth=0;
	//		}
	//	}
	//}

	int zzzz=0;

	///----------------------------

	//разбиение на цифры

	TWL::Bitmap cb;
	GetBitmap(cb);
	
	int nHPos=0,nVPos=0;
	int nMWidth=0;

	TCL::Array<TIL::GrayImage> &aIm=pMW.aDaBest;

	for (int i=0; i<aIm.Size(); i++)
	{

		//if (i>=pMW.aSyms.Size()) continue;
		if (!pMW.aBad.Size()) continue;
		if (pMW.aBad[i]) continue;
		TWL::Bitmap bmp;

		bmp.FromImage(aIm[i]);

		TWL::DeviceContext dc;
		dc.CreateCompatible();
		HGDIOBJ hOlb=dc.Select(bmp);

		rContext.Copy(dc,0,0,nHPos,nVPos,
			bmp.Width(), bmp.Height());		
		

		//нарисуем гистограмму проекции
		BuildGraph(rContext,pMW.aBestHisto[i],
			TWL::Rect(nHPos,nVPos+bmp.Height(),nHPos+pMW.aBestHisto[i].Size(),nVPos+300),
			TWL::Color::Green());
		//нарисовали

		//нарисуем границы разбиения
		for (int j=0; j<pMW.aDivs[i].Size(); j++)
		{
			rContext.Line(nHPos+pMW.aDivs[i][j],nVPos,
				nHPos+pMW.aDivs[i][j],nVPos+bmp.Height(), TWL::Color::Red());
		}
		//нарисовали


		if (nMWidth<bmp.Width()) nMWidth=bmp.Width();

		//график интенсивности разбиения
		int nP=nMWidth;
		if (pMW.aDivProj.Size())
		{
			for (int j=0; j<pMW.aDivProj[i].Size(); j++)
			{
				BuildGraph(rContext,pMW.aDivProj[i][j],
					TWL::Rect(nP,nVPos,nP+pMW.aDivProj[i][j].Size(),nVPos+300),TWL::Color::Red());
				nP+=pMW.aDivProj[i][j].Size();
				rContext.Line(nP,nVPos,nP,nVPos+300,TWL::Color::Black());
				nP++;
			}
		}
		//нарисовали

		////вывод разделенных символов
		//nP=nMWidth + 3;
		//if (pMW.aParts.Size())
		//{
		//	for (int j=0; j<pMW.aParts[i].Size(); j++)
		//	{

		//		TWL::Bitmap bmp;
		//		bmp.FromImage(pMW.aParts[i][j]);

		//		dc.Select(bmp.Handle());

		//		rContext.Copy(dc,0,0,nP,nVPos,bmp.Width(),bmp.Height());
		//		nP+=bmp.Width()+6;
		//	}
		//}
		////нарисовали

		//int nPX=nMWidth + 5,
		//	  nPY=nVPos;
		////вывод компонентов связности
		//if (pMW.aComps.Size())
		//{
		//	for (int j=0; j<pMW.aComps[i].Size(); j++)
		//	{
		//		nPY=nVPos;
		//		int nMX=0;
		//		for (int k=0; k<pMW.aComps[i][j].Size(); k++)
		//		{
		//			TWL::Bitmap bmp;
		//			bmp.FromImage(pMW.aComps[i][j][k]);

		//			dc.Select(bmp.Handle());

		//			rContext.Copy(dc,0,0,nPX,nPY,bmp.Width(),bmp.Height());
		//			//nP+=bmp.Width()+6;
		//			nPY+=bmp.Height()+3;

		//			if (bmp.Width()>nMX) nMX=bmp.Width();
		//		}
		//		nPX+=nMX+6;

		//	}
		//}
		////закончили вывод компонентов

		//вывод символов
		int nPX=nMWidth + 5,
			nPY=nVPos;

		if (i<pMW.aSyms.Size())
		{
			for (int j=0; j<pMW.aSyms[i].Size(); j++)
			{

				TWL::Bitmap bmp;
				bmp.FromImage(pMW.aSyms[i][j]);

				dc.Select(bmp.Handle());

				rContext.Copy(dc,0,0,nPX,nVPos,bmp.Width(),bmp.Height());
				nPX+=bmp.Width()+6;
			}
		}
		//нарисовали
		

		//nVPos+=aIm[i].Height();
		nVPos=nPY+100;
		if (i<aIm.Size()-1)
		{
			if (nVPos+aIm[i+1].Height()>cb.Height())
			{
				nHPos+=nMWidth;
				nVPos=0;
				nMWidth=0;
			}
		}
	}	
	int zzzzz=0;
	



}



LRESULT AnalyzImage::OnMouseMove( WPARAM wParam, LPARAM lParam )
{
	MainWindow &pMW=RecognitionApp::Instance().cMainWindow;

	int x=GET_X_LPARAM(lParam),
		y=GET_Y_LPARAM(lParam);

	TWL::Bitmap bmp;

	GetBitmap(bmp);

	TWL::DeviceContext dc;
	dc.CreateCompatible();

	dc.Select((HBITMAP)bmp.Handle());

	TWL::Color clr(::GetPixel(dc.Handle(), x, y));

	int r=clr.R(),g=clr.G(),b=clr.B();

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
	
		
			
	return NextHandler(wParam, lParam);
}

void AnalyzImage::BuildGraph(
	TWL::DeviceContext &dc, TCL::Array<int> &a, TWL::Rect &r,
	TWL::Color &c)
{
	int h=r.Height(),
		H=a.MaxValue();
	for (int i=0; i<min(r.Width(),a.Size()); i++)
	{	
		int x=i+r.left,
			y=TML::Round(r.bottom-double(a[i])*h/H);
		if (!i) // мувту для первого элемента
		{
			dc.MoveTo(x,y);
		} else
		{
			dc.LineTo(x,y,c);
		}
	}
}


