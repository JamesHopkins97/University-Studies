
#pragma once

// Note: Comment this out to remove WIC features from texture_loader
#define __CG_USE_WINDOWS_IMAGING_COMPONENT___		1

#include <glew\glew.h>
#include <Windows.h>
#include <string>

// COM initialisation and shutdown methods
HRESULT initCOM(void);
void shutdownCOM(void);

// Windows Imaging Component texture loader
#ifdef __CG_USE_WINDOWS_IMAGING_COMPONENT___
GLuint wicLoadTexture(const std::wstring& filename);
#endif

// FreeImage texture loader
GLuint fiLoadTexture(const char *filename);
