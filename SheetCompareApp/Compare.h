#pragma once
#include "libxl.h"
#include <string>
#include <list>

class Compare
{
public:
	Compare(std::wstring srcPath, std::wstring destPath,
		unsigned int headRow);
	~Compare();

	void CompareBooks();

private:
	unsigned int headRow;
	std::wstring srcPath;
	std::wstring destPath;
	libxl::Book* src = nullptr;
	libxl::Book* dest = nullptr;
	libxl::Sheet* srcSheet = nullptr;
	libxl::Sheet* destSheet = nullptr;

	int getSheet(libxl::Book* book, std::wstring label);
	int getRow(libxl::Sheet* sheet, std::wstring label, int idCol);
	int getCol(libxl::Sheet* sheet, std::wstring label);
	std::list<int> getColList(libxl::Sheet* sheet, std::wstring label);
};

