#include "stdafx.h"
#include "PythonHook.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID /*lpReserved*/)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        OutputDebugStringW(L"---DllMain routine started. Starting injection thread...");
        if (CreateThread(nullptr, 0, EntryFunc, hModule, 0, nullptr) == nullptr) 
            MessageBox(0, L"Failed to create thread", L"", 0);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

