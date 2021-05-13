// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <iostream>
#include <Windows.h>

typedef int(__cdecl* tstrcmp) (const char* Str1, const char* Str2);

tstrcmp ostrcmp;

int  __cdecl hkstrcmp(const char* Str1, const char* Str2) {

    std::cout << "hooked" << std::endl;

    std::cout << Str1 << std::endl;
    std::cout << Str2 << std::endl;

    return ostrcmp(Str1, Str2);
}


bool Detour(BYTE* src, BYTE* dst, const uintptr_t len) {

    if (len < 5) {
        return false;
    }

    DWORD curProtection;
    VirtualProtect(src, len, PAGE_EXECUTE_READWRITE, &curProtection);

    memset(src, 0x90, len);

    uintptr_t relAddr = dst - src - 5;

    *(uintptr_t*)(src + 0) = 0xE9;
    *(uintptr_t*)(src + 1) = relAddr;

    VirtualProtect(src, len, curProtection, &curProtection);

    return true;
}

BYTE* TrampHook(BYTE* src, BYTE* dst, const uintptr_t len) {

    if (len < 5) {
        return 0;
    }

    BYTE* gateway = (BYTE*)VirtualAlloc(0, len, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    memcpy_s(gateway, len, src, len);


    uintptr_t gatewayRelAddr = src - gateway - 5;
    *(gateway + len) = 0xE9;


    *(uintptr_t*)((uintptr_t)gateway + len + 1) = gatewayRelAddr;

    Detour(src, dst, len);


    return gateway;
}

DWORD WINAPI MainThread(LPVOID param) {
    uintptr_t moduleBaseAddr = (uintptr_t)GetModuleHandle(L"My Dog5.exe");
    uintptr_t funcAddr = moduleBaseAddr + 0x3850;

    ostrcmp = (tstrcmp)funcAddr;
    ostrcmp = (tstrcmp)TrampHook((BYTE*)ostrcmp, (BYTE*)hkstrcmp, 9);

    while (true) {
        //*(uintptr_t*)funcAddr = firstByte;
        if (GetAsyncKeyState(VK_BACK)) {
            break;
        }
        Sleep(5);
    }

    std::cout << "Detaching DLL and closing program \n";

    FreeLibraryAndExitThread((HMODULE)param, 0);

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(0, 0, MainThread, hModule, 0, 0);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

