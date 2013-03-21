#include "recognition.h"


MainWindow::MainWindow()
{
	SetHandler(WM_CREATE, &MainWindow::OnCreate);
	SetHandler(WM_CLOSE, &MainWindow::OnClose);
	SetHandler(WM_COMMAND, &MainWindow::OnCommand);

	flVerticalProjection=false;
	flHorizontalProjection=false;

	nImagesIndex=-1;

	symsratio=1.4;
	nSyms=5;

	ShowState=0;
}

MainWindow::~MainWindow()
{


}


void MainWindow::LoadBitmap(TWL::Bitmap &bmp)
{
		bmp.ToImage(imCurrentImage);
		bmp.ToImage(imGrayCurrentImage);
		TWL::Bitmap tBmp;
		tBmp.FromImage(imGrayCurrentImage);


		
		bmpGrayCurrent.FromImage(imGrayCurrentImage);
	

		cImage.SetBitmap(bmpGrayCurrent);
		cHelpWindow.SetBitmap(bmp);
}
void MainWindow::OnImageLoad()
{
	TWL::ImageFileDialog ifDialog;

	TWL::String sName;


	if (ifDialog.Open(bmpCurrentBitmap, &sName, this))
	{	
		LoadBitmap(bmpCurrentBitmap);
		cStatusBar.SetPartText(sName.Pointer());
		BuildProjections();
	}



}

void MainWindow::OnVEdge()
{
	ImageProcessor::VerticalEdgeDetection(imGrayCurrentImage,
		imVerticalEdge);

	TWL::Bitmap bmpNew;
	bmpNew.FromImage(imVerticalEdge);

	//cImage.SetBitmap(bmpNew);
		

}

void MainWindow::OnHEdge()
{
	ImageProcessor::HorizontalEdgeDetection(imGrayCurrentImage,
		imHorizontalEdge);

	TWL::Bitmap bmpNew;
	bmpNew.FromImage(imHorizontalEdge);

	cImage.SetBitmap(bmpNew);
}

void MainWindow::OnVProjection()
{

	ImageProcessor::SmoothVerticalProjection(imHorizontalEdge,
		aVProjection);

	flVerticalProjection=true;		

}

void MainWindow::OnHProjection()
{

	ImageProcessor::SmoothHorizontalProjection(imVerticalEdge,
		aHProjection);


	flHorizontalProjection=true;	
}

void MainWindow::LocatePlates()
{
	aPlateBounds.Resize(aBandBounds.Size());
	aHPr.Resize(aBandBounds.Size());
	for (int i=0; i<aBandBounds.Size(); i++)
	{	
		
		ImageProcessor::HBandProjection(imHorizontalEdge,
			aBandBounds[i], aHPr[i]);
		ImageProcessor::LocatePlateGradient(aHPr[i],
			aPlateBounds[i], aBandBounds[i].C(1)-aBandBounds[i].C(0));				
	}	
}
void MainWindow::MakePlates()
{
	int nSize=0;
	for (int i=0; i<aPlateBounds.Size(); i++)
	{
		nSize+=aPlateBounds[i].Size();
	}

	aPlates.Resize(nSize);
	
	int nPos=0;
	for (int i=0; i<aPlateBounds.Size(); i++)
	{
		for (int j=0; j<aPlateBounds[i].Size(); j++)
		{
			aPlates[nPos].r.left=aPlateBounds[i][j].C(0);
			aPlates[nPos].r.right=aPlateBounds[i][j].C(1);
			aPlates[nPos].r.top=aBandBounds[i].C(0);
			aPlates[nPos].r.bottom=aBandBounds[i].C(1);
			aPlates[nPos].nHProjection=i;
			nPos++;
		}
	}

	

}
void MainWindow::Hough()
{
	//aHoughs.Resize(aPlates.Size());
	//abmpH.Resize(aPlates.Size());

	//for (int i=0; i<aPlates.Size(); i++)
	//{
	//	ImageProcessor::HoughLineTransform(imHorizontalEdge,
	//		aHoughs[i],aPlates[i]);

	//	ImageProcessor::HoughResult(aHoughs[i],abmpH[i],
	//		aPlates[i].Width(), aPlates[i].Height());
	//}
	//TGL::SingleRobustEstimator
}
void MainWindow::Deskew()
{
	Deskewed.Copy(imGrayCurrentImage);

	TGL::AffineTransform<double,2,2> afTrans;

	aLines.Resize(aPlates.Size());

	aMPoints.Resize(aPlates.Size());
	aPlateIms.Resize(aPlates.Size());
	aVProjs.Resize(aPlates.Size());
	aDesk.Resize(aPlates.Size());


	for (int i=0; i<aPlates.Size(); i++)
	{	
		aLines[i]=ImageProcessor::RansacDeskew(
			imVerticalEdge,
			afTrans,
			aMPoints[i],
			aPlates[i].r);
		ImageProcessor::MakeImage(imGrayCurrentImage,
			Deskewed,
			afTrans,
			aPlates[i].r,
			aPlateIms[i],
			aDesk[i],
			aVProjs[i]);
	}


}
//void MainWindow::BuildCandidates()
//{
//	aPlateIms.Resize(aPlates.Size());
//	for (int i=0; i<aPlates.Size(); i++)
//	{
//		int nLInd=ADDCANDIDATEWIDTH,
//			nRInd=ADDCANDIDATEWIDTH;
//
//		aPlateIms[i].Create(aPlates[i].r.Width()+nLInd+nRInd,
//			aPlates[i].r.Height());
//
//		
//		
//
//
//		TIL::GrayPixel *p=aPlateIms[i].Pointer(),
//			*s=Deskewed.Pointer();
//
//		//aPlateIms[i].CopyBlock(Deskewed,aPlates[i].r.left,
//		//	aPlates[i].r.top, aPlates[i].r.Width(), aPlates[i].r.Height(),
//		//	TIL::GrayPixel(255));
//		aPlateIms[i].Copy(Deskewed, nLInd, 0, aPlates[i].r.left,
//			aPlates[i].r.top, aPlates[i].r.Width(), aPlates[i].r.Height());
//
//		for (int j=0; j<aPlates[i].r.Height(); j++)
//		{
//			for (int k=0; k<nLInd; k++)
//			{
//				int sx=aPlates[i].r.left,
//					sy=j+aPlates[i].r.top;
//				int dx=k,
//					dy=j;
//				p[dy*aPlateIms[i].Width()+dx]=s[sy*Deskewed.Width()+sx];
//			}
//			for (int k=0; k<nRInd; k++)
//			{
//				int sx=aPlates[i].r.left+aPlates[i].r.Width()-1,
//					sy=j+aPlates[i].r.top;
//				int dx=aPlates[i].r.Width()+nLInd+k,
//					dy=j;
//				p[dy*aPlateIms[i].Width()+dx]=s[sy*Deskewed.Width()+sx];
//			}
//		}
//
//
//	}
//}
void MainWindow::Value()
{
	aValues.Resize(aImprPlates.Size());
	for (int i=0; i<aImprPlates.Size(); i++)
	{
		ImageProcessor::EstimateWeight(aImprPlates[i],
			aValues[i]);
	}



}
void MainWindow::ImprovePlates()
{
	int s=aPlateIms.Size();

	aImprPlates.Resize(s);
	aDervs.Resize(s);

	for (int i=0; i<s; i++)
	{	
		ImageProcessor::ImprovePlatePosition(aPlateIms[i],
			aImprPlates[i], aDervs[i]);
	}

}
void MainWindow::CollectBest()
{
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
		for (int j=0; j<aValues.Size(); j++)
		{
			if (aValues[j].vSummary<m) 
			{
				m=aValues[j].vSummary;
				nMin=j;
			}
		}
		if (nMin>-1)
		{
			aBest[nCount].val=m;
			aBest[nCount].pos=nMin;
			aValues[nMin].vSummary=10000000.0;
			nCount++;
		}
	}

	aDaBest.Resize(nCount);

	for (int i=0; i<nCount; i++)
	{
		aValues[aBest[i].pos].vSummary=aBest[i].val;
		aDaBest[i].Copy(aImprPlates[aBest[i].pos]);
	}
}
void MainWindow::NormBrightnessContrast()
{
	for (int i=0; i<aDaBest.Size(); i++)
	{
		ImageProcessor::Normalization(aDaBest[i]);
	}
}
void MainWindow::CandidatesToBW()
{
	aBWThres.Resize(aDaBest.Size());
	aP.Resize(aDaBest.Size());


	for (int i=0; i<aDaBest.Size(); i++)
	{


		//ImageProcessor::ContrastGrayImage(aDaBest[i],
		//	ImageProcessor::AdaptiveThreshold(aDaBest[i],
		//	aBWThres[i], aP[i].p1, aP[i].p2));

		//ImageProcessor::ContrastGrayImage(aDaBest[i],
		//	ImageProcessor::OtsuBinarizationThreshold(aDaBest[i]));
		
		//TIL::GrayImage imT(aDaBest[i]);
		//ImageProcessor::BernsenBinarization(imT,aDaBest[i]);

		TIL::GrayImage imT(aDaBest[i]);
		ImageProcessor::NiblekBinarization(imT,aDaBest[i]);
	}
}
void MainWindow::Divide()
{
	aDivs.Resize(aDaBest.Size());
	aParts.Resize(aDaBest.Size());
	aBestHisto.Resize(aDaBest.Size());

	for (int i=0; i<aDaBest.Size(); i++)
	{
		TIL::GrayImage imT;
		ImageProcessor::NiblekBinarization(aDaBest[i],imT);
		ImageProcessor::GetDivision(imT,aDivs[i],
			aBestHisto[i]);
		ImageProcessor::ImproveDivision1(imT,aDivs[i]);
		ImageProcessor::ImproveDivision2(imT,aDivs[i]);

		ImageProcessor::DivideIntoBlocks(aDaBest[i],
			aDivs[i], aParts[i]);		
	}	
}

void MainWindow::SymbolsWork()
{
	int s=aParts.Size();

	aPRects.Resize(s);
	
	aComps.Resize(s);
	aSyms.Resize(s);

	for (int i=0; i<s; i++)
	{
		int is=aParts[i].Size();

		aPRects[i].Resize(is);

		aComps[i].Resize(is);
		aSyms[i].Resize(is);
		for (int j=0; j<is; j++)
		{
			TIL::GrayImage imT,	 imTT;
			imT.Copy(aParts[i][j]);
			//ImageProcessor::ContrastGrayImage(aParts[i][j],
			//	ImageProcessor::OtsuBinarizationThreshold(aParts[i][j]));
			TIL::ImageSmoothGaussian3x3(imTT,imT);
			ImageProcessor::NiblekBinarization(imTT,aParts[i][j]);
			//ImageProcessor::AverageThreshold(imT,aParts[i][j]);
			//ImageProcessor::MedianThreshold(imT,aParts[i][j]);

			//выделим компоненты связности
			TCL::Array<TIL::GrayImage> aT;
			TCL::Array<TWL::Rect> aR;
			ImageProcessor::GetComponents(aParts[i][j],aT,aR);

			aComps[i][j].Resize(aT.Size());
			for (int k=0; k<aT.Size(); k++)
			{
				aComps[i][j][k].Copy(aT[k]);
			}

			TWL::Rect r;
			//выберем то, что - символ
			ImageProcessor::ChooseSymbol(aComps[i][j],aSyms[i][j],
				aR,aPRects[i][j]);
			
			imT.Copy(aSyms[i][j]);
			//ImageProcessor::Skeletonize(imT,aSyms[i][j]);
		}

	}

}

void MainWindow::FilterBySymSize()
{
	int s=aSyms.Size();
	int nS=s;

	TCL::Array<TCL::Array<TIL::GrayImage>> aT(s);
	for (int i=0; i<s; i++)
	{
		aT[i].Resize(aSyms[i].Size());
		for (int j=0; j<aSyms[i].Size(); j++)
		{
			aT[i][j].Copy(aSyms[i][j]);
		}
	}

	TCL::Array<bool> &aDel=aBad;
	aDel.Resize(s);
	aDel.Fill(false);

	for (int i=0; i<s; i++)
	{
		int nProper=0;
		for (int j=0; j<aT[i].Size(); j++)
		{
			if (aT[i][j].Height()>aT[i][j].Width()*symsratio)
			{
				nProper++;
			}
		}

		int VERYVERYVERYMAJIC=10;

		if ((nProper<nSyms)||(nProper>VERYVERYVERYMAJIC))
		{
			aDel[i]=true;
			nS--;
		}		
	}

	//aSyms.Resize(nS);
	//nS=0;
	//for (int i=0; i<s; i++)
	//{
	//	if (!aDel[i])
	//	{
	//		aSyms[nS].Resize(aT[i].Size());
	//		for (int j=0; j<aT[i].Size(); j++)
	//		{
	//			aSyms[nS][j].Copy(aT[i][j]);
	//		}
	//		nS++;
	//	}
	//}

	//for (int i=s-1; i>=0; i--)
	//{
	//	if (aDel[i])
	//	{
	//		aSyms.Remove(i);
	//	}
	//}

	


}
void MainWindow::KillBorder()
{	
	for (int i=0; i<aDaBest.Size(); i++)
	{
		TIL::GrayImage imT;
		imT.Copy(aDaBest[i]);

		ImageProcessor::RecessHorizontalBorders(imT,aDaBest[i]);
		imT.Copy(aDaBest[i]);
		//ImageProcessor::RecessVerticalBorders(imT,aDaBest[i]);
	
	}		
}

void MainWindow::CollectDivisionIntensity()
{
	aDivProj.Resize(aDaBest.Size());

	for (int i=0; i<aDaBest.Size(); i++)
	{
		ImageProcessor::GetIntensityOnDivisions(
			aDaBest[i], aDivs[i],aDivProj[i]);
	}


}

void MainWindow::Gaussian()
{
	int s=aDaBest.Size();
	for (int i=0; i<s; i++)
	{
		TIL::GrayImage t(aDaBest[i]);

		TIL::ImageSmoothGaussian5x5(aDaBest[i],t);

	}
}
void MainWindow::BuildProjections()
{
	//if (!imGrayCurrentImage.Pointer()) return;

	//ShowState=0;

	OnVEdge();
	OnHEdge();
	OnVProjection();
	OnHProjection();
	
	ImageProcessor::LocateBands(aVProjection,aBandBounds);
	LocatePlates();

	MakePlates();
	//Hough();
	Deskew();
	//BuildCandidates();
	ImprovePlates();
	Value();
	CollectBest();
	NormBrightnessContrast();
	
	KillBorder();

	//CandidatesToBW();
	//Gaussian();
	Divide();
	//CollectDivisionIntensity();

	SymbolsWork();

	FilterBySymSize();

	TWL::Bitmap bmpDeskewed;
	bmpDeskewed.FromImage(Deskewed);
	
	//cHelpWindow.SetBitmap(bmpDeskewed);
	//cHelpWindow.SetBitmap(bmpGrayCurrent);

	TWL::Bitmap bmpDebug;
	bmpDebug.Create(1000,1000);
	
	cImage.SetBitmap(bmpCurrentBitmap);
	//cImage.SetBitmap(bmpDebug);
	cImage.Redraw();

}


void MainWindow::OnMakeImageList()
{
	TWL::ImageFileDialog fd;
	TCL::Array<TWL::String> aNames;
	if (fd.OpenMultiple(aNames,0,Handle(),MAXIMAGES))
	{
		TWL::XMLDocument xmlDocument;
		TWL::XMLElement xmlElement, xmlElement2;

		TWL::String sPath;
		RecognitionApp::Path(sPath);

		xmlDocument.Create();
		xmlDocument.CreateHeader();


		xmlDocument.AddElement(xmlElement, TEXT("Images"));
		xmlElement.SetInteger(TEXT("Quantity"),aNames.Size());
		xmlElement.SaveArray<TWL::String, SaveString>(aNames, TEXT("Items"));
		xmlElement.AddElement(xmlElement2, TEXT("Last"));
		xmlElement2.SetText(sCurrentBitmap.Pointer());

		sPath+=TEXT("\\images.xml");
		xmlDocument.Save(sPath);

		
	}
}
void MainWindow::LoadImageList()
{
	TWL::XMLDocument xmlDocument;

	TWL::String sPath;
	RecognitionApp::Path(sPath);
	sPath+=TEXT("\\images.xml");

	if (xmlDocument.Load(sPath))
	{
		TWL::XMLElement xmlElement;
		if (xmlDocument.FindElement(xmlElement, TEXT("Images")))
		{
			int nSize=0;
			
			xmlElement.GetInteger(TEXT("Quantity"),nSize);
			if (nSize)
			{
				aImages.Resize(nSize);
				xmlElement.LoadArray<TWL::String,LoadString>(aImages, TEXT("Items"));

				nImagesIndex=TML::Random(nSize);
				bmpCurrentBitmap.LoadExtended(aImages[nImagesIndex].Pointer());
				LoadBitmap(bmpCurrentBitmap);

			}
		}
	}



}
bool MainWindow::SaveString(const TWL::String &sString, TWL::XMLElement &xmlElement )
{
	xmlElement.SetText(sString.Pointer());

	return true;
}
bool MainWindow::LoadString( TWL::String &sString, TWL::XMLElement &xmlElement )
{
	xmlElement.GetText(sString);
	return true;
}
void MainWindow::OnNextImage()
{
	if (aImages.Size())
	{
		if (++nImagesIndex==aImages.Size()) nImagesIndex=0;
		bmpCurrentBitmap.LoadExtended(aImages[nImagesIndex]);
		LoadBitmap(bmpCurrentBitmap);

		if (aImages.Size()&&(nImagesIndex>=0))
		{
			cStatusBar.SetPartText(aImages[nImagesIndex].Pointer());		
		}

		BuildProjections();
	}
}
void MainWindow::OnPrevImage()
{
	if (aImages.Size())
	{
		if (--nImagesIndex<0) nImagesIndex=aImages.Size()-1;
		bmpCurrentBitmap.LoadExtended(aImages[nImagesIndex]);
		LoadBitmap(bmpCurrentBitmap);

		if (aImages.Size()&&(nImagesIndex>=0))
		{
			cStatusBar.SetPartText(aImages[nImagesIndex].Pointer());		
		}

	}
	BuildProjections();

}
LRESULT MainWindow::OnCreate(WPARAM wParam, LPARAM lParam)
{
	cImage.Create(0,WS_VISIBLE|WS_CHILD,Handle());
	//cHelpWindow.Create(0, WS_VISIBLE|WS_CHILD, TEXT(" "), Handle());	
	cHelpWindow.Create(0,WS_VISIBLE|WS_CHILD,Handle());

	ToolBars[0].SetImages(IDB_PREV_NEXT);
	ToolBars[0].InsertButton(0,IDM_PREV_IMAGE);
	ToolBars[0].InsertButton(1,IDM_NEXT_IMAGE);
	ToolBars[0].InsertButton(0,IDM_PREV_STATE);
	ToolBars[0].InsertButton(1,IDM_NEXT_STATE);

	ToolBars[0].SetDefaultState();
	

	cStrip.SetSplitterType(true);
	cStrip.SetWindows(&cImage, &cHelpWindow);
	cStrip.Create(0, WS_VISIBLE|WS_CHILD, Handle());
	cStrip.SetCenter();

	TCL::Array<int> aParts;
	int nParts=7;

	aParts.Resize(nParts);

	aParts[0]=400;
	aParts[1]=440;
	aParts[2]=480;
	aParts[3]=520;
	aParts[4]=560;
	aParts[5]=600;
	aParts[6]=640;
	
	cStatusBar.SetParts(aParts.Pointer(), nParts);

	

	LoadImageList();
	BuildProjections();

	



	return 0;
}

LRESULT MainWindow::OnClose( WPARAM wParam, LPARAM lParam )
{
	::PostQuitMessage(0);
	return 0;

}

LRESULT MainWindow::OnCommand( WPARAM wParam, LPARAM lParam )
{
	switch LOWORD(wParam)
	{
	case IDM_IMAGE_LOAD:
	{
		OnImageLoad();
		break;	
	}
	case IDM_V_EDGE:
	{
		OnVEdge();
		break;
	}
	case IDM_H_EDGE:
	{
		OnHEdge();
		break;
	}
	case IDM_VPROJECTION:
	{
		OnVProjection();
		break;
	}
	case IDM_HPROJECTION:
	{
		OnHProjection();
		break;
	}
	case IDM_MAKE_IMAGELIST:
	{
		OnMakeImageList();

		break;
	}
	case IDM_EXIT:
	{
		::PostQuitMessage(0);
		break;
	}
	case IDM_NEXT_IMAGE:
	{
		OnNextImage();
		break;
	}
	case IDM_PREV_IMAGE:
	{
		OnPrevImage();
		break;
	}
	case IDM_NEXT_STATE:
	{
		ShowState++;
		if (ShowState>8)
		{
			TWL::Bitmap bmpDebug;
			bmpDebug.Create(1000,1000);

			cImage.SetBitmap(bmpDebug);
		} else
		{
			cImage.SetBitmap(bmpCurrentBitmap);
		}
		cImage.Redraw();
		break;
	}
	case IDM_PREV_STATE:
		{
			ShowState--;
			if (ShowState>8)
			{
				TWL::Bitmap bmpDebug;
				bmpDebug.Create(1000,1000);

				cImage.SetBitmap(bmpDebug);
			} else
			{
				cImage.SetBitmap(bmpCurrentBitmap);
			}
			cImage.Redraw();
			break;
		}

	}


	

	return 0;
}

void MainWindow::OnClientSize(TWL::WindowDocker &rDocker,
															int nLeft, int nTop, int nRight, int nBottom)
{
	 rDocker.Dock(cStrip.Handle(), nLeft, nTop, nRight, nBottom);	
}