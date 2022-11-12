#pragma once
#include <cstdint>
#include <map>
#include <string>

struct Glyph
{
    float x;    //x position (normalized)
    float y;    //y position (normalized)
    float width;    //Width (normalized)
    float height;   //Height (normalized)
    uint16_t offsetX;   //Cursor offset X (px)
    uint16_t offsetY;   //Cursor offset Y (px)
    uint16_t advanceX;  //Cursor advance X (px)
};

struct Font
{
    std::basic_string<wchar_t> m_Bitmap;
    std::map<char, Glyph> m_Glyphs;
    Engine::Vector2f Size;
};