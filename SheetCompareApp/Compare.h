#pragma once
#include "libxl.h"
#include <string>
#include <queue>
#include <unordered_map>
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

	std::unordered_map<std::wstring, int> addedRecords;
	std::unordered_map<std::wstring, int> deletedRecords;
	std::unordered_map<std::wstring, std::pair<int,int>> consistantRecords;
	std::map<std::wstring, std::pair<int, int>> allCols;

	void UpdateCols();
	void CompareSheets();

	int getSheet(libxl::Book* book, std::wstring label);
	int getRow(libxl::Sheet* sheet, std::wstring label, int idCol);
	int getCol(libxl::Sheet* sheet, std::wstring label);
	std::queue<int> getColList(libxl::Sheet* sheet, std::wstring label);
};

