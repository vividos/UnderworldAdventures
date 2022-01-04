#ifndef __ATLOPENGL_H__
#define __ATLOPENGL_H__

#pragma once

#ifndef __cplusplus
	#error ATL requires C++ compilation (use a .cpp suffix)
#endif

#ifndef __ATLAPP_H__
	#error atlopengl.h requires atlapp.h to be included first
#endif

#ifndef __ATLWIN_H__
	#error atlopengl.h requires atlwin.h to be included first
#endif

#ifndef __ATLGDI_H__
	#error atlopengl.h requires atlgdi.h to be included first
#endif

#if (WINVER < 0x0400)
	#error WTL requires Windows version 4.0 or higher
#endif


#ifdef _ATL_NO_OPENGL
	#error atlopengl.h requires OpenGL support
#endif

// required libraries
#ifndef _ATL_NO_OPENGL_UTIL
	#include <gl/glu.h>
	#pragma comment (lib, "glu32.lib")     /* link with Microsoft OpenGL Utility lib */
#endif

namespace WTL
{

/////////////////////////////////////////////////////////////////////////////
// CGLMessageLoop - used for OpenGL animation

class CGLMessageLoop : public CMessageLoop {
	
	BOOL OnIdle(int nIdleCount) {
		return !CMessageLoop::OnIdle(nIdleCount);
	}
	
};

/////////////////////////////////////////////////////////////////////////////
// COpenGL

template <class T>
class ATL_NO_VTABLE COpenGL
{
public:

	HGLRC m_hRC;			// Handle to RC

// Message map and handlers
	typedef COpenGL<T>	thisClass;
		
	BEGIN_MSG_MAP(thisClass)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
	END_MSG_MAP()
	
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		T* pT = static_cast<T*>(this);
		CClientDC dc(pT->m_hWnd);
		
		// Fill in the pixel format descriptor.
		PIXELFORMATDESCRIPTOR pfd;
		memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
		pfd.nSize      = sizeof(PIXELFORMATDESCRIPTOR); 
		pfd.nVersion   = 1; 
		pfd.dwFlags    = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
		pfd.iPixelType = PFD_TYPE_RGBA; 
		pfd.cColorBits = 24;
		pfd.cDepthBits = 32; 
		pfd.iLayerType = PFD_MAIN_PLANE ;
		
		int nPixelFormat = dc.ChoosePixelFormat(&pfd);
	    ATLASSERT(nPixelFormat != 0);
		
		BOOL bResult = dc.SetPixelFormat (nPixelFormat, &pfd);
		ATLASSERT(bResult);
		
		m_hRC = dc.wglCreateContext();
		ATLASSERT(m_hRC);

		dc.wglMakeCurrent(m_hRC);
		pT->OnInit();
		dc.wglMakeCurrent(NULL);

		bHandled = FALSE;
		return 0;
	}
	
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (m_hRC) {
			::wglDeleteContext(m_hRC);
			m_hRC = NULL;
		}
		
		bHandled = FALSE;
		return 0;
	}
	
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		T* pT = static_cast<T*>(this);
		CPaintDC dc(pT->m_hWnd);
		dc.wglMakeCurrent(m_hRC);
		pT->OnRender();
		dc.SwapBuffers();
		dc.wglMakeCurrent(NULL);
		
		return 0;
	}
	
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		T* pT = static_cast<T*>(this);
		int cx = LOWORD(lParam);
		int cy = HIWORD(lParam);
		
		if	(cx==0 || cy==0 || pT->m_hWnd==NULL)
			return 0;

		CClientDC dc(pT->m_hWnd);
		dc.wglMakeCurrent(m_hRC);
		pT->OnResize(cx, cy);
		dc.wglMakeCurrent(NULL);

		bHandled = FALSE;
		return 0;
	}

	LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return 1;
	}
	
};

}; //namespace WTL

#endif // __ATLOPENGL_H__