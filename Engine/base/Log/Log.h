#pragma once
#include<Windows.h>
#include <string>
#include<format>

//stirngをwstringに
std::wstring ConvertString(const std::string& str);
//wstirngをstringに
std::string ConvertString(const std::wstring& str);

//ログ出力
void Log(const std::string& message);
