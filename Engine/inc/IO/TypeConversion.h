#pragma once

#include <locale>
#include <codecvt>
#include <string>

namespace Engine
{
    inline std::basic_string<char> WStringToString(const std::basic_string<wchar_t>& wide)
    {
        //Create a wstring converter
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cvt;

        //Return the converted wstring
        return(cvt.to_bytes(wide));
    }

    inline std::basic_string<wchar_t> StringToWString(const std::basic_string<char>& str)
    {
        //Create a wstring converter
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cvt;

        //Return the converted string
        return(cvt.from_bytes(str));
    }
}