/*
* Author: Iain Weissburg
* Date: 8/1/2022
*/
#pragma once
#include "libxl.h"
#include <string>
#include <deque>
#include <unordered_map>

class Compare
{
public:
	/* Constructor for Compare object
	* 
	*  wstring pastPath - Path to the old spreadsheet
	*  wstring recentPath - Path to the new spreadsheet
	*  unsigned int headRow - the spreadsheet row that contains header labels
	*  
	*  returns none
	*/
	Compare(std::wstring pastPath, std::wstring recentPath,
		unsigned int headRow);

	/* Destructor for Compare object
	* 
	*  returns none
	*/
	~Compare();

	/* Initiate comparison of the input spreadsheets
	* 
	*  returns void
	*/
	void CompareBooks();

	/* Checks if given spreadsheets contain unique ids in the given header row
	* 
	*  returns true if a header with the word "unique" is found 
	*  in the first sheet of both files
	*/
	bool isID();

private:
	struct ColData {
		std::wstring label;
		int past, recent, out;

		ColData(std::wstring label, int past = -1, int recent = -1, int out = -1) 
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
	std::deque<ColData> allCols;

	void UpdateCols();
	void CompareSheets();

	int getSheet(libxl::Book* book, std::wstring label);
	int getRow(libxl::Sheet* sheet, std::wstring label, int idCol);
	int getCol(libxl::Sheet* sheet, std::wstring label);
	std::deque<int> getColList(libxl::Sheet* sheet, std::wstring label);

	std::wstring getCellString(libxl::Sheet* sheet, int row, int col);
};

