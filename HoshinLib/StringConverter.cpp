#include "StringConverter.h"

/// <summary>
/// string��wideString�ɕϊ�����
/// </summary>
/// <param name="str">�ԊґO��string�^</param>
/// <returns>�ϊ����wstring�^</returns>
std::wstring StringConverter::StringToWide(std::string str)
{
	std::wstring wide_string(str.begin(), str.end());
	return wide_string;
}

