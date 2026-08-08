//VolumeApp.cpp
#define UNICODE
#define _UNICODE

//- Commmon Header Files -

#include<Windows.h>
#include<Windowsx.h>
#include<stdio.h>// for FileIO
#include<stdlib.h>// for exit(),malloc()
#include <fstream> // FOR std::ifstream
#include <iostream>
#include<math.h>
#include<stdarg.h>
#include <commdlg.h>

#include "OGL.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"


// -OpenGL Header Files-
#include "dependencies/glew/include/GL/glew.h"			//! Must be before gl.h
#include<gl/GL.h>

#include "vmath.h"
using namespace vmath;

#include "Tables.h"
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "./include/stb_image.h"

// MACROS:
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#define MENU_OPEN_NEW_FILE 861
#define ID_RESET_BUTTON 1001


const float EPSILON = 0.0001f;//for floating point inaccuracy


// Link with OpenGL Library:
#pragma comment(lib,"dependencies/glew/lib/Release/x64/glew32")
#pragma comment(lib,"OpenGL32")
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

GLuint shaderProgramObject = 0;

// CUBE
GLuint VAO_Cube;
GLuint VBO_Position_Cube;
GLuint VBO_TexCoord_Cube;
GLfloat fAngleCube = 0.0f;

GLuint mvpMatrixUniform_vvg = 0;
GLuint textureSamplerUniform_vvg = 0;

glm::mat4 perspectiveProjMatrix_glm;
vmath::mat4 perspectiveProjectionMatrix;

GLuint giWinWidth=0;
GLuint giWinHeight = 0;

enum
{
	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_COLOR,
	AMC_ATTRIBUTE_NORMAL,
	AMC_ATTRIBUTE_TEXCOORD,
};



// function declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void ToggleFullscreen(void);
BOOL LoadPNGImage(GLuint* texture, const char* imageFilePath);
BOOL LoadPNGImage_New(GLuint* texture, const char* imageFilePath);
BOOL LoadGLTexture(GLuint* Texture, TCHAR ImageResourceID[]);




//! VOLUME RENDERING GLOBAL VARIABLES:

int iOption = -1;

GLuint shaderProgramObject_Grid;
GLuint mvpUniform_GridObject;
int gridWidth=5;
int gridDepth=5;
int totalVertices;
int totalIndices;

GLuint vaoID;
GLuint vboID_Vertices;
GLuint vboID_Indices;
glm::vec3 viewDirection;

vmath::vec3 viewDirection_v3;

GLuint shaderProgramObject_Slicer1;
GLuint modelViewProjectionUniform_Slicer1 = 0;
GLuint textureVolumeUniform_Slicer1 = 0;

GLuint VBO_volume;
GLuint VAO_volume;
GLuint textureID = 0;


//unit cube edges
int edgeList[8][12] = {
	{ 0,1,5,6,   4,8,11,9,  3,7,2,10 }, // v0 is front
	{ 0,4,3,11,  1,2,6,7,   5,9,8,10 }, // v1 is 
	{ 1,5,0,8,   2,3,7,4,   6,10,9,11}, // v2 is 
	{ 7,11,10,8, 2,6,1,9,   3,0,4,5  }, // v3 is 
	{ 8,5,9,1,   11,10,7,6, 4,3,0,2  }, // v4 is 
	{ 9,6,10,2,  8,11,4,7,  5,0,1,3  }, // v5 is 
	{ 9,8,5,4,   6,1,2,0,   10,7,11,3}, // v6 is 
	{ 10,9,6,5,  7,2,3,1,   11,4,8,0 }  // v7 is 
};
const int edges[12][2] = { {0,1},{1,2},{2,3},{3,0},{0,4},{1,5},{2,6},{3,7},{4,5},{5,6},{6,7},{7,4} };


float fZPlus_FrontFace = 0.5f;
float fYPlus_TopFace = 0.5f;
float fXPlus_SideFace = 0.5f;

float fXMinus_SideFace = -0.5f;
float fYMinus_BottomFace = -0.5f;
float fZMinus_BackFace = -0.5f;

glm::vec3 vertexList[8] = { 
						   // Minus Z-Vertices
						   glm::vec3(-0.5,-0.5,-0.5), // Left Bottom // 0th
						   glm::vec3(0.5,-0.5,-0.5),  // Right Bottom // 1st
						   glm::vec3(0.5, 0.5,-0.5),  // Right Top
						   glm::vec3(-0.5, 0.5,-0.5), // Left Top

						   // Plus Z-Vertices
						   glm::vec3(-0.5,-0.5, 0.5), // Left Bottom
						   glm::vec3(0.5,-0.5, 0.5),  // Right Bottom
						   glm::vec3(0.5, 0.5, 0.5),  // Right Top
						   glm::vec3(-0.5, 0.5, 0.5)  // Left Top
							};



//* Object 1 Data Info
const int MAX_SLICES = 512;
const int XDIM = 256;
const int YDIM = 256;
const int ZDIM = 256;
const std::string volume_file = "./resources/model/Engine256.raw";


const int XDIM_2 = 256;
const int YDIM_2 = 256;
const int ZDIM_2 = 256;
const std::string volume_file_2 = "./resources/model/skull_256x256x256_uint8.raw";


const int XDIM_3 = 256;
const int YDIM_3 = 256;
const int ZDIM_3 = 256;
const std::string volume_file_3 = "./resources/model/bonsai_256x256x256_uint8.raw";

const int XDIM_4 = 256;
const int YDIM_4 = 256;
const int ZDIM_4 = 256;
const std::string volume_file_4 = "./resources/model/head_256x256x256_uint8.raw";


const int XDIM_5 = 64;
const int YDIM_5 = 64;
const int ZDIM_5 = 64;
const std::string volume_file_5= "./resources/model/neghip_64x64x64_uint8.raw";


const int XDIM_6 = 103;
const int YDIM_6 = 94;
const int ZDIM_6 = 161;
const std::string volume_file_6= "./resources/model/tooth_103x94x161_uint8.raw";



glm::vec3 vTextureSlices[MAX_SLICES * 12];
int num_slices = 256;
float dist = -2.0f;

float rotationX = 0.0f;
float rotationY = 0.0f;
float rotationZ = 0.0f;


BOOL bRotateX = FALSE;
BOOL bRotateY = FALSE;
BOOL bRotateZ = FALSE;
BOOL bSliceUpdate = TRUE;

BOOL bMouseClicked = FALSE;
BOOL bMouseControl = FALSE;

GLfloat oldX = 0.0f;
GLfloat oldY = 0.0f;
GLfloat xMouseValue = 0.0f;
GLfloat yMouseValue = 0.0f;

GLfloat xMouseValue_NDC = 0.0f;
GLfloat yMouseValue_NDC = 0.0f;


//!  VOLUME RENDERING Function Prototypes:
int Initialize_Slicing_shader(void);
void Slice_Volume(void);
int FindAbsMax(glm::vec3 v);
int LoadVolumeData(void);
void Uninitialize_Slicing_shader(void);
int LoadGridObject_Shader(int width, int depth);
int GetTotalVertices(void);
int GetTotalIndices(void);
void FillIndexBuffer(GLuint* pBuffer);
void FillVertexBuffer(GLfloat* pBuffer);
int FindAbsMax(glm::vec3 v);

// Shader Type 2: Ray Casting Method : global variables:

GLuint shaderProgramObject_RayCasting;
GLuint modelViewProjectionUniform_RayCasting = 0;
GLuint textureVolumeUniform_RayCasting = 0;
GLuint cameraPositionUniform=0;
GLuint step_size_Uniform = 0;

GLuint VAO_cube_RayCastingCube;
GLuint VBO_cube_ID_RayCastingCube;
GLuint VBO_cube_Indices_RayCastingCube;

// Shader Type 2: Ray Casting Method : prototypes:
int Initialize_Raycasting_shader(void);
void Initialize_Raycasting_Geomatry(void);
void Render_Raycasting_Output(void);
void Update_Raycasting_Output(void);
void Update_Raycasting_Cube_VBO(void);
void Uninitialize_Raycasting_shader(void);


// Shader Type 3:Pseudo IsoSurface with Ray Casting Method :  global variables:
GLuint shaderProgramObject_IsoSurface;
GLuint modelViewProjectionUniform_IsoSurface = 0;
GLuint textureVolumeUniform_IsoSurface = 0;
GLuint cameraPositionUniform_IsoSurface = 0;
GLuint step_size_Uniform_IsoSurface = 0;


// Shader Type 3:Pseudo IsoSurface with Ray Casting Method : prototypes:
int Initialize_IsoSurface_shader(void);
void Initialize_IsoSurface_Geomatry(void);
void Render_IsoSurface_Output(void);
void Update_IsoSurface_Output(void);
void Uninitialize_IsoSurface_shader(void);


// Shader Type 4: Colomap Classifaction Method : global variables:
GLuint shaderProgramObject_Colormap;
GLuint modelViewProjectionUniform_Colormap = 0;
GLuint textureVolumeUniform_Colormap = 0;
GLuint levelOfDetail_Uniform = 0;
GLuint texture_TransferFunction = 0;

const glm::vec4 jet_values[9] = {
								glm::vec4(0,0,0.5,0),
								glm::vec4(0,0,1,0.1),
								glm::vec4(0,0.5,1,0.3),
								glm::vec4(0,1,1,0.5),
								glm::vec4(0.5,1,0.5,0.75),
								glm::vec4(1,1,0,0.8),
								glm::vec4(1,0.5,0,0.6),
								glm::vec4(1,0,0,0.5),
								glm::vec4(0.5,0,0,0.0)
};

// Shader Type 4: Colomap Classifaction Method : prototypes:
int Initialize_ColormapClassification_shader(void);
void LoadTransferFunction(void);
void Initialize_ColormapClassification_Geomatry(void);
void Render_ColormapClassification_Output(void);
void Update_ColormapClassification_Output(void);
void Uninitialize_ColormapClassification_shader(void);

//! Shader Type 5: Marching Tetrahedra Method : global variables:
struct Vertex
{
	glm::vec3 pos, normal;  //our vertex struct stores the position and normals
};

int XDIM_TM, YDIM_TM, ZDIM_TM;//the volume dataset dimensions and inverse volume dimensions

GLuint shaderProgramObject_TM;
GLuint modelViewProjectionUniform_TM = 0;

glm::vec3 invDim;
int X_SAMPLING_DIST;
int Y_SAMPLING_DIST;
int Z_SAMPLING_DIST;

//volume data pointer
GLubyte* pVolume;
GLubyte isoValue;
//vertices vector storing positions and normals
std::vector<Vertex> vertices;
GLuint volumeMarcherVBO;
GLuint volumeMarcherVAO;


//flag to set wireframe rendering mode
BOOL bWireframe = FALSE;


//! Shader Type 5: Marching Tetrahedra Method : prototypes:
void Initialize_TetrahedraMarcher_Constructor(void);
void Initialize_TetrahedraMarcher_Shaders(void);
void Initialize_TetrahedraMarcher_Geomatry(void);
void SetVolumeDimensions(const int xdim, const int ydim, const int zdim);//function to set the volume dimension
void SetNumSamplingVoxels(const int x, const int y, const int z);//function to set the total number of sampling voxels + more voxels will give a higher density mesh
void SetIsosurfaceValue(const GLubyte value);//set the isosurface value
bool LoadVolume(const std::string& filename);//load the volume dataset
bool Load_New_Volume(const std::string& filename, int X_DIM, int Y_DIM, int Z_DIM);

void MarchVolume();//march the volume dataset
size_t GetTotalVertices_TM();//get the total number of vertices generated
Vertex* GetVertexPointer_TM();//get the pointer to the vertex buffer
GLubyte SampleVolume(const int x, const int y, const int z);
glm::vec3 GetNormal(const int x, const int y, const int z);//get the normal at the given location using center finite difference approximation
void SampleVoxel(const int x, const int y, const int z, glm::vec3 scale);//samples a voxel at the given location and scale
float GetOffset(const GLubyte v1, const GLubyte v2);//returns the offset between the two sample values
void Render_MarchingTetrahedra(void);
void Update_MarchingTetrahedra(void);
void Uninitialize_MarchingTetrahedra(void);


GLuint VAO_Volume_Axes;
GLuint VBO_Volume_Axes;

void Initialize_Volume_Box_Axes(void);
void Update_Volume_Box_Axes(void);
void Render_Volume_Box_Axes(glm::mat4);

// Splatting Method Variables:

//int XDIM, YDIM, ZDIM;
GLubyte* pVolume_Splatting;
GLubyte isoValue_Splatting;
std::vector<Vertex> vertices_Splatting;
glm::vec3 invDim_Splatting;

glm::vec3 scaleMatrix_splat;

// for volume splatter:
GLuint volumeSplatter_VAO;
GLuint volumeSplatter_VBO;

// for FBO and filters:
GLuint filterFBO_ID;
GLuint fbo_ID;
GLuint rbo_ID;

// for textures:
GLuint blurTexID[2];
GLuint texID;

// for Quad geometry:
GLuint quadVAO_ID;
GLuint quadVBO_ID;
GLuint quadVBO_Indices_ID;


// #1 : Splatting Shader Object related variables:
GLuint shaderProgramObject_Splatting;
GLuint mvMatrixUniform_Splatting = 0;
GLuint normalMatrixUniform_Splatting = 0;
GLuint projectionMatrixUniform_Splatting = 0;
GLuint splatSizeUniform = 0;

GLfloat fSplatSize = 256.0f / 64.0f;

GLuint shaderProgramObject_Gaussian_H;
GLuint splatTextureMapSampler = 0;

// for shaderObjects:
GLuint shaderProgramObject_Gaussian_V;
GLuint splatTextureMapSampler_V = 0;

GLuint shaderProgramObject_Splatting_Quad;
GLuint textureSampler_Quad=0;




// Splatting Method Prototypes:
int VolumeSplatterConstructor(void);
void SetVolumeDimensions_Splatting(int, int, int);
bool LoadVolume_Splatting(const std::string);
void SetIsoSurfaceValue(int);
void SetNumSamplingVoxels_Splatting(int, int, int);
void GenerateSplatVolumes(void);
void SampleVoxel_Splatting(unsigned int, unsigned int, unsigned int);
glm::vec3 GetNormal_Splatting(int x, int y, int z);
GLubyte SampleVolume_Splat(const int x, const int y, const int z);


void setup_VolumeSplatter_VAO(void);

int init_Shader_for_SplattingS(void);
int init_Shader_for_Gaussian_H(void);
int init_Shader_for_Gaussian_V(void);
int init_Shader_for_Quad(void);

void setup_Textures(void);
void setup_Filter_and_FBOs(void);

void setup_Quad_VAO(void);


void uninitialize_Splatting(void);



GLuint textureLeft = 0;
GLuint textureRight = 0;

struct HUDControls
{
	float x;
	float y;
	float width;
	float height;

	GLuint textureID;
	
	bool bHovered;
	bool bVisible;
};

HUDControls hUP_Arrow;
HUDControls hDOWN_Arrow;
HUDControls hLEFT_Arrow;
HUDControls hRIGHT_Arrow;


void initi_HUDControls_data(void);


GLuint shaderProgramObject_Texture = 0;
GLuint modelMatUniform_Tex = 0;
GLuint viewMatUniform_Tex = 0;
GLuint projMatUniform_Tex = 0;
GLuint textureSamplerUniform_Tex = 0;

GLuint VAO_ButtonQuad;
GLuint VBO_ButtonQuad_Position;
GLuint VBO_ButtonQuad_Texcoord;


int init_TextureShader(void);
void draw_Quad_with_Texture(GLuint texture_, GLfloat xPos, GLfloat yPos);
void uninit_TextureShader(void);



GLuint testTex = 0;
void LoadDebuGTex(void);



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
	TCHAR szAppName[] = TEXT("Window by Varun Vijay Gajre");
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
		fprintf(gpFile, "Shree Gajanan Prasanna #\n");
		fprintf(gpFile, "Log File Created Successfully.\n");
		fprintf(gpFile, "Program started Successfully.\n");
	}



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


	
	HMENU hMenuBar1 = CreateMenu();

	// Object Types
	HMENU hmenu_ObjType = CreatePopupMenu();
	AppendMenuW(hmenu_ObjType, MF_STRING, 1, L"Option 1");
	AppendMenuW(hmenu_ObjType, MF_STRING, 2, L"Option 2");
	AppendMenuW(hmenu_ObjType, MF_STRING, 3, L"Option 3");
	AppendMenuW(hMenuBar1, MF_POPUP, (UINT_PTR)hmenu_ObjType, L"ObjectType");

	
	// Shader Types
	HMENU hmenu_ShaderType = CreatePopupMenu();
	AppendMenuW(hmenu_ShaderType, MF_STRING, 4, L"Basic Shader");
	AppendMenuW(hmenu_ShaderType, MF_STRING, 5, L"RayCasting");
	AppendMenuW(hmenu_ShaderType, MF_STRING, 6, L"Pseudo IsoSurface with Ray casting");
	AppendMenuW(hmenu_ShaderType, MF_STRING, 7, L"Colormap Classification");
	AppendMenuW(hmenu_ShaderType, MF_STRING, 8, L"Marching Tetrahedra");
	AppendMenuW(hmenu_ShaderType, MF_STRING, 9, L"Half Angle Slicing");
	AppendMenuW(hMenuBar1, MF_POPUP, (UINT_PTR)hmenu_ShaderType, L"ShaderType");

	CheckMenuItem(hmenu_ShaderType, 4, MF_CHECKED | MF_BYCOMMAND);
	iOption = 2;
	
	// 2D View
	HMENU hmenu_2DView = CreatePopupMenu();
	AppendMenuW(hmenu_2DView, MF_STRING, 10, L"2D View");
	AppendMenuW(hmenu_2DView, MF_STRING, 11, L"Slicer View");
	AppendMenuW(hMenuBar1, MF_POPUP, (UINT_PTR)hmenu_2DView, L"2DView");

	
	// Help
	HMENU hmenu_Help = CreatePopupMenu();
	AppendMenuW(hmenu_Help, MF_STRING, 1, L"About");
	AppendMenuW(hMenuBar1, MF_POPUP, (UINT_PTR)hmenu_Help, L"Help");


	// Reset button:
	HWND hResetButton = CreateWindow(
		L"BUTTON",  // Predefined class; Unicode assumed 
		L"RESET",      // Button text 
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		20,                 // x
		20,                 // y
		100,                // width
		35,                 // height
		hwnd,               // parent window
		(HMENU)ID_RESET_BUTTON,
		(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
		NULL
	);



	//SetMenu(hwnd, hMenuBar1);
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

std::wstring OpenRawFileDialog(HWND hwnd_);

std::wstring OpenRawFileDialog(HWND hwnd_)
{
	// local:
	
	wchar_t fileName[MAX_PATH] = L"Engine256.raw";

	// code:

	OPENFILENAME openFileName = {};

	// initialize members of openFileName:
	openFileName.lStructSize = sizeof(openFileName);
	openFileName.hwndOwner = hwnd_;
	openFileName.lpstrFilter = L"RAW Files (*.raw)\0*.raw\0All Files (*.*)\0*.*\0";
	openFileName.lpstrFile = fileName;
	openFileName.nMaxFile = MAX_PATH;
	openFileName.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	openFileName.lpstrDefExt = L"raw";

	// call to GetOepnFileNames:

	BOOL bResult = GetOpenFileNameW(&openFileName);
	if (bResult != FALSE)
	{
		return (std::wstring(fileName));
	}


	return (L"");
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	//function declarations:
	void resize(int, int);
	void uninitialize(void);
	void ToggleFullscreen(void);

	// local:

	//int newX = 0;
	//int newY = 0;

	// code
	switch (iMsg)
	{

	case WM_CREATE:
		break;

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
			case 1:
				{

					std::wstring filePath = OpenRawFileDialog(hwnd);

					if (!filePath.empty())
					{
						char str[255];
						sprintf(str, " Path is not Empty");
						//MessageBox(hwnd, str,TEXT(" TITLE "), MB_ICONINFORMATION | MB_OK);
					}
				}

				break;
			case 2:
				break;
			case 3:
				break;
			case 4:
				iOption = 0;
				for (int i=4;i<=9;i++)
				{
					CheckMenuItem(GetMenu(hwnd), i, MF_UNCHECKED | MF_BYCOMMAND);
				}
				CheckMenuItem(GetMenu(hwnd), 4, MF_CHECKED | MF_BYCOMMAND);
				break;
			case 5:
				iOption = 1;
				for (int i = 4; i <= 9; i++)
				{
					CheckMenuItem(GetMenu(hwnd), i, MF_UNCHECKED | MF_BYCOMMAND);
				}
				CheckMenuItem(GetMenu(hwnd), 5, MF_CHECKED | MF_BYCOMMAND);
				break;
			case 6:
				iOption = 2;
				for (int i = 4; i <= 9; i++)
				{
					CheckMenuItem(GetMenu(hwnd), i, MF_UNCHECKED | MF_BYCOMMAND);
				}
				CheckMenuItem(GetMenu(hwnd), 6, MF_CHECKED | MF_BYCOMMAND);
				break;
			case 7:
				iOption = 3;
				for (int i = 4; i <= 9; i++)
				{
					CheckMenuItem(GetMenu(hwnd), i, MF_UNCHECKED | MF_BYCOMMAND);
				}
				CheckMenuItem(GetMenu(hwnd), 7, MF_CHECKED | MF_BYCOMMAND);
				break;
			case 8:
				iOption = 4;
				for (int i = 4; i <= 9; i++)
				{
					CheckMenuItem(GetMenu(hwnd), i, MF_UNCHECKED | MF_BYCOMMAND);
				}
				CheckMenuItem(GetMenu(hwnd), 8, MF_CHECKED | MF_BYCOMMAND);
				break;
			case 9:
				iOption = 5;
				for (int i = 4; i <= 9; i++)
				{
					CheckMenuItem(GetMenu(hwnd), i, MF_UNCHECKED | MF_BYCOMMAND);
				}
				CheckMenuItem(GetMenu(hwnd), 9, MF_CHECKED | MF_BYCOMMAND);
				break;

			case MENU_OPEN_NEW_FILE:
				{
					//////std::wstring filePath = OpenRawFileDialog(hwnd);

					//////if (!filePath.empty())
					//////{
					//////	char str[255];
					//////	sprintf(str, " Path is not Empty");
					//////	//MessageBox(hwnd, str,TEXT(" TITLE "), MB_ICONINFORMATION | MB_OK);
					//////}

				}
				break;

			case ID_RESET_BUTTON:
				{
					bRotateX = FALSE;
					bRotateY = FALSE;
					bRotateZ = FALSE;
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
					bSliceUpdate = TRUE;
					SetFocus(hwnd);
				}
				break;

			default:
				break;
		}
		break;

	case WM_KEYDOWN:
		bSliceUpdate = TRUE;
		switch (LOWORD(wParam))
		{
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
		case '1':
			iOption = 0;
			break;

		case '3':
			iOption = 1;
			break;
			
		case '5':
			iOption = 2;
			break;
			
		case '7':
			iOption = 3;
			break;
			
		case '0':
			iOption = 4;
			break;

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
				bRotateX = FALSE;
				bRotateY = FALSE;
				bRotateZ = FALSE;
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
			}
			break;
		case 'w':
		case 'W':
			bWireframe = !bWireframe;
			break;

		case 'x':
		case 'X':
			bRotateX = !bRotateX;
			break;

		case 'y':
		case 'Y':
			bRotateY = !bRotateY;
			break;

		case 'z':
			dist -= 0.1f;
			break;
		case 'Z':
			dist += 0.1f;
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
		}
		break;

	case WM_MOUSEWHEEL:
		{
			int iWheelDelta= GET_WHEEL_DELTA_WPARAM(wParam);
			char str[255];
			sprintf(str, "Inside WM_MOUSEHWHEEL with value iWheelDelta=> %d", iWheelDelta);
			//MessageBox(hwnd, str, TEXT("CAPTION"), MB_OK);
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
		bRotateX = FALSE;
		bRotateY = FALSE;
		bRotateZ = FALSE;
		SetCapture(hwnd);
		break;

	case WM_LBUTTONUP:
		bMouseClicked = FALSE;
		ReleaseCapture();
		break;

	case WM_SETFOCUS:
		gbActiveWindow = TRUE;
		break;

	case WM_KILLFOCUS:
		gbActiveWindow = FALSE;
		break;

	case WM_SIZE:
		resize(LOWORD(lParam), HIWORD(lParam));
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

/*
# Use Function : CreateWindowEx() with 1st Parameter ->WS_EX_APPWINDOW
This style is on Top Of TASKBAR.

1]IF NOT FULLSCREEN: Proceed with Following Steps:
	A) Get the current Window style GetWindowStyle():
		IF that WindowStyle "contains" WS_OVERLAPPEDWINDOW.
		a) Get Current Window's Placement i.e Position,Length,Bredth, etc.
		b) Get Monitor's Properties -> Width , Height:
			for this you need MONITOR's HANDLE.
		c) IF both (a) and (b) are Successful/TRUE then follow subsubsteps:

			i) Remove WS_OVERLAPPEDWINDOW from Current Window Style.(we only need WS_OVERLAPPED out of this MACRO.)
			ii) Set the New Fullscreened Window such that it will be Overlapped,
			its ClientArea will Begin from LEFT TOP of the Monitor,its Width/Height will be WIDTH,HEIGHT of Monitor,
			AND it will be at the TOP of the Z-ORDER.
			[What is Z-ORDER = ]

	B) Hide the Cursor, because Conventionally in FullscreenMode SystemCursor is Hidden.
	[Window is Now Fullscreen-set the Flag to Fullscreen.]

2] IF ALREADY in FULLSCREEN MODE: Proceed with Following Steps:

	A) Set the Window Placement i.e Position,Width,Height as per previous state of Fullscreen.
	B) Add/Set WindowStyle again just like WS_OVERLAPPEDWINDOW.
	C) Set the WindowPosition such that it will be UnAffected by Parent's Z-ORDER ,Movement and RePaint,
	i.e make the Window Normal Again.
	D) Start showing CURSOR again.
	[Window is Now back to normals -ReSet the Flag]

*/

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
	pfd.cDepthBits = 32; //FOR DEPTH

	// 2: Get the OS's HDC i.e specialist.
	ghdc = GetDC(ghwnd);
	if (ghdc == NULL)
	{
		// GetDC() Failed.
		fprintf(gpFile, "\n# GetDC() Failed.");
		return (-1);
	}

	// 3: Use HDC Handle to get index of closest PFD from OS.
	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if (iPixelFormatIndex == 0)
	{
		fprintf(gpFile, "\n# ChoosePixelFormat() Failed.");
		return (-2);
	}

	// 4: Set Pixel Format:
	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)
	{
		fprintf(gpFile, "\n# SetPixelFormat() Failed.");
		return (-3);
	}

	// 5: Tell 'wgl' briding library to give OpenGL Compatible Context from this DeviceContext-ghdc.
	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL)
	{
		fprintf(gpFile, "\n# wglCreateContext() Failed.");
		return (-4);
	}

	// 6: Make this Rendering Context of OpenGL 'ghrc' as Current Context.
	if (wglMakeCurrent(ghdc, ghrc) == FALSE)
	{
		fprintf(gpFile, " wglMakeCurrent() Failed.\n");
		return (-5);
	}


	// initialize GLEW
	if (glewInit() != GLEW_OK)
	{
		fprintf(gpFile, " Error: glewInit() Failed.\n");
		return (-6);
	}

	// printGLInfo();

	// VERTEX SHADER SECTION

	const GLchar* vertexShaderSourceCode =
		"#version 460 core" \
		"\n" \
		"in vec4 aPosition;" \
		"in vec2 aTexCoord;" \
		"uniform mat4 uMVPMatrix;" \
		"out vec2 oTexCoord_OUT;" \
		"void main(void)" \
		"{" \
			"gl_Position= uMVPMatrix * aPosition;" \
			"oTexCoord_OUT=aTexCoord;" \
		"}";

	GLuint vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL);
	glCompileShader(vertexShaderObject);

	GLint status = 0;
	GLint infoLogLength = 0;
	GLchar* szInfoLog = NULL;
	glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			//szInfoLog=(GLchar *)malloc(infoLogLength);
			szInfoLog = (GLchar*)malloc(infoLogLength + 1 * sizeof(GLchar));
			if (szInfoLog != NULL)
			{
				glGetShaderInfoLog(vertexShaderObject, infoLogLength, NULL, szInfoLog);
				fprintf(gpFile, "Error: Vertex Shader Compilation ErrorLog: %s \n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
			}
		}
		uninitialize();
	}
	else
	{
		fprintf(gpFile, "Vertex Shader Compilation Successful.   \n");
	}


	// FRAGMENT SHADER SECTION
	const GLchar* fragmentShaderSourceCode =
		"#version 460 core" \
		"\n" \
		"in vec2 oTexCoord_OUT;" \
		"uniform sampler2D uTextureSampler;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
			"FragColor=texture(uTextureSampler,oTexCoord_OUT);" \
		"}";

	GLuint fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);
	glCompileShader(fragmentShaderObject);

	status = 0;
	infoLogLength = 0;
	szInfoLog = NULL;
	glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			//szInfoLog = (char *)malloc(infoLogLength);
			szInfoLog = (char*)malloc(infoLogLength + 1 * sizeof(GLchar));
			if (szInfoLog != NULL)
			{
				glGetShaderInfoLog(fragmentShaderObject, infoLogLength, NULL, szInfoLog);
				fprintf(gpFile, "Error: Fragment Shader Compilation Error Log: %s \n", szInfoLog);
				fflush(gpFile);
				free(szInfoLog);
				szInfoLog = NULL;
			}
		}
		uninitialize();
	}
	else
	{
		fprintf(gpFile, "Fragment Shader Compilation Successful. status: %d \n", status);
	}


	// SHADER ProgramObject SECTION
	shaderProgramObject = glCreateProgram();

	glAttachShader(shaderProgramObject, vertexShaderObject);
	glAttachShader(shaderProgramObject, fragmentShaderObject);

	glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_POSITION, "aPosition");
	glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_TEXCOORD, "aTexCoord");

	glLinkProgram(shaderProgramObject);

	status = 0;
	infoLogLength = 0;
	szInfoLog = NULL;

	glGetProgramiv(shaderProgramObject, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szInfoLog = (char*)malloc(infoLogLength);
			if (szInfoLog != NULL)
			{
				glGetProgramInfoLog(shaderProgramObject, infoLogLength, NULL, szInfoLog);
				fprintf(gpFile, "Error: shaderProgramObject Linking Error Log: %s \n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
			}
		}
		uninitialize();
	}
	else
	{
		fprintf(gpFile, "shaderProgramObject Linking Successful. status: %d \n", status);
	}

	// post link: uniform binding
	mvpMatrixUniform_vvg = glGetUniformLocation(shaderProgramObject, "uMVPMatrix");
	textureSamplerUniform_vvg = glGetUniformLocation(shaderProgramObject, "uTextureSampler");

	const GLfloat cube_positions[] =
	{

		// front
		 1.0f,  1.0f,  1.0f, // top-right of front
		-1.0f,  1.0f,  1.0f, // top-left of front
		-1.0f, -1.0f,  1.0f, // bottom-left of front
		 1.0f, -1.0f,  1.0f, // bottom-right of front

		 // right
		  1.0f,  1.0f, -1.0f, // top-right of right
		  1.0f,  1.0f,  1.0f, // top-left of right
		  1.0f, -1.0f,  1.0f, // bottom-left of right
		  1.0f, -1.0f, -1.0f, // bottom-right of right

		  // back
		   1.0f,  1.0f, -1.0f, // top-right of back
		  -1.0f,  1.0f, -1.0f, // top-left of back
		  -1.0f, -1.0f, -1.0f, // bottom-left of back
		   1.0f, -1.0f, -1.0f, // bottom-right of back

		   // left
		   -1.0f,  1.0f,  1.0f, // top-right of left
		   -1.0f,  1.0f, -1.0f, // top-left of left
		   -1.0f, -1.0f, -1.0f, // bottom-left of left
		   -1.0f, -1.0f,  1.0f, // bottom-right of left

		   // top
			1.0f,  1.0f, -1.0f, // top-right of top
		   -1.0f,  1.0f, -1.0f, // top-left of top
		   -1.0f,  1.0f,  1.0f, // bottom-left of top
			1.0f,  1.0f,  1.0f, // bottom-right of top

			// bottom
			 1.0f, -1.0f,  1.0f, // top-right of bottom
			-1.0f, -1.0f,  1.0f, // top-left of bottom
			-1.0f, -1.0f, -1.0f, // bottom-left of bottom
			 1.0f, -1.0f, -1.0f, // bottom-right of bottom

	};


	GLfloat cubeTexcoords[] =
	{
		// front
		1.0f, 1.0f, // top-right of front
		0.0f, 1.0f, // top-left of front
		0.0f, 0.0f, // bottom-left of front
		1.0f, 0.0f, // bottom-right of front

		// right
		1.0f, 1.0f, // top-right of right
		0.0f, 1.0f, // top-left of right
		0.0f, 0.0f, // bottom-left of right
		1.0f, 0.0f, // bottom-right of right

		// back
		1.0f, 1.0f, // top-right of back
		0.0f, 1.0f, // top-left of back
		0.0f, 0.0f, // bottom-left of back
		1.0f, 0.0f, // bottom-right of back

		// left
		1.0f, 1.0f, // top-right of left
		0.0f, 1.0f, // top-left of left
		0.0f, 0.0f, // bottom-left of left
		1.0f, 0.0f, // bottom-right of left

		// top
		1.0f, 1.0f, // top-right of top
		0.0f, 1.0f, // top-left of top
		0.0f, 0.0f, // bottom-left of top
		1.0f, 0.0f, // bottom-right of top

		// bottom
		1.0f, 1.0f, // top-right of bottom
		0.0f, 1.0f, // top-left of bottom
		0.0f, 0.0f, // bottom-left of bottom
		1.0f, 0.0f, // bottom-right of bottom
	};


	// VAO and VBOs for CUBE
	glGenVertexArrays(1, &VAO_Cube);
	glBindVertexArray(VAO_Cube);
	{
		//VBO POSITION
		glGenBuffers(1, &VBO_Position_Cube);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_Position_Cube);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cube_positions), cube_positions, GL_STATIC_DRAW);
		glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//VBO TEXCOORDS
		glGenBuffers(1, &VBO_TexCoord_Cube);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_TexCoord_Cube);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeTexcoords), cubeTexcoords, GL_STATIC_DRAW);
		glVertexAttribPointer(AMC_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXCOORD);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

	}
	glBindVertexArray(0);


	if (LoadPNGImage(&textureLeft, "./resources/textures/LEFT.png")==FALSE)
	{
		fprintf(gpFile, "ERR: LoadPNGImage() Failed for LEFT.png\n");
		uninitialize();
	}
	else
	{
		fprintf(gpFile, "SUCCESS: LoadPNGImage() DONE for LEFT.png\n");
	}
	
	if (LoadPNGImage(&textureRight, "./resources/textures/RIGHT.png")==FALSE)
	{
		fprintf(gpFile, "ERR: LoadPNGImage() Failed for RIGHT.png\n");
		uninitialize();
	}
	else
	{
		fprintf(gpFile, "SUCCESS: LoadPNGImage() DONE for RIGHT.png\n");
	}


	initi_HUDControls_data();

	fprintf(gpFile,"SUCCESS: After initi_HUDControls_data().....\n");

	init_TextureShader();
	fprintf(gpFile,"SUCCESS: After init_TextureShader().....\n");


	LoadDebuGTex();



	LoadVolumeData();
	//Load_New_Volume(volume_file_2,XDIM_2,YDIM_2,ZDIM_2);
	//Load_New_Volume(volume_file_3,XDIM_3,YDIM_3,ZDIM_3);
	//Load_New_Volume(volume_file_4,XDIM_4,YDIM_4,ZDIM_4);
	//Load_New_Volume(volume_file_5,XDIM_5,YDIM_5,ZDIM_5);
	//Load_New_Volume(volume_file_6,XDIM_6,YDIM_6,ZDIM_6);


	Initialize_Slicing_shader();


	glGenVertexArrays(1, &VAO_Volume_Axes);
	glBindVertexArray(VAO_Volume_Axes);
	{
		glm::vec3 vertexList[24] = {
			//// Minus Z-Vertices
			//glm::vec3(-0.5f,-0.5f,-0.5f), // 1. Left Bottom
			//glm::vec3(fXPlus_SideFace,-0.5f,-0.5f),  // 2. Right Bottom
			//glm::vec3(fXPlus_SideFace, fYPlus_TopFace,-0.5f),  // 3. Right Top
			//glm::vec3(-0.5f, fYPlus_TopFace,-0.5f), // 4. Left Top

			// FRONT FACE: All Plus Z-Vertices
			glm::vec3(-0.5f,-0.5f, fZPlus_FrontFace), // 5. Left Bottom
			glm::vec3(fXPlus_SideFace,-0.5f, fZPlus_FrontFace),  // 6. Right Bottom
			glm::vec3(fXPlus_SideFace, fYPlus_TopFace, fZPlus_FrontFace),  // 7. Right Top
			glm::vec3(-0.5f, fYPlus_TopFace, fZPlus_FrontFace),  // 8. Left Top

			// RIGHT SIDE FACE : All Plus X-Vertices:
			glm::vec3(fXPlus_SideFace, 0.5f, 0.5f),
			glm::vec3(fXPlus_SideFace, 0.5f,-0.5f),
			glm::vec3(fXPlus_SideFace,-0.5f,-0.5f),
			glm::vec3(fXPlus_SideFace,-0.5f, 0.5f),

			// TOP FACE : All Plus Y-Vertices:
			glm::vec3(-0.5f, fYPlus_TopFace,-0.5f),
			glm::vec3(0.5f, fYPlus_TopFace,-0.5f),
			glm::vec3(0.5f, fYPlus_TopFace, 0.5f),
			glm::vec3(-0.5f, fYPlus_TopFace, 0.5f),

			// BOTTOM FACE : All Minus  Y-Vertices:
			glm::vec3(-0.5f, fYMinus_BottomFace,-0.5f),
			glm::vec3(0.5f, fYMinus_BottomFace,-0.5f),
			glm::vec3(0.5f, fYMinus_BottomFace, 0.5f),
			glm::vec3(-0.5f, fYMinus_BottomFace, 0.5f),

			// BACK FACE: All Minus Z-Vertices
			glm::vec3(-0.5f,-0.5f, fZMinus_BackFace),
			glm::vec3(0.5f,-0.5f, fZMinus_BackFace),
			glm::vec3(0.5f,  0.5f, fZMinus_BackFace),
			glm::vec3(-0.5f,  0.5f, fZMinus_BackFace),

			// LEFT SIDE FACE: All Minus X-Vertices
			glm::vec3(fXMinus_SideFace, 0.5f, 0.5f),
			glm::vec3(fXMinus_SideFace, 0.5f,-0.5f),
			glm::vec3(fXMinus_SideFace,-0.5f,-0.5f),
			glm::vec3(fXMinus_SideFace,-0.5f, 0.5f),

		};

		glGenBuffers(1, &VBO_Volume_Axes);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_Volume_Axes);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertexList), NULL, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
		glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	glBindVertexArray(0);

	Update_Volume_Box_Axes();

	//Slice_Volume();

	LoadGridObject_Shader(5,5);


	Initialize_Raycasting_shader();
	Initialize_Raycasting_Geomatry();

	Initialize_IsoSurface_shader();

	Initialize_ColormapClassification_shader();
	LoadTransferFunction();


	//Initialize_TetrahedraMarcher_Constructor();
	//SetVolumeDimensions(256, 256, 256);
	//if (LoadVolume(volume_file))
	//{
	//	fprintf(gpFile, "------- LoadVolume() Successful.------- \n");
	//}
	//else
	//{
	//	fprintf(gpFile, "------- LoadVolume() Failed.------- \n");
	//}
	////set the isosurface value
	//SetIsosurfaceValue(48);
	////set the number of sampling voxels 
	//SetNumSamplingVoxels(128, 128, 128);
	////begin tetrahedra marching
	//MarchVolume();
	//Initialize_TetrahedraMarcher_Geomatry();
	//Initialize_TetrahedraMarcher_Shaders();


	/************** SPLATTING METHOD INITIALIZE ****************/

	//VolumeSplatterConstructor();
	//SetVolumeDimensions_Splatting(256, 256, 256);
	////LoadVolume_Splatting("");
	//SetIsoSurfaceValue(40);
	//SetNumSamplingVoxels_Splatting(64, 64, 64);

	//GenerateSplatVolumes();

	//setup_VolumeSplatter_VAO();

	//init_Shader_for_SplattingS();
	//init_Shader_for_Gaussian_H();
	//init_Shader_for_Gaussian_V();
	//init_Shader_for_Quad();


	//int VolumeSplatterConstructor();
	//void SetVolumeDimensions(int, int, int);
	//void LoadVolume_Splatting(const std::string);
	//void SetIsoSurfaceValue(int);
	//void SetNumSamplingVoxels(int, int, int);
	//void GenerateSplatVolumes(void);

	//void setup_VolumeSplatter_VAO(void);

	//int init_Shader_for_SplattingS(void);
	//int init_Shader_for_Gaussian_H(void);
	//int init_Shader_for_Gaussian_V(void);
	//int init_Shader_for_Quad(void);

	//void setup_Textures(void);
	//void setup_Filter_and_FBOs(void);

	//void setup_Quad_VAO(void);





	// Set the background color to BLUE.
	glClearColor(0.75f, 0.75f, 0.75f, 0.0f);

	// To Enable Depth: 3 lines 
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_3D); //OPTIONAL

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	// No WarmUpResize in this version
	perspectiveProjectionMatrix = vmath::mat4::identity();
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


	
	perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height,0.1f,1000.0f);

	perspectiveProjMatrix_glm = glm::perspective(glm::radians(60.0f), (GLfloat)width / (GLfloat)height, 0.1f, 1000.0f);


}

void display(void)
{
	// prototype:
	void Render_Basic_Volume(void);
	void Update_Basic_Volume(void);

	// code:

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



	switch (iOption)
	{
		case 0:
			glClearColor(0.75f, 0.75f, 0.75f, 0.0f);
			Render_Basic_Volume();
			Update_Basic_Volume();
			if (bRotateX || bRotateY || bRotateZ || bMouseClicked || bSliceUpdate)
			{
				Slice_Volume();
				Update_Volume_Box_Axes();
			}
			break;
		case 1:
			glClearColor(0.75f, 0.75f, 0.75f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			Render_Raycasting_Output();
			Update_Raycasting_Output();
			if (bRotateX || bRotateY || bRotateZ || bMouseClicked || bSliceUpdate)
			{
				Update_Volume_Box_Axes();
				Update_Raycasting_Cube_VBO();
			}
			break;

		case 2:
			glClearColor(0.75f, 0.75f, 0.75f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			Render_IsoSurface_Output();
			Update_IsoSurface_Output();
			if (bRotateX || bRotateY || bRotateZ || bMouseClicked || bSliceUpdate)
			{
				Update_Volume_Box_Axes();
				Update_Raycasting_Cube_VBO();
			}

			break;
		case 3:
			glClearColor(0.15f, 0.15f, 0.15f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			Render_ColormapClassification_Output();
			Update_ColormapClassification_Output();
			if (bRotateX || bRotateY || bRotateZ || bMouseClicked || bSliceUpdate)
			{
				Slice_Volume();
				Update_Volume_Box_Axes();
			}
			break;
		case 4:
			glClearColor(0.75f, 0.75f, 0.75f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			Render_MarchingTetrahedra();
			Update_MarchingTetrahedra();
			if (bRotateX || bRotateY || bRotateZ || bMouseClicked || bSliceUpdate)
			{
				Update_Volume_Box_Axes();
				Update_Raycasting_Cube_VBO();
				//Update_MarchingTetrahedra_Cube();
			}
			break;
		case 5:
			glClearColor(0.2f, 0.0f, 0.2f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			break;

		default:
			break;
	}




	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, giWinWidth, giWinHeight);

	glDisable(GL_DEPTH_TEST);
	draw_Quad_with_Texture(textureLeft,-0.25, -1.5f);
	draw_Quad_with_Texture(textureRight, 0.25f, -1.5f);
	glEnable(GL_DEPTH_TEST);

	// Debug Textures

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
	//SetWindowText(ghwnd, (LPCSTR)str);
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
	void Uninitialize_ShaderProgramObject(GLuint);
	void ToggleFullscreen(void);

	// code:

	Uninitialize_ShaderProgramObject(shaderProgramObject_Splatting);

	Uninitialize_MarchingTetrahedra();
	Uninitialize_ColormapClassification_shader();
	Uninitialize_IsoSurface_shader();
	Uninitialize_Raycasting_shader();
	Uninitialize_Slicing_shader();

	if (shaderProgramObject)
	{
		glUseProgram(shaderProgramObject);
		GLint numShaders = 0;
		glGetProgramiv(shaderProgramObject, GL_ATTACHED_SHADERS, &numShaders);
		if (numShaders > 0)
		{
			GLuint* pShaders = (GLuint*)malloc(numShaders * sizeof(GLuint));
			if (pShaders != NULL)
			{
				glGetAttachedShaders(shaderProgramObject, numShaders, &numShaders, pShaders);
				for (GLint i = 0; i < numShaders; i++)
				{
					glDetachShader(shaderProgramObject, pShaders[i]);
					glDeleteShader(pShaders[i]);
					pShaders[i] = 0;
				}
				free(pShaders);
			}
		}
		glUseProgram(0);
		glDeleteProgram(shaderProgramObject);
		shaderProgramObject = 0;
	}


	// DELETE CUBE
	if (VBO_TexCoord_Cube)
	{
		glDeleteBuffers(1, &VBO_TexCoord_Cube);
		VBO_TexCoord_Cube = 0;
	}
	
	if (VBO_Position_Cube)
	{
		glDeleteBuffers(1, &VBO_Position_Cube);
		VBO_Position_Cube = 0;
	}

	if (VAO_Cube)
	{
		glDeleteVertexArrays(1, &VAO_Cube);
		VAO_Cube = 0;
	}

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


void Slice_Volume(void)
{
	// local:


	glm::vec3 vertexList[8] = {

		// Minus Z-Vertices
		glm::vec3(fXMinus_SideFace,fYMinus_BottomFace,fZMinus_BackFace),			// 1. Left Bottom
		glm::vec3(fXPlus_SideFace,fYMinus_BottomFace,fZMinus_BackFace),				// 2. Right Bottom
		glm::vec3(fXPlus_SideFace, fYPlus_TopFace,fZMinus_BackFace),				// 3. Right Top
		glm::vec3(fXMinus_SideFace, fYPlus_TopFace,fZMinus_BackFace),				// 4. Left Top

		// Plus Z-Vertices
		glm::vec3(fXMinus_SideFace,fYMinus_BottomFace, fZPlus_FrontFace),			// 5. Left Bottom
		glm::vec3(fXPlus_SideFace,fYMinus_BottomFace, fZPlus_FrontFace),			// 6. Right Bottom
		glm::vec3(fXPlus_SideFace, fYPlus_TopFace, fZPlus_FrontFace),				// 7. Right Top
		glm::vec3(fXMinus_SideFace, fYPlus_TopFace, fZPlus_FrontFace)				// 8. Left Top
	};

	// get the max and min distance of each vertex of the unit cube in the viewing direction
	float maxDistance = glm::dot(viewDirection, vertexList[0]);

	float minDistance = maxDistance;
	int maxIndex = 0;
	int count = 0;

	// code:

	// # 1 : get the distance between the current unit cube vertex and the view vector by dot product

	for (int i = 1; i < 8; i++)
	{
		// 1:
		float fDistance = glm::dot(viewDirection, vertexList[i]);

		// 2:
		if (fDistance > maxDistance)
		{
			maxDistance = fDistance;
			maxIndex = i;
		}

		// 3 : 
		if (fDistance < minDistance)
			minDistance = fDistance;

	}
	// 4 : //find tha abs maximum of the view direction vector
	int max_dim = FindAbsMax(viewDirection);

	minDistance -= EPSILON;
	maxDistance  += EPSILON;

	//local variables to store the start, direction vectors, 
	//lambda intersection values
	glm::vec3 vecStart[12];
	glm::vec3 vecDir[12];
	float lambda[12];
	float lambda_inc[12];
	float denom = 0;

	//set the minimum distance as the plane_dist
	//subtract the max and min distances and divide by the 
	//total number of slices to get the plane increment
	float plane_dist = minDistance;
	float plane_dist_inc = (maxDistance - minDistance) / float(num_slices);

	//for all edges
	for (int i = 0; i < 12; i++) 
	{
		//get the start position vertex by table lookup
		vecStart[i] = vertexList[edges[edgeList[maxIndex][i]][0]];

		//get the direction by table lookup
		vecDir[i] = vertexList[edges[edgeList[maxIndex][i]][1]] - vecStart[i];

		//do a dot of vecDir with the view direction vector
		denom = glm::dot(vecDir[i], viewDirection);

		//determine the plane intersection parameter (lambda) and 
		//plane intersection parameter increment (lambda_inc)
		if (1.0 + denom != 1.0) {
			lambda_inc[i] = plane_dist_inc / denom;
			lambda[i] = (plane_dist - glm::dot(vecStart[i], viewDirection)) / denom;
		}
		else {
			lambda[i] = -1.0;
			lambda_inc[i] = 0.0;
		}
	}


	// local variables to store the intesected points
	//note that for a plane and sub intersection, we can have 
	//a minimum of 3 and a maximum of 6 vertex polygon
	glm::vec3 intersection[6];

	float dL[12];

	//loop through all slices
	for (int i = num_slices - 1; i >= 0; i--) {

		//determine the lambda value for all edges
		for (int e = 0; e < 12; e++)
		{
			dL[e] = lambda[e] + i * lambda_inc[e];
		}

		//if the values are between 0-1, we have an intersection at the current edge
		//repeat the same for all 12 edges
		if ((dL[0] >= 0.0) && (dL[0] < 1.0)) {
			intersection[0] = vecStart[0] + dL[0] * vecDir[0];
		}
		else if ((dL[1] >= 0.0) && (dL[1] < 1.0)) {
			intersection[0] = vecStart[1] + dL[1] * vecDir[1];
		}
		else if ((dL[3] >= 0.0) && (dL[3] < 1.0)) {
			intersection[0] = vecStart[3] + dL[3] * vecDir[3];
		}
		else continue;

		if ((dL[2] >= 0.0) && (dL[2] < 1.0)) {
			intersection[1] = vecStart[2] + dL[2] * vecDir[2];
		}
		else if ((dL[0] >= 0.0) && (dL[0] < 1.0)) {
			intersection[1] = vecStart[0] + dL[0] * vecDir[0];
		}
		else if ((dL[1] >= 0.0) && (dL[1] < 1.0)) {
			intersection[1] = vecStart[1] + dL[1] * vecDir[1];
		}
		else {
			intersection[1] = vecStart[3] + dL[3] * vecDir[3];
		}

		if ((dL[4] >= 0.0) && (dL[4] < 1.0)) {
			intersection[2] = vecStart[4] + dL[4] * vecDir[4];
		}
		else if ((dL[5] >= 0.0) && (dL[5] < 1.0)) {
			intersection[2] = vecStart[5] + dL[5] * vecDir[5];
		}
		else {
			intersection[2] = vecStart[7] + dL[7] * vecDir[7];
		}
		if ((dL[6] >= 0.0) && (dL[6] < 1.0)) {
			intersection[3] = vecStart[6] + dL[6] * vecDir[6];
		}
		else if ((dL[4] >= 0.0) && (dL[4] < 1.0)) {
			intersection[3] = vecStart[4] + dL[4] * vecDir[4];
		}
		else if ((dL[5] >= 0.0) && (dL[5] < 1.0)) {
			intersection[3] = vecStart[5] + dL[5] * vecDir[5];
		}
		else {
			intersection[3] = vecStart[7] + dL[7] * vecDir[7];
		}
		if ((dL[8] >= 0.0) && (dL[8] < 1.0)) {
			intersection[4] = vecStart[8] + dL[8] * vecDir[8];
		}
		else if ((dL[9] >= 0.0) && (dL[9] < 1.0)) {
			intersection[4] = vecStart[9] + dL[9] * vecDir[9];
		}
		else {
			intersection[4] = vecStart[11] + dL[11] * vecDir[11];
		}

		if ((dL[10] >= 0.0) && (dL[10] < 1.0)) {
			intersection[5] = vecStart[10] + dL[10] * vecDir[10];
		}
		else if ((dL[8] >= 0.0) && (dL[8] < 1.0)) {
			intersection[5] = vecStart[8] + dL[8] * vecDir[8];
		}
		else if ((dL[9] >= 0.0) && (dL[9] < 1.0)) {
			intersection[5] = vecStart[9] + dL[9] * vecDir[9];
		}
		else {
			intersection[5] = vecStart[11] + dL[11] * vecDir[11];
		}

		//after all 6 possible intersection vertices are obtained,
		//we calculated the proper polygon indices by using indices of a triangular fan
		int indices[] = { 0,1,2, 0,2,3, 0,3,4, 0,4,5 };

		//! //Using the indices, pass the intersection vertices to the vTextureSlices vector
		for (int i = 0; i < 12; i++)
			vTextureSlices[count++] = intersection[indices[i]];
	}

	//update volume VBO with the new vertices
	glBindBuffer(GL_ARRAY_BUFFER, VBO_volume);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vTextureSlices), &(vTextureSlices[0].x));
	glBindBuffer(GL_ARRAY_BUFFER, 0);


}

void Update_Volume_Box_Axes(void)
{
	// code:

	glm::vec3 vertexList[24] = {

		// FRONT FACE: All Plus Z-Vertices
		glm::vec3(-0.5f,fYMinus_BottomFace, fZPlus_FrontFace),				// 5. Left Bottom
		glm::vec3(fXPlus_SideFace,fYMinus_BottomFace, fZPlus_FrontFace),	// 6. Right Bottom
		glm::vec3(fXPlus_SideFace, fYPlus_TopFace, fZPlus_FrontFace),		// 7. Right Top
		glm::vec3(-0.5f, fYPlus_TopFace, fZPlus_FrontFace),					// 8. Left Top

		// RIGHT FACE : All Plus X-Vertices:
		glm::vec3(fXPlus_SideFace, 0.5f, 0.5f),
		glm::vec3(fXPlus_SideFace, 0.5f,-0.5f),
		glm::vec3(fXPlus_SideFace,fYMinus_BottomFace,-0.5f),
		glm::vec3(fXPlus_SideFace,fYMinus_BottomFace, 0.5f),

		// TOP FACE: All Plus Y-Vertices
		glm::vec3(-0.5f, fYPlus_TopFace,-0.5f),
		glm::vec3(0.5f, fYPlus_TopFace,-0.5f),
		glm::vec3(0.5f, fYPlus_TopFace, 0.5f),
		glm::vec3(-0.5f, fYPlus_TopFace, 0.5f),

		// BOTTOM FACE: All Minus Y-Vertices
		glm::vec3(-0.5f, fYMinus_BottomFace,-0.5f),
		glm::vec3(0.5f, fYMinus_BottomFace,-0.5f),
		glm::vec3(0.5f, fYMinus_BottomFace, 0.5f),
		glm::vec3(-0.5f, fYMinus_BottomFace, 0.5f),

		// LEFT FACE : All Minus X-Vertices:
		glm::vec3(fXMinus_SideFace, 0.5f, 0.5f),
		glm::vec3(fXMinus_SideFace, 0.5f,-0.5f),
		glm::vec3(fXMinus_SideFace,fYMinus_BottomFace,-0.5f),
		glm::vec3(fXMinus_SideFace,fYMinus_BottomFace, 0.5f),

		// BACK FACE: All Minus Z-Vertices
		glm::vec3(-0.5f,fYMinus_BottomFace, fZMinus_BackFace),				// 5. Left Bottom
		glm::vec3(fXPlus_SideFace,fYMinus_BottomFace, fZMinus_BackFace),	// 6. Right Bottom
		glm::vec3(fXPlus_SideFace, fYPlus_TopFace, fZMinus_BackFace),		// 7. Right Top
		glm::vec3(-0.5f, fYPlus_TopFace, fZMinus_BackFace),					// 8. Left Top

	};


	glBindBuffer(GL_ARRAY_BUFFER, VBO_Volume_Axes);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexList), &(vertexList[0].x), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void Render_Volume_Box_Axes(glm::mat4 MVPMatrix_)
{
	// code:

	glUseProgram(shaderProgramObject_Grid);
	{
		glUniformMatrix4fv(mvpUniform_GridObject, 1, GL_FALSE, glm::value_ptr(MVPMatrix_));
		glUniform4f(glGetUniformLocation(shaderProgramObject_Grid, "u_Color"), 1.0f, 0.0f, 0.0f, 1.0f);
		glBindVertexArray(VAO_Volume_Axes);
		glDrawArrays(GL_LINE_LOOP, 0, 4);
		glBindVertexArray(0);

		glUniformMatrix4fv(mvpUniform_GridObject, 1, GL_FALSE, glm::value_ptr(MVPMatrix_));
		glUniform4f(glGetUniformLocation(shaderProgramObject_Grid, "u_Color"), 0.0f, 1.0f, 0.0f, 1.0f);
		glBindVertexArray(VAO_Volume_Axes);
		glDrawArrays(GL_LINE_LOOP, 4, 4);
		glBindVertexArray(0);

		glUniformMatrix4fv(mvpUniform_GridObject, 1, GL_FALSE, glm::value_ptr(MVPMatrix_));
		glUniform4f(glGetUniformLocation(shaderProgramObject_Grid, "u_Color"), 0.0f, 0.0f, 1.0f, 1.0f);
		glBindVertexArray(VAO_Volume_Axes);
		glDrawArrays(GL_LINE_LOOP, 8, 4);
		glBindVertexArray(0);

		glUniformMatrix4fv(mvpUniform_GridObject, 1, GL_FALSE, glm::value_ptr(MVPMatrix_));
		glUniform4f(glGetUniformLocation(shaderProgramObject_Grid, "u_Color"), 1.0f, 1.0f, 0.0f, 1.0f);
		glBindVertexArray(VAO_Volume_Axes);
		glDrawArrays(GL_LINE_LOOP, 12, 4);
		glBindVertexArray(0);

		glUniformMatrix4fv(mvpUniform_GridObject, 1, GL_FALSE, glm::value_ptr(MVPMatrix_));
		glUniform4f(glGetUniformLocation(shaderProgramObject_Grid, "u_Color"), 0.0f, 1.0f, 1.0f, 1.0f);
		glBindVertexArray(VAO_Volume_Axes);
		glDrawArrays(GL_LINE_LOOP, 16, 4);
		glBindVertexArray(0);

		glUniformMatrix4fv(mvpUniform_GridObject, 1, GL_FALSE, glm::value_ptr(MVPMatrix_));
		glUniform4f(glGetUniformLocation(shaderProgramObject_Grid, "u_Color"), 1.0f, 0.0f, 1.0f, 1.0f);
		glBindVertexArray(VAO_Volume_Axes);
		glDrawArrays(GL_LINE_LOOP, 20, 4);
		glBindVertexArray(0);
	}
	glUseProgram(0);


}

//function to get the max (abs) dimension of the given vertex v
int FindAbsMax(glm::vec3 v) 
{
	// code:
	
	v = glm::abs(v);
	int max_dim = 0;

	float val = v.x;
	if (v.y > val) {
		val = v.y;
		max_dim = 1;
	}
	if (v.z > val) {
		val = v.z;
		max_dim = 2;
	}

	return max_dim;
}

int Initialize_Slicing_shader(void)
{
	// prototype:
	void uninitialize(void);

	// local:

	GLuint vertexShaderObject;
	GLuint fragmentShaderObject;

	GLint status;
	GLint infoLogLength;
	char *Log = NULL;

	// code:

	//* ///////////////////// # VERTEX SHADER # ////////////////////////
	const GLchar *vertexShaderSource = R"(
			
			#version 460 core
			layout (location = 0) in vec3 aPosition;
			uniform mat4 u_MVPMatrix;
			smooth out vec3 oTexCoords; //3D texture coordinates for texture lookup in the fragment shader
			void main()
			{
				gl_Position = u_MVPMatrix * vec4(aPosition.xyz, 1.0);
				oTexCoords = aPosition + vec3(0.5);

				//get the 3D texture coordinates by adding (0.5,0.5,0.5) to the object space 
				//vertex position. Since the unit cube is at origin (min: (-0.5,-0.5,-0.5) and max: (0.5,0.5,0.5))
				//adding (0.5,0.5,0.5) to the unit cube object space position gives us values from (0,0,0) to 
				//(1,1,1)
			}
			)";

	vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderObject, 1, (const GLchar **)&vertexShaderSource, NULL);

	glCompileShader(vertexShaderObject);
	glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			Log = (char *)malloc(infoLogLength);
			if (Log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(vertexShaderObject, infoLogLength, &written, Log);
				fprintf(gpFile, "Slicer1 Vertex Shader Compilation Log : %s\n", Log);
				free(Log);
				Log = NULL;
				uninitialize();
			}
		}
	}
	else
	{
		fprintf(gpFile, "Slicer1  Success at  Vertex Shader Compilation \n");
	}



	//* ///////////////////// # FRAGMENT SHADER # ////////////////////////
	const GLchar *fragmentShaderSource = R"(
			
			#version 460 core
			smooth in vec3 oTexCoords; //3D texture coordinates form vertex shader, interpolated by rasterizer
			uniform sampler3D u_Volume3DSampler; //volume dataset

			out vec4 FragColor;		
			void main(void)
			{             
				FragColor = texture(u_Volume3DSampler, oTexCoords).rrrr;
			}

			//Here we sample the volume dataset using the 3D texture coordinates from the vertex shader.
			//Note that since at the time of texture creation, we gave the internal format as GL_RED
			//we can get the sample value from the texture using the red channel. Here, we set all 4
			//components as the sample value in the texture which gives us a shader of grey.
	
			)";
	fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderObject, 1, (const GLchar **)&fragmentShaderSource, NULL);

	glCompileShader(fragmentShaderObject);

	status = 0;
	infoLogLength=0;
	Log=NULL;
	glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			Log = (char *)malloc(infoLogLength);
			if (Log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(fragmentShaderObject, infoLogLength, &written, Log);
				fprintf(gpFile, "Slicer1  FRAGMENT Shader Compilation Log : %s\n", Log);
				free(Log);
				Log = NULL;
				uninitialize();
			}
		}
	}
	else
	{
		fprintf(gpFile, "Slicer1 Success at NEW FRAGMENT Shader Compilation \n");
	}

	shaderProgramObject_Slicer1 = glCreateProgram();


	glAttachShader(shaderProgramObject_Slicer1, vertexShaderObject);
	glAttachShader(shaderProgramObject_Slicer1, fragmentShaderObject);

	// MOVED BELOW

	status = 0;
	infoLogLength=0;
	Log=NULL;
	glLinkProgram(shaderProgramObject_Slicer1);
	glGetProgramiv(shaderProgramObject_Slicer1, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObject_Slicer1, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			Log = (char*)malloc(infoLogLength);
			if (Log != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(shaderProgramObject_Slicer1, infoLogLength, &written, Log);
				fprintf(gpFile, "Slicer1  SHADEROBJECT  Linking Log : %s\n", Log);
				free(Log);
				uninitialize();
				return FALSE;
			}
		}
	}
	else
	{
		fprintf(gpFile, "Slicer1  shaderProgramObject_Slicer1 Linking Successful \n");
	}

	glUseProgram(shaderProgramObject_Slicer1);
	
	glBindAttribLocation(shaderProgramObject_Slicer1, AMC_ATTRIBUTE_POSITION, "aPosition");
	modelViewProjectionUniform_Slicer1 = glGetUniformLocation(shaderProgramObject_Slicer1, "u_MVPMatrix");
	textureVolumeUniform_Slicer1= glGetUniformLocation(shaderProgramObject_Slicer1, "u_Volume3DSampler");
	glUniform1i(textureVolumeUniform_Slicer1, 0);
	glUseProgram(0);


	return (0);
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

	vmath::mat4 T_mat = vmath::translate(0.0f,0.0f,dist);
	vmath::mat4 MV_mat = T_mat;

	viewDirection_v3 = -(vmath::vec3(MV_mat[0][2],MV_mat[1][2],MV_mat[2][2]));


	fprintf(gpFile, "LoadVolumeData() Success Step2 and Last\n"); fflush(gpFile);


	return 0;
}

int LoadGridObject_Shader(int width, int depth)
{
	// prototype:
	void uninitialize(void);

	// local:

	GLuint vertexShaderObject;
	GLuint fragmentShaderObject;

	GLint status;
	GLint infoLogLength;
	char *Log = NULL;

	// code:

	///* //////////////////// # VERTEX SHADER # ////////////////////////
	const GLchar *vertexShaderSource = R"(
			
			#version 460 core
			
			layout (location = 0) in vec3 aPosition;
			uniform mat4 u_MVPMatrix;
			void main()
			{
				gl_Position = u_MVPMatrix * vec4(aPosition.xyz, 1.0);
			}
	
			)";

	vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderObject, 1, (const GLchar **)&vertexShaderSource, NULL);

	glCompileShader(vertexShaderObject);
	glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			Log = (char *)malloc(infoLogLength);
			if (Log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(vertexShaderObject, infoLogLength, &written, Log);
				fprintf(gpFile, " GridObject Vertex Shader Compilation Log : %s\n", Log);

				free(Log);
				Log = NULL;
				uninitialize();
				return -1;
			}
		}
	}
	else
	{
		fprintf(gpFile, "GridObject Success at  Vertex Shader Compilation \n");
	}



	//* ///////////////////// # FRAGMENT SHADER # ////////////////////////
	const GLchar *fragmentShaderSource = R"(
			
			#version 460 core

			uniform vec4 u_Color;
			out vec4 FragColor;		
			void main(void)
			{             
				FragColor = u_Color ;
			}

			)";
	fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderObject, 1, (const GLchar **)&fragmentShaderSource, NULL);

	glCompileShader(fragmentShaderObject);
	glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			Log = (char *)malloc(infoLogLength);
			if (Log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(fragmentShaderObject, infoLogLength, &written, Log);
				fprintf(gpFile, "GridObject  FRAGMENT Shader Compilation Log : %s\n", Log);
				free(Log);
				Log = NULL;
				uninitialize();
				return -1;
			}
		}
	}
	else
	{
		fprintf(gpFile, "GridObject Success at  FRAGMENT Shader Compilation \n");
	}

	shaderProgramObject_Grid = glCreateProgram();


	glAttachShader(shaderProgramObject_Grid, vertexShaderObject);
	glAttachShader(shaderProgramObject_Grid, fragmentShaderObject);


	glBindAttribLocation(shaderProgramObject_Grid, AMC_ATTRIBUTE_POSITION, "aPosition");

	infoLogLength=0;
	Log = NULL;
	glLinkProgram(shaderProgramObject_Grid);
	glGetProgramiv(shaderProgramObject_Grid, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObject_Grid, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			Log = (char*)malloc(infoLogLength);
			if (Log != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(shaderProgramObject_Grid, infoLogLength, &written, Log);
				fprintf(gpFile, "GridObject  Shader Compilation Log : %s\n", Log);
				free(Log);
				uninitialize();
				return -1;
			}
		}
	}
	else
	{
		fprintf(gpFile, "GridObject  Linking Successful \n");
	}

	mvpUniform_GridObject = glGetUniformLocation(shaderProgramObject_Grid, "u_MVPMatrix");



	if (vaoID)
	{
		fprintf(gpFile, "******* vaoID is NON - NULL *******  \n");
	}

	totalVertices = GetTotalVertices();
	totalIndices = GetTotalIndices();

	glGenVertexArrays(1, &vaoID);
	glBindVertexArray(vaoID);
	{
		glGenBuffers(1, &vboID_Vertices);
		glBindBuffer(GL_ARRAY_BUFFER, vboID_Vertices);
		{
			glBufferData(GL_ARRAY_BUFFER, totalVertices * sizeof(vmath::vec3), 0, GL_STATIC_DRAW);
			GLfloat* pBuffer = static_cast<GLfloat*>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
			FillVertexBuffer(pBuffer);
			glUnmapBuffer(GL_ARRAY_BUFFER);
			glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
			glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0);
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &vboID_Indices);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboID_Indices);
		{
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, totalIndices * sizeof(GLuint), 0, GL_STATIC_DRAW);
			GLuint* pIBuffer = static_cast<GLuint*>(glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY));
			FillIndexBuffer(pIBuffer);
			glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
		}
		//!!!!!! FIND RCA : Why uncommenting this causing CRASH //! glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	glBindVertexArray(0);

	if (vaoID)
	{
		fprintf(gpFile, "******* vaoID is NON - NULL *******  \n");
	}

	return 0;
}


void FillIndexBuffer(GLuint* pBuffer) 
{
	// local:
	int i = 0;
	GLuint* id = pBuffer;

	// code : //fill indices array
	for (i = 0; i < gridWidth * gridDepth; i += 4) 
	{
		*id++ = i;
		*id++ = i + 1;
		*id++ = i + 2;
		*id++ = i + 3;
	}
}

void FillVertexBuffer(GLfloat* pBuffer) 
{
	// local:
	vmath::vec3 *vertices = (vmath::vec3 *)pBuffer;
	int count = 0;
	int width_2 = gridWidth / 2;
	int depth_2 = gridDepth / 2;
	int i = 0;

	// code:
	for (i = -width_2; i <= width_2; i++) {
		vertices[count++] = vmath::vec3(i, 0, -depth_2);
		vertices[count++] = vmath::vec3(i, 0, depth_2);

		vertices[count++] = vmath::vec3(-width_2, 0, i);
		vertices[count++] = vmath::vec3(width_2, 0, i);
	}
}


int GetTotalVertices(void)
{
	// code:
	return ((gridWidth + 1) + (gridDepth + 1)) * 2;
}

int GetTotalIndices(void)
{
	// code:
	return (gridWidth*gridDepth);
}



void Render_Basic_Volume(void)
{
	// local:
	
	//setup the camera transform
	glm::mat4 ModelViewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, dist));

	ModelViewMatrix = glm::rotate(ModelViewMatrix, glm::radians(rotationX), glm::vec3(1.0f, 0.0f, 0.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, glm::radians(rotationY), glm::vec3(0.0f, 1.0f, 0.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, rotationZ, glm::vec3(0.0f, 0.0f, 1.0f));


	viewDirection = -glm::vec3(ModelViewMatrix[0][2], ModelViewMatrix[1][2], ModelViewMatrix[2][2]);

	glm::mat4 modelViewProjectionMatrix = perspectiveProjMatrix_glm * ModelViewMatrix;

	vmath::mat4 ModelViewMatrix_mat4 = vmath::translate(0.0f, 0.0f, dist);
	viewDirection_v3 =  -vmath::vec3(ModelViewMatrix_mat4[0][2], ModelViewMatrix_mat4[1][2], ModelViewMatrix_mat4[2][2]);


	// code:

	glUseProgram(shaderProgramObject_Grid);
	{
		glUniformMatrix4fv(mvpUniform_GridObject, 1, GL_FALSE, glm::value_ptr(modelViewProjectionMatrix));
		glUniform4f(glGetUniformLocation(shaderProgramObject_Grid,"u_Color"),1.0f,1.0f,0.0f,1.0f);
		glBindVertexArray(vaoID);
		glDrawElements(GL_LINES, totalIndices, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
	glUseProgram(0);


	Render_Volume_Box_Axes(modelViewProjectionMatrix);


	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);



	glUseProgram(shaderProgramObject_Slicer1);
	{
		glUniformMatrix4fv(modelViewProjectionUniform_Slicer1, 1, GL_FALSE, glm::value_ptr(modelViewProjectionMatrix));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_3D, textureID);
		glUniform1i(textureVolumeUniform_Slicer1, 0);
		glBindVertexArray(VAO_volume);
		glDrawArrays(GL_TRIANGLES, 0, sizeof(vTextureSlices) / sizeof(vTextureSlices[0]));
		glBindTexture(GL_TEXTURE_3D, 0);
		glBindVertexArray(0);
	}
	glUseProgram(0);
	glDisable(GL_BLEND);



}

void Update_Basic_Volume(void)
{
	// code:

	if (bRotateX)
	{
		rotationX += 0.01;
		if (rotationX >= 360.0f)
		{
			rotationX -= 360.0f;
		}
	}
	if (bRotateY)
	{
		rotationY += 0.01;
		if (rotationY >= 360.0f)
		{
			rotationY -= 360.0f;
		}
	}
	if (bRotateZ)
	{
		rotationZ += 0.01;
		if (rotationZ >= 360.0f)
		{
			rotationZ -= 360.0f;
		}
	}

}

void Uninitialize_Slicing_shader(void)
{
	// code:
	
	if (vboID_Indices)
	{
		glDeleteBuffers(1, &vboID_Indices);
		vboID_Indices = 0;
	}

	if (vboID_Vertices)
	{
		glDeleteBuffers(1, &vboID_Vertices);
		vboID_Vertices = 0;
	}
	if (vaoID)
	{
		glDeleteVertexArrays(1, &vaoID);
		vaoID = 0;
	}

	
	if (shaderProgramObject_Grid)
	{
		glUseProgram(shaderProgramObject_Grid);
		{
			GLsizei numAttachedShaders;
			glGetProgramiv(shaderProgramObject_Grid, GL_ATTACHED_SHADERS, &numAttachedShaders);
			GLuint* shaderObjects = NULL;
			shaderObjects = (GLuint*)malloc(numAttachedShaders * sizeof(GLuint));
			glGetAttachedShaders(shaderProgramObject_Grid, numAttachedShaders, &numAttachedShaders, shaderObjects);
			for (GLsizei i = 0; i < numAttachedShaders; i++)
			{
				glDetachShader(shaderProgramObject_Grid, shaderObjects[i]);
				glDeleteShader(shaderObjects[i]);
				shaderObjects[i] = 0;
			}
			free(shaderObjects);
			shaderObjects = NULL;
		}
		glUseProgram(0);
		glDeleteProgram(shaderProgramObject_Grid);

		shaderProgramObject_Grid = 0;
	}

	if (VBO_volume)
	{
		glDeleteBuffers(1, &VBO_volume);
		VBO_volume = 0;
	}
	if (VAO_volume)
	{
		glDeleteVertexArrays(1, &VAO_volume);
		VAO_volume = 0;
	}

	if (textureID)
	{
		glDeleteTextures(1, &textureID);
		textureID = 0;
	}

	if (shaderProgramObject_Slicer1)
	{
		glUseProgram(shaderProgramObject_Slicer1);
		{
			GLsizei numAttachedShaders;
			glGetProgramiv(shaderProgramObject_Slicer1, GL_ATTACHED_SHADERS, &numAttachedShaders);
			GLuint* shaderObjects = NULL;
			shaderObjects = (GLuint*)malloc(numAttachedShaders * sizeof(GLuint));
			glGetAttachedShaders(shaderProgramObject_Slicer1, numAttachedShaders, &numAttachedShaders, shaderObjects);
			for (GLsizei i = 0; i < numAttachedShaders; i++)
			{
				glDetachShader(shaderProgramObject_Slicer1, shaderObjects[i]);
				glDeleteShader(shaderObjects[i]);
				shaderObjects[i] = 0;
			}
			free(shaderObjects);
			shaderObjects = NULL;
		}
		glUseProgram(0);
		glDeleteProgram(shaderProgramObject_Slicer1);

		shaderProgramObject_Slicer1 = 0;
	}

}


//! Shader Type 2: RAY CASTING METHOD Method Definitions:

int Initialize_Raycasting_shader(void)
{

	// local:

	GLuint vertexShaderObject;
	GLuint fragmentShaderObject;

	GLint status;
	GLint infoLogLength;
	char* Log = NULL;

	// code:

	/////////////////////// # VERTEX SHADER # ////////////////////////
	const GLchar* vertexShaderSource = R"(
			
			#version 460 core
			
			layout (location = 0) in vec3 aPosition;
			uniform mat4 u_MVPMatrix;
			smooth out vec3 oTexCoords; //3D texture coordinates for texture lookup in the fragment shader
			void main()
			{
				// step1: // to get the Clipspace position
				gl_Position = u_MVPMatrix * vec4(aPosition.xyz, 1.0);

				// step2: //get the 3D texture coordinates from vertex position from step1
				oTexCoords = aPosition + vec3(0.5);


				/* 	step 2 notes:
				we will get the 3D texture coordinates by adding (0.5,0.5,0.5) to the object space vertex position. 
				Since the unit cube is at origin (min: (-0.5,-0.5,-0.5) and max: (0.5,0.5,0.5)), by adding (0.5,0.5,0.5) to the unit cube object space position gives us values from (0,0,0) to (1,1,1)
				*/
			}
			)";

	vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderObject, 1, (const GLchar**)&vertexShaderSource, NULL);

	glCompileShader(vertexShaderObject);
	glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			Log = (char*)malloc(infoLogLength);
			if (Log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(vertexShaderObject, infoLogLength, &written, Log);
				fprintf(gpFile, "Error in Raycating Vertex Shader .\n VS Compilation Log : %s\n", Log);
				free(Log);
				Log = NULL;
				Uninitialize_Raycasting_shader();
			}
		}
	}
	else
	{
		fprintf(gpFile, "Success in Raycating  Vertex Shader Compilation \n");
	}



	/////////////////////// # FRAGMENT SHADER # ////////////////////////
	const GLchar* fragmentShaderSource = R"(
			
			#version 460 core

			smooth in vec3 oTexCoords; //3D texture coordinates form vertex shader, interpolated by rasterizer

			uniform sampler3D u_Volume3DSampler; //volume dataset
			uniform vec3 u_cameraPosition; //camera or eye position // 27August2024: VVG: SinglePass GPU_RayCasting :
			uniform vec3 u_stepSize; //ray step size // 27August2024: VVG: SinglePass GPU_RayCasting :

			// 27August2024: VVG: SinglePass GPU_RayCasting : Adding 3 new constants below:
			const int MAX_SAMPLES = 300;	//total samples for each ray march step
			const vec3 texMin = vec3(0);	//minimum texture access coordinate
			const vec3 texMax = vec3(1);	//maximum texture access coordinate

			out vec4 FragColor;		
			void main(void)
			{             

				// step 1: // 27August2024: VVG: SinglePass GPU_RayCasting : save texcoords to local
				vec3 dataPosition = oTexCoords; 

				// step 2: // 27August2024: VVG: SinglePass GPU_RayCasting : Getting the ray marching direction:
				/*
				get the object space position by subracting 0.5 from the 3D texture coordinates.
				Then subtraact it from camera position and normalize to get the ray marching direction
				*/

				vec3 geomatryDirection = normalize((oTexCoords - vec3(0.5)) - u_cameraPosition);


				// step 3: // 27August2024: VVG: SinglePass GPU_RayCasting : calculate sub-step size for each Ray marching step
				/*
				multiply the raymarching direction with the step size to get the sub-step size we need to take at each raymarching step
				*/

				vec3 directionStep = geomatryDirection * u_stepSize;

				//flag to indicate if the raymarch loop should terminate
				bool bStop = false;

				for(int i = 0; i<MAX_SAMPLES; i++)
				{
					dataPosition = dataPosition + directionStep;

					bStop = dot(sign(dataPosition - texMin),sign(texMax - dataPosition)) < 3.0;

					if(bStop)
						break;

					// data fetching from the Red Channel of Volume Texture:
					float fSample = texture(u_Volume3DSampler,dataPosition).r;


					float prev_alpha = fSample - (fSample * FragColor.a);
					FragColor.rgb = prev_alpha * vec3(fSample) + FragColor.rgb; 
					FragColor.a += prev_alpha; 

					// Early Ray Termination : if the currently composited colour alpha is already fully saturated,we terminated the loop
					if(FragColor.a > 0.99)
						break;

				}

				//FragColor = texture(u_Volume3DSampler, oTexCoords).rrrr; // 27August2024: VVG: SinglePass GPU_RayCasting : original line ,rest is new calculation
				//FragColor = vec4(1.0,0.5,1.0,1.0); //DEBUG

			}

			//Here we sample the volume dataset using the 3D texture coordinates from the vertex shader.
			//Note that since at the time of texture creation, we gave the internal format as GL_RED
			//we can get the sample value from the texture using the red channel. Here, we set all 4
			//components as the sample value in the texture which gives us a shader of grey.
	
			)";
	fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderObject, 1, (const GLchar**)&fragmentShaderSource, NULL);

	glCompileShader(fragmentShaderObject);
	glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			Log = (char*)malloc(infoLogLength);
			if (Log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(fragmentShaderObject, infoLogLength, &written, Log);
				fprintf(gpFile, "Error in Raycating FRAGMENT Shader.\nFS Compilation Log : %s\n", Log);
				
				free(Log);
				Log = NULL;
				Uninitialize_Raycasting_shader();
			}
		}
	}
	else
	{
		fprintf(gpFile, "Success  in Raycating FRAGMENT Shader Compilation \n");
	}

	shaderProgramObject_RayCasting = glCreateProgram();


	glAttachShader(shaderProgramObject_RayCasting, vertexShaderObject);
	glAttachShader(shaderProgramObject_RayCasting, fragmentShaderObject);

	// MOVED BELOW
	//glBindAttribLocation(shaderProgramObject_RayCasting, AMC_ATTRIBUTE_POSITION, "aPosition");


	Log = NULL;

	glLinkProgram(shaderProgramObject_RayCasting);

	glGetProgramiv(shaderProgramObject_RayCasting, GL_LINK_STATUS, &status);

	if (status == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObject_RayCasting, GL_INFO_LOG_LENGTH, &infoLogLength);

		if (infoLogLength > 0)
		{
			Log = (char*)malloc(infoLogLength);

			if (Log != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(shaderProgramObject_RayCasting, infoLogLength, &written, Log);
				fprintf(gpFile, "Error in RayCasting shaderObject Linking\nLinking Log : % s\n", Log);

				free(Log);
				Uninitialize_Raycasting_shader();
				return FALSE;
			}
		}
	}
	else
	{
		fprintf(gpFile, "RayCasting shaderObject Linking Successful \n");
	}

	glUseProgram(shaderProgramObject_RayCasting);

	glBindAttribLocation(shaderProgramObject_RayCasting, AMC_ATTRIBUTE_POSITION, "aPosition");
	modelViewProjectionUniform_RayCasting = glGetUniformLocation(shaderProgramObject_RayCasting, "u_MVPMatrix");
	textureVolumeUniform_RayCasting = glGetUniformLocation(shaderProgramObject_RayCasting, "u_Volume3DSampler");
	cameraPositionUniform = glGetUniformLocation(shaderProgramObject_RayCasting, "u_cameraPosition");
	step_size_Uniform = glGetUniformLocation(shaderProgramObject_RayCasting, "u_stepSize");

	glUniform1i(textureVolumeUniform_RayCasting, 0);

	glUseProgram(0);



	return (0);
}

void Initialize_Raycasting_Geomatry(void)
{
	// code:

	glm::vec3 vertices[8] = { glm::vec3(-0.5f,-0.5f,-0.5f),
							glm::vec3(0.5f,-0.5f,-0.5f),
							glm::vec3(0.5f, 0.5f,-0.5f),
							glm::vec3(-0.5f, 0.5f,-0.5f),
							glm::vec3(-0.5f,-0.5f, 0.5f),
							glm::vec3(0.5f,-0.5f, 0.5f),
							glm::vec3(0.5f, 0.5f, 0.5f),
							glm::vec3(-0.5f, 0.5f, 0.5f) };

	//unit cube indices
	GLushort cubeIndices[36] = { 0,5,4,
							  5,0,1,
							  3,7,6,
							  3,6,2,
							  7,4,6,
							  6,4,5,
							  2,1,3,
							  3,1,0,
							  3,0,7,
							  7,0,4,
							  6,5,2,
							  2,5,1 };


	glGenVertexArrays(1, &VAO_cube_RayCastingCube);
	glBindVertexArray(VAO_cube_RayCastingCube);
	{
		// VBO_cube_ID : //pass cube vertices to buffer object memory
		glGenBuffers(1, &VBO_cube_ID_RayCastingCube);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_cube_ID_RayCastingCube);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &(vertices[0].x), GL_DYNAMIC_DRAW);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
		glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// VBO_cube_Indices : //pass indices to element array  buffer
		glGenBuffers(1, &VBO_cube_Indices_RayCastingCube);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO_cube_Indices_RayCastingCube);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), &cubeIndices[0], GL_DYNAMIC_DRAW);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	glBindVertexArray(0);


}

void Update_Raycasting_Cube_VBO(void)
{
	// code:

	glm::vec3 vertices[8] =
	{
		// Minus Z-Vertices
		glm::vec3(fXMinus_SideFace,fYMinus_BottomFace,fZMinus_BackFace), // 1. Left Bottom
		glm::vec3(fXPlus_SideFace,fYMinus_BottomFace,fZMinus_BackFace),  // 2. Right Bottom
		glm::vec3(fXPlus_SideFace, fYPlus_TopFace,fZMinus_BackFace),  // 3. Right Top
		glm::vec3(fXMinus_SideFace, fYPlus_TopFace,fZMinus_BackFace), // 4. Left Top

		// Plus Z-Vertices
		glm::vec3(fXMinus_SideFace,fYMinus_BottomFace, fZPlus_FrontFace), // 5. Left Bottom
		glm::vec3(fXPlus_SideFace,fYMinus_BottomFace, fZPlus_FrontFace),  // 6. Right Bottom
		glm::vec3(fXPlus_SideFace, fYPlus_TopFace, fZPlus_FrontFace),  // 7. Right Top
		glm::vec3(fXMinus_SideFace, fYPlus_TopFace, fZPlus_FrontFace)  // 8. Left Top
	};

	//unit cube indices
	GLushort cubeIndices[36] =
	{
		0,5,4,
		5,0,1,
		3,7,6,
		3,6,2,
		7,4,6,
		6,4,5,
		2,1,3,
		3,1,0,
		3,0,7,
		7,0,4,
		6,5,2,
		2,5,1
	};

	glBindVertexArray(VAO_cube_RayCastingCube);
	{
		// VBO_cube_ID : //pass cube vertices to buffer object memory
		glBindBuffer(GL_ARRAY_BUFFER, VBO_cube_ID_RayCastingCube);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &(vertices[0].x), GL_DYNAMIC_DRAW);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
		glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// VBO_cube_Indices : //pass indices to element array  buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO_cube_Indices_RayCastingCube);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), &cubeIndices[0], GL_DYNAMIC_DRAW);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	glBindVertexArray(0);

}

void Update_MarchingTetrahedra_Cube(void)
{
	// code:


	glm::vec3 vertices[8] =
	{
		// Minus Z-Vertices
		glm::vec3(-0.5f,-0.5f,-0.5f), // 1. Left Bottom
		glm::vec3(fXPlus_SideFace,-0.5f,-0.5f),  // 2. Right Bottom
		glm::vec3(fXPlus_SideFace, fYPlus_TopFace,-0.5f),  // 3. Right Top
		glm::vec3(-0.5f, fYPlus_TopFace,-0.5f), // 4. Left Top

		// Plus Z-Vertices
		glm::vec3(-0.5f,-0.5f, fZPlus_FrontFace), // 5. Left Bottom
		glm::vec3(fXPlus_SideFace,-0.5f, fZPlus_FrontFace),  // 6. Right Bottom
		glm::vec3(fXPlus_SideFace, fYPlus_TopFace, fZPlus_FrontFace),  // 7. Right Top
		glm::vec3(-0.5f, fYPlus_TopFace, fZPlus_FrontFace)  // 8. Left Top
	};

	//unit cube indices
	GLushort cubeIndices[36] =
	{
		0,5,4,
		5,0,1,
		3,7,6,
		3,6,2,
		7,4,6,
		6,4,5,
		2,1,3,
		3,1,0,
		3,0,7,
		7,0,4,
		6,5,2,
		2,5,1
	};

	glBindVertexArray(volumeMarcherVAO);
	glBindBuffer(GL_ARRAY_BUFFER, volumeMarcherVBO);
	{
		//pass the obtained vertices from the tetrahedra marcher and pass to the //buffer object memory
		glBufferData(GL_ARRAY_BUFFER, GetTotalVertices_TM() * sizeof(Vertex), GetVertexPointer_TM(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);//enable vertex attribute array for position
		glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

		//enable vertex attribute array for normals
		glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);
		glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, normal));

	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Render_Raycasting_Output(void)
{
	// local:

	glm::mat4 ModelViewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, dist));

	ModelViewMatrix = glm::rotate(ModelViewMatrix, glm::radians(rotationX), glm::vec3(1.0f, 0.0f, 0.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, glm::radians(rotationY), glm::vec3(0.0f, 1.0f, 0.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, rotationZ, glm::vec3(0.0f, 0.0f, 1.0f));

	glm::mat4 modelViewProjectionMatrix = perspectiveProjMatrix_glm * ModelViewMatrix;
	glm::vec3 cameraPosition = glm::vec3(glm::inverse(ModelViewMatrix) * glm::vec4(0.0, 0.0, 0.0, 1.0));


	// code:

	///*glUseProgram(shaderProgramObject_Grid);
	//{
	//	glUniformMatrix4fv(mvpUniform_GridObject, 1, GL_FALSE, glm::value_ptr(modelViewProjectionMatrix));
	//	glUniform4f(glGetUniformLocation(shaderProgramObject_Grid, "u_Color"), 1.0f, 1.0f, 0.0f, 1.0f);
	//	glBindVertexArray(vaoID);
	//	glDrawElements(GL_LINES, totalIndices, GL_UNSIGNED_INT, 0);
	//	glBindVertexArray(0);
	//}
	//glUseProgram(0);*/


	Render_Volume_Box_Axes(modelViewProjectionMatrix);


	glEnable(GL_BLEND);
	glUseProgram(shaderProgramObject_RayCasting);
	{
		glUniformMatrix4fv(modelViewProjectionUniform_RayCasting, 1, GL_FALSE, glm::value_ptr(modelViewProjectionMatrix));

		glUniform3fv(cameraPositionUniform, 1, &(cameraPosition.x));
		glUniform3f(step_size_Uniform, (1.0f / XDIM), (1.0f / YDIM), (1.0f / ZDIM));

		// Texture : 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_3D, textureID);
		glUniform1i(textureVolumeUniform_RayCasting, 0);


		glBindVertexArray(VAO_cube_RayCastingCube);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_3D, 0);
	}
	glUseProgram(0);
	glDisable(GL_BLEND);

}

void Update_Raycasting_Output(void)
{
	// local:

	// code;

}

void Uninitialize_Raycasting_shader(void)
{
	// local:

	// code:

	if (VBO_cube_Indices_RayCastingCube)
	{
		glDeleteBuffers(1, &VBO_cube_Indices_RayCastingCube);
		VBO_cube_Indices_RayCastingCube = 0;
	}
	if (VBO_cube_ID_RayCastingCube)
	{
		glDeleteBuffers(1, &VBO_cube_ID_RayCastingCube);
		VBO_cube_ID_RayCastingCube = 0;
	}

	if (VAO_cube_RayCastingCube)
	{
		glDeleteVertexArrays(1, &VAO_cube_RayCastingCube);
		VAO_cube_RayCastingCube = 0;
	}

	if (shaderProgramObject_RayCasting)
	{
		glUseProgram(shaderProgramObject_RayCasting);
		{
			GLsizei numAttachedShaders;
			glGetProgramiv(shaderProgramObject_RayCasting, GL_ATTACHED_SHADERS, &numAttachedShaders);
			GLuint* shaderObjects = NULL;
			shaderObjects = (GLuint*)malloc(numAttachedShaders * sizeof(GLuint));
			glGetAttachedShaders(shaderProgramObject_RayCasting, numAttachedShaders, &numAttachedShaders, shaderObjects);
			for (GLsizei i = 0; i < numAttachedShaders; i++)
			{
				glDetachShader(shaderProgramObject_RayCasting, shaderObjects[i]);
				glDeleteShader(shaderObjects[i]);
				shaderObjects[i] = 0;
			}
			free(shaderObjects);
			shaderObjects = NULL;
		}
		glUseProgram(0);
		glDeleteProgram(shaderProgramObject_RayCasting);

		shaderProgramObject_RayCasting = 0;
	}

}



//! Shader Type 3: Iso Surface with RAY CASTING Method Definitions:

int Initialize_IsoSurface_shader(void)
{
	// local:

	GLuint vertexShaderObject;
	GLuint fragmentShaderObject;

	GLint status;
	GLint infoLogLength;
	char* Log = NULL;

	// code:

	/////////////////////// # VERTEX SHADER # ////////////////////////
	const GLchar* vertexShaderSource = R"(
			
			#version 460 core
			
			layout (location = 0) in vec3 aPosition;
			uniform mat4 u_MVPMatrix;
			smooth out vec3 oTexCoords; //3D texture coordinates for texture lookup in the fragment shader

			void main()
			{
				// step1: // to get the Clipspace position
				gl_Position = u_MVPMatrix * vec4(aPosition.xyz, 1.0);


				// step2: //get the 3D texture coordinates from vertex position from step1
				oTexCoords = aPosition + vec3(0.5);
				/* 	step 2 notes:
				we will get the 3D texture coordinates by adding (0.5,0.5,0.5) to the object space vertex position. 
				Since the unit cube is at origin (min: (-0.5,-0.5,-0.5) and max: (0.5,0.5,0.5)), by adding (0.5,0.5,0.5) to the unit cube object space position gives us values from (0,0,0) to (1,1,1)
				*/
		
			}
			)";

	vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderObject, 1, (const GLchar**)&vertexShaderSource, NULL);

	glCompileShader(vertexShaderObject);
	glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			Log = (char*)malloc(infoLogLength);
			if (Log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(vertexShaderObject, infoLogLength, &written, Log);
				fprintf(gpFile, "Error in IsoSurface Vertex Shader .\n VS Compilation Log : %s\n", Log);
				free(Log);
				Log = NULL;
				Uninitialize_Raycasting_shader();
			}
		}
	}
	else
	{
		fprintf(gpFile, "Success in IsoSurface  Vertex Shader Compilation \n");
	}



	/////////////////////// # FRAGMENT SHADER # ////////////////////////
	const GLchar* fragmentShaderSource = R"(
			
			#version 460 core

			smooth in vec3 oTexCoords; //3D texture coordinates form vertex shader, interpolated by rasterizer

			uniform sampler3D u_Volume3DSampler; //volume dataset
			uniform vec3 u_cameraPosition; //camera or eye position
			uniform vec3 u_stepSize; //ray step size

			const int MAX_SAMPLES = 300;	//total samples for each ray march step
			const vec3 texMin = vec3(0);	//minimum texture access coordinate
			const vec3 texMax = vec3(1);	//maximum texture access coordinate
			const float DELTA = 0.01;			//the step size for gradient calculation
			const float isoValue = 40/255.0;	//the isovalue for iso-surface detection

			out vec4 FragColor;


			//function to give a more accurate position of where the given iso-value (iso) is found given the initial minimum limit (left) and maximum limit (right)

			vec3 Bisection(vec3 left, vec3 right , float iso)
			{ 
				//loop 4 times
				for(int i=0;i<4;i++)
				{ 
					//get the mid value between the left and right limit
					vec3 midpoint = (right + left) * 0.5;
					//sample the texture at the middle point
					float cM = texture(u_Volume3DSampler, midpoint).x ;
					//check if the value at the middle point is less than the given iso-value
					if(cM < iso)
						//if so change the left limit to the new middle point
						left = midpoint;
					else
						//otherwise change the right limit to the new middle point
						right = midpoint; 
				}
				//finally return the middle point between the left and right limit
				return vec3(right + left) * 0.5;
			}

			//function to calculate the gradient at the given location in the volume dataset
			//The function user center finite difference approximation to estimate the gradient
			vec3 GetGradient(vec3 uvw) 
			{
				vec3 s1, s2;  

				//Using center finite difference 
				s1.x = texture(u_Volume3DSampler, uvw-vec3(DELTA,0.0,0.0)).x ;
				s2.x = texture(u_Volume3DSampler, uvw+vec3(DELTA,0.0,0.0)).x ;

				s1.y = texture(u_Volume3DSampler, uvw-vec3(0.0,DELTA,0.0)).x ;
				s2.y = texture(u_Volume3DSampler, uvw+vec3(0.0,DELTA,0.0)).x ;

				s1.z = texture(u_Volume3DSampler, uvw-vec3(0.0,0.0,DELTA)).x ;
				s2.z = texture(u_Volume3DSampler, uvw+vec3(0.0,0.0,DELTA)).x ;
	 
				return normalize((s1-s2)/2.0); 
			}

			vec4 PhongLighting(vec3 L, vec3 N, vec3 V, float specPower, vec3 diffuseColor)
			{
				float diffuse = max(dot(L,N),0.0);
				vec3 halfVec = normalize(L+V);
				float specular = pow(max(0.00001,dot(halfVec,N)),specPower);	
				return vec4((diffuse*diffuseColor + specular),1.0);
			}

			void main(void)
			{             

				// step 1: save texcoords to local
				vec3 dataPosition = oTexCoords; 

				// step 2: Getting the ray marching direction:
				/*
				get the object space position by subracting 0.5 from the 3D texture coordinates.
				Then subtraact it from camera position and normalize to get the ray marching direction
				*/
				vec3 geomatryDirection = normalize((oTexCoords - vec3(0.5)) - u_cameraPosition);

				// step 3: calculate sub-step size for each Ray marching step
				/*
				multiply the raymarching direction with the step size to get the sub-step size we need to take at each raymarching step
				*/
				vec3 directionStep = geomatryDirection * u_stepSize;

				//flag to indicate if the raymarch loop should terminate
				bool bStop = false;

				for(int i = 0; i<MAX_SAMPLES; i++)
				{
					dataPosition = dataPosition + directionStep; //advance ray by directionStep

					bStop = dot(sign(dataPosition - texMin),sign(texMax - dataPosition)) < 3.0;

					if(bStop)
						break;

					// data fetching from the Red Channel of Volume Texture:
					float fSample = texture(u_Volume3DSampler,dataPosition).r; // sample1
					float fSample2 = texture(u_Volume3DSampler,dataPosition + directionStep).r; // sample2: next sample

					/*
					In case of iso-surface rendering, we do not use compositing. 
					Instead, we find the zero crossing of the volume dataset iso function by sampling two consecutive samples. 
					*/

					if((fSample - isoValue) < 0 && (fSample2 - isoValue) >= 0.0)
					{
						//If there is a zero crossing, we refine the detected iso-surface location by using bisection based refinement.
						vec3 xN = dataPosition;
						vec3 xF = dataPosition + directionStep;
						vec3 tc = Bisection(xN,xF,isoValue);

						//This returns the first hit surface //vFragColor = make_float4(xN,1);
          	
						//To get the shaded iso-surface, we first estimate the normal at the refined position
						vec3 N = GetGradient(tc);					

						//The view vector is simply opposite to the ray marching direction
						vec3 V = -geomatryDirection;

						//We keep the view vector as the light vector to give us a head light
						vec3 L =  V;

						//Finally, we call PhongLighing function to get the final colour with diffuse and specular components. Try changing this call to this
						//vFragColor =  PhongLighting(L,N,V,250,  tc); to get a multi colour
						//iso-surface
						FragColor =  PhongLighting(L,N,V,250, vec3(0.5));	
						break;

					}
				}
				
			}

			//Here we sample the volume dataset using the 3D texture coordinates from the vertex shader.
			//Note that since at the time of texture creation, we gave the internal format as GL_RED
			//we can get the sample value from the texture using the red channel. Here, we set all 4
			//components as the sample value in the texture which gives us a shader of grey.
	
			)";
	fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderObject, 1, (const GLchar**)&fragmentShaderSource, NULL);

	glCompileShader(fragmentShaderObject);
	glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			Log = (char*)malloc(infoLogLength);
			if (Log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(fragmentShaderObject, infoLogLength, &written, Log);
				fprintf(gpFile, "Error in IsoSurface FRAGMENT Shader.\nFS Compilation Log : %s\n", Log);

				free(Log);
				Log = NULL;
				Uninitialize_Raycasting_shader();
			}
		}
	}
	else
	{
		fprintf(gpFile, "Success  in IsoSurface FRAGMENT Shader Compilation \n");
	}

	shaderProgramObject_IsoSurface = glCreateProgram();


	glAttachShader(shaderProgramObject_IsoSurface, vertexShaderObject);
	glAttachShader(shaderProgramObject_IsoSurface, fragmentShaderObject);

	// MOVED BELOW
	//glBindAttribLocation(shaderProgramObject_IsoSurface, AMC_ATTRIBUTE_POSITION, "aPosition");


	Log = NULL;

	glLinkProgram(shaderProgramObject_IsoSurface);

	glGetProgramiv(shaderProgramObject_IsoSurface, GL_LINK_STATUS, &status);

	if (status == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObject_IsoSurface, GL_INFO_LOG_LENGTH, &infoLogLength);

		if (infoLogLength > 0)
		{
			Log = (char*)malloc(infoLogLength);

			if (Log != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(shaderProgramObject_IsoSurface, infoLogLength, &written, Log);
				fprintf(gpFile, "Error in IsoSurface shaderObject Linking\nLinking Log : % s\n", Log);

				free(Log);
				Uninitialize_Raycasting_shader();
				return FALSE;
			}
		}
	}
	else
	{
		fprintf(gpFile, "IsoSurface shaderObject Linking Successful \n");
	}

	glUseProgram(shaderProgramObject_IsoSurface);

	glBindAttribLocation(shaderProgramObject_IsoSurface, AMC_ATTRIBUTE_POSITION, "aPosition");
	modelViewProjectionUniform_IsoSurface = glGetUniformLocation(shaderProgramObject_IsoSurface, "u_MVPMatrix");
	textureVolumeUniform_IsoSurface = glGetUniformLocation(shaderProgramObject_IsoSurface, "u_Volume3DSampler");
	cameraPositionUniform_IsoSurface = glGetUniformLocation(shaderProgramObject_IsoSurface, "u_cameraPosition");
	step_size_Uniform_IsoSurface = glGetUniformLocation(shaderProgramObject_IsoSurface, "u_stepSize");

	glUniform1i(textureVolumeUniform_IsoSurface, 0);

	glUseProgram(0);


		
	return (0);
}


void Initialize_IsoSurface_Geomatry(void)
{
	// local:

	// code:

}


void Render_IsoSurface_Output(void)
{
	// local:
	glm::mat4 ModelViewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, dist));

	ModelViewMatrix = glm::rotate(ModelViewMatrix, glm::radians(rotationX), glm::vec3(1.0f, 0.0f, 0.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, glm::radians(rotationY), glm::vec3(0.0f, 1.0f, 0.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, rotationZ, glm::vec3(0.0f, 0.0f, 1.0f));

	glm::mat4 modelViewProjectionMatrix = perspectiveProjMatrix_glm * ModelViewMatrix;
	glm::vec3 cameraPosition = glm::vec3(glm::inverse(ModelViewMatrix) * glm::vec4(0.0, 0.0, 0.0, 1.0));

	// code:


	///*glUseProgram(shaderProgramObject_Grid);
	//{
	//	glUniformMatrix4fv(mvpUniform_GridObject, 1, GL_FALSE, glm::value_ptr(modelViewProjectionMatrix));
	//	glUniform4f(glGetUniformLocation(shaderProgramObject_Grid, "u_Color"), 1.0f, 1.0f, 0.0f, 1.0f);
	//	glBindVertexArray(vaoID);
	//	glDrawElements(GL_LINES, totalIndices, GL_UNSIGNED_INT, 0);
	//	glBindVertexArray(0);
	//}
	//glUseProgram(0);*/


	Render_Volume_Box_Axes(modelViewProjectionMatrix);



	glEnable(GL_BLEND);
	glUseProgram(shaderProgramObject_IsoSurface);
	{
		glUniformMatrix4fv(modelViewProjectionUniform_IsoSurface, 1, GL_FALSE, glm::value_ptr(modelViewProjectionMatrix));//pass the shader uniform
		glUniform3fv(cameraPositionUniform_IsoSurface, 1, &(cameraPosition.x));
		glUniform3f(step_size_Uniform_IsoSurface, (1.0f / XDIM), (1.0f / YDIM), (1.0f / ZDIM));

		// Texture : 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_3D, textureID);
		glUniform1i(textureVolumeUniform_IsoSurface, 0);

		//	//enable alpha blending (use over operator)  *****  DONE *****

		glBindVertexArray(VAO_cube_RayCastingCube);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_3D, 0);
	}
	glUseProgram(0);
	glDisable(GL_BLEND);

}


void Update_IsoSurface_Output(void)
{
	// local:

	// code:

}


void Uninitialize_IsoSurface_shader(void)
{
	// local:

	// code:

	if (shaderProgramObject_IsoSurface)
	{
		glUseProgram(shaderProgramObject_IsoSurface);
		{
			GLsizei numAttachedShaders;
			glGetProgramiv(shaderProgramObject_IsoSurface, GL_ATTACHED_SHADERS, &numAttachedShaders);
			GLuint* shaderObjects = NULL;
			shaderObjects = (GLuint*)malloc(numAttachedShaders * sizeof(GLuint));
			glGetAttachedShaders(shaderProgramObject_IsoSurface, numAttachedShaders, &numAttachedShaders, shaderObjects);
			for (GLsizei i = 0; i < numAttachedShaders; i++)
			{
				glDetachShader(shaderProgramObject_IsoSurface, shaderObjects[i]);
				glDeleteShader(shaderObjects[i]);
				shaderObjects[i] = 0;
			}
			free(shaderObjects);
			shaderObjects = NULL;
		}
		glUseProgram(0);
		glDeleteProgram(shaderProgramObject_IsoSurface);

		shaderProgramObject_IsoSurface = 0;
	}

}


//! Shader Type 4: Colormap Classification Method Definitions:

int Initialize_ColormapClassification_shader(void)
{

	// prototype:
	void uninitialize(void);

	// local:

	GLuint vertexShaderObject;
	GLuint fragmentShaderObject;

	GLint status;
	GLint infoLogLength;
	char* Log = NULL;

	// code:

	//* ///////////////////// # VERTEX SHADER # ////////////////////////
	const GLchar* vertexShaderSource = R"(
			
			#version 460 core

			layout (location = 0) in vec3 aPosition;
			uniform mat4 u_MVPMatrix;
			smooth out vec3 oTexCoords; //3D texture coordinates for texture lookup in the fragment shader

			void main()
			{
				gl_Position = u_MVPMatrix * vec4(aPosition.xyz, 1.0);

				oTexCoords = aPosition + vec3(0.5);

				//get the 3D texture coordinates by adding (0.5,0.5,0.5) to the object space 
				//vertex position. Since the unit cube is at origin (min: (-0.5,-0.5,-0.5) and max: (0.5,0.5,0.5))
				//adding (0.5,0.5,0.5) to the unit cube object space position gives us values from (0,0,0) to 
				//(1,1,1)

			}

			)";

	vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderObject, 1, (const GLchar**)&vertexShaderSource, NULL);

	glCompileShader(vertexShaderObject);
	glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			Log = (char*)malloc(infoLogLength);
			if (Log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(vertexShaderObject, infoLogLength, &written, Log);
				fprintf(gpFile, "Error in Colormap VERTEX Shader.\nVS Compilation Log : %s\n", Log);
				free(Log);
				Log = NULL;
				uninitialize();
			}
		}
	}
	else
	{
		fprintf(gpFile, "Success in Colormap VERTEX Shader.\n");
	}



	//* ///////////////////// # FRAGMENT SHADER # ////////////////////////
	const GLchar* fragmentShaderSource = R"(
			
			#version 460 core

			smooth in vec3 oTexCoords; //3D texture coordinates form vertex shader, interpolated by rasterizer
			uniform sampler3D u_Volume3DSampler; //volume dataset
			uniform sampler1D u_LevelOfDetail;  //transfer function (lookup table) texture

			out vec4 FragColor;		
			void main(void)
			{             

				FragColor = texture(u_LevelOfDetail, texture(u_Volume3DSampler,oTexCoords).r);
			}

			//Here we sample the volume dataset using the 3D texture coordinates from the vertex shader.
			//Note that since at the time of texture creation, we gave the internal format as GL_RED
			//we can get the sample value from the texture using the red channel. 

			//Then we can use the Density Value obtained from the Volume dataset and Lookup the Colour From the transfer Function texture
			// By doing a Dependent Texture Lookup.		
	
			)";
	fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderObject, 1, (const GLchar**)&fragmentShaderSource, NULL);

	glCompileShader(fragmentShaderObject);

	status = 0;
	infoLogLength = 0;
	Log = NULL;
	glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			Log = (char*)malloc(infoLogLength);
			if (Log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(fragmentShaderObject, infoLogLength, &written, Log);
				fprintf(gpFile, "Error in Colormap FRAGMENT Shader.\nFS Compilation Log : %s\n", Log);
				free(Log);
				Log = NULL;
				uninitialize();
			}
		}
	}
	else
	{
		fprintf(gpFile, "Success in Colormap FRAGMENT Shader.\n");
	}

	shaderProgramObject_Colormap= glCreateProgram();

	glAttachShader(shaderProgramObject_Colormap, vertexShaderObject);
	glAttachShader(shaderProgramObject_Colormap, fragmentShaderObject);


	status = 0;
	infoLogLength = 0;
	Log = NULL;
	glLinkProgram(shaderProgramObject_Colormap);
	glGetProgramiv(shaderProgramObject_Colormap, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObject_Colormap, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			Log = (char*)malloc(infoLogLength);
			if (Log != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(shaderProgramObject_Colormap, infoLogLength, &written, Log);
				fprintf(gpFile, "Error in Colormap shaderObject Linking\nLinking Log : % s\n", Log);
				free(Log);
				uninitialize();
				return FALSE;
			}
		}
	}
	else
	{
		fprintf(gpFile, "Success in Colormap shaderObject Linking\n");
	}

	glUseProgram(shaderProgramObject_Colormap);

	glBindAttribLocation(shaderProgramObject_Colormap, AMC_ATTRIBUTE_POSITION, "aPosition");
	modelViewProjectionUniform_Colormap = glGetUniformLocation(shaderProgramObject_Colormap, "u_MVPMatrix");
	textureVolumeUniform_Colormap = glGetUniformLocation(shaderProgramObject_Colormap, "u_Volume3DSampler");
	levelOfDetail_Uniform = glGetUniformLocation(shaderProgramObject_Colormap, "u_LevelOfDetail");

	glUniform1i(textureVolumeUniform_Colormap, 0);
	glUniform1i(levelOfDetail_Uniform, 1);
	glUseProgram(0);


	return (0);
}

void LoadTransferFunction(void)
{
	// local: 
	float pData[256][4];
	int indices[9];


	// code:

	for (int i = 0; i < 9; i++) {
		int index = i * 28;
		pData[index][0] = jet_values[i].x;
		pData[index][1] = jet_values[i].y;
		pData[index][2] = jet_values[i].z;
		pData[index][3] = jet_values[i].w;
		indices[i] = index;
	}


	for (int j = 0; j < 9 - 1; j++)
	{
		float dDataR = (pData[indices[j + 1]][0] - pData[indices[j]][0]);
		float dDataG = (pData[indices[j + 1]][1] - pData[indices[j]][1]);
		float dDataB = (pData[indices[j + 1]][2] - pData[indices[j]][2]);
		float dDataA = (pData[indices[j + 1]][3] - pData[indices[j]][3]);
		int dIndex = indices[j + 1] - indices[j];

		float dDataIncR = dDataR / float(dIndex);
		float dDataIncG = dDataG / float(dIndex);
		float dDataIncB = dDataB / float(dIndex);
		float dDataIncA = dDataA / float(dIndex);
		for (int i = indices[j] + 1; i < indices[j + 1]; i++)
		{
			pData[i][0] = (pData[i - 1][0] + dDataIncR);
			pData[i][1] = (pData[i - 1][1] + dDataIncG);
			pData[i][2] = (pData[i - 1][2] + dDataIncB);
			pData[i][3] = (pData[i - 1][3] + dDataIncA);
		}
	}


	glGenTextures(1, &texture_TransferFunction);//generate the OpenGL texture
	glActiveTexture(GL_TEXTURE1);//bind this texture to texture unit 1
	glBindTexture(GL_TEXTURE_1D, texture_TransferFunction);
	// set the texture parameters
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	//allocate the data to texture memory. Since pData is on stack, we donot delete it 
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, 256, 0, GL_RGBA, GL_FLOAT, pData);

	//glBindTexture(GL_TEXTURE_1D, 0);


}

void Initialize_ColormapClassification_Geomatry(void)
{
	// local:

	// code:

}

void Render_ColormapClassification_Output(void)
{
	// local:

	glm::mat4 ModelViewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, dist));

	ModelViewMatrix = glm::rotate(ModelViewMatrix, glm::radians(rotationX), glm::vec3(1.0f, 0.0f, 0.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, glm::radians(rotationY), glm::vec3(0.0f, 1.0f, 0.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, rotationZ, glm::vec3(0.0f, 0.0f, 1.0f));

	viewDirection = -glm::vec3(ModelViewMatrix[0][2], ModelViewMatrix[1][2], ModelViewMatrix[2][2]);

	glm::mat4 modelViewProjectionMatrix = perspectiveProjMatrix_glm * ModelViewMatrix;

	// code:

	///*glUseProgram(shaderProgramObject_Grid);
	//{
	//	glUniformMatrix4fv(mvpUniform_GridObject, 1, GL_FALSE, glm::value_ptr(modelViewProjectionMatrix));
	//	glUniform4f(glGetUniformLocation(shaderProgramObject_Grid, "u_Color"), 1.0f, 1.0f, 0.0f, 1.0f);
	//	glBindVertexArray(vaoID);
	//	glDrawElements(GL_LINES, totalIndices, GL_UNSIGNED_INT, 0);
	//	glBindVertexArray(0);
	//}
	//glUseProgram(0);*/


	Render_Volume_Box_Axes(modelViewProjectionMatrix);



	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glUseProgram(shaderProgramObject_Colormap);
	{
		glUniformMatrix4fv(modelViewProjectionUniform_Colormap, 1, GL_FALSE, glm::value_ptr(modelViewProjectionMatrix));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_3D, textureID);
		glUniform1i(textureVolumeUniform_Colormap, 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_1D, texture_TransferFunction);
		glUniform1i(levelOfDetail_Uniform, 1);

		glBindVertexArray(VAO_volume);
		glDrawArrays(GL_TRIANGLES, 0, sizeof(vTextureSlices) / sizeof(vTextureSlices[0]));
		glBindTexture(GL_TEXTURE_3D, 0);
		glBindVertexArray(0);
	}
	glUseProgram(0);
	glDisable(GL_BLEND);


}

void Update_ColormapClassification_Output(void)
{
	// local:

	// code:

}

void Uninitialize_ColormapClassification_shader(void)
{
	// local:

	// code:

	if (texture_TransferFunction)
	{
		glDeleteTextures(1, &texture_TransferFunction);
		texture_TransferFunction = 0;
	}

	if (shaderProgramObject_Colormap)
	{
		glUseProgram(shaderProgramObject_Colormap);
		{
			GLsizei numAttachedShaders;
			glGetProgramiv(shaderProgramObject_Colormap, GL_ATTACHED_SHADERS, &numAttachedShaders);
			GLuint* shaderObjects = NULL;
			shaderObjects = (GLuint*)malloc(numAttachedShaders * sizeof(GLuint));
			glGetAttachedShaders(shaderProgramObject_Colormap, numAttachedShaders, &numAttachedShaders, shaderObjects);
			for (GLsizei i = 0; i < numAttachedShaders; i++)
			{
				glDetachShader(shaderProgramObject_Colormap, shaderObjects[i]);
				glDeleteShader(shaderObjects[i]);
				shaderObjects[i] = 0;
			}
			free(shaderObjects);
			shaderObjects = NULL;
		}
		glUseProgram(0);
		glDeleteProgram(shaderProgramObject_Colormap);

		shaderProgramObject_Texture  = 0;
	}

}


//! Shader Type 5: Marching Tetrahedra Method Definitions:


void Initialize_TetrahedraMarcher_Constructor()
{
	XDIM_TM = 256;
	YDIM_TM = 256;
	ZDIM_TM = 256;
	pVolume = NULL;
}

void Initialize_TetrahedraMarcher_Shaders()
{
	// prototype:
	void uninitialize(void);

	// local:

	GLuint vertexShaderObject;
	GLuint fragmentShaderObject;

	GLint status;
	GLint infoLogLength;
	char* Log = NULL;

	// code:


	/////////////////////// # VERTEX SHADER # ////////////////////////
	const GLchar* vertexShaderSource = R"(
			
			#version 460 core
			
			layout (location = 0) in vec3 aPosition; //object space vertex position
			//layout (location = 1) in vec3 aColor_Dummy;
			layout (location = 2) in vec3 aNormal; //object space vertex normal

			uniform mat4 u_MVPMatrix;

			smooth out vec3 oNormal;  //output object space normal

			void main()
			{
				// step1: // to get the Clipspace position
				gl_Position = u_MVPMatrix * vec4(aPosition.xyz, 1.0);


				// step2: 
				oNormal = aNormal;
				/* 	step 2 notes:
				*/
		
			}
	
			)";

	vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderObject, 1, (const GLchar**)&vertexShaderSource, NULL);

	glCompileShader(vertexShaderObject);
	glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			Log = (char*)malloc(infoLogLength);
			if (Log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(vertexShaderObject, infoLogLength, &written, Log);
				fprintf(gpFile, "Error in MarchingTetrahedra Vertex Shader.\nVS Compilation Log : %s\n", Log);
				free(Log);
				Log = NULL;
				uninitialize();
			}
		}
	}
	else
	{
		fprintf(gpFile, "Success in MarchingTetrahedra Vertex Shader.\n");
	}



	/////////////////////// # FRAGMENT SHADER # ////////////////////////
	const GLchar* fragmentShaderSource = R"(
			
			#version 460 core

			smooth in vec3 oNormal; //varying input from the vertex shader  interpolated by rasterizer
			out vec4 FragColor;

			void main(void)
			{             

				FragColor = vec4(oNormal,1.0);  	//output the object space normal as colour
				//FragColor = vec4(1.0,0.5,1.0,1.0); //DEBUG
			}
	

			)";
	fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderObject, 1, (const GLchar**)&fragmentShaderSource, NULL);

	glCompileShader(fragmentShaderObject);
	glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			Log = (char*)malloc(infoLogLength);
			if (Log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(fragmentShaderObject, infoLogLength, &written, Log);
				fprintf(gpFile, "Error in MarchingTetrahedra Fragment Shader.\nFS Compilation Log : %s\n", Log);
				free(Log);
				Log = NULL;
				uninitialize();
			}
		}
	}
	else
	{
		fprintf(gpFile, "Success in MarchingTetrahedra Fragment Shader.\n");
	}

	shaderProgramObject_TM = glCreateProgram();

	glAttachShader(shaderProgramObject_TM, vertexShaderObject);
	glAttachShader(shaderProgramObject_TM, fragmentShaderObject);

	// pre Linking binding code has been MOVED BELOW

	Log = NULL;
	
	glLinkProgram(shaderProgramObject_TM);
	glGetProgramiv(shaderProgramObject_TM, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObject_TM, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			Log = (char*)malloc(infoLogLength);
			if (Log != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(shaderProgramObject_TM, infoLogLength, &written, Log);
				fprintf(gpFile, "Error in MarchingTetrahedra shaderObject Linking\nLinking Log : % s\n", Log);
				free(Log);
				uninitialize();
			}
		}
	}
	else
	{
		fprintf(gpFile, "Success in MarchingTetrahedra shaderObject Linking\n");
	}

	glUseProgram(shaderProgramObject_TM);
	glBindAttribLocation(shaderProgramObject_TM, AMC_ATTRIBUTE_POSITION, "aPosition");
	glBindAttribLocation(shaderProgramObject_TM, AMC_ATTRIBUTE_NORMAL, "aNormal");
	modelViewProjectionUniform_TM = glGetUniformLocation(shaderProgramObject_TM, "u_MVPMatrix");
	glUseProgram(0);

}

void Initialize_TetrahedraMarcher_Geomatry(void)
{
	// code:

	// VAO + VBO CODE for MarchingTetrahedra
	glGenVertexArrays(1, &volumeMarcherVAO);
	glBindVertexArray(volumeMarcherVAO);
	glGenBuffers(1, &volumeMarcherVBO);
	glBindBuffer(GL_ARRAY_BUFFER, volumeMarcherVBO);
	{
		//pass the obtained vertices from the tetrahedra marcher and pass to the //buffer object memory
		glBufferData(GL_ARRAY_BUFFER, GetTotalVertices_TM() * sizeof(Vertex), GetVertexPointer_TM(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);//enable vertex attribute array for position
		glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

		//enable vertex attribute array for normals
		glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);
		glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, normal));

	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}

void SetVolumeDimensions(const int xdim, const int ydim, const int zdim)
{
	XDIM_TM = xdim;
	YDIM_TM = ydim;
	ZDIM_TM = zdim;
	invDim.x = 1.0f / XDIM_TM;
	invDim.y = 1.0f / YDIM_TM;
	invDim.z = 1.0f / ZDIM_TM;
}
void SetNumSamplingVoxels(const int x, const int y, const int z)
{
	X_SAMPLING_DIST = x;
	Y_SAMPLING_DIST = y;
	Z_SAMPLING_DIST = z;
}
void SetIsosurfaceValue(const GLubyte value)
{
	isoValue = value;
}

bool LoadVolume(const std::string& filename)
{

	//std::ifstream infile(filename.c_str(), std::ios_base::binary);
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

void LoadDebuGTex(void)
{
	unsigned char testTexture[] =
	{
		255,   0,   0, 255,     // Red
		  0, 255,   0, 255,     // Green
		0,     0, 255, 255,     // Blue
		255, 255, 255, 255      // White
	};

	

	glGenTextures(1, &testTex);
	glBindTexture(GL_TEXTURE_2D, testTex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGBA8,
		2,
		2,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		testTexture
	);

	glBindTexture(GL_TEXTURE_2D, 0);
}

bool Load_New_Volume(const std::string& filename,int X_DIM,int Y_DIM,int Z_DIM)
{

	std::ifstream infile(filename.c_str(), std::ios_base::binary);


	if (infile.good())
	{
		//read the volume data file
		GLubyte* pData = new GLubyte[X_DIM * Y_DIM * Z_DIM];
		infile.read(reinterpret_cast<char*>(pData), X_DIM * Y_DIM * Z_DIM * sizeof(GLubyte));
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
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAX_LEVEL, 4);

			// create 3D texture with pData : //allocate data with internal format and foramt as (GL_RED)	
			glTexImage3D(
				/*target*/GL_TEXTURE_3D,
				/* level*/0,
				/*internalFormat*/GL_R8,
				/* width*/X_DIM,
				/* height*/Y_DIM,
				/* depth*/Z_DIM,
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
		return (false);
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

	viewDirection = -glm::vec3(MV[0][2], MV[1][2], MV[2][2]);	//get the current view direction vector

	vmath::mat4 T_mat = vmath::translate(0.0f, 0.0f, dist);
	vmath::mat4 MV_mat = T_mat;

	viewDirection_v3 = -(vmath::vec3(MV_mat[0][2], MV_mat[1][2], MV_mat[2][2]));


	fprintf(gpFile, "LoadVolumeData() Success Step2 and Last\n"); fflush(gpFile);


	return (true);
}

void SampleVoxel(const int x, const int y, const int z, glm::vec3 scale)
{
	GLubyte cubeCornerValues[8];
	int flagIndex, edgeFlags, i;
	glm::vec3 edgeVertices[12];
	glm::vec3 edgeNormals[12];

	//Make a local copy of the values at the cube's corners
	for (i = 0; i < 8; i++)
	{
		cubeCornerValues[i] = SampleVolume(x + (int)(a2fVertexOffset[i][0] * scale.x),
			y + (int)(a2fVertexOffset[i][1] * scale.y),
			z + (int)(a2fVertexOffset[i][2] * scale.z));
	}

	//Find which vertices are inside of the surface and which are outside
	//Obtain a flagIndex based on if the value at the cube vertex is less 
	//than the given isovalue
	flagIndex = 0;
	for (i = 0; i < 8; i++)
	{
		if (cubeCornerValues[i] <= isoValue)
			flagIndex |= 1 << i;
	}

	//Find which edges are intersected by the surface
	edgeFlags = aiCubeEdgeFlags[flagIndex];

	//If the cube is entirely inside or outside of the surface, then there will be no intersections
	if (edgeFlags == 0)
	{
		return;
	}

	//for all edges
	for (i = 0; i < 12; i++)
	{
		//if there is an intersection on this edge
		if (edgeFlags & (1 << i))
		{
			//get the offset 
			float offset = GetOffset(cubeCornerValues[a2iEdgeConnection[i][0]], cubeCornerValues[a2iEdgeConnection[i][1]]);

			//use offset to get the vertex position
			edgeVertices[i].x = x + (a2fVertexOffset[a2iEdgeConnection[i][0]][0] + offset * a2fEdgeDirection[i][0]) * scale.x;
			edgeVertices[i].y = y + (a2fVertexOffset[a2iEdgeConnection[i][0]][1] + offset * a2fEdgeDirection[i][1]) * scale.y;
			edgeVertices[i].z = z + (a2fVertexOffset[a2iEdgeConnection[i][0]][2] + offset * a2fEdgeDirection[i][2]) * scale.z;

			//use the vertex position to get the normal
			edgeNormals[i] = GetNormal((int)edgeVertices[i].x, (int)edgeVertices[i].y, (int)edgeVertices[i].z);
		}
	}

	//Draw the triangles that were found.  There can be up to five per cube
	for (i = 0; i < 5; i++)
	{
		if (a2iTriangleConnectionTable[flagIndex][3 * i] < 0)
			break;

		for (int j = 0; j < 3; j++)
		{
			int vertex = a2iTriangleConnectionTable[flagIndex][3 * i + j];
			Vertex v;
			v.normal = (edgeNormals[vertex]);
			v.pos = (edgeVertices[vertex]) * invDim;
			vertices.push_back(v);
		}
	}
}

void MarchVolume()
{
	vertices.clear();
	int dx = XDIM_TM / X_SAMPLING_DIST;
	int dy = YDIM_TM / Y_SAMPLING_DIST;
	int dz = ZDIM_TM / Z_SAMPLING_DIST;
	glm::vec3 scale = glm::vec3(dx, dy, dz);
	for (int z = 0; z < ZDIM_TM; z += dz)
	{
		for (int y = 0; y < YDIM_TM; y += dy)
		{
			for (int x = 0; x < XDIM_TM; x += dx)
			{
				SampleVoxel(x, y, z, scale);
			}
		}
	}
}

size_t GetTotalVertices_TM()
{
	return vertices.size();
}

Vertex* GetVertexPointer_TM()
{
	return  &vertices[0];
}

GLubyte SampleVolume(const int x, const int y, const int z) {
	int index = (x + (y * XDIM_TM)) + z * (XDIM_TM * YDIM_TM);
	if (index < 0)
		index = 0;
	if (index >= XDIM_TM * YDIM_TM * ZDIM_TM)
		index = (XDIM_TM * YDIM_TM * ZDIM_TM) - 1;
	return pVolume[index];
}

glm::vec3 GetNormal(const int x, const int y, const int z) {
	glm::vec3 N;
	N.x = (SampleVolume(x - 1, y, z) - SampleVolume(x + 1, y, z)) * 0.5f;
	N.y = (SampleVolume(x, y - 1, z) - SampleVolume(x, y + 1, z)) * 0.5f;
	N.z = (SampleVolume(x, y, z - 1) - SampleVolume(x, y, z + 1)) * 0.5f;
	return glm::normalize(N);
}
float GetOffset(const GLubyte v1, const GLubyte v2) {
	float delta = (float)(v2 - v1);
	if (delta == 0)
		return 0.5f;
	else
		return (isoValue - v1) / delta;
}

void Render_MarchingTetrahedra(void)
{
	// local:

	//set the modelling transform to move the marching result to origin
	glm::mat4 ModelViewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, dist));

	ModelViewMatrix = glm::rotate(ModelViewMatrix, glm::radians(rotationX), glm::vec3(1.0f, 0.0f, 0.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, glm::radians(rotationY), glm::vec3(0.0f, 1.0f, 0.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, rotationZ, glm::vec3(0.0f, 0.0f, 1.0f));

	glm::mat4 modelViewProjectionMatrix = perspectiveProjMatrix_glm * ModelViewMatrix;
	glm::vec3 cameraPosition = glm::vec3(glm::inverse(ModelViewMatrix) * glm::vec4(0.0, 0.0, 0.0, 1.0));
	
	// code:

	////// # OPTIONAL BLOCK to draw  GRID LINES //////////
	glUseProgram(shaderProgramObject_Grid);
	{
		glUniformMatrix4fv(mvpUniform_GridObject, 1, GL_FALSE, glm::value_ptr(modelViewProjectionMatrix));
		glUniform4f(glGetUniformLocation(shaderProgramObject_Grid, "u_Color"), 1.0f, 1.0f, 0.0f, 1.0f);
		glBindVertexArray(vaoID);
		glDrawElements(/*Primitives*/GL_LINES, /*count*/totalIndices,/*Type*/GL_UNSIGNED_INT,/*void *indices*/0);
		glBindVertexArray(0);
	}
	glUseProgram(0);


	Render_Volume_Box_Axes(modelViewProjectionMatrix);


	if (bWireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glm::mat4 TranslationMatrix_2 = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, -0.5f, -0.5f));

	glUseProgram(shaderProgramObject_TM);
	{
		glUniformMatrix4fv(modelViewProjectionUniform_TM, 1, GL_FALSE, glm::value_ptr(modelViewProjectionMatrix * TranslationMatrix_2));//pass the shader uniform

		glBindVertexArray(volumeMarcherVAO);
		glDrawArrays(GL_TRIANGLES, 0, GetTotalVertices_TM());
		glBindVertexArray(0);
		//glBindTexture(GL_TEXTURE_3D, 0);
	}
	glUseProgram(0);

	//restore the default polygon mode
	if (bWireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

}

void Update_MarchingTetrahedra()
{
	// code:
}

void Uninitialize_MarchingTetrahedra(void)
{
	// code:


	if (volumeMarcherVBO)
	{
		glDeleteBuffers(1, &volumeMarcherVBO);
		volumeMarcherVBO = 0;
	}

	if (volumeMarcherVAO)
	{
		glDeleteVertexArrays(1, &volumeMarcherVAO);
		volumeMarcherVAO = 0;
	}

	if (shaderProgramObject_TM)
	{
		glUseProgram(shaderProgramObject_TM);
		{
			GLsizei numAttachedShaders;
			glGetProgramiv(shaderProgramObject_TM, GL_ATTACHED_SHADERS, &numAttachedShaders);
			GLuint* shaderObjects = NULL;
			shaderObjects = (GLuint*)malloc(numAttachedShaders * sizeof(GLuint));
			glGetAttachedShaders(shaderProgramObject_TM, numAttachedShaders, &numAttachedShaders, shaderObjects);
			for (GLsizei i = 0; i < numAttachedShaders; i++)
			{
				glDetachShader(shaderProgramObject_TM, shaderObjects[i]);
				glDeleteShader(shaderObjects[i]);
				shaderObjects[i] = 0;
			}
			free(shaderObjects);
			shaderObjects = NULL;
		}
		glUseProgram(0);
		glDeleteProgram(shaderProgramObject_TM);

		shaderProgramObject_TM = 0;
	}

	if (pVolume)
	{
		free(pVolume);
		pVolume = NULL;
	}

	if (!vertices.empty())
	{
		vertices.clear();
	}
}







/************** SPLATTING METHOD FUNCTION DEFINITIONS ****************/

//! DONE
int VolumeSplatterConstructor(void)
{
	// code:

	/* ALEADY DONE */
	////XDIM = 256;
	////YDIM = 256;
	////ZDIM = 256;
	/* ALEADY DONE */
	pVolume_Splatting = NULL;


	return (0);
}


//! DONE
void SetVolumeDimensions_Splatting(int X_DIM, int Y_DIM, int Z_DIM)
{
	// code:

	/* ALEADY DONE */
	//XDIM = xdim;
	//YDIM = ydim;
	//ZDIM = zdim;
	/* ALEADY DONE */
	invDim.x = 1.0f / X_DIM;
	invDim.y = 1.0f / Y_DIM;
	invDim.z = 1.0f / Z_DIM;
}


//! DONE
bool LoadVolume_Splatting(const std::string volumeFileName)
{
	// code:

	std::ifstream infile(volumeFileName.c_str(), std::ios_base::binary);

	if (infile.good())
	{
		pVolume_Splatting = new GLubyte[XDIM * YDIM * ZDIM];
		infile.read(reinterpret_cast<char*>(pVolume_Splatting), XDIM * YDIM * ZDIM * sizeof(GLubyte));
		infile.close();
		return true;
	}
	else
	{
		return false;
	}

}


//! DONE
void SetIsoSurfaceValue(int isoValue)
{
	// code:

	isoValue_Splatting = isoValue;

}


//! DONE
void SetNumSamplingVoxels_Splatting(int x, int y, int z)
{
	// code:
	X_SAMPLING_DIST = x;
	Y_SAMPLING_DIST = y;
	Z_SAMPLING_DIST = z;

}


//! DONE 


/* splatter->SplatVolume();*/

/* START of  GenerateSplatVolumes() Related Functions or  splatter->SplatVolume(); */

void SampleVoxel_Splatting(int x, int y, int z)
{
	// code:

}

void GenerateSplatVolumes(void)
{
	// local:
	const unsigned int dx = XDIM / X_SAMPLING_DIST;
	const unsigned int dy = YDIM / Y_SAMPLING_DIST;
	const unsigned int dz = ZDIM / Z_SAMPLING_DIST;

	// code:

	vertices_Splatting.clear();
	vertices.reserve((X_SAMPLING_DIST + 1) * (Y_SAMPLING_DIST + 1) * (Z_SAMPLING_DIST + 1));
	scaleMatrix_splat = glm::vec3(dx,dy,dz);

	for (unsigned int z = 0; z < ZDIM; z += dz)
	{
		for (unsigned int y = 0; y < YDIM; y += dy)
		{
			for (unsigned int x = 0; x < XDIM; x += dx)
			{
				SampleVoxel_Splatting(x, y, z);
			}
		}
	}

}

GLubyte SampleVolume_Splatting(unsigned int x, unsigned int y, unsigned int z)
{
	// local:

	GLubyte newData = NULL;

	// code:


	return (newData);
}

void SampleVoxel_Splatting(unsigned int x, unsigned int y, unsigned int z)
{
	// code:

	GLubyte data = SampleVolume_Splatting(x, y, z);

	if (data > isoValue_Splatting)
	{
		Vertex v;
		v.pos.x = (float)x;
		v.pos.y = (float)y;
		v.pos.z = (float)z;
		v.normal = GetNormal_Splatting(x, y, z);
		v.pos *= invDim_Splatting;
		vertices_Splatting.push_back(v);
	}

}

glm::vec3 GetNormal_Splatting(int x, int y, int z)
{
	// code:
	glm::vec3 N;
	N.x = SampleVolume_Splat(int(x - scaleMatrix_splat.x), y, z) - SampleVolume_Splat(int(x + scaleMatrix_splat.x), y, z) / (2 * scaleMatrix_splat.x);
	N.y = SampleVolume_Splat(x, int(y - scaleMatrix_splat.y), z) - SampleVolume_Splat(x, int(y + scaleMatrix_splat.y), z) / (2 * scaleMatrix_splat.y);
	N.z = SampleVolume_Splat(x, y, int(z - scaleMatrix_splat.z)) - SampleVolume_Splat(x, y, int(z + scaleMatrix_splat.z)) /(2*scaleMatrix_splat.z);

	return (glm::normalize(N));
}

GLubyte SampleVolume_Splat(const int x, const int y, const int z)
{
	// code:

	int index = (x + (y * XDIM)) + z * (XDIM * YDIM);

	if (index < 0)
	{
		index = 0;
	}
	
	if (index >= XDIM*YDIM*ZDIM)
	{
		index = (XDIM * YDIM * ZDIM-1);
	}

	return (pVolume_Splatting[index]);
}


/* END of  GenerateSplatVolumes() Related Functions or  splatter->SplatVolume(); */


size_t GetTotalVertices_Splat()
{
	return vertices.size();
}

Vertex* GetVertexPointer_Splat()
{
	return  &vertices[0];
}


void setup_VolumeSplatter_VAO(void)
{
	// code:

	glGenVertexArrays(1, &volumeSplatter_VAO);
	glBindVertexArray(volumeSplatter_VAO);
	{
		glGenBuffers(1, &volumeSplatter_VBO);
		glBindBuffer(GL_ARRAY_BUFFER,volumeSplatter_VBO);
		{
			glBufferData(GL_ARRAY_BUFFER, GetTotalVertices_Splat() * sizeof(Vertex), GetVertexPointer_Splat(),GL_STATIC_DRAW);
			glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
			glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

			glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);
			glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *)offsetof(Vertex, normal));
		}
		glBindBuffer(GL_ARRAY_BUFFER,0);
	}
	glBindVertexArray(0);

}


int init_Shader_for_SplattingS(void)
{
	// prototype:
	void uninitialize(void);

	// local:

	GLuint vertexShaderObject;
	GLuint fragmentShaderObject;

	GLint status;
	GLint infoLogLength;
	char* Log = NULL;


	// code:

	//* ///////////////////// # VERTEX SHADER # ////////////////////////
	const GLchar* vertexShaderSource = R"(
			
			#version 460 core

			// IN :
			in vec3 vVertex;
			in vec3 vNormal;

			// UNIFORM:
			uniform mat4 u_MVMatrix;
			uniform mat3 u_NormalMatrix;
			uniform mat4 u_ProjetionMatrix;
			uniform float u_SplatSize;

			// OUT:
			smooth out vec3 outNormal;

			void main()
			{
				// calculate eyeCoordinates in vertex position:
				vec4 eyeCoordinates = u_MVMatrix * vec4(vVertex,1.0);

				// calculate splat size :
				gl_PointSize = 2.0 * u_SplatSize/-eyeCoordinates.z;

				// calculate vertices in clipspace position:
				gl_Position = u_ProjetionMatrix * u_MVMatrix * vec4(vVertex,1.0);

				outNormal = u_NormalMatrix * vNormal;
			}
			)";

	vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderObject, 1, (const GLchar**)&vertexShaderSource, NULL);

	glCompileShader(vertexShaderObject);
	glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			Log = (char*)malloc(infoLogLength);
			if (Log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(vertexShaderObject, infoLogLength, &written, Log);
				fprintf(gpFile, "ERROR: Splatting Vertex Shader Compilation Log : %s\n", Log);
				free(Log);
				Log = NULL;
				uninitialize();
			}
		}
	}
	else
	{
		fprintf(gpFile, "SUCCESS: Splatting Vertex Shader Compilation \n");
	}



	//* ///////////////////// # FRAGMENT SHADER # ////////////////////////
	const GLchar* fragmentShaderSource = R"(
			
			#version 460 core

			// IN:
			smooth in vec3 outNormal;

			// UNIFORMS / CONSTANTS:

			const vec3 lightVector = vec3(0.0,0.0,1.0);
			const vec3 viewVector = lightVector;
			const vec4 diffuse_color = vec4(0.75,0.5,0.5,1.0); // surface diffuse color
			const vec4 specular_color = vec4(1.0);		// surface specular color


			// OUT:
			out vec4 FragColor;

			void main(void)
			{      

				// normalized eyeCoordinates:
				vec3 normalizedEyeCoordinates = normalize(outNormal);

				// get point coordinate and check if the current fragment is outside the sphere:
				 vec2 pointCoordinate = gl_PointCoord * 2.0 - vec2(1.0);
				float fMag = dot(pointCoordinate.xy,pointCoordinate.xy);

				// discard fragment if its outside, this will render splat as circular point:
				if(fMag > 1)
				{
					discard;
				}

				// calculate diffuse and specular components:
				float fDiffuse = max(0.0,dot(normalizedEyeCoordinates,lightVector));
				vec3 halfVector = normalize(lightVector + viewVector);
				float fSpecular = pow(max(0.0,dot(halfVector,normalizedEyeCoordinates)),400);

				FragColor = (fSpecular  * specular_color) + (fDiffuse * diffuse_color);

			}
			)";
	fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderObject, 1, (const GLchar**)&fragmentShaderSource, NULL);

	glCompileShader(fragmentShaderObject);

	status = 0;
	infoLogLength = 0;
	Log = NULL;
	glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			Log = (char*)malloc(infoLogLength);
			if (Log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(fragmentShaderObject, infoLogLength, &written, Log);
				fprintf(gpFile, "ERROR: Splatting FRAGMENT Shader Compilation Log : %s\n", Log);
				free(Log);
				Log = NULL;
				uninitialize();
			}
		}
	}
	else
	{
		fprintf(gpFile, "SUCCESS: Splatting FRAGMENT Shader Compilation \n");
	}

	shaderProgramObject_Splatting = glCreateProgram();


	glAttachShader(shaderProgramObject_Splatting, vertexShaderObject);
	glAttachShader(shaderProgramObject_Splatting, fragmentShaderObject);

	// MOVED BELOW

	status = 0;
	infoLogLength = 0;
	Log = NULL;
	glLinkProgram(shaderProgramObject_Splatting);
	glGetProgramiv(shaderProgramObject_Splatting, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObject_Splatting, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			Log = (char*)malloc(infoLogLength);
			if (Log != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(shaderProgramObject_Splatting, infoLogLength, &written, Log);
				fprintf(gpFile, "ERROR: Splatting ShaderProgram Object  Linking Log : %s\n", Log);
				free(Log);
				uninitialize();
				return FALSE;
			}
		}
	}
	else
	{
		fprintf(gpFile, "SUCCESS: Splatting ShaderProgram ObjectLinking Successful \n");
	}

	glUseProgram(shaderProgramObject_Splatting);
	{
		glBindAttribLocation(shaderProgramObject_Splatting, AMC_ATTRIBUTE_POSITION, "vVertex");
		glBindAttribLocation(shaderProgramObject_Splatting, AMC_ATTRIBUTE_NORMAL, "vNormal");

		mvMatrixUniform_Splatting = glGetUniformLocation(shaderProgramObject_Splatting, "u_MVMatrix");
		normalMatrixUniform_Splatting = glGetUniformLocation(shaderProgramObject_Splatting, "u_NormalMatrix");
		projectionMatrixUniform_Splatting = glGetUniformLocation(shaderProgramObject_Splatting, "u_ProjetionMatrix");
		splatSizeUniform = glGetUniformLocation(shaderProgramObject_Splatting, "u_SplatSize");

		glUniform1f(splatSizeUniform, fSplatSize);
	}
	glUseProgram(0);


	return (0);
}


int init_Shader_for_Gaussian_H(void)
{
	// prototype:
	void uninitialize(void);

	// local:

	GLuint vertexShaderObject;
	GLuint fragmentShaderObject;

	GLint status;
	GLint infoLogLength;
	char* Log = NULL;


	// code:

	//* ///////////////////// # VERTEX SHADER # ////////////////////////
	const GLchar* vertexShaderSource = R"(
			
			#version 460 core

			// IN:
			in vec2 vVertex;


			// OUT:
			smooth out vec2 vUV;

			void main()
			{

				// calc gl_Position:
				gl_Position = vec4(vVertex * 2.0 - 1.0,0.0,1.0);

				// use position vertices as texcoords
				vUV = vVertex;

			}
			)";

	vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderObject, 1, (const GLchar**)&vertexShaderSource, NULL);

	glCompileShader(vertexShaderObject);
	glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			Log = (char*)malloc(infoLogLength);
			if (Log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(vertexShaderObject, infoLogLength, &written, Log);
				fprintf(gpFile, "ERROR: Pass Through VS for Gaussian_H Shader Compilation Log : %s\n", Log);
				free(Log);
				Log = NULL;
				uninitialize();
			}
		}
	}
	else
	{
		fprintf(gpFile, "SUCCESS: Gaussian_H Vertex Shader Compilation \n");
	}



	//* ///////////////////// # FRAGMENT SHADER # ////////////////////////
	const GLchar* fragmentShaderSource = R"(
			
			#version 460 core

			// IN:
			smooth in vec2 vUV;

			// UNIFORMS / CONSTANTS:
			uniform sampler2D u_textureMap;

			const float kernel[]=float[21] 
								(
								0.000272337,  0.00089296, 0.002583865, 0.00659813,  0.014869116,
								0.029570767, 0.051898313, 0.080381679, 0.109868729, 0.132526984, 
								0.14107424,  0.132526984, 0.109868729, 0.080381679, 0.051898313, 
								0.029570767, 0.014869116, 0.00659813,  0.002583865, 0.00089296, 0.000272337
								);
 
             
			// OUT:
			out vec4 FragColor;


			void main(void)
			{
				// calculate inverse of texture size:
				vec2 delta = 1.0/textureSize(u_textureMap,0);
				vec4 color = vec4(0.0);
				int index = 20;

				// iterate for all neighbors and multiply kernel with texture value:
				for(int i=-10;i <= 10;i++)
				{
					color+= kernel[index--] * texture(u_textureMap,vUV + (vec2(i * delta.x,0.0)));
				}

				FragColor = color;

			}
			)";

	fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderObject, 1, (const GLchar**)&fragmentShaderSource, NULL);

	glCompileShader(fragmentShaderObject);

	status = 0;
	infoLogLength = 0;
	Log = NULL;
	glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			Log = (char*)malloc(infoLogLength);
			if (Log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(fragmentShaderObject, infoLogLength, &written, Log);
				fprintf(gpFile, "ERROR: Gaussian_H FRAGMENT Shader Compilation Log : %s\n", Log);
				free(Log);
				Log = NULL;
				uninitialize();
			}
		}
	}
	else
	{
		fprintf(gpFile, "SUCCESS: Gaussian_H FRAGMENT Shader Compilation \n");
	}

	shaderProgramObject_Gaussian_H = glCreateProgram();


	glAttachShader(shaderProgramObject_Gaussian_H, vertexShaderObject);
	glAttachShader(shaderProgramObject_Gaussian_H, fragmentShaderObject);

	// MOVED BELOW

	status = 0;
	infoLogLength = 0;
	Log = NULL;
	glLinkProgram(shaderProgramObject_Gaussian_H);
	glGetProgramiv(shaderProgramObject_Gaussian_H, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObject_Gaussian_H, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			Log = (char*)malloc(infoLogLength);
			if (Log != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(shaderProgramObject_Gaussian_H, infoLogLength, &written, Log);
				fprintf(gpFile, "ERROR: Gaussian_H ShaderProgram Object  Linking Log : %s\n", Log);
				free(Log);
				uninitialize();
				return FALSE;
			}
		}
	}
	else
	{
		fprintf(gpFile, "SUCCESS: Gaussian_H ShaderProgram ObjectLinking Successful \n");
	}

	glUseProgram(shaderProgramObject_Gaussian_H);
	{
		glBindAttribLocation(shaderProgramObject_Gaussian_H, AMC_ATTRIBUTE_POSITION, "vVertex");

		splatTextureMapSampler = glGetUniformLocation(shaderProgramObject_Splatting, "u_textureMap");
		glUniform1i(splatTextureMapSampler, 1);
	}
	glUseProgram(0);


	return (0);
}


int init_Shader_for_Gaussian_V(void)
{
	// prototype:
	void uninitialize(void);

	// local:

	GLuint vertexShaderObject;
	GLuint fragmentShaderObject;

	GLint status;
	GLint infoLogLength;
	char* Log = NULL;


	// code:

	//* ///////////////////// # VERTEX SHADER # ////////////////////////
	const GLchar* vertexShaderSource = R"(
			
			#version 460 core

			// IN:
			in vec2 vVertex;

			// OUT:
			smooth out vec2 vUV;

			void main()
			{
				gl_Position = vec4(vVertex * 2.0-1.0,0.0,1.0);
			}
			)";

	vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderObject, 1, (const GLchar**)&vertexShaderSource, NULL);

	glCompileShader(vertexShaderObject);
	glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			Log = (char*)malloc(infoLogLength);
			if (Log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(vertexShaderObject, infoLogLength, &written, Log);
				fprintf(gpFile, "ERROR: Pass Through VS for Gaussian_V Compilation Log : %s\n", Log);
				free(Log);
				Log = NULL;
				uninitialize();
			}
		}
	}
	else
	{
		fprintf(gpFile, "SUCCESS: Pass Through VS for Gaussian_V Compilation \n");
	}



	//* ///////////////////// # FRAGMENT SHADER # ////////////////////////
	const GLchar* fragmentShaderSource = R"(
			
			#version 460 core

			// IN:
			smooth in vec2 vUV;

			// UNIFORM / CONSTANTS:

			uniform sampler2D u_textureMap;
			const float kernel[]=float[21] (
								0.000272337,  0.00089296, 0.002583865, 0.00659813,  0.014869116,
								0.029570767, 0.051898313, 0.080381679, 0.109868729, 0.132526984, 
								0.14107424,  0.132526984, 0.109868729, 0.080381679, 0.051898313, 
								0.029570767, 0.014869116, 0.00659813,  0.002583865, 0.00089296, 0.000272337
								);


			// OUT:

			out vec4 FragColor;

			void main(void)
			{

				vec2 delta = 1.0/textureSize(u_textureMap,0);
				vec4 color = vec4(0.0);
				int index =20;

				for(int i = -10;i<=10;i++)
				{
					color += kernel[index--]*texture(u_textureMap,vUV+(vec2(0,i*delta.y)));
				}

				FragColor = color;
			}
			)";

	fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderObject, 1, (const GLchar**)&fragmentShaderSource, NULL);

	glCompileShader(fragmentShaderObject);

	status = 0;
	infoLogLength = 0;
	Log = NULL;
	glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			Log = (char*)malloc(infoLogLength);
			if (Log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(fragmentShaderObject, infoLogLength, &written, Log);
				fprintf(gpFile, "ERROR: Gaussian_V FRAGMENT Shader Compilation Log : %s\n", Log);
				free(Log);
				Log = NULL;
				uninitialize();
			}
		}
	}
	else
	{
		fprintf(gpFile, "SUCCESS: Gaussian_V FRAGMENT Shader Compilation \n");
	}

	shaderProgramObject_Gaussian_V = glCreateProgram();


	glAttachShader(shaderProgramObject_Gaussian_V, vertexShaderObject);
	glAttachShader(shaderProgramObject_Gaussian_V, fragmentShaderObject);

	// MOVED BELOW

	status = 0;
	infoLogLength = 0;
	Log = NULL;
	glLinkProgram(shaderProgramObject_Gaussian_V);
	glGetProgramiv(shaderProgramObject_Gaussian_V, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObject_Gaussian_V, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			Log = (char*)malloc(infoLogLength);
			if (Log != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(shaderProgramObject_Gaussian_V, infoLogLength, &written, Log);
				fprintf(gpFile, "ERROR: Gaussian_V ShaderProgram Object  Linking Log : %s\n", Log);
				free(Log);
				uninitialize();
				return FALSE;
			}
		}
	}
	else
	{
		fprintf(gpFile, "SUCCESS: Gaussian_V ShaderProgram ObjectLinking Successful \n");
	}

	glUseProgram(shaderProgramObject_Gaussian_V);
	{
		glBindAttribLocation(shaderProgramObject_Gaussian_H, AMC_ATTRIBUTE_POSITION, "vVertex");

		splatTextureMapSampler_V=glGetUniformLocation(shaderProgramObject_Gaussian_H, "u_textureMap");
		glUniform1i(splatTextureMapSampler_V,0);
	}
	glUseProgram(0);


	return (0);
}


int init_Shader_for_Quad(void)
{
	// prototype:
	void uninitialize(void);

	// local:

	GLuint vertexShaderObject;
	GLuint fragmentShaderObject;

	GLint status;
	GLint infoLogLength;
	char* Log = NULL;


	// code:

	//* ///////////////////// # VERTEX SHADER # ////////////////////////
	const GLchar* vertexShaderSource = R"(
			
			#version 460 core

			// IN:
			in vec2 vVertex;


			// OUT:
			smooth out vec2 vUV;

			void main()
			{
				gl_Position = vec4(vVertex.xy * 2.0-1.0,0.0,1.0);

				vUV = vVertex;
			}
			)";

	vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderObject, 1, (const GLchar**)&vertexShaderSource, NULL);

	glCompileShader(vertexShaderObject);
	glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			Log = (char*)malloc(infoLogLength);
			if (Log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(vertexShaderObject, infoLogLength, &written, Log);
				fprintf(gpFile, "ERROR: Splatting Quad Vertex Shader Compilation Log : %s\n", Log);
				free(Log);
				Log = NULL;
				uninitialize();
			}
		}
	}
	else
	{
		fprintf(gpFile, "SUCCESS: Splatting Quad Vertex Shader Compilation \n");
	}



	//* ///////////////////// # FRAGMENT SHADER # ////////////////////////
	const GLchar* fragmentShaderSource = R"(
			
			#version 460 core

			// IN:
			smooth in vec2 vUV;

			// UNIFORM / CONSTANTS:
			uniform sampler2D u_textureMap;

			// OUT:
			out vec4 FragColor;

			void main(void)
			{
				FragColor = texture(u_textureMap,vUV);
			}
			)";

	fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderObject, 1, (const GLchar**)&fragmentShaderSource, NULL);

	glCompileShader(fragmentShaderObject);

	status = 0;
	infoLogLength = 0;
	Log = NULL;
	glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			Log = (char*)malloc(infoLogLength);
			if (Log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(fragmentShaderObject, infoLogLength, &written, Log);
				fprintf(gpFile, "ERROR: Splatting Quad FRAGMENT Shader Compilation Log : %s\n", Log);
				free(Log);
				Log = NULL;
				uninitialize();
			}
		}
	}
	else
	{
		fprintf(gpFile, "SUCCESS: Splatting Quad FRAGMENT Shader Compilation \n");
	}

	shaderProgramObject_Splatting_Quad = glCreateProgram();


	glAttachShader(shaderProgramObject_Splatting_Quad, vertexShaderObject);
	glAttachShader(shaderProgramObject_Splatting_Quad, fragmentShaderObject);

	// MOVED BELOW

	status = 0;
	infoLogLength = 0;
	Log = NULL;
	glLinkProgram(shaderProgramObject_Splatting_Quad);
	glGetProgramiv(shaderProgramObject_Splatting_Quad, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObject_Splatting_Quad, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			Log = (char*)malloc(infoLogLength);
			if (Log != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(shaderProgramObject_Splatting_Quad, infoLogLength, &written, Log);
				fprintf(gpFile, "ERROR: Splatting Quad ShaderProgram Object  Linking Log : %s\n", Log);
				free(Log);
				uninitialize();
				return FALSE;
			}
		}
	}
	else
	{
		fprintf(gpFile, "SUCCESS: Splatting Quad ShaderProgram ObjectLinking Successful \n");
	}


	glUseProgram(shaderProgramObject_Splatting_Quad);
	{
		glBindAttribLocation(shaderProgramObject_Splatting_Quad, AMC_ATTRIBUTE_POSITION, "vVertex");
		glGetUniformLocation(shaderProgramObject_Splatting_Quad, "u_MVPMatrix");
		textureSampler_Quad = glGetUniformLocation(shaderProgramObject_Splatting_Quad, "u_textureMap");
		glUniform1i(textureSampler_Quad,2);
	}
	glUseProgram(0);


	return (0);
}


void setup_Textures(void)
{
	// local:

	// code:

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

	//glGenFramebuffers

}


void setup_Filter_and_FBOs(void)
{
	// local:

	// code:

}


void setup_Quad_VAO(void)
{
	// local:

	// code:

}


void uninitialize_Splatting(void)
{
	// local:

	// code:

}


void Uninitialize_ShaderProgramObject(GLuint shaderProgramObject_)
{
	// code:

	if (shaderProgramObject_)
	{
		glUseProgram(shaderProgramObject_);
		GLint numShaders = 0;
		glGetProgramiv(shaderProgramObject_, GL_ATTACHED_SHADERS, &numShaders);
		if (numShaders > 0)
		{
			GLuint* pShaders = (GLuint*)malloc(numShaders * sizeof(GLuint));
			if (pShaders != NULL)
			{
				glGetAttachedShaders(shaderProgramObject_, numShaders, &numShaders, pShaders);
				for (GLint i = 0; i < numShaders; i++)
				{
					glDetachShader(shaderProgramObject_, pShaders[i]);
					glDeleteShader(pShaders[i]);
					pShaders[i] = 0;
				}
				free(pShaders);
			}
		}
		glUseProgram(0);
		glDeleteProgram(shaderProgramObject_);
		shaderProgramObject_ = 0;
	}

}



BOOL LoadPNGImage_New(GLuint* texture, const char* imageFilePath)
{
	// variable declarations
	int image_width = 0;
	int image_height = 0;
	int numImageComponents = 0;
	unsigned char* image_data = NULL;
	GLenum image_format = 0;

	// code
	glGenTextures(1, texture);
	image_data = stbi_load(imageFilePath, &image_width, &image_height, &numImageComponents, 0);
	if (image_data != NULL)
	{
		if (numImageComponents == 1)
			image_format = GL_RED;
		else if (numImageComponents == 3)
			image_format = GL_RGB;
		else if (numImageComponents == 4)
			image_format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, *texture);

		if (image_format == GL_RGBA)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}

		else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, image_format, image_width, image_height, 0, image_format, GL_UNSIGNED_BYTE, (void*)image_data);
		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(image_data);
		return(TRUE);
	}
	return(FALSE);
}



BOOL LoadPNGImage(GLuint* texture, const char* imageFilePath)
{
	// variable declarations
	int image_width = 0;
	int image_height = 0;
	int numImageComponents = 0;
	unsigned char* image_data = NULL;
	GLenum image_format = 0;

	// code

	image_data = stbi_load(imageFilePath, &image_width, &image_height, &numImageComponents, 0);

	if (image_data == NULL)
	{
		fprintf(gpFile,
			"ERROR : stbi_load() failed for %s\n",
			imageFilePath);

		return(FALSE);
	}

	fprintf(gpFile,
		"PNG loaded: %s  Width=%d Height=%d OriginalChannels=%d\n",
		imageFilePath,
		image_width,
		image_height,
		numImageComponents);

	glGenTextures(1, texture);
	
	if (image_data != NULL)
	{
		if (numImageComponents == 1)
			image_format = GL_RED;
		else if (numImageComponents == 3)
			image_format = GL_RGB;
		else if (numImageComponents == 4)
			image_format = GL_RGBA;

		fprintf(gpFile, "MESSAGE : ***** inside LoadPNGImage():  numImageComponents = %d  \n", numImageComponents);

		glBindTexture(GL_TEXTURE_2D, *texture);

		if (image_format == GL_RGBA)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}

		else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)image_data);
		//glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);

		fprintf(gpFile, "MESSAGE : ***** inside LoadPNGImage():  *texture = %d  \n", *texture);

		stbi_image_free(image_data);

		fprintf(gpFile,
			"Texture created successfully: ID=%u\n",
			*texture
		);

		return (TRUE);
	}
	return (FALSE);
}




BOOL LoadGLTexture(GLuint* Texture, TCHAR ImageResourceID[])
{
	// local:

	HBITMAP hBitmap = NULL;
	BITMAP bmp;
	BOOL bResult = FALSE;

	// code:

	// Load The Image:
	hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL),	//hInstance for Current Window
		ImageResourceID,	// LPCSTR type of Resource ID
		IMAGE_BITMAP,	// Type of our Resource / Image
		0,		// Width of Image (It is Zero bcz its NA for Bitmaps, only Applicable for ICONs)
		0,		// Height of Image (Zero means When our TEXTURE is RESOURCE->so it can be calculated from it.)
		LR_CREATEDIBSECTION		// Image Kashi LoadKaru: LR=>Load Resource,DIB=>Device Independant Bitmap Section.(so we load this as Resource into a DeviceIndependantBitmap Section.
	);
	if (hBitmap == NULL)
	{
		fprintf(gpFile, "LoadImage Failed for hbitmap");
		return (FALSE);
	}


	// Load the Image Data:
	GetObject(hBitmap,	// HANDLE to GDI Object
		sizeof(BITMAP),	// size of BITMAP
		&bmp	//LPtr to Void=> OUT for BITMAP variable (Can be typecasted to BITMAP)
	);

	// # OpenGL Code now #

	//# Step 6: Create OpenGL Texture Object + 7: Bind with Created OpenGL Texture
	glGenTextures(1	// how many Textures
		, Texture);	// Ptr to Texture Variable
	glBindTexture(GL_TEXTURE_2D, *Texture);
	{
		//# Step 8: Decide Image Pixel Alignment And Unpacking: (Pixel Kasa Vaparu )
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // (4 is Good for RGBA Alignment) BUT (1 is Good for Performance)
		// # Step 9: Set Texture Parameters  // (Texture Kasa Vaparu )
		glTexParameteri(GL_TEXTURE_2D,	// Binding Point for our Texture
			GL_TEXTURE_MAG_FILTER,	//Parameter to Send to our Texture
			GL_LINEAR	// go to Binding Point and MagFilter and give Image Data Linearly( AS IT IS)
		);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_LINEAR		// Value of Min Filter->how to send MIN FILTER
		);

		// # Step 10: Create Multiple Mipmap Images # 

			// Old Function gluBuild2DMipmaps = glTexImage2D + glGenerateMipmap

			// now we use glTexImage2D(....)
		glTexImage2D(
			GL_TEXTURE_2D, //TARGET
			0,//MipMap Level for MultiSampling(default-0)
			GL_RGB,	//Internal Image/Pixel FORMAT Components->RGB->3(BMP is Always RGB)
			bmp.bmWidth,
			bmp.bmHeight,
			0, //BorderWidth
			GL_BGR, //DIBitmaps Need to be HR-Flipped by OGL when its from Windows OS
			GL_UNSIGNED_BYTE,	// Our Data will be always Positive Bytes
			(void*)bmp.bmBits	// our Data 
		);

		glGenerateMipmap(GL_TEXTURE_2D);

	}
	glBindTexture(GL_TEXTURE_2D, 0);	//# Step 11: UnBind the Texture.


	//# Step 12: Delete Image Resource
	DeleteObject(hBitmap);
	hBitmap = NULL;

	return TRUE;
}



void initi_HUDControls_data(void)
{
	// code:


	//! LEFT ARROW:
	hLEFT_Arrow.x = 20.0f;
	hLEFT_Arrow.y = 20.0f;

	hLEFT_Arrow.width = 64.0f;
	hLEFT_Arrow.height = 64.0f;

	if (LoadPNGImage(&hLEFT_Arrow.textureID, "./resources/textures/LEFT.png") == FALSE)
	{
		fprintf(gpFile, "ERR: LoadPNGImage() Failed for LEFT.png\n");
		uninitialize();
	}

	hLEFT_Arrow.bHovered = false;
	hLEFT_Arrow.bVisible = true;

	//! RIGHT ARROW:
	hRIGHT_Arrow.x = 20.0f;
	hRIGHT_Arrow.y = 20.0f;

	hRIGHT_Arrow.width = 64.0f;
	hRIGHT_Arrow.height = 64.0f;

	if (LoadPNGImage(&hRIGHT_Arrow.textureID, "./resources/textures/RIGHT.png") == FALSE)
	{
		fprintf(gpFile, "ERR: LoadPNGImage() Failed for RIGHT.png\n");
		uninitialize();
	}

	hRIGHT_Arrow.bHovered = false;
	hRIGHT_Arrow.bVisible = true;

}


int init_TextureShader(void)
{
	// local:

	// codel:

	// prototype:
	void uninitialize(void);

	// local:

	GLuint vertexShaderObject;
	GLuint fragmentShaderObject;

	GLint status;
	GLint infoLogLength;
	char* Log = NULL;

	// code:

	//* ///////////////////// # VERTEX SHADER # ////////////////////////
	const GLchar* vertexShaderSource = R"(
			
			#version 460 core

			layout (location = 0) in vec4 a_position;
			layout (location = 1) in vec2 a_texcoord;

			uniform mat4 u_modelMatrix;
			uniform mat4 u_viewMatrix;
			uniform mat4 u_projectionMatrix;

			out vec2 a_texcoords_out;

			void main(void)
			{
				gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * a_position;
				a_texcoords_out = a_texcoord;
			}

			)";

	vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderObject, 1, (const GLchar**)&vertexShaderSource, NULL);

	glCompileShader(vertexShaderObject);
	glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			Log = (char*)malloc(infoLogLength);
			if (Log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(vertexShaderObject, infoLogLength, &written, Log);
				fprintf(gpFile, "Error in init_TextureShader VERTEX Shader.\nVS Compilation Log : %s\n", Log);
				free(Log);
				Log = NULL;
				uninitialize();
			}
		}
	}
	else
	{
		fprintf(gpFile, "Success in init_TextureShader VERTEX Shader.\n");
	}



	//* ///////////////////// # FRAGMENT SHADER # ////////////////////////
	const GLchar* fragmentShaderSource = R"(
			
					#version 460 core

					in vec2 a_texcoords_out;
					uniform sampler2D u_textureSampler;
					out vec4 FragColor;

					void main(void)
					{
						//FragColor = vec4(a_texcoords_out.x, 0.0,0.0, 1.0);
						//FragColor = vec4(a_texcoords_out, 0.0, 1.0);
						FragColor = texture(u_textureSampler, a_texcoords_out);
					}
	
			)";

		const GLchar* fragmentShaderSource_BACKUP = R"(
			
			#version 460 core

			in vec2 a_texcoords_out;

			//uniform sampler2D u_textureSampler;
			out vec4 FragColor;

			void main(void)
			{
				//FragColor = texture(u_textureSampler, a_texcoords_out);
				//FragColor = vec4(texture(u_textureSampler, a_texcoords_out).rgb,1.0);
				//FragColor = vec4(1.0,0.5,0.5,1.0);
				FragColor = vec4(texture(a_texcoords_out, 0.0,1.0));
			}
	
			)";


	fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderObject, 1, (const GLchar**)&fragmentShaderSource, NULL);

	glCompileShader(fragmentShaderObject);

	status = 0;
	infoLogLength = 0;
	Log = NULL;
	glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			Log = (char*)malloc(infoLogLength);
			if (Log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(fragmentShaderObject, infoLogLength, &written, Log);
				fprintf(gpFile, "Error in init_TextureShader FRAGMENT Shader.\nFS Compilation Log : %s\n", Log);
				free(Log);
				Log = NULL;
				uninitialize();
			}
		}
	}
	else
	{
		fprintf(gpFile, "Success in init_TextureShader FRAGMENT Shader.\n");
	}

	shaderProgramObject_Texture = glCreateProgram();

	glAttachShader(shaderProgramObject_Texture , vertexShaderObject);
	glAttachShader(shaderProgramObject_Texture , fragmentShaderObject);


	status = 0;
	infoLogLength = 0;
	Log = NULL;

	glBindAttribLocation(shaderProgramObject_Texture, 0, "a_position");
	glBindAttribLocation(shaderProgramObject_Texture, 1, "a_texcoord");

	glLinkProgram(shaderProgramObject_Texture );
	glGetProgramiv(shaderProgramObject_Texture , GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObject_Texture , GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			Log = (char*)malloc(infoLogLength);
			if (Log != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(shaderProgramObject_Texture , infoLogLength, &written, Log);
				fprintf(gpFile, "Error in init_TextureShader shaderObject Linking\nLinking Log : % s\n", Log);
				free(Log);
				uninitialize();
				return FALSE;
			}
		}
	}
	else
	{
		fprintf(gpFile, "Success in init_TextureShader shaderObject Linking\n");
	}


	modelMatUniform_Tex = glGetUniformLocation(shaderProgramObject_Texture , "u_modelMatrix");
	viewMatUniform_Tex= glGetUniformLocation(shaderProgramObject_Texture , "u_viewMatrix");
	projMatUniform_Tex= glGetUniformLocation(shaderProgramObject_Texture , "u_projectionMatrix");
	textureSamplerUniform_Tex= glGetUniformLocation(shaderProgramObject_Texture, "u_textureSampler");



	float quadPosition[12] =
	{
		1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f
	};

	float quadTexcoord[8] =
	{
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
	};


	glGenVertexArrays(1, &VAO_ButtonQuad);
	glBindVertexArray(VAO_ButtonQuad);
	{
		glGenBuffers(1, &VBO_ButtonQuad_Position);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_ButtonQuad_Position);
		{
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadPosition), quadPosition, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
			glEnableVertexAttribArray(0);
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &VBO_ButtonQuad_Texcoord);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_ButtonQuad_Texcoord);
		{
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadTexcoord), quadTexcoord, GL_STATIC_DRAW);
			glVertexAttribPointer(1, 2, GL_FLOAT, false, 0, 0);
			glEnableVertexAttribArray(1);
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	glBindVertexArray(0);

	return (0);

}



void draw_Quad_with_Texture(GLuint texture_, GLfloat xPos, GLfloat yPos)
{
	// local:

	// codel:

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glUseProgram(shaderProgramObject_Texture);
	{
		glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(xPos, yPos, -3.5f));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.15f, 0.15f, 1.0f));

		glm::mat4 viewMatrix = glm::identity<glm::mat4>();

		glUniformMatrix4fv(glGetUniformLocation(shaderProgramObject_Texture,"u_modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
		glUniformMatrix4fv(glGetUniformLocation(shaderProgramObject_Texture, "u_viewMatrix"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(glGetUniformLocation(shaderProgramObject_Texture, "u_projectionMatrix"), 1, GL_FALSE, glm::value_ptr(perspectiveProjMatrix_glm));
	
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_);
		glUniform1i(textureSamplerUniform_Tex, 0);
		glBindVertexArray(VAO_ButtonQuad);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	glUseProgram(0);
	glDisable(GL_BLEND);

}



void uninit_TextureShader(void)
{
	// local:

	// codel:

	Uninitialize_ShaderProgramObject(shaderProgramObject_Texture);

}





