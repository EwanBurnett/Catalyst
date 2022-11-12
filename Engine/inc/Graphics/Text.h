#pragma once
#include "Font.h"
#include "Sprite.h"
#include <string>
#include <vector>

struct Text
{
    Text(std::basic_string<char> text, Font& font, Engine::Vector2f pos)
    {
        pFont = &font;
        position = pos;

        SetText(text);
    }
    
    void SetText(std::basic_string<char> text)
    {
        Engine::Vector2f pos = position;

        m_Glyphs.clear();
        m_Glyphs.resize(text.length());

        for (auto i = 0; i < text.length(); i++)
        {
            auto glyph = text.data()[i];
            const auto& g = pFont->m_Glyphs.at(glyph);

            Engine::Vector2f size = { (float)g.width, (float)g.height };
            Engine::Vector2f glyphPos = { pos.x + g.offsetX, pos.y + g.offsetY };
            Engine::Rect texRect = 
            {
                (float)g.x,
                (float)g.y,
                (float)(g.x + (g.width / pFont->Size.x)),
                (float)(g.y + (g.height / pFont->Size.x))
            };

            Engine::Sprite spr(pFont->m_Bitmap, size, glyphPos, texRect);
            //spr.SetTextureRect(texRect);
            spr.m_Sprite.Name = glyph;
            spr.m_Renderer.technique = "Text";
            m_Glyphs.push_back(spr);


            pos.x += g.advanceX;
        }

        m_Glyphs.shrink_to_fit();

        string = text;

    }

    
    Font* pFont;
    std::basic_string<char> string;
    Engine::Vector2f position;
    std::vector<Engine::Sprite> m_Glyphs;
};
