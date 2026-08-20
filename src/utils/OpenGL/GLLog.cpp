#include "GLLog.h"

//global variable declarations
FILE *gpFile_Log = NULL;



BOOL CreateLogFile(void)
{
	//code
	if (gpFile_Log != NULL)
		return(FALSE);

	if (fopen_s(&gpFile_Log, "Log_New.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("Failed To Create Log File \"Log_New.txt\" !!! Exitting Now ..."), TEXT("LOG FILE ERROR"), MB_ICONERROR | MB_OK);
		return(FALSE);
	}

	else
	{
		fprintf(gpFile_Log, "============================================================\n");
		fprintf(gpFile_Log, "Log File \"Log_New.txt\" Has Been Created Successfully !!!\n");
		fprintf(gpFile_Log, "============================================================\n\n");
	}
	return(TRUE);
}

void PrintGLInfo(void)
{
	// variable declarations
	GLint numExtensions, i;

	// code
	fprintf(gpFile_Log, "\n");
	fprintf(gpFile_Log, "OpenGL Related Information : \n");
	fprintf(gpFile_Log, "===============================\n\n");
	fprintf(gpFile_Log, "OpenGL Vendor   : %s\n", glGetString(GL_VENDOR));
	fprintf(gpFile_Log, "OpenGL Renderer : %s\n", glGetString(GL_RENDERER));
	fprintf(gpFile_Log, "OpenGL Version  : %s\n", glGetString(GL_VERSION));
	fprintf(gpFile_Log, "GLSL Version    : %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
	fprintf(gpFile_Log, "Number Of Supportrd Extensions = %d\n", numExtensions);

	for (i = 0; i < numExtensions; i++)
	{
		fprintf(gpFile_Log, "%s\n", glGetStringi(GL_EXTENSIONS, i));
	}

	fprintf(gpFile_Log, "===============================\n\n");
}

void PrintLog(const char *fmt, ...)
{
	//variable declarations
	va_list arg;
	int ret;

	//code
	if (gpFile_Log == NULL)
		return;
	va_start(arg, fmt);
	ret = vfprintf(gpFile_Log, fmt, arg);
	va_end(arg);
}

void CloseLogFile(void)
{
	//code
	if (gpFile_Log == NULL)
		return;

	fprintf(gpFile_Log, "============================================================\n");
	fprintf(gpFile_Log, "Log File \"Log_New.txt\" Has Been Closed Successfully !!!\n");
	fprintf(gpFile_Log, "============================================================\n");

	fclose(gpFile_Log);
	gpFile_Log = NULL;
}

