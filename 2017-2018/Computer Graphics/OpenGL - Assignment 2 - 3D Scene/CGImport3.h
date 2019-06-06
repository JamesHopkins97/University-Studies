#pragma once

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the CGIMPORT3_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// CGIMPORT3_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef CGIMPORT3_EXPORTS
#define CGIMPORT3_API __declspec(dllexport)
#define CGEXP_TEMPLATE_API
#pragma warning(disable:4251)
#else
#define CGIMPORT3_API __declspec(dllimport)
#define CGEXP_TEMPLATE_API extern
#endif

CGIMPORT3_API enum CG_IMPORT_RESULT {CG_IMPORT_OK,
									 CG_IMPORT_CANCELLED,
									 CG_FRAME_OUT_OF_RANGE,
									 CG_FILE_NOT_FOUND,
									 CG_FILE_NOT_RECOGNISED,
									 CG_FILE_IO_ERROR,
									 CG_NO_INTEGRITY,
									 CG_NOT_MANIFOLD,
									 CG_BUFFER_ERROR};

// DLL version number
extern CGIMPORT3_API const char *CGIMPORT_VERSION;

// Return version number of DLL
CGIMPORT3_API const char *cgimportVersion(void);


// Perform library initialisation - do at start of host app or when library loaded
CGIMPORT3_API void init_CGImport(void);

