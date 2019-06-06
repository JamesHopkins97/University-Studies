//
// stdafx.h
//

// Pre-compiled header

#pragma once

#include <targetver.h>


// Windows Header Files

// Exclude rarely-used stuff from Windows headers
#define WIN32_LEAN_AND_MEAN

#include <windows.h>


// Memory handling headers
#include <GUMemory.h>


// C RunTime and STL Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <cstdint>
#include <conio.h>
#include <iostream>
#include <fstream>
#include <functional>
#include <vector>
#include <exception>


// Core types
#include <CGDClock.h>
#include <CGDConsole.h>

// DirectX headers
#include <d3d11_2.h>
#include <dxgi1_3.h>
#include <d2d1_1.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXTK\DDSTextureLoader.h>
#include <DirectXTK\WICTextureLoader.h>

#include <System.h>
