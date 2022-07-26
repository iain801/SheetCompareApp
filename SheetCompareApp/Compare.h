#pragma once
#include "libxl.h"
#include <string>
#include <deque>
#include <unordered_map>

class Compare
{
public:
	Compare(std::wstring pastPath, std::wstring recentPath,
		unsigned int headRow);
	~Compare();

	void CompareBooks();
	bool isID();

private:
	struct cpastata {
		std::wstring label;
		int past, recent, out;

		cpastata(std::wstring label, int past = -1, int recent = -1, int out = -1) 
			: label(label), past(past), recent(recent), out(out) { }
	};


	unsigned int headRow;
	std::wstring pastPath;
	std::wstring recentPath;
	libxl::Book* past = nullptr;
	libxl::Book* recent = nullptr;
	libxl::Book* output = nullptr;
	libxl::Sheet* pastSheet = nullptr;
	libxl::Sheet* recentSheet = nullptr;
	libxl::Sheet* outSheet = nullptr;

	std::unordered_map<std::wstring, int> addedRecords;
	std::unordered_map<std::wstring, int> deletedRecords;
	std::unordered_map<std::wstring, std::pair<int,int>> consistantRecords;
	std::deque<cpastata> allCols;

	void UpdateCols();
	void CompareSheets();

	int getSheet(libxl::Book* book, std::wstring label);
	int getRow(libxl::Sheet* sheet, std::wstring label, int idCol);
	int getCol(libxl::Sheet* sheet, std::wstring label);
	std::deque<int> getColList(libxl::Sheet* sheet, std::wstring label);

	std::wstring getCellString(libxl::Sheet* sheet, int row, int col);
};

