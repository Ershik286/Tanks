#pragma once

#ifdef BLOCKMAPLOADER_EXPORTS
#define BLOCKMAPLOADER_API __declspec(dllexport)
#else
#define BLOCKMAPLOADER_API __declspec(dllimport)
#endif

extern "C" {
    typedef void (*InitializeBlockFunc)(int x, int y, int blockType);
    typedef void (*PositionBlockFunc)(int x, int y, int posX, int posY);

    BLOCKMAPLOADER_API bool LoadMapFromFile(
        const wchar_t* filename,  // const wchar_t* вместо wstring
        int width,
        int height,
        InitializeBlockFunc initFunc,
        PositionBlockFunc posFunc
    );
}