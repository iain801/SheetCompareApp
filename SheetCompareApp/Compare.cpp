#include "Compare.h"
#include <iostream>
#include <algorithm>

using namespace libxl;

Compare::Compare(std::wstring sourcePath, std::wstring destinationPath, unsigned int headRow)
	: srcPath(sourcePath), destPath(destinationPath), headRow(headRow)
{
	if (srcPath.compare(destPath) == 0) {
		std::wcout << "ERROR: Duplicate input paths" << std::endl;
		return;
	}

	if (srcPath.find(L".xlsx") != std::wstring::npos)
		src = xlCreateXMLBook();
	else if (srcPath.find(L".xls") != std::wstring::npos)
		src = xlCreateBook();
	else {
		std::wcout << "ERROR: Invaid source filetype" << std::endl;
		return;
	}
	src->setKey(L"Iain Weissburg", L"windows-2a242a0d01cfe90a6ab8666baft2map2");
	std::wcout << "Created source book" << std::endl;

	if (destPath.find(L".xlsx") != std::wstring::npos)
		dest = xlCreateXMLBook();
	else if (destPath.find(L".xls") != std::wstring::npos)
		dest = xlCreateBook();
	else {
		std::wcout << "ERROR: Invaid destination filetype" << std::endl;
		return;
	}
	dest->setKey(L"Iain Weissburg", L"windows-2a242a0d01cfe90a6ab8666baft2map2");
	std::wcout << "Created destination book" << std::endl;

	src->load(srcPath.c_str());
	dest->load(destPath.c_str());
	std::wcout << "Loaded books" << std::endl;
}

Compare::~Compare() {
	dest->save(destPath.replace(destPath.find(L".xls"), 4, L"_processed.xls").c_str());
	std::wcout << "Output saved as: " << destPath << std::endl;

	src->release();
	dest->release();
}

int Compare::getSheet(libxl::Book* book, std::wstring label)
{
	for (int sheet = 0; sheet < book->sheetCount(); sheet++)
	{
		std::wstring sheetName(book->getSheetName(sheet));
		if (sheetName.compare(label) == 0)
			return sheet;
	}
	return -1;
}

int Compare::getRow(libxl::Sheet* sheet, std::wstring label, int idCol)
{
	for (int row = headRow; row < sheet->lastFilledRow(); row++)
	{
		std::wstring cellData(sheet->readStr(row, idCol));
		if (cellData.compare(label) == 0)
			return row;
	}
	return -1;
}

int Compare::getCol(Sheet* sheet, std::wstring label)
{
	auto colList = getColList(sheet, label);
	if (colList.empty())
		return sheet->lastFilledCol();
	else
		return colList.front();
}

std::list<int> Compare::getColList(Sheet* sheet, std::wstring label)
{
	std::list<int> colList(0);
	for (int col = sheet->firstFilledCol(); col < sheet->lastFilledCol(); col++)
	{
		if (sheet->cellType(headRow, col) == CELLTYPE_STRING)
		{
			std::wstring cellData(sheet->readStr(headRow, col));
			std::transform(cellData.begin(), cellData.end(), cellData.begin(),
				[](wchar_t c) { return tolower(c); });
			if (cellData.find(label) != std::wstring::npos)
				colList.push_back(col);
		}
	}
	return colList;
}