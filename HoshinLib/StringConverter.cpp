#include "StringConverter.h"

/// <summary>
/// string‚ðwideString‚É•ÏŠ·‚·‚é
/// </summary>
/// <param name="str">•ÔŠÒ‘O‚ÌstringŒ^</param>
/// <returns>•ÏŠ·Œã‚ÌwstringŒ^</returns>
std::wstring StringConverter::StringToWide(std::string str)
{
	std::wstring wide_string(str.begin(), str.end());
	return wide_string;
}

