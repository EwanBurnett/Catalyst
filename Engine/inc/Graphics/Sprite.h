#pragma once
#include "Model.h"
#include "..\Entity/Components.h"

namespace Engine {
    //TODO: Sprite Scaling to Screen, Texture Rects
    struct Sprite
    {
        Sprite()
        {
            auto spr = new Engine::SpriteRenderer;
            m_Renderer = {
                Engine::EPrimitiveTopology::TriangleStrip,
                Engine::EShaderType::SpriteRenderer,
                spr,
                "Sprite"
            };
        }

        explicit Sprite(const std::basic_string<wchar_t>& texture, Engine::Vector2f size, Engine::Vector2f Position, Engine::Rect texRect = {}) : size(size)
        {
            auto spr = new Engine::SpriteRenderer;

            spr->TextureAtlas = texture;

            m_Renderer = {
                Engine::EPrimitiveTopology::TriangleStrip,
                Engine::EShaderType::SpriteRenderer,
                spr,
                "Sprite"
            };

            transform.Position = { Position.x, Position.y, 0.0f };
            transform.Scale = { size.x, size.y, 0.0f };

            m_Sprite = Engine::Primitives::Quad();

            m_Sprite.TexCoords[0] = { texRect.Left, texRect.Bottom };
            m_Sprite.TexCoords[1] = { texRect.Right,texRect.Bottom };
            m_Sprite.TexCoords[2] = { texRect.Right,texRect.Top };
            m_Sprite.TexCoords[3] = { texRect.Left, texRect.Top };

        }

        void SetTextureRect(const Engine::Rect& rect)
        {
            m_Sprite.TexCoords[0] = {rect.Left, rect.Bottom};
            m_Sprite.TexCoords[1] = {rect.Right, rect.Bottom};
            m_Sprite.TexCoords[2] = {rect.Right, rect.Top};
            m_Sprite.TexCoords[3] = {rect.Left, rect.Top};
        }

        Engine::Transform transform;
        Engine::Vector2f size;
        Engine::MeshRenderer m_Renderer;
        Engine::Primitives::Quad m_Sprite;
    };
}