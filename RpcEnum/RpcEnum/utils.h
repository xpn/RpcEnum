#pragma once
#include <Windows.h>
#include <iostream>
#include <map>
#include <string>

class Utils {
public:
	static void PrintGUID(UUID &input);
	static std::string ConvertGUID(UUID &uuid);
};