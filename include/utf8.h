#pragma once

/*

문자열 인코딩 정책

1. std::string은 set 단계에서 반드시 UTF-8 포맷으로 저장한다.

    예시:

    std::string s;
    s = "아하하";   <- 잘못됨. "아하하"가 UTF-8로 인코딩 되리라는 보장이 없음.

    std::string s2;
    s2 = to_utf8(L"아하하"); <- OK.

2. 끝.

*/

std::string from_utf8(const std::string& original);

#ifdef WIN32

inline std::string from_utf8(const std::string& str)
{
    wchar_t temp[4096] = { 0, };
    char result[4096] = { 0, };
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.size()) + 1, temp, _countof(temp));
    WideCharToMultiByte(CP_ACP, 0, temp, static_cast<int>(wcslen(temp)), result, sizeof(result) / sizeof(char), 0, 0);
    return std::string(result);
}

inline std::string wide_to_multibyte(int codepage, const std::wstring& str)
{
    std::string input_utf8;
    char input_utf8_raw[4096];
    WideCharToMultiByte(codepage, 0, str.c_str(), -1, input_utf8_raw, sizeof(input_utf8_raw), NULL, NULL);
    input_utf8 = input_utf8_raw;

    return input_utf8;
}

inline std::string to_utf8(const std::wstring& str)
{
	return wide_to_multibyte(CP_UTF8, str);
}

inline std::wstring wstring_from_utf8(const std::string& str)
{
    std::wstring ret_str;
    wchar_t ret_str_raw[4096];
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.size()) + 1, ret_str_raw, _countof(ret_str_raw));
    ret_str = ret_str_raw;

    return ret_str;
}

inline std::string string949_from_utf8(const std::string& str)
{	
    std::wstring w = wstring_from_utf8(str);

	return wide_to_multibyte(949, w);
}

#else

inline std::string from_utf8(const std::string& str)
{
    return str;
}

inline std::string to_utf8(const std::string& str)
{
    return str;
}


inline std::string to_utf8(const std::wstring& str)
{
    std::string input_utf8;
    char input_utf8_raw[4096];
    wcstombs(&input_utf8_raw[0], str.c_str(), sizeof(input_utf8_raw));
    input_utf8 = input_utf8_raw;

    return input_utf8;
}

inline std::wstring wstring_from_utf8(const std::string& str)
{
    std::wstring ret_str;
    wchar_t ret_str_raw[4096];
    mbstowcs(&ret_str_raw[0], str.c_str(), sizeof(ret_str_raw));
    ret_str = ret_str_raw;

    return ret_str;
}

#endif

#define UTF8(x) to_utf8(L##x)

