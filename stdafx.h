#pragma once

#include <algorithm>
#include <memory>
#include <stdexcept>
#include <string> 
#include <thread>
#include <chrono>

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             
#pragma warning(push, 0)    
#include <windows.h>
#include <fcntl.h>
#include <Python.h>
#include <frameobject.h>
#include <CXX/Objects.hxx>
#include <mhook-lib/mhook.h>
#pragma warning(pop)

