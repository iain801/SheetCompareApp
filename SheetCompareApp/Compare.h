#pragma once
#include "libxl.h"
#include <string>
#include <queue>
#include <map>

class Compare
{
public:
	Compare(std::wstring srcPath, std::wstring destPath,
		unsigned int headRow);
	~Compare();

	void CompareBooks();
	bool isID();

private:
	unsigned int headRow;
	std::wstring srcPath;
	std::wstring destPath;
	libxl::Book* src = nullptr;
	libxl::Book* dest = nullptr;
	libxl::Book* output = nullptr;
	libxl::Sheet* srcSheet = nullptr;
	libxl::Sheet* destSheet = nullptr;
	libxl::Sheet* outSheet = nullptr;

	int getSheet(libxl::Book* book, std::wstring label);
	int getRow(libxl::Sheet* sheet, std::wstring label, int idCol);
	int getCol(libxl::Sheet* sheet, std::wstring label);
	std::queue<int> getColList(libxl::Sheet* sheet, std::wstring label);
};

