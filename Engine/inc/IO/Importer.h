#pragma once
#include "..\Graphics\Model.h"
#include "..\Graphics/Font.h"
#include <iostream>
#include <fstream>
#include <direct.h>
#include <string>
#include "TypeConversion.h"
#include "Logger.h"

constexpr uint16_t ASSET_VERSION = 1;

namespace Engine {
    class Texture;

    namespace Importer
    {
        Model LoadFromFile(const std::basic_string<char>& filePath);
        void LoadFromFile(Model& model, const std::basic_string<char>& filePath);
        void LoadFromFile(Font& font, const std::basic_string<char> filePath);

        void ImportModelFromMemory(Model& model, std::basic_string<char> destPath);
        Model ImportModelFromFile(const std::basic_string<char>& filePath, std::basic_string<char> destPath = "");
    };
}