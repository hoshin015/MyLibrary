#pragma once
#include <string>

// シングルトン
class StringConverter
{
public:
	// stringをwideStringに変換する
	static std::wstring StringToWide(std::string str);
};