//VolumeApp.cpp
#define UNICODE
#define _UNICODE

#define KEYBOARD_SLICING_CONTROLS
#undef KEYBOARD_SLICING_CONTROLS

//- Commmon Header Files -
#include<Windows.h>
#include<Windowsx.h>
#include<stdio.h>
#include<stdlib.h>
#include <fstream>
#include <iostream>
#include<math.h>
#include<stdarg.h>
#include <commdlg.h>

#include "OGL.h"


#include "./src/utils/common.h"


#include "./src/effects/Basic_TextureSlicing/BasicTextureSlicing.h"
#include "./src/effects/RayCasting/RayCasting.h"
#include "./src/effects/MarchingTetrahedra/MarchingTetrahedra.h"
#include "./src/effects/GridBoxes/GridBoxes.h"





#define STB_IMAGE_IMPLEMENTATION
#include "./src/include/stb_image.h"

// MACROS and 
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#define ID_RESET_BUTTON 1001

#define ID_LEFT_ARROW_EFFECT  1002
#define ID_RIGHT_ARROW_EFFECT 1003
#define ID_LABEL_EFFECT   1004

#define ID_FRONT_FACE_MINUS 1005
#define ID_FRONT_FACE_PLUS 1006
#define ID_LABEL_FRONT_FACE 1007

#define ID_RIGHT_FACE_MINUS 1008
#define ID_RIGHT_FACE_PLUS 1009
#define ID_LABEL_RIGHT_FACE 1010

#define ID_TOP_FACE_MINUS 1011
#define ID_TOP_FACE_PLUS 1012
#define ID_LABEL_TOP_FACE 1013


#define ID_BACK_FACE_MINUS 1014
#define ID_BACK_FACE_PLUS 1015
#define ID_LABEL_BACK_FACE 1016

#define ID_LEFT_FACE_MINUS 1017
#define ID_LEFT_FACE_PLUS 1018
#define ID_LABEL_LEFT_FACE 1019

#define ID_BOTTOM_FACE_MINUS 1020
#define ID_BOTTOM_FACE_PLUS 1021
#define ID_LABEL_BOTTOM_FACE 1022







#pragma comment(lib, "Comdlg32")

// global variable declarations:
HWND ghwnd = NULL;
DWORD dwStyle = 0;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
BOOL gbFullscreen = FALSE;
MONITORINFO mi;

BOOL gbActiveWindow = FALSE;
FILE* gpFile = NULL;

// - OpenGL Related Variables -
HDC ghdc = NULL;
HGLRC ghrc = NULL;



GLuint giWinWidth=0;
GLuint giWinHeight = 0;

enum
{
	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_COLOR,
	AMC_ATTRIBUTE_NORMAL,
	AMC_ATTRIBUTE_TEXCOORD,
};



// global function declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void ToggleFullscreen(void);




//! VOLUME RENDERING GLOBAL VARIABLES:

int iEffectUsed= -1;

const wchar_t* effectNames[] =
{
	L"Basic",
	L"RayCasting Method",
	L"Pseudo IsoSurface",
	L"Colormap",
	L"MarchingTetrahedra",
};

const wchar_t* textHolder = L"";



//* Object 1 Data Info

const std::string volume_file = "./resources/model/Engine256.raw";









BOOL bSliceUpdate = TRUE;

BOOL bMouseClicked = FALSE;
BOOL bMouseControl = FALSE;

GLfloat oldX = 0.0f;
GLfloat oldY = 0.0f;
GLfloat xMouseValue = 0.0f;
GLfloat yMouseValue = 0.0f;

GLfloat xMouseValue_NDC = 0.0f;
GLfloat yMouseValue_NDC = 0.0f;







// Handle to UI Buttons and  Labels
HWND hwndValueLabel = NULL;

HWND hLabel_FrontFace = NULL;
HWND hLabel_RightFace = NULL;
HWND hLabel_TopFace = NULL;

HWND hLabel_BackFace = NULL;
HWND hLabel_LeftFace = NULL;
HWND hLabel_BottomFace = NULL;

HWND hResetButton = NULL;
HWND hwndLeftButton = NULL;
HWND hwndRightButton = NULL;
HWND hFrontClip_Minus = NULL;
HWND hFrontClip_Plus = NULL;
HWND hRightClip_Minus = NULL;
HWND hRightClip_Plus = NULL;
HWND hTopClip_Minus = NULL;
HWND hTopClip_Plus = NULL;
HWND hBackClip_Minus = NULL;
HWND hBackClip_Plus = NULL;
HWND hLeftClip_Minus = NULL;
HWND hLeftClip_Plus = NULL;
HWND hBottomClip_Minus = NULL;
HWND hBottomClip_Plus = NULL;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//function declarations:
	int initialize(void);
	void uninitialize(void);
	void resize(int, int);
	void display(void);
	void update(void);

	// local variables
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("Win32 SDK Window");
	int iHeight;
	int iWidth;
	BOOL bDone = FALSE;
	int iResult = 0;

	// code

	// File IO code:
	if (fopen_s(&gpFile, "Log.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("Failed to open Log File."), TEXT("File IO Error."), MB_ICONERROR | MB_OK);
		exit(0);
	}
	else
	{
		fprintf(gpFile, "Log File Created Successfully.\n");
		fprintf(gpFile, "Program started Successfully.\n");
	}


	CreateLogFile();

	//to initialize members of wndclass
	wndclass.cbSize = sizeof(wndclass);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC; //DeviceContext of our class will be in Class's OWNERSHIP.
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));


	//Registering above class 
	RegisterClassEx(&wndclass);

	iWidth = GetSystemMetrics(SM_CXFULLSCREEN);
	iHeight = GetSystemMetrics(SM_CYSCREEN);
	fprintf(gpFile, "Message: iWidth : %d \n", iWidth);
	fprintf(gpFile, "Message: iHeight: %d \n", iHeight);

	// code to create window
	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		szAppName,
		TEXT("VolumeApp 3D_Viewer"),	
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		(iWidth / 4),
		(iHeight / 4),
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL
	);
	ghwnd = hwnd;


	// initialization:
	iResult = initialize();
	if (iResult != 0)
	{
		MessageBox(hwnd, TEXT("initialize() Failed.\nExitting Program."), TEXT("Initialize Error."), MB_OK | MB_ICONERROR);
		DestroyWindow(hwnd);
	}
	else
	{
		fprintf(gpFile, "Message: initialize() successful.\n");
	}


	
	iEffectUsed = 3;
	

	// Reset button:
	hResetButton = CreateWindow(
		L"BUTTON",  // Predefined class; Unicode assumed 
		L"RESET",      // Button text 
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		1465,                 // x
		550,                 // y
		100,                // width
		35,                 // height
		hwnd,               // parent window
		(HMENU)ID_RESET_BUTTON,
		(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
		NULL
	);


	// ------------------ EFFECTS ARROW Two Buttons and Label ------------------ 
	hwndLeftButton = CreateWindow(
		L"BUTTON",
		L"<",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
		1395,                  // x
		450,                  // y
		40,                  // width
		30,                  // height
		hwnd,
		(HMENU)ID_LEFT_ARROW_EFFECT,
		(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
		NULL
	);

	hwndValueLabel = CreateWindow(
		L"STATIC",
		L"Value 1",
		WS_VISIBLE | WS_CHILD | SS_CENTER| SS_CENTERIMAGE,
		1440,                  // x
		450,                  // y
		150,                 // width
		30,                  // height
		hwnd,
		(HMENU)ID_LABEL_EFFECT,
		(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
		NULL
	);

	wchar_t text1[64];
	swprintf_s(text1, 64, L"Colormap");
	SetWindowText(hwndValueLabel, text1);

	hwndRightButton = CreateWindow(
		L"BUTTON",
		L">",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
		1595,                 // x
		450,                  // y
		40,                  // width
		30,                  // height
		hwnd,
		(HMENU)ID_RIGHT_ARROW_EFFECT,
		(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
		NULL
	);



	// ------------------ FRONT FACE:  ARROW Two Buttons and Label ------------------ 
	hFrontClip_Minus= CreateWindow(
		L"BUTTON",
		L"<",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
		1420,                  // x
		630,                  // y
		40,                  // width
		30,                  // height
		hwnd,
		(HMENU)ID_FRONT_FACE_MINUS,
		(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
		NULL
	);

	hLabel_FrontFace = CreateWindow(
		L"STATIC",
		L"0.5",
		WS_VISIBLE | WS_CHILD | SS_CENTER | SS_CENTERIMAGE,
		1465,                  // x
		630,                  // y
		100,                 // width
		30,                  // height
		hwnd,
		(HMENU)ID_LABEL_FRONT_FACE,
		(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
		NULL
	);

	wchar_t text2[64];
	swprintf_s(text2, 64, L"Front face: %.2f", fZPlus_FrontFace);
	SetWindowText(hLabel_FrontFace, text2);

	hFrontClip_Plus = CreateWindow(
		L"BUTTON",
		L">",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
		1570,                 // x
		630,                  // y
		40,                  // width
		30,                  // height
		hwnd,
		(HMENU)ID_FRONT_FACE_PLUS,
		(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
		NULL
	);


	//// ------------------ RIGHT FACE:  ARROW Two Buttons and Label ------------------ 
	hRightClip_Minus = CreateWindow(
		L"BUTTON",
		L"<",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
		1420,                  // x
		670,                  // y
		40,                  // width
		30,                  // height
		hwnd,
		(HMENU)ID_RIGHT_FACE_MINUS,
		(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
		NULL
	);

	hLabel_RightFace = CreateWindow(
		L"STATIC",
		L"0.5",
		WS_VISIBLE | WS_CHILD | SS_CENTER | SS_CENTERIMAGE,
		1465,                  // x
		670,                  // y
		100,                 // width
		30,                  // height
		hwnd,
		(HMENU)ID_LABEL_RIGHT_FACE,
		(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
		NULL
	);

	wchar_t text3[64];
	swprintf_s(text3, 64, L"Right face: %.2f", fXPlus_SideFace);
	SetWindowText(hLabel_RightFace, text3);

	hRightClip_Plus = CreateWindow(
		L"BUTTON",
		L">",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
		1570,                 // x
		670,                  // y
		40,                  // width
		30,                  // height
		hwnd,
		(HMENU)ID_RIGHT_FACE_PLUS,
		(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
		NULL
	);

	// ------------------ TOP FACE:  ARROW Two Buttons and Label ------------------ 
	hTopClip_Minus = CreateWindow(
		L"BUTTON",
		L"<",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
		1420,                  // x
		710,                  // y
		40,                  // width
		30,                  // height
		hwnd,
		(HMENU)ID_TOP_FACE_MINUS,
		(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
		NULL
	);

	hLabel_TopFace = CreateWindow(
		L"STATIC",
		L"0.5",
		WS_VISIBLE | WS_CHILD | SS_CENTER | SS_CENTERIMAGE,
		1465,                  // x
		710,                  // y
		100,                 // width
		30,                  // height
		hwnd,
		(HMENU)ID_LABEL_TOP_FACE,
		(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
		NULL
	);

	wchar_t text4[64];
	swprintf_s(text4, 64, L"Top face: %.2f", fYPlus_TopFace);
	SetWindowText(hLabel_TopFace, text4);

	hTopClip_Plus = CreateWindow(
		L"BUTTON",
		L">",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
		1570,                 // x
		710,                  // y
		40,                  // width
		30,                  // height
		hwnd,
		(HMENU)ID_TOP_FACE_PLUS,
		(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
		NULL
	);

	// ------------------ BACK FACE:  ARROW Two Buttons and Label ------------------ 
	hBackClip_Minus = CreateWindow(
		L"BUTTON",
		L"<",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
		1420,                  // x
		760,                  // y
		40,                  // width
		30,                  // height
		hwnd,
		(HMENU)ID_BACK_FACE_MINUS,
		(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
		NULL
	);

	hLabel_BackFace= CreateWindow(
		L"STATIC",
		L"0.5",
		WS_VISIBLE | WS_CHILD | SS_CENTER | SS_CENTERIMAGE,
		1465,                  // x
		760,                  // y
		100,                 // width
		30,                  // height
		hwnd,
		(HMENU)ID_LABEL_BACK_FACE,
		(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
		NULL
	);

	wchar_t text5[64];
	swprintf_s(text5, 64, L"Back face: %.2f", fZMinus_BackFace);
	SetWindowText(hLabel_BackFace, text5);

	hBackClip_Plus = CreateWindow(
		L"BUTTON",
		L">",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
		1570,                 // x
		760,                  // y
		40,                  // width
		30,                  // height
		hwnd,
		(HMENU)ID_BACK_FACE_PLUS,
		(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
		NULL
	);


	// ------------------ LEFT FACE:  ARROW Two Buttons and Label ------------------ 
	hLeftClip_Minus = CreateWindow(
		L"BUTTON",
		L"<",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
		1420,                  // x
		800,                  // y
		40,                  // width
		30,                  // height
		hwnd,
		(HMENU)ID_LEFT_FACE_MINUS,
		(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
		NULL
	);

	hLabel_LeftFace = CreateWindow(
		L"STATIC",
		L"0.5",
		WS_VISIBLE | WS_CHILD | SS_CENTER | SS_CENTERIMAGE,
		1465,                  // x
		800,                  // y
		100,                 // width
		30,                  // height
		hwnd,
		(HMENU)ID_LABEL_LEFT_FACE,
		(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
		NULL
	);

	wchar_t text6[64];
	swprintf_s(text6, 64, L"Left face: %.2f", fXMinus_SideFace);
	SetWindowText(hLabel_LeftFace, text6);

	hLeftClip_Plus = CreateWindow(
		L"BUTTON",
		L">",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
		1570,                 // x
		800,                  // y
		40,                  // width
		30,                  // height
		hwnd,
		(HMENU)ID_LEFT_FACE_PLUS,
		(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
		NULL
	);
	
	// ------------------ BOTTOM FACE:  ARROW Two Buttons and Label ------------------ 
	hBottomClip_Minus = CreateWindow(
		L"BUTTON",
		L"<",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
		1420,                  // x
		840,                  // y
		40,                  // width
		30,                  // height
		hwnd,
		(HMENU)ID_BOTTOM_FACE_MINUS,
		(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
		NULL
	);

	hLabel_BottomFace = CreateWindow(
		L"STATIC",
		L"0.5",
		WS_VISIBLE | WS_CHILD | SS_CENTER | SS_CENTERIMAGE,
		1465,                  // x
		840,                  // y
		100,                 // width
		30,                  // height
		hwnd,
		(HMENU)ID_LABEL_BOTTOM_FACE,
		(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
		NULL
	);

	wchar_t text7[64];
	swprintf_s(text7, 64, L"Bottom : %.2f",fYMinus_BottomFace);
	SetWindowText(hLabel_BottomFace, text7);

	hBottomClip_Plus = CreateWindow(
		L"BUTTON",
		L">",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
		1570,                 // x
		840,                  // y
		40,                  // width
		30,                  // height
		hwnd,
		(HMENU)ID_BOTTOM_FACE_PLUS,
		(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
		NULL
	);



	ShowWindow(hwnd, SW_MAXIMIZE);
	SetForegroundWindow(hwnd);//Z-Order madhe Pudhe aan.
	SetFocus(hwnd);//interally goes to WM_SETFOCUS.

	if (gbFullscreen == FALSE)
	{
		ToggleFullscreen();
		gbFullscreen = TRUE;
	}


	while (bDone == FALSE)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				bDone = TRUE;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if (gbActiveWindow == TRUE)
			{
				display();
				update();
			}
		}
	}


	uninitialize();

	return ((int)msg.wParam);
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	//function declarations:
	void resize(int, int);
	void Set_UI_Objects_Position(HWND hwnd);
	void uninitialize(void);
	void ToggleFullscreen(void);

	// local:


	// code
	switch (iMsg)
	{

	case WM_CREATE:
		break;

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
			case ID_RESET_BUTTON:
				{
					rotationX = 0.0f;
					rotationY = 0.0f;
					rotationZ = 0.0f;
					fZPlus_FrontFace = 0.5f;
					fXPlus_SideFace = 0.5f;
					fYPlus_TopFace = 0.5f;
					fYMinus_BottomFace = -0.5f;
					fZMinus_BackFace = -0.5f;
					fXMinus_SideFace = -0.5f;
					dist = -2.0f;
					
					fClipPlane_Front = 1.0f;
					fClipPlane_Back = 0.0f;
					fClipPlane_Right = 1.0f;
					fClipPlane_Left = 0.0f;
					fClipPlane_Top = 1.0f;
					fClipPlane_Bottom = 0.0f;

					bSliceUpdate = TRUE;
					// reset all labels:
					wchar_t text2[64];
					swprintf_s(text2, 64, L"Front face: %.2f", fZPlus_FrontFace);
					SetWindowText(hLabel_FrontFace, text2);
					swprintf_s(text2, 64, L"Right face: %.2f", fXPlus_SideFace);
					SetWindowText(hLabel_RightFace, text2);
					swprintf_s(text2, 64, L"Top face: %.2f", fYPlus_TopFace);
					SetWindowText(hLabel_TopFace, text2);
					swprintf_s(text2, 64, L"Back face: %.2f", fZMinus_BackFace);
					SetWindowText(hLabel_BackFace, text2);
					swprintf_s(text2, 64, L"Left face: %.2f", fXMinus_SideFace);
					SetWindowText(hLabel_LeftFace, text2);
					swprintf_s(text2, 64, L"Bottom face: %.2f", fYMinus_BottomFace);
					SetWindowText(hLabel_BottomFace, text2);

					SetFocus(hwnd);
				}
				break;

			/* Effect Switch Arrows */
			case ID_LEFT_ARROW_EFFECT:
				iEffectUsed -= 1;
				if (iEffectUsed < 0)
				{
					iEffectUsed = 4;
				}
				bSliceUpdate = TRUE;

				textHolder = effectNames[iEffectUsed];
				SetWindowText(hwndValueLabel, textHolder);
				SetFocus(hwnd);
				break;

			case ID_RIGHT_ARROW_EFFECT:
				iEffectUsed += 1;
				if (iEffectUsed > 4)
				{
					iEffectUsed = 0;
				}
				bSliceUpdate = TRUE;
				textHolder = effectNames[iEffectUsed];
				SetWindowText(hwndValueLabel, textHolder);
				SetFocus(hwnd);
				break;

			/* FRONT FACE  ARROWS */
			case ID_FRONT_FACE_MINUS:

				if (fZPlus_FrontFace >= -0.49f)
				{
					fZPlus_FrontFace -= 0.01f;
				}

				if (fClipPlane_Front >= 0.0001f)
				{
					fClipPlane_Front -= 0.01f;
				}
				bSliceUpdate = TRUE;
				wchar_t text2[64];
				swprintf_s(text2, 64, L"Front face: %.2f", fZPlus_FrontFace);
				SetWindowText(hLabel_FrontFace, text2);
				SetFocus(hwnd);
				break;

			case ID_FRONT_FACE_PLUS:
				if (fZPlus_FrontFace < 0.5f)
				{
					fZPlus_FrontFace += 0.01f;
				}

				if (fClipPlane_Front < 1.0f)
				{
					fClipPlane_Front += 0.01f;
				}
				bSliceUpdate = TRUE;
				swprintf_s(text2, 64, L"Front face: %.2f", fZPlus_FrontFace);
				SetWindowText(hLabel_FrontFace, text2);
				SetFocus(hwnd);
				break;

			/* RIGHT FACE  ARROWS */
			case ID_RIGHT_FACE_MINUS:

				if (fXPlus_SideFace >= -0.49f)
				{
					fXPlus_SideFace -= 0.01f;
				}

				if (fClipPlane_Right >= 0.0001f)
				{
					fClipPlane_Right -= 0.01f;
				}
				bSliceUpdate = TRUE;
				wchar_t text3[64];
				swprintf_s(text3, 64, L"Right face: %.2f", fXPlus_SideFace);
				SetWindowText(hLabel_RightFace, text3);
				SetFocus(hwnd);
				break;

			case ID_RIGHT_FACE_PLUS:
				if (fXPlus_SideFace < 0.5f)
				{
					fXPlus_SideFace += 0.01f;
				}

				if (fClipPlane_Right < 1.0f)
				{
					fClipPlane_Right += 0.01f;
				}
				bSliceUpdate = TRUE;
				swprintf_s(text3, 64, L"Right face: %.2f", fXPlus_SideFace);
				SetWindowText(hLabel_RightFace, text3);
				SetFocus(hwnd);
				break;

			/* TOP FACE  ARROWS */
			case ID_TOP_FACE_MINUS:
				if (fYPlus_TopFace >= -0.49f)
				{
					fYPlus_TopFace -= 0.01f;
				}

				if (fClipPlane_Top >= 0.0001f)
				{
					fClipPlane_Top -= 0.01f;
				}
				bSliceUpdate = TRUE;
				wchar_t text4[64];
				swprintf_s(text4, 64, L"Top face: %.2f", fYPlus_TopFace);
				SetWindowText(hLabel_TopFace, text4);
				SetFocus(hwnd);
				break;

			case ID_TOP_FACE_PLUS:
				if (fYPlus_TopFace < 0.5f)
				{
					fYPlus_TopFace += 0.01f;
				}

				if (fClipPlane_Top < 1.0f)
				{
					fClipPlane_Top += 0.01f;
				}
				bSliceUpdate = TRUE;
				swprintf_s(text4, 64, L"Top face: %.2f", fYPlus_TopFace);
				SetWindowText(hLabel_TopFace, text4);
				SetFocus(hwnd);
				break;

			/* BACK Face */
			case ID_BACK_FACE_MINUS:
				if (fZMinus_BackFace >= -0.49f)
				{
					fZMinus_BackFace -= 0.01f;
				}

				if (fClipPlane_Back >= 0.0001f)
				{
					fClipPlane_Back -= 0.01f;
				}
				bSliceUpdate = TRUE;
				wchar_t text5[64];
				swprintf_s(text5, 64, L"Back face: %.2f", fZMinus_BackFace);
				SetWindowText(hLabel_BackFace, text5);
				SetFocus(hwnd);
				break;
			case ID_BACK_FACE_PLUS:
				if (fZMinus_BackFace < 0.5f)
				{
					fZMinus_BackFace += 0.01f;
				}

				if (fClipPlane_Back < 1.0f)
				{
					fClipPlane_Back += 0.01f;
				}
				bSliceUpdate = TRUE;
				swprintf_s(text5, 64, L"Back face: %.2f", fZMinus_BackFace);
				SetWindowText(hLabel_BackFace, text5);
				SetFocus(hwnd);
				break;

			/* LEFT Face */
			case ID_LEFT_FACE_MINUS:
				if (fXMinus_SideFace >= -0.49f)
				{
					fXMinus_SideFace -= 0.01f;
				}

				if (fClipPlane_Left >= 0.0001f)
				{
					fClipPlane_Left -= 0.01f;
				}
				bSliceUpdate = TRUE;
				wchar_t text6[64];
				swprintf_s(text6, 64, L"Left face: %.2f", fXMinus_SideFace);
				SetWindowText(hLabel_LeftFace, text6);
				SetFocus(hwnd);
				break;

			case ID_LEFT_FACE_PLUS:
				if (fXMinus_SideFace < 0.5f)
				{
					fXMinus_SideFace += 0.01f;
				}

				if (fClipPlane_Left < 1.0f)
				{
					fClipPlane_Left += 0.01f;
				}
				bSliceUpdate = TRUE;
				swprintf_s(text6, 64, L"Left face: %.2f", fXMinus_SideFace);
				SetWindowText(hLabel_LeftFace, text6);
				SetFocus(hwnd);
				break;

			/* BOTTOM Face */
			case ID_BOTTOM_FACE_MINUS:
				if (fYMinus_BottomFace >= -0.49f)
				{
					fYMinus_BottomFace -= 0.01f;
				}

				if (fClipPlane_Bottom >= 0.0001f)
				{
					fClipPlane_Bottom -= 0.01f;
				}
				bSliceUpdate = TRUE;
				wchar_t text7[64];
				swprintf_s(text7, 64, L"Bottom : %.2f", fYMinus_BottomFace);
				SetWindowText(hLabel_BottomFace, text7);
				SetFocus(hwnd);
				break;

			case ID_BOTTOM_FACE_PLUS:
				if (fYMinus_BottomFace < 0.5f)
				{
					fYMinus_BottomFace += 0.01f;
				}

				if (fClipPlane_Bottom < 1.0f)
				{
					fClipPlane_Bottom += 0.01f;
				}
				bSliceUpdate = TRUE;
				swprintf_s(text7, 64, L"Bottom : %.2f", fYMinus_BottomFace);
				SetWindowText(hLabel_BottomFace, text7);
				SetFocus(hwnd);
				break;


			}
			break;

	case WM_KEYDOWN:
		bSliceUpdate = TRUE;
		switch (LOWORD(wParam))
		{
#ifdef KEYBOARD_SLICING_CONTROLS
		case VK_UP:
			if (fZPlus_FrontFace >= -0.49f)
			{
				fZPlus_FrontFace -= 0.01f;
			}

			break;

		case VK_DOWN:
			if (fZPlus_FrontFace < 0.5f)
			{
				fZPlus_FrontFace += 0.01f;
			}
			break;

		case VK_LEFT:
			if (fXPlus_SideFace>= -0.49f)
			{
				fXPlus_SideFace -= 0.01f;
			}
			break;

		case VK_RIGHT:
			if (fXPlus_SideFace < 0.5f)
			{
				fXPlus_SideFace += 0.01f;
			}
			break;

		case VK_SUBTRACT:
			if (fYPlus_TopFace >= -0.49f)
			{
				fYPlus_TopFace -= 0.01f;
			}
			break;

		case VK_ADD:
			if (fYPlus_TopFace < 0.5f)
			{
				fYPlus_TopFace += 0.01f;
			}
			break;
		case VK_MULTIPLY:
			if (fYMinus_BottomFace < 0.5f)
			{
				fYMinus_BottomFace += 0.01f;
			}
			break;

		case VK_DIVIDE:
			if (fYMinus_BottomFace >= -0.49f)
			{
				fYMinus_BottomFace -= 0.01f;
			}
			break;

		case VK_NUMPAD2:
			if (fZMinus_BackFace < 0.5f)
			{
				fZMinus_BackFace += 0.01f;
			}
			break;

		case VK_NUMPAD8:
			if (fZMinus_BackFace >= -0.49f)
			{
				fZMinus_BackFace -= 0.01f;
			}
			break;

		case VK_NUMPAD6:
			if (fXMinus_SideFace < 0.5f)
			{
				fXMinus_SideFace += 0.01f;
			}
			break;

		case VK_NUMPAD4:
			if (fXMinus_SideFace >= -0.49f)
			{
				fXMinus_SideFace -= 0.01f;
			}
			break;
#endif

		case VK_ESCAPE:
			DestroyWindow(hwnd);
			break;

		default:
			break;
		}
		break;

	case WM_KEYUP:
		bSliceUpdate = FALSE;
		break;

	case WM_CHAR:
		switch (LOWORD(wParam))
		{
		case 'f':
		case 'F':
			if (gbFullscreen == FALSE)
			{
				ToggleFullscreen();
				gbFullscreen = TRUE;
			}
			else
			{
				ToggleFullscreen();
				gbFullscreen = FALSE;
			}
			break;

		case 'r':
		case 'R':
			// Reset All :
			{
				rotationX = 0.0f;
				rotationY = 0.0f;
				rotationZ = 0.0f;
				fZPlus_FrontFace = 0.5f;
				fXPlus_SideFace = 0.5f;
				fYPlus_TopFace= 0.5f;
				fYMinus_BottomFace = -0.5f;
				fZMinus_BackFace= -0.5f;
				fXMinus_SideFace = -0.5f;
				dist = -2.0f;
				// reset all labels:
				wchar_t text2[64];
				swprintf_s(text2, 64, L"Front face: %.2f", fZPlus_FrontFace);
				SetWindowText(hLabel_FrontFace, text2);
				swprintf_s(text2, 64, L"Right face: %.2f", fXPlus_SideFace);
				SetWindowText(hLabel_RightFace, text2);
				swprintf_s(text2, 64, L"Top face: %.2f", fYPlus_TopFace);
				SetWindowText(hLabel_TopFace, text2);
				swprintf_s(text2, 64, L"Back face: %.2f", fZMinus_BackFace);
				SetWindowText(hLabel_BackFace, text2);
				swprintf_s(text2, 64, L"Left face: %.2f", fXMinus_SideFace);
				SetWindowText(hLabel_LeftFace, text2);
				swprintf_s(text2, 64, L"Bottom face: %.2f", fYMinus_BottomFace);
				SetWindowText(hLabel_BottomFace, text2);
			}
			break;
		case 'w':
		case 'W':
			bWireframe = !bWireframe;
			break;
		default:
			break;
		}
		break;

	case WM_MOUSEMOVE:

		if (bMouseClicked)
		{
			int newX = GET_X_LPARAM(lParam);
			int newY = GET_Y_LPARAM(lParam);

			float dX = (newX - oldX) / 5.0f;
			float dY = (newY - oldY) / 5.0f;

			rotationX += dY;

			float fSign = (cos(glm::radians(rotationX)) >= 0.0 ? 1.0f : -1.0f);
			//?? float factor = cos(glm::radians(rotationX));
			rotationY += dX * fSign;

			oldX = newX;
			oldY = newY;

		}
		else
		{
			oldX = GET_X_LPARAM(lParam);
			oldY = GET_Y_LPARAM(lParam);

			xMouseValue= GET_X_LPARAM(lParam);
			yMouseValue= GET_Y_LPARAM(lParam);
		}
		break;

	case WM_MOUSEWHEEL:
		{
			int iWheelDelta= GET_WHEEL_DELTA_WPARAM(wParam);
			char str[255];
			sprintf(str, "Inside WM_MOUSEHWHEEL with value iWheelDelta=> %d", iWheelDelta);
			if (iWheelDelta > 0)
			{
				// WHEEL UP:
				dist += 0.1f;
				//MessageBox(hwnd, TEXT("WHEEL UP"), TEXT("CAPTION"), MB_OK);
			}
			else if(iWheelDelta <0)
			{
				// WHEEL DOWN:
				dist -= 0.1f;
				//MessageBox(hwnd, TEXT("WHEEL DOWN"), TEXT("CAPTION"), MB_OK);
			}
			bSliceUpdate = TRUE;
			return (0);
		}
		break;

	case WM_LBUTTONDOWN:
		bMouseClicked = TRUE;
		SetCapture(hwnd);
		break;

	case WM_LBUTTONUP:
		bMouseClicked = FALSE;
		if (GetCapture() == hwnd)
			ReleaseCapture();
		SetFocus(hwnd);
		break;

	case WM_SETFOCUS:
		gbActiveWindow = TRUE;
		break;

	case WM_KILLFOCUS:
		gbActiveWindow = FALSE;
		break;

	case WM_SIZE:
		resize(LOWORD(lParam), HIWORD(lParam));
		Set_UI_Objects_Position(hwnd);
		break;

	case WM_ERASEBKGND:
		return (0);

	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return (DefWindowProc(hwnd, iMsg, wParam, lParam));
}

void ToggleFullscreen(void)
{
	// local:
	MONITORINFO mi = { sizeof(MONITORINFO) };

	// code:

	if (gbFullscreen == FALSE)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				//MONITOR+INF+F+PRIMARY = Monitor+Information+Flag(of Info)+PRIMARY(GPU Primary MonitorFlag Value).

				// i)
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				// ii)
				SetWindowPos(ghwnd,	 // global HWND
					HWND_TOP,//gives WS_OVERLAPPED
					mi.rcMonitor.left,// mi->RECT->LEFT
					mi.rcMonitor.top,// mi->RECT->TOP
					mi.rcMonitor.right - mi.rcMonitor.left,// mi->RECT->WIDTH
					mi.rcMonitor.bottom - mi.rcMonitor.top,// mi->RECT->HEIGHT
					SWP_NOZORDER | SWP_FRAMECHANGED
					//SWP_NOZORDER->Parent's Z-Order shouldn't Affect.
					//SWP_FRAMECHANGED->internally removes Frame by sending msg->WM_NCCALCSIZE.
				);
				//ShowCursor(FALSE);
				ShowCursor(TRUE);
			}
		}

	}
	else
	{
		SetWindowPlacement(ghwnd, &wpPrev);

		SetWindowLong(ghwnd,	//global HWND
			GWL_STYLE,	//for Style
			dwStyle | WS_OVERLAPPEDWINDOW	//set to WS_OVERLAPPEDWINDOW
		);

		SetWindowPos(ghwnd,
			HWND_TOP, //SWPos has > priority than SWPlacement and SWStyle hence we are confirming using HWND_TOP here
			0, // these four are
			0, // not needed now
			0, // since SWPlacement > priority than SWPos in case of 
			0, // these 4 parameters
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED
			// 5 macros to set behavior with Parent.

			//SWP_NOMOVE-> dont move while returning to Normal
			//SWP_NOSIZE-> dont change size aft Fullscreen exit.
			//SWP_NOOWNERZORDER-> if during Fullscreen() owner's ZOrder is changed,still retain its old ZOrder
			//SWP_NOZORDER->Dont change its old ZOrder
			//SWP_FRAMECHANGED-> send WM_NCCALCSIZE to recalculate FrameSize or ClientArea+Frame.
		);
		ShowCursor(TRUE);
	}

}

int initialize(void)
{
	// function declarations:
	void printGLInfo(void);
	void uninitialize(void);
	void resize(int, int);
	
	// local:
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex = 0;
	BOOL bResult = FALSE;

	// code:

	// 1: Initialize PixelFormatDescriptor.
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 32;

	// 2: get HDC 
	ghdc = GetDC(ghwnd);
	if (ghdc == NULL)
	{
		fprintf(gpFile, "Error: GetDC() Failed.\n");
		return (-1);
	}

	// 3: Use HDC Handle to get index of closest PFD from OS.
	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if (iPixelFormatIndex == 0)
	{
		fprintf(gpFile, "Error: ChoosePixelFormat() Failed.\n");
		return (-2);
	}

	// 4: Set Pixel Format:
	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)
	{
		fprintf(gpFile, "SetPixelFormat() Failed.\n");
		return (-3);
	}

	// 5: Tell 'wgl' briding library to give OpenGL Compatible Context from this DeviceContext-ghdc.
	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL)
	{
		fprintf(gpFile, "wglCreateContext() Failed.\n");
		return (-4);
	}

	// 6: Make this Rendering Context of OpenGL 'ghrc' as Current Context.
	if (wglMakeCurrent(ghdc, ghrc) == FALSE)
	{
		fprintf(gpFile, "Error: wglMakeCurrent() Failed.\n");
		return (-5);
	}


	// initialize GLEW
	if (glewInit() != GLEW_OK)
	{
		fprintf(gpFile, " Error: glewInit() Failed.\n");
		return (-6);
	}

	printGLInfo();


	LoadVolumeData();


	Initialize_Slicing_shader();





	Slice_Volume();

	LoadGridObject_Shader(5,5);

	setup_Axes_BufferObjects();
	Update_Volume_Box_Axes();


	Initialize_Raycasting_shader();
	Initialize_Raycasting_Geomatry();

	Initialize_IsoSurface_shader();

	Initialize_ColormapClassification_shader();
	LoadTransferFunction();


	Initialize_TetrahedraMarcher_Constructor();
	SetVolumeDimensions(256, 256, 256);
	if (LoadVolume_MT())
	{
		fprintf(gpFile, "------- LoadVolume() Successful.------- \n");
	}
	else
	{
		fprintf(gpFile, "------- LoadVolume() Failed.------- \n");
	}
	//set the isosurface value
	SetIsosurfaceValue(48);
	//set the number of sampling voxels 
	SetNumSamplingVoxels(128, 128, 128);
	//begin tetrahedra marching
	MarchVolume();
	Initialize_TetrahedraMarcher_Geomatry();
	Initialize_TetrahedraMarcher_Shaders();



	// Set the background color to BLUE.
	glClearColor(0.75f, 0.75f, 0.75f, 0.0f);

	// To Enable Depth: 3 lines 
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_TEXTURE_3D);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// No WarmUpResize in this version

	perspectiveProjMatrix_glm = glm::identity<glm::mat4>();

	resize(WIN_WIDTH, WIN_HEIGHT);
	return (0);
}

void printGLInfo(void)
{
	// local:
	GLint iNumExtensions;
	GLint i;

	// code:
	fprintf(gpFile, "=======================================================\n");
	fprintf(gpFile, "OpenGL Vendor: %s \n", glGetString(GL_VENDOR));
	fprintf(gpFile, "=======================================================\n");
	fprintf(gpFile, "OpenGL Renderer: %s \n", glGetString(GL_RENDERER));
	fprintf(gpFile, "=======================================================\n");
	fprintf(gpFile, "OpenGL Version: %s \n", glGetString(GL_VERSION));
	fprintf(gpFile, "=======================================================\n");
	fprintf(gpFile, "OpenGL GLSL Version: %s \n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	fprintf(gpFile, "=======================================================\n");

	// list supported extensions:
	glGetIntegerv(GL_NUM_EXTENSIONS, &iNumExtensions);

	//printing NoOfExtensions like RTR4:
	fprintf(gpFile, "OpenGL No of Supported Extenstions: %d \n", iNumExtensions);
	fprintf(gpFile, "=======================================================\n");
	for (i = 0; i < iNumExtensions; i++)
	{
		fprintf(gpFile, "%d]  %s \n", i, glGetStringi(GL_EXTENSIONS, i));
	}

	fprintf(gpFile, "=======================================================\n");
}


void resize(int width, int height)
{
	// code:
	if (height <= 0)
	{
		height = 1;
	}

	giWinWidth = width;
	giWinHeight= height;

	glViewport(
		0,	// x coordinate
		0,	// y coordinate
		(GLsizei)width,  //	type=> OpenGL's integer Size i.e sizei
		(GLsizei)height
	);

	
	perspectiveProjMatrix_glm = glm::perspective(glm::radians(60.0f), (GLfloat)width / (GLfloat)height, 0.1f, 1000.0f);
}

void display(void)
{
	// prototype:


	// code:

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 ModelViewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, dist));

	ModelViewMatrix = glm::rotate(ModelViewMatrix, glm::radians(rotationX), glm::vec3(1.0f, 0.0f, 0.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, glm::radians(rotationY), glm::vec3(0.0f, 1.0f, 0.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, rotationZ, glm::vec3(0.0f, 0.0f, 1.0f));

	glm::mat4 modelViewProjectionMatrix = perspectiveProjMatrix_glm * ModelViewMatrix;



	switch (iEffectUsed)
	{
		case 0:
			glClearColor(0.75f, 0.75f, 0.75f, 0.0f);
			Render_Basic_Volume();
			Render_Volume_Box_Axes(modelViewProjectionMatrix);
			Update_Basic_Volume();
			if (bMouseClicked || bSliceUpdate)
			{
				Slice_Volume();
				Update_Volume_Box_Axes();
			}
			break;
			
		case 1:
			glClearColor(0.75f, 0.75f, 0.75f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			Render_Raycasting_Output();
			Render_Volume_Box_Axes(modelViewProjectionMatrix);
			Update_Raycasting_Output();
			if (bMouseClicked || bSliceUpdate)
			{
				Update_Volume_Box_Axes();
				Update_Raycasting_Cube_VBO();
			}
			break;

		case 2:
			glClearColor(0.75f, 0.75f, 0.75f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			Render_IsoSurface_Output();
			Render_Volume_Box_Axes(modelViewProjectionMatrix);
			Update_IsoSurface_Output();
			if (bMouseClicked || bSliceUpdate)
			{
				Update_Volume_Box_Axes();
				Update_Raycasting_Cube_VBO();
			}
			break;

		case 3:
			glClearColor(0.15f, 0.15f, 0.15f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			Render_ColormapClassification_Output();
			Render_Volume_Box_Axes(modelViewProjectionMatrix);
			Update_ColormapClassification_Output();
			if (bMouseClicked || bSliceUpdate)
			{
				Slice_Volume();
				Update_Volume_Box_Axes();
			}
			break;

		case 4:
			glClearColor(0.75f, 0.75f, 0.75f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			Render_MarchingTetrahedra();
			Render_Volume_Box_Axes(modelViewProjectionMatrix);
			Update_MarchingTetrahedra();
			if (bMouseClicked || bSliceUpdate)
			{
				Update_Volume_Box_Axes();
			}

			break;
		case 5:
			glClearColor(0.2f, 0.0f, 0.2f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			break;

		default:
			glClearColor(0.2f, 0.0f, 0.2f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			break;
	}




	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, giWinWidth, giWinHeight);


	//****** Message using 2D Texture for always on Top can be added below with temporary disabled DEPTH for them ***********


	wchar_t str[255];
	if (bMouseClicked)
	{
		xMouseValue_NDC = xMouseValue / giWinWidth * 2.0f;
		yMouseValue_NDC = yMouseValue / giWinHeight * 2.0f;
		xMouseValue_NDC = (xMouseValue_NDC - 1.0f);
		yMouseValue_NDC = (1.0f - yMouseValue_NDC);
		swprintf_s(str, L"VolumeApp 3D_Viewer : x ,y  ( %.2f ,%.2f )| Left Click DOWN xMouseValue_NDC,yMouseValue_NDC ( %.2f ,%.2f )  ", xMouseValue, yMouseValue, xMouseValue_NDC, yMouseValue_NDC);
	}
	else
	{
		swprintf_s(str, L"VolumeApp 3D_Viewer : x ,y ( %.2f ,%.2f )", xMouseValue, yMouseValue);
	}
	SetWindowTextW(ghwnd, str);

	SwapBuffers(ghdc);
}

void update(void)
{
	// code:
	
}

void uninitialize(void)
{
	// function declarations
	void ToggleFullscreen(void);

	// code:

	Uninitialize_MarchingTetrahedra();
	Uninitialize_ColormapClassification_shader();
	Uninitialize_IsoSurface_shader();
	Uninitialize_Raycasting_shader();
	Uninitialize_Slicing_shader();

	Uninitialize_Grid();


	if (gbFullscreen == TRUE)
	{
		ToggleFullscreen();
		gbFullscreen = FALSE;
	}

	// # Make hdc as CurrentDC()
	if (wglGetCurrentContext() == ghrc)
	{
		wglMakeCurrent(NULL, NULL);
	}

	if (ghrc)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
	}

	if (ghdc)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	// Destroy Window:
	if (ghwnd)
	{
		DestroyWindow(ghwnd);
		ghwnd = NULL;
	}

	
	CloseLogFile();

	// close the Log File:
	if (gpFile)
	{
		fprintf(gpFile, "Inside uninitialize().\nClosing the LogFile.\n");
		fprintf(gpFile, "Program ended successfully.\n");
		fprintf(gpFile, "=======================================================\n");
		fclose(gpFile);
		gpFile = NULL;
	}
}





int LoadVolumeData(void)
{
	void uninitialize();
	// local:

	// code:

	std::ifstream infile(volume_file.c_str(), std::ios_base::binary);	// Engine
	//std::ifstream infile(volume_file_4.c_str(), std::ios_base::binary); // Head
	//std::ifstream infile(volume_file_2.c_str(), std::ios_base::binary); // Skull


	if (infile.good())
	{
		//read the volume data file
		GLubyte* pData = new GLubyte[XDIM*YDIM*ZDIM];
		infile.read(reinterpret_cast<char*>(pData), XDIM*YDIM*ZDIM * sizeof(GLubyte));
		infile.close();


		// 1. Generate and bind OpenGL Texture:
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_3D, textureID);
		{
			// set parameters for 'textureID'
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

			// set mipmap levers for base and max:
			glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_BASE_LEVEL,0);
			glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MAX_LEVEL,4);

			// create 3D texture with pData : //allocate data with internal format and foramt as (GL_RED)	
			glTexImage3D(
				/*target*/GL_TEXTURE_3D,
				/* level*/0,
				/*internalFormat*/GL_R8,
				/* width*/XDIM,
				/* height*/YDIM,
				/* depth*/ZDIM,
				/* border*/0,
				/* Format*/GL_RED,
				/* type*/GL_UNSIGNED_BYTE,
				/*pixel data*/pData
			);

			// generate mipmaps:
			glGenerateMipmap(GL_TEXTURE_3D);


		}
		//glBindTexture(GL_TEXTURE_3D, 0);

		fprintf(gpFile, "LoadVolumeData() Success Step1\n"); fflush(gpFile);

		delete[] pData;
	}
	else
	{
		fprintf(gpFile, "LoadVolumeData() FAILED Step1\n"); fflush(gpFile);
		uninitialize();
		return -1;
	}

	glGenVertexArrays(1, &VAO_volume);
	glBindVertexArray(VAO_volume);
	{
		glGenBuffers(1, &VBO_volume);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_volume);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vTextureSlices), 0, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
		glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	glBindVertexArray(0);




	//setup the current camera transform and get the view direction vector
	glm::mat4 T = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, dist));
	glm::mat4 Rx = glm::rotate(T, rotationX, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 Ry = glm::rotate(Rx, rotationY, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 MV = glm::rotate(Ry, rotationZ, glm::vec3(0.0f, 0.0f, 1.0f));

	viewDirection= -glm::vec3(MV[0][2], MV[1][2], MV[2][2]);	//get the current view direction vector


	fprintf(gpFile, "LoadVolumeData() Success Step2 and Last\n"); fflush(gpFile);


	return 0;
}



bool LoadVolume_MT(void)
{

	std::ifstream infile(volume_file.c_str(), std::ios_base::binary);

	if (infile.good())
	{
		pVolume = new GLubyte[XDIM_TM * YDIM_TM * ZDIM_TM];
		infile.read(reinterpret_cast<char*>(pVolume), XDIM_TM * YDIM_TM * ZDIM_TM * sizeof(GLubyte));
		infile.close();
		return true;
	}
	else
	{
		return false;
	}
}


// to Update UI Button and Label Position on resize():

void Set_UI_Objects_Position(HWND hwnd)
{
	// local:
	RECT rc;

	// code:

	GetClientRect(hwnd, &rc);

	int clientWidth = rc.right - rc.left;
	int clientHeight = rc.bottom - rc.top;

	// ration from original position x,y  i,e 155.0 and 620.0 but using the 1/5 part of width from Right Side
	int columnX = clientWidth *  1/5;
	int columnWidth = clientWidth - columnX;

	
	const int buttonWidth = 40;
	const int buttonHeight = 30;
	
	int labelWidth = 150;
	int labelHeight = 30;

	const int space = 50;
	const int padding = 10;


	int x = columnWidth + 20;
	int y = clientHeight * (620.0f / 1080.0f);

	/****** RESET Button *********/
	SetWindowPos(
		hResetButton,
		NULL,
		x+space, y-space,
		buttonWidth*2, buttonHeight,
		SWP_NOZORDER | SWP_NOACTIVATE
	);

	/********* Effects Buttons ********/
	SetWindowPos(
		hwndLeftButton,
		NULL,
		x, y,
		buttonWidth, buttonHeight,
		SWP_NOZORDER | SWP_NOACTIVATE
	);


	SetWindowPos(
		hwndValueLabel,
		NULL,
		x+space, 
		y,
		labelWidth, labelHeight,
		SWP_NOZORDER | SWP_NOACTIVATE
	);


	SetWindowPos(
		hwndRightButton,
		NULL,
		x + buttonWidth + padding + labelWidth + padding,
		y,
		buttonWidth, buttonHeight,
		SWP_NOZORDER | SWP_NOACTIVATE
	);



	labelWidth = 100;
	labelHeight = 30;

	/********* FRONT   FACE ********/
	y = y + 40;
	SetWindowPos(
		hFrontClip_Minus,
		NULL,
		x, y,
		buttonWidth, buttonHeight,
		SWP_NOZORDER | SWP_NOACTIVATE
	);


	SetWindowPos(
		hLabel_FrontFace,
		NULL,
		x + space,
		y,
		labelWidth, labelHeight,
		SWP_NOZORDER | SWP_NOACTIVATE
	);


	SetWindowPos(
		hFrontClip_Plus,
		NULL,
		x + buttonWidth + padding + labelWidth + padding,
		y,
		buttonWidth, buttonHeight,
		SWP_NOZORDER | SWP_NOACTIVATE
	);

	
	/********* RIGHT   FACE ********/
	y = y + 40;
	SetWindowPos(
		hRightClip_Minus,
		NULL,
		x, y,
		buttonWidth, buttonHeight,
		SWP_NOZORDER | SWP_NOACTIVATE
	);


	SetWindowPos(
		hLabel_RightFace,
		NULL,
		x + space,
		y,
		labelWidth, labelHeight,
		SWP_NOZORDER | SWP_NOACTIVATE
	);


	SetWindowPos(
		hRightClip_Plus,
		NULL,
		x + buttonWidth + padding + labelWidth + padding,
		y,
		buttonWidth, buttonHeight,
		SWP_NOZORDER | SWP_NOACTIVATE
	);


	/********* TOP  FACE ********/
	y = y + 40;
	SetWindowPos(
		hTopClip_Minus,
		NULL,
		x, y,
		buttonWidth, buttonHeight,
		SWP_NOZORDER | SWP_NOACTIVATE
	);


	SetWindowPos(
		hLabel_TopFace,
		NULL,
		x + space,
		y,
		labelWidth, labelHeight,
		SWP_NOZORDER | SWP_NOACTIVATE
	);


	SetWindowPos(
		hTopClip_Plus,
		NULL,
		x + buttonWidth + padding + labelWidth + padding,
		y,
		buttonWidth, buttonHeight,
		SWP_NOZORDER | SWP_NOACTIVATE
	);


	/********* BACK FACE ********/
	y = y + 40;
	SetWindowPos(
		hBackClip_Minus,
		NULL,
		x, y,
		buttonWidth, buttonHeight,
		SWP_NOZORDER | SWP_NOACTIVATE
	);


	SetWindowPos(
		hLabel_BackFace,
		NULL,
		x + space,
		y,
		labelWidth, labelHeight,
		SWP_NOZORDER | SWP_NOACTIVATE
	);


	SetWindowPos(
		hBackClip_Plus,
		NULL,
		x + buttonWidth + padding + labelWidth + padding,
		y,
		buttonWidth, buttonHeight,
		SWP_NOZORDER | SWP_NOACTIVATE
	);

	/********* LEFT FACE ********/
	y = y + 40;
	SetWindowPos(
		hLeftClip_Minus,
		NULL,
		x, y,
		buttonWidth, buttonHeight,
		SWP_NOZORDER | SWP_NOACTIVATE
	);


	SetWindowPos(
		hLabel_LeftFace,
		NULL,
		x + space,
		y,
		labelWidth, labelHeight,
		SWP_NOZORDER | SWP_NOACTIVATE
	);


	SetWindowPos(
		hLeftClip_Plus,
		NULL,
		x + buttonWidth + padding + labelWidth + padding,
		y,
		buttonWidth, buttonHeight,
		SWP_NOZORDER | SWP_NOACTIVATE
	);

	/********* BOTTOM FACE ********/
	y = y + 40;
	SetWindowPos(
		hBottomClip_Minus,
		NULL,
		x, y,
		buttonWidth, buttonHeight,
		SWP_NOZORDER | SWP_NOACTIVATE
	);


	SetWindowPos(
		hLabel_BottomFace,
		NULL,
		x + space,
		y,
		labelWidth, labelHeight,
		SWP_NOZORDER | SWP_NOACTIVATE
	);


	SetWindowPos(
		hBottomClip_Plus,
		NULL,
		x + buttonWidth + padding + labelWidth + padding,
		y,
		buttonWidth, buttonHeight,
		SWP_NOZORDER | SWP_NOACTIVATE
	);



}
