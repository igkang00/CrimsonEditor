/***********************************************************************************************************/
// XPTabCtrl.cpp : implementation file
/////////////////////////////////////////////////////////////////////////////
//	Compiler:	Visual C++, Service Pack 5, Windows NT 
//	Tested on:	Windows NT, Windows XP
//
//	Created:	25/February/2004
//	Updated:	
//
//	Author:		Adi DEDIC
//  e-mail:		adi_dedic@hotmail.com
//  www:		http://web.onetel.net.uk/~adidedic/
//
//	Disclaimer
//	----------
//	THIS SOFTWARE AND THE ACCOMPANYING FILES ARE DISTRIBUTED "AS IS" AND WITHOUT
//	ANY WARRANTIES WHETHER EXPRESSED OR IMPLIED. NO REPONSIBILITIES FOR POSSIBLE
//	DAMAGES OR EVEN FUNCTIONALITY CAN BE TAKEN. THE USER MUST ASSUME THE ENTIRE
//	RISK OF USING THIS SOFTWARE.
//
//	Terms of use
//	------------
//	THIS SOFTWARE IS FREE FOR PERSONAL USE OR FREEWARE APPLICATIONS.
//	IF YOU USE THIS SOFTWARE IN COMMERCIAL OR SHAREWARE APPLICATIONS YOU
//	ARE GENTLY ASKED TO DONATE £1 (ONE GB PUND) TO THE AUTHOR
/***********************************************************************************************************/
#include "StdAfx.h"
#include "XPTabCtrl.h"

//#define USE_DEFAULT_XP_TOPTAB		// XP top tab is drawn only for test purpose. To use default, uncoment this line

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/***********************************************************************************************************/
// constant string definitions here (or you can put it into resource string table)
#define IDS_UTIL_TAB            _T("TAB")
#define IDS_UTIL_UXTHEME        _T("UxTheme.dll")
#define IDS_UTIL_THEMEACT       _T("IsThemeActive")
#define IDS_UTIL_THEMEOPN       _T("OpenThemeData")
#define IDS_UTIL_THEMEBCKG      _T("DrawThemeBackground")

// close button geometry, in pixels: a square box inset from the tab's right edge
#define CLOSE_BTN_SIZE          14
#define CLOSE_BTN_MARGIN        5
#define CLOSE_BTN_RESERVE       (CLOSE_BTN_SIZE+4)	// taken off the text area's right side
// The control spends its horizontal padding three times -- before the icon, between icon and text,
// and after the text -- so the tab grows by three times the extra padding. Growing it by exactly the
// reserve keeps the text where it was without the button; any more is slack that the centred text
// spreads to both sides, pushing it away from the icon.
#define CLOSE_BTN_PADDING       (CLOSE_BTN_RESERVE/3)
/***********************************************************************************************************/
// CXPTabCtrl
/***********************************************************************************************************/
CXPTabCtrl::CXPTabCtrl()
{
	m_bTabExtended=TRUE;			// default is to use extended tab (if it is Themes XP)
	m_eTabOrientation=e_tabBottom;	// default initial orientation is: bottom
	m_ixSelOld=-1;
	m_bCloseButtons=FALSE;
	m_ixCloseHot=-1;
	m_ixClosePressed=-1;
	m_bTrackingLeave=FALSE;
}
//----------------------------------------------------------------------------------------------------------
// nBitmapID Resource IDs of the bitmap to be associated with the image list.
void CXPTabCtrl::InitImageList(UINT nBitmapID)
{
	if(!::IsWindow(GetSafeHwnd()) || m_ilTabs.operator HIMAGELIST())
		{ ASSERT(FALSE); return; }				// tab control has to be created first and image list can be created only once
	if(m_ilTabs.Create(nBitmapID, 16, 1, RGB(0xFF,0,0xFF)))	// add an images list with appropriate background (transparent) color
		SetImageList(&m_ilTabs);
}
//----------------------------------------------------------------------------------------------------------
// only three messages used
BEGIN_MESSAGE_MAP(CXPTabCtrl, CTabCtrl)
	//{{AFX_MSG_MAP(CXPTabCtrl)
	ON_WM_PAINT()
	ON_NOTIFY_REFLECT(TCN_SELCHANGING,OnTabSelChanging)
	ON_NOTIFY_REFLECT_EX(TCN_SELCHANGE, OnTabSelChanged) //<----- CHANGED
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_WM_CAPTURECHANGED()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
//----------------------------------------------------------------------------------------------------------
void CXPTabCtrl::OnPaint() 
{
	if(!IsExtendedTabTheamedXP())			// if it is not XP or it is not Themes, behave as default
		{ Default(); return; }	
	CPaintDC dc(this);						// device context for painting

	CRect rcClip; rcClip.SetRectEmpty();
	dc.GetClipBox(rcClip);

	// 1st paint the tab body
	CRect rcPage,rcItem,rcClient;
	GetClientRect(&rcPage);
	rcClient=rcPage;
	AdjustRect(FALSE,rcPage);
	
	switch(m_eTabOrientation)
	{	case e_tabTop:	  rcClient.top   =rcPage.top   -2; break;
		case e_tabBottom: rcClient.bottom=rcPage.bottom+3; break;
		case e_tabLeft:	  rcClient.left  =rcPage.left  -1; break;
		case e_tabRight:  rcClient.right =rcPage.right +3; break;
		default: ASSERT(FALSE); return;
	}
	UINT uiVertBottm;
	uiVertBottm =(m_eTabOrientation&1)? 8:0;		//  8=bottom
	uiVertBottm|=(m_eTabOrientation&2)?16:0;		// 16=vertical
	UINT uiFlags=1|uiVertBottm;						//  1=body	
	DrawThemesXpTabItem(&dc, -1, rcClient,uiFlags);	// TABP_PANE=9,0,'TAB'
	
	int nTab=GetItemCount();						// paint the tabs first and then the borders
	if(!nTab) return;								// no tab pages added

	// 2nd paint the inactive tabs
	TCHITTESTINFO hti;	hti.flags=0;
	::GetCursorPos(&hti.pt); ScreenToClient(&hti.pt);
	int ixHot=HitTest(&hti);
	int ixSel=GetCurSel();
		
	for(int ixTab=0; ixTab<nTab; ixTab++)
	{	if(ixTab==ixSel)
			continue;
		VERIFY(GetItemRect(ixTab, &rcItem));
		if(m_eTabOrientation==e_tabLeft) rcItem.right++;
		uiFlags=uiVertBottm|(ixTab==ixHot?4:0);		// 4= hot
		DrawThemesXpTabItem(&dc,ixTab,rcItem,uiFlags);
		DrawCloseButton(&dc,ixTab);
	}
	// 3rd paint the active selected tab
	VERIFY(GetItemRect(ixSel, &rcItem));			// now selected tab
	rcItem.InflateRect(2,2);
	if(m_eTabOrientation==e_tabTop) rcItem.bottom--;
	uiFlags=uiVertBottm|2;							// 2= selected
	DrawThemesXpTabItem(&dc, ixSel, rcItem,uiFlags);
	DrawCloseButton(&dc,ixSel);
}
//----------------------------------------------------------------------------------------------------------
// This function draws Themes Tab control parts: a) Tab-Body and b) Tab-tabs
void CXPTabCtrl::DrawThemesXpTabItem(CDC* pDC, int ixItem, const CRect& rcItem, UINT uiFlag) 
{			// uiFlag(1/0):1=Type(body/tab);2=Sel(y/n);4=Hot(y/n);8=bBottom(y/n);16=rotate(y/n)
	BOOL bBody  =(uiFlag& 1)?TRUE:FALSE;
	BOOL bSel   =(uiFlag& 2)?TRUE:FALSE;
	BOOL bHot   =(uiFlag& 4)?TRUE:FALSE;
	BOOL bBottom=(uiFlag& 8)?TRUE:FALSE;	// mirror
	BOOL bVertic=(uiFlag&16)?TRUE:FALSE;	// rotate
	BOOL bLeftTab=!bBottom && bVertic && !bBody;

	CSize szBmp=rcItem.Size();
	if(bVertic) SwapVars(szBmp.cx,szBmp.cy);
	// 1st draw background
	CDC     dcMem;	dcMem .CreateCompatibleDC(pDC);
	CBitmap bmpMem; bmpMem.CreateCompatibleBitmap(pDC,szBmp.cx,szBmp.cy);
	CBitmap* pBmpOld=dcMem.SelectObject(&bmpMem);
	CRect rcMem(CPoint(0,0),szBmp); if(bSel) rcMem.bottom++;
	if(bBody)
		 DrawThemesPart(dcMem.GetSafeHdc(), 9, 0, (LPCSTR)IDS_UTIL_TAB, &rcMem);	// TABP_PANE=9,  0, 'TAB'
	else DrawThemesPart(dcMem.GetSafeHdc(), 1, bSel?3:(bHot?2:1), (LPCSTR)IDS_UTIL_TAB, &rcMem);
																// TABP_TABITEM=1, TIS_SELECTED=3:TIS_HOT=2:TIS_NORMAL=1, 'TAB'
	// 2nd init some extra parameters
	BITMAPINFO biOut; ZeroMemory(&biOut,sizeof(BITMAPINFO));	// Fill local pixel arrays
	BITMAPINFOHEADER& bihOut=biOut.bmiHeader;
	bihOut.biSize  =sizeof (BITMAPINFOHEADER);
	bihOut.biCompression=BI_RGB;
	bihOut.biPlanes=1;		  bihOut.biBitCount=24;	// force as RGB: 3 bytes,24 bits -> good for rotating bitmap in any resolution
	bihOut.biWidth =szBmp.cx; bihOut.biHeight=szBmp.cy;

	int nBmpWdtPS=DWordAlign(szBmp.cx*3);
	int nSzBuffPS=((nBmpWdtPS*szBmp.cy)/8+2)*8;
	LPBYTE pcImg=NULL;
	if(bBottom || bVertic) { pcImg=new BYTE[nSzBuffPS]; ASSERT(pcImg); }
	int nStart=0,nLenSub=0;
	if(bBody && bBottom && !bVertic) nStart=3,nLenSub=4;	// if bottom oriented flip the body contest only (no shadows were flipped)
	// 3rd if it is left oriented tab, draw tab context before mirroring or rotating (before GetDIBits)
	if(bVertic)
	{	if(bBody || !bBottom) bihOut.biHeight=-szBmp.cy;
		if(!bBottom && !bBody && ixItem>=0)					// 
		{	if(bSel) rcMem.bottom--;
			DrawTabItem(&dcMem, ixItem, rcMem, uiFlag); ixItem=-1;
	}	}														// rotate or mirror
	// 4th get bits (for rotate) and mirror: body=(all except top) tab=(all except top)
	if(bVertic || bBottom)										// get bits: 
	{	GetDIBits(*pDC, bmpMem.operator HBITMAP(),nStart,szBmp.cy-nLenSub,pcImg,&biOut,DIB_RGB_COLORS);
		if(bBottom)									// mirror: body=(bottom and right) tab=(bottom and right)
		{	bihOut.biHeight=-szBmp.cy; 				// to mirror bitmap is eough to use negative height between Get/SetDIBits
			SetDIBits(*pDC, bmpMem.operator HBITMAP(),nStart,szBmp.cy-nLenSub,pcImg,&biOut,DIB_RGB_COLORS);
			if(bBody && bVertic)					// when it is right oriented body -> flip twice, first flip border shadows, than flip shaded inside body again
			{	nStart=2; nLenSub=4; bihOut.biHeight=szBmp.cy;
				GetDIBits(*pDC, bmpMem.operator HBITMAP(),nStart,szBmp.cy-nLenSub,pcImg,&biOut,DIB_RGB_COLORS);
				bihOut.biHeight=-szBmp.cy;			// to mirror bitmap is eough to use negative height between Get/SetDIBits
				SetDIBits(*pDC, bmpMem.operator HBITMAP(),nStart,szBmp.cy-nLenSub,pcImg,&biOut,DIB_RGB_COLORS);
	}	}	}
	// 5th if it is bottom or right oriented tab, draw after mirroring background (do GetDIBits again)
	if(!bBody && ixItem>=0)							// 
	{	if(bSel) rcMem.bottom--;
		DrawTabItem(&dcMem, ixItem, rcMem, uiFlag);
		if(bVertic)											// if it is right tab, do GetDIBits again
		{	bihOut.biHeight=-szBmp.cy;
			GetDIBits(*pDC, bmpMem.operator HBITMAP(),nStart,szBmp.cy-nLenSub,pcImg,&biOut,DIB_RGB_COLORS);
	}	}
	// 6th: do rotate now, finaly
	if(bVertic)							// force rotating bitmap as RGB -> good for any resolution
	{	SwapVars(szBmp.cx,szBmp.cy);

		int nBmpWdtPD=DWordAlign(szBmp.cx*3);
		int nPadD=nBmpWdtPD-szBmp.cx*3;
		int nSzBuffPD=((nBmpWdtPD*szBmp.cy)/8+2)*8;
		LPBYTE pcImgRotate=new BYTE[nSzBuffPD]; ASSERT(pcImgRotate);
		int nWidth,nHeight=szBmp.cy,nHeight1=nHeight-1;
		//====================================
		//------------------------------------
								// here is the example how to speed up lengthy repeatetive processing by using inline assembler
//	#define __USE_MASM__		// the same processing is in C and in asm. To use C -> comment the beginning of this line
//  Commented by Ingyu Kang

								// Do the actual whole RGB bitmap rotating in C or assembler
	#ifndef __USE_MASM__
		LPBYTE pcImgS=pcImg;
		LPBYTE pcImgD=pcImgRotate;
		int ixHeight=0;
		BOOL bLast=FALSE;
		while(ixHeight<nHeight)					// for all destination height lines
		{	nWidth=szBmp.cx;
			if(ixHeight==nHeight1) { bLast=TRUE; nWidth--; }
			
			while(nWidth--)
			{	*(PDWORD)pcImgD=*(PDWORD)pcImgS; // do all Rgb triplets read/write qicker as DWORD
				pcImgS+=nBmpWdtPS;	// increment source in padded source lines
				pcImgD+=3;			// increment destination in rgb triplets
			}
			if(bLast)				// when the last line, the last pixel - colud be a problem if bitmap DWORD alligned 
				for(int c=3;c;c--) *pcImgD++=*pcImgS++;		// (only last three bytes available->could not read/write DWORD)!!
			else
			{	ixHeight++;
				pcImgD+=nPadD;				// destination bitmap horizontal padding to DWORD
				pcImgS=pcImg+(ixHeight*3);	// reset the source to the begining of the next vertical line
		}	}
	#else	// __USE_MASM__
			nBmpWdtPS-=4;					// adjust esi increment (due to esi self-incrementing by movsd)
			nWidth=szBmp.cx;
		__asm
		{		mov		esi, pcImg			// source index
				mov		edi, pcImgRotate	// destination index
				xor		ebx, ebx			// vertical counter
			loop_height:
				mov		ecx, nWidth			// horizontal counter
				cmp		ebx, nHeight1		// check is it the last line
				jne		loop_width
				dec		ecx					// if it is decremnt for the last pixel

			loop_width:
				movsd						// copies 4 bytes and increments source and destination by 4 (we need only 3 bytes copied 'one pixel' RGB triplet)
				dec		edi					// adjust edi to 'as incremented by 3'
				add		esi,nBmpWdtPS		// adjust esi to the next source line
				loop	loop_width			// loop one hotizontal destination line 

				cmp		ebx, nHeight1		// check is it the last line
				je		do_last				// if not last, do incrementing here
											
				inc		ebx					// increment vertical counter
				add		edi, nPadD			// adjust destination index by possible padding to DWORD
				mov		esi, ebx			// reset the source index: add vertical counter * 3 
				shl		esi, 1				// (is the same as * 2 +1*)
				add		esi, ebx			// +1*
				add		esi, pcImg			// add to the beginning of the source
				jmp		loop_height			// loop whole height

			do_last:						// the last pixel is done by
				movsw						// moving first two bytes
				movsb						// and than by moving the very last byte
		}
	#endif // 	__USE_MASM__
		dcMem.SelectObject(pBmpOld); bmpMem.DeleteObject();		// recreate rotated bitmap
		bmpMem.CreateCompatibleBitmap(pDC,szBmp.cx,szBmp.cy);
		dcMem.SelectObject(&bmpMem);
		bihOut.biWidth =szBmp.cx; bihOut.biHeight=bBody?-szBmp.cy:szBmp.cy;
		SetDIBits(*pDC, bmpMem.operator HBITMAP(),0,szBmp.cy,pcImgRotate,&biOut,DIB_RGB_COLORS); // set rotated bitmap bits
		delete pcImgRotate;
	}
	if(pcImg) delete pcImg;
	// 6th blit mirrored/rotated image to the screen
	pDC->BitBlt(rcItem.left,rcItem.top,szBmp.cx,szBmp.cy,&dcMem,0,0,SRCCOPY); // 
	dcMem.SelectObject(pBmpOld);
}
//----------------------------------------------------------------------------------------------------------
// draw tab item context: possible icon and text
void CXPTabCtrl::DrawTabItem(CDC* pDC, int ixItem, const CRect& rcItemC, UINT uiFlags)
{
	TCITEM tci;   CString sText;
	tci.mask      =TCIF_TEXT | TCIF_IMAGE;
	tci.pszText   =sText.GetBuffer(128);
	tci.cchTextMax=127;
	GetItem(ixItem,&tci);
	sText.ReleaseBuffer();

	BOOL bSel   =(uiFlags&2)?TRUE:FALSE;
	BOOL bBottom=(uiFlags&8)?TRUE:FALSE;
	CRect rcItem=rcItemC;
	if(bSel) rcItem.bottom -= 1;
	else	 rcItem.bottom += 2;
	rcItem.left+=(bBottom?3:6);					// text & icon
	rcItem.top +=(bBottom?3:2) + (bSel ? 1 : (bBottom?0:3));

	int nOldMode=pDC->SetBkMode(TRANSPARENT);
	HIMAGELIST hilTabs = (HIMAGELIST)TabCtrl_GetImageList(GetSafeHwnd());	// icon
	if(hilTabs && tci.iImage>=0)
	{	ImageList_Draw(hilTabs, tci.iImage, *pDC, rcItem.left+(bSel?2:0), rcItem.top+(bSel?0:-2), ILD_TRANSPARENT);
		rcItem.left+=19;
	}
	else rcItem.OffsetRect(-2,0);

	if(sText.GetLength())
	{
		// Code added by Ingyu Kang, 2005.01.04
		sText.Replace(_T("&&"), _T("\x1B"));
		sText.Replace(_T("&"), _T(""));
		sText.Replace(_T("\x1B"), _T("&"));
		// Until this line

		CFont* pOldFont=pDC->SelectObject(GetFont());		// prepare dc
		rcItem.right-=3;						// text
		if(m_bCloseButtons && m_eTabOrientation==e_tabTop)
			rcItem.right-=CLOSE_BTN_RESERVE;	// keep the text clear of the close button
		CRect rect(0,0,rcItem.Width(),20);
		::DrawText(pDC->GetSafeHdc(),sText.GetBuffer(sText.GetLength()+4),-1,rect,DT_CALCRECT|DT_SINGLELINE|DT_MODIFYSTRING|DT_END_ELLIPSIS);
		sText.ReleaseBuffer();

		rcItem.OffsetRect((bBottom?-1:0),(bSel?1:-1));
		pDC->DrawText(sText, rcItem, DT_NOPREFIX|DT_CENTER);
		pDC->SelectObject(pOldFont);
	}
	pDC->SetBkMode(nOldMode);
}
//----------------------------------------------------------------------------------------------------------
BOOL CXPTabCtrl::IsExtendedTabTheamedXP()
{
	if(!m_bTabExtended || !::IsWindow(GetSafeHwnd()))
		return FALSE;
	DWORD dwStyle=GetStyle();
	m_eTabOrientation=(dwStyle&TCS_BOTTOM)?e_tabBottom:e_tabTop;
	if(dwStyle&TCS_VERTICAL)
		m_eTabOrientation=(m_eTabOrientation==e_tabTop)?e_tabLeft:e_tabRight;
#ifdef USE_DEFAULT_XP_TOPTAB
	if(m_eTabOrientation==e_tabTop)	return FALSE;
#endif
	return IsThemeActiveXP()?TRUE:FALSE;
}
//==========================================================================================================
// these two messages are necessary only to properly redraw deselected tab background, because 
void CXPTabCtrl::OnTabSelChanging(NMHDR* pNMHDR, LRESULT* pResult)	// selected rect was inflated by 2 points
{
    UNUSED_ALWAYS(pNMHDR);
	m_ixSelOld=GetCurSel();
	*pResult=0L;
}
//----------------------------------------------------------------------------------------------------------
BOOL CXPTabCtrl::OnTabSelChanged(NMHDR* pNMHDR, LRESULT* pResult) //<--- returntype
{
	UNUSED_ALWAYS(pNMHDR);
	if(m_ixSelOld>=0 && m_ixSelOld!=GetCurSel() && IsExtendedTabTheamedXP()) // else 
	{	CWnd* pWndParent=GetParent();
		if(pWndParent)
		{	CRect rcOldSel; GetItemRect(m_ixSelOld, rcOldSel); rcOldSel.InflateRect(2,2);
			ClientToScreen(&rcOldSel); pWndParent->ScreenToClient(&rcOldSel);
			pWndParent->InvalidateRect(rcOldSel);
		}
	}
	*pResult=1L;
	return FALSE; //<-- NEW
}

/* Modified by Ingyu Kang
void CXPTabCtrl::OnTabSelChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
    UNUSED_ALWAYS(pNMHDR);
	if(m_ixSelOld>=0 && m_ixSelOld!=GetCurSel()  && IsExtendedTabTheamedXP())			// else  
	{	CWnd* pWndParent=GetParent();
		if(pWndParent)
		{	CRect rcOldSel; GetItemRect(m_ixSelOld, rcOldSel); rcOldSel.InflateRect(2,2);
			ClientToScreen(&rcOldSel); pWndParent->ScreenToClient(&rcOldSel);
			pWndParent->InvalidateRect(rcOldSel);
	}	}
	*pResult=1L;
} */
/***********************************************************************************************************/
// Close buttons -- added by Ingyu Kang
//
// Drawn only on the themed path above (OnPaint falls back to Default() otherwise), so hit-testing
// is gated the same way: an unthemed tab gets no button, and no invisible one to click either.
// A click on the button is a real button press: it captures the mouse, closes only if released on
// the same button, and never reaches the tab control -- so a background tab closes without being
// activated first.
/***********************************************************************************************************/
void CXPTabCtrl::EnableCloseButtons()
{
	m_bCloseButtons=TRUE;
	SetPadding(CSize(6+CLOSE_BTN_PADDING, 3));	// 6,3 is the control's default padding
}
//----------------------------------------------------------------------------------------------------------
// The box sits where DrawTabItem puts the icon vertically: a selected tab is drawn 2px higher.
BOOL CXPTabCtrl::GetCloseButtonRect(int ixItem, CRect& rcButton)
{
	if(!m_bCloseButtons || m_eTabOrientation!=e_tabTop) return FALSE;
	if(ixItem<0 || ixItem>=GetItemCount()) return FALSE;

	CRect rcItem; if(!GetItemRect(ixItem, &rcItem)) return FALSE;
	int nTop=rcItem.top+(ixItem==GetCurSel()?2:4);
	rcButton.SetRect(rcItem.right-CLOSE_BTN_MARGIN-CLOSE_BTN_SIZE, nTop, rcItem.right-CLOSE_BTN_MARGIN, nTop+CLOSE_BTN_SIZE);
	return TRUE;
}
//----------------------------------------------------------------------------------------------------------
int CXPTabCtrl::HitTestCloseButton(CPoint point)
{
	if(!m_bCloseButtons || !IsExtendedTabTheamedXP()) return -1;

	int nTab=GetItemCount();
	for(int ixTab=0; ixTab<nTab; ixTab++)
	{	CRect rcButton;
		if(GetCloseButtonRect(ixTab, rcButton) && rcButton.PtInRect(point)) return ixTab;
	}
	return -1;
}
//----------------------------------------------------------------------------------------------------------
void CXPTabCtrl::DrawCloseButton(CDC* pDC, int ixItem)
{
	CRect rcButton; if(!GetCloseButtonRect(ixItem, rcButton)) return;

	BOOL bHot    =(ixItem==m_ixCloseHot);
	BOOL bPressed=(ixItem==m_ixClosePressed && bHot);
	if(bHot)
	{	COLORREF crFill=bPressed?RGB(0xB4,0xB4,0xB4):RGB(0xD8,0xD8,0xD8);
		CBrush brFill(crFill); CPen pnFill(PS_SOLID, 1, crFill);
		CBrush* pOldBrush=pDC->SelectObject(&brFill);
		CPen*   pOldPen  =pDC->SelectObject(&pnFill);
		pDC->RoundRect(rcButton, CPoint(4,4));
		pDC->SelectObject(pOldPen); pDC->SelectObject(pOldBrush);
	}

	// a 9x8 cross: each diagonal is two 1px lines side by side, which reads as one bold stroke
	BOOL bStrong=bHot || ixItem==GetCurSel();
	CPen pnGlyph(PS_SOLID, 1, ::GetSysColor(bStrong?COLOR_BTNTEXT:COLOR_GRAYTEXT));
	CPen* pOldPen=pDC->SelectObject(&pnGlyph);
	int x0=rcButton.left+(CLOSE_BTN_SIZE-9)/2, y0=rcButton.top+(CLOSE_BTN_SIZE-8)/2;
	for(int dx=0; dx<2; dx++)
	{	pDC->MoveTo(x0+dx, y0  ); pDC->LineTo(x0+dx+8, y0+8);	// LineTo leaves out the end pixel
		pDC->MoveTo(x0+dx, y0+7); pDC->LineTo(x0+dx+8, y0-1);
	}
	pDC->SelectObject(pOldPen);
}
//----------------------------------------------------------------------------------------------------------
void CXPTabCtrl::InvalidateCloseButton(int ixItem)
{
	CRect rcButton;
	if(GetCloseButtonRect(ixItem, rcButton)) InvalidateRect(rcButton, FALSE);
}
//----------------------------------------------------------------------------------------------------------
void CXPTabCtrl::SetCloseHot(int ixItem)
{
	if(ixItem==m_ixCloseHot) return;
	InvalidateCloseButton(m_ixCloseHot);
	m_ixCloseHot=ixItem;
	InvalidateCloseButton(m_ixCloseHot);
}
//----------------------------------------------------------------------------------------------------------
void CXPTabCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	int ixItem=HitTestCloseButton(point);
	if(ixItem<0) { CTabCtrl::OnLButtonDown(nFlags, point); return; }

	m_ixClosePressed=ixItem;
	SetCapture();
	SetCloseHot(ixItem); InvalidateCloseButton(ixItem);
}
//----------------------------------------------------------------------------------------------------------
// Clicking close buttons in quick succession turns every second press into a double click.
void CXPTabCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if(HitTestCloseButton(point)>=0) { OnLButtonDown(nFlags, point); return; }
	CTabCtrl::OnLButtonDblClk(nFlags, point);
}
//----------------------------------------------------------------------------------------------------------
void CXPTabCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(m_ixClosePressed<0) { CTabCtrl::OnLButtonUp(nFlags, point); return; }

	int ixItem=m_ixClosePressed;
	BOOL bClose=(HitTestCloseButton(point)==ixItem);
	m_ixClosePressed=-1;	// before ReleaseCapture, so OnCaptureChanged has nothing to undo
	ReleaseCapture();
	InvalidateCloseButton(ixItem);

	if(bClose && GetParent()) GetParent()->SendMessage(WM_CLOSE_TAB_ITEM, (WPARAM)ixItem, 0L);
}
//----------------------------------------------------------------------------------------------------------
void CXPTabCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	int ixItem=HitTestCloseButton(point);
	if(m_ixClosePressed>=0)		// while pressed, only the pressed button lights up
		SetCloseHot(ixItem==m_ixClosePressed?ixItem:-1);
	else SetCloseHot(ixItem);

	if(!m_bTrackingLeave)
	{	TRACKMOUSEEVENT tme={ sizeof(TRACKMOUSEEVENT), TME_LEAVE, m_hWnd, 0 };
		if(::TrackMouseEvent(&tme)) m_bTrackingLeave=TRUE;
	}
	CTabCtrl::OnMouseMove(nFlags, point);
}
//----------------------------------------------------------------------------------------------------------
LRESULT CXPTabCtrl::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	m_bTrackingLeave=FALSE;
	if(m_ixClosePressed<0) SetCloseHot(-1);
	return Default();	// the control tracks its own hot tab with the same message
}
//----------------------------------------------------------------------------------------------------------
void CXPTabCtrl::OnCaptureChanged(CWnd* pWnd)
{
	if(m_ixClosePressed>=0)		// capture taken away mid-press: cancel, do not close
	{	InvalidateCloseButton(m_ixClosePressed);
		m_ixClosePressed=-1;
	}
	CTabCtrl::OnCaptureChanged(pWnd);
}
/***********************************************************************************************************/
// Helper functions
/***********************************************************************************************************/
int DWordAlign(int n)
	{ const int rem=n%4; if(rem) n+=(4-rem); return n; }
//----------------------------------------------------------------------------------------------------------
BOOL IsThemeActiveEx()
{								// check theme activity always (could change during application running)
	HINSTANCE hDll=LoadLibrary(CString((LPCTSTR)IDS_UTIL_UXTHEME));							// 'UxTheme.dll'
	if(hDll==NULL) return FALSE;				// the DLL won't be available on anything except Windows XP
	UINT (PASCAL *pfnIsThemeActive)();
	CStringA sIsThemeActive(CString((LPCTSTR)IDS_UTIL_THEMEACT));
	(FARPROC&)pfnIsThemeActive=GetProcAddress(hDll,(LPCSTR)sIsThemeActive);	// 'IsThemeActive'
	UINT uiThemeActive=0;
	if(pfnIsThemeActive)
		uiThemeActive=pfnIsThemeActive();			
	FreeLibrary(hDll);
	return uiThemeActive?TRUE:FALSE;
}
//----------------------------------------------------------------------------------------------------------
#define PACKVERSION(major,minor) MAKELONG(minor,major)
DWORD GetWinVersion()
{
	static DWORD c_dwWinVers=0;	// check win version only once (will not change during application)
	if(!c_dwWinVers)
	{	OSVERSIONINFO osvi;	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));	// Initialize the OSVERSIONINFO structure.
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
#pragma warning(push)
#pragma warning(disable:4996) // GetVersionEx is deprecated but still works
		GetVersionEx(&osvi);
#pragma warning(pop)
		c_dwWinVers=PACKVERSION(osvi.dwMajorVersion,osvi.dwMinorVersion);
	}
	return c_dwWinVers;
}
//----------------------------------------------------------------------------------------------------------
BOOL IsWinXP()         { return GetWinVersion()>=PACKVERSION(5,1)?TRUE:FALSE; }
//----------------------------------------------------------------------------------------------------------
BOOL IsThemeActiveXP() { return (IsWinXP()  && IsThemeActiveEx())?TRUE:FALSE; }
//----------------------------------------------------------------------------------------------------------
#define WPART_NAME_SZ 128
HRESULT DrawThemesPart(HDC hDC, int iPartId, int iStateId, LPCSTR uiPartNameID, LPRECT prcBox)
{
	if(!IsWinXP()) return E_FAIL;
	HINSTANCE hDll=LoadLibrary(CString((LPCTSTR)IDS_UTIL_UXTHEME));								// 'UxTheme.dll'
	if(!hDll) return E_FAIL;
	BOOL (PASCAL* pfnIsThemeActive)();	UINT hTheme=0;
	CStringA sIsThemeActive(CString((LPCTSTR)IDS_UTIL_THEMEACT));
	(FARPROC&)pfnIsThemeActive=GetProcAddress(hDll,(LPCSTR)sIsThemeActive);		// 'IsThemeActive'
	HRESULT hResult=E_FAIL;
	if(pfnIsThemeActive && pfnIsThemeActive())
	{	CString sPartName((LPCTSTR)uiPartNameID);
		if(sPartName.GetLength()>0)
		{	WCHAR swPartName[WPART_NAME_SZ];
			lstrcpynW(swPartName, sPartName, sizeof(swPartName)/sizeof(WCHAR));
			UINT (PASCAL* pfnOpenThemeData)(HWND hwnd, LPCWSTR pszClassList);
			CStringA sOpenThemeData(CString((LPCTSTR)IDS_UTIL_THEMEOPN));
			(FARPROC&)pfnOpenThemeData=GetProcAddress(hDll,(LPCSTR)sOpenThemeData);// 'OpenThemeData'
			if(pfnOpenThemeData && (hTheme=pfnOpenThemeData(NULL, swPartName))!=0)
			{	UINT (PASCAL* pfnDrawThemeBackground)(UINT htheme,HDC hdc,int iPartID,int iStateID,const RECT* prcBx,const RECT* prcClip);
				CStringA sDrawThemeBackground(CString((LPCTSTR)IDS_UTIL_THEMEBCKG));
				(FARPROC&)pfnDrawThemeBackground=GetProcAddress(hDll,(LPCSTR)sDrawThemeBackground);	// 'DrawThemeBackground'
				if(pfnDrawThemeBackground)
					hResult=pfnDrawThemeBackground(hTheme, hDC, iPartId, iStateId, prcBox, NULL);
	}	}	}
	FreeLibrary(hDll);
	return hResult;
}
//----------------------------------------------------------------------------------------------------------
