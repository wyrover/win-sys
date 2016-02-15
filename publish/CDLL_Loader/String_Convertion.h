#pragma once
#pragma warning(push)
#pragma warning(disable: 4244) // possible loss of data
#include <xutility>
#pragma warning(pop)

#include <string>

class String_Convertion {
public:
	static std::wstring StringToWString(const std::string& s);
	static std::string WStringToString(const std::wstring& s);
};
