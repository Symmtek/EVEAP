#pragma once
#include "stdafx.h"

class EVEScript
{
public:
	EVEScript();
	~EVEScript();

	static PyObject* PyEval_CallObjectWithKeywordsHook(PyObject* obj, PyObject* arg, PyObject* kw);
protected:
	bool isRecursiveCall;
	bool autoPilotDetected;

    PyObject* onCall( PyObject* obj, PyObject* arg, PyObject* kw );  
};

DWORD __stdcall EntryFunc( void* dllHandle );
DWORD __stdcall UnloadFunc( void* dllHandle );



