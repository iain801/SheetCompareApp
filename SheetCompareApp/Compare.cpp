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

	output = xlCreateXMLBook();
	output->setKey(L"Iain Weissburg", L"windows-2a242a0d01cfe90a6ab8666baft2map2");
}

Compare::~Compare() 
{
	output->save(destPath.replace(destPath.find(L".xls"), 5, L"_comparison.xlsm").c_str());
	std::wcout << "Output saved as: " << destPath << std::endl;

	src->release();
	dest->release();
}

void Compare::CompareBooks()
{
	/* TODO:
	* 
	*  - go thru each sheet and in each sheet
	*  - go thru src(old) and if any are missing from dest, add to deleted
	*  - if in both, add to consistant
	*  - go thru dest(new) and if any are missing from src, add to added
	* 
	*  - go through each consistant row, and if there are changes then 
	*    add it to the out sheet with changes highlighted
	* 
	*/
	std::map<std::wstring, int> addedRecords;
	std::map<std::wstring, int> deletedRecords;
	std::map<std::wstring, std::pair<int,int>> consistantRecords;
	std::map<std::wstring, int> srcMap;
	std::map<std::wstring, int> destMap;


	int srcSheetNum, destSheetNum, outSheetNum;
	for (srcSheetNum = 0; srcSheetNum < src->sheetCount(); srcSheetNum++) 
	{
		srcSheet = src->getSheet(srcSheetNum);
		destSheetNum = getSheet(dest, srcSheet->name());

		if (destSheetNum >= 0) 
		{
			destSheet = dest->getSheet(destSheetNum);
			outSheet = output->addSheet(srcSheet->name());

			int srcIDCol = getCol(srcSheet, L"unique");
			int destIDCol = getCol(destSheet, L"unique");

			for (int row = headRow + 1; row < srcSheet->lastFilledRow(); row++)
			{
				std::wstring id = srcSheet->readStr(row, srcIDCol);
				srcMap.insert(std::make_pair(id, row));
			}
			for (int row = headRow + 1; row < destSheet->lastFilledRow(); row++)
			{
				std::wstring id = destSheet->readStr(row, destIDCol);
				destMap.insert(std::make_pair(id, row));
			}

			for (auto srcit = srcMap.begin(); srcit != srcMap.end(); srcit++) 
			{
				auto destit = destMap.find(srcit->first);
				if (destit == destMap.end())
					deletedRecords.insert(*srcit);
				else
				{
					consistantRecords.insert(std::make_pair(srcit->first,
						std::make_pair(srcit->second, destit->second)));
				}
			}

		}
	}

}

bool Compare::isID()
{
	return getCol(src->getSheet(0), L"unique") != -1
		&& getCol(dest->getSheet(0), L"unique") != -1;
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
		return -1;
	else
		return colList.front();
}

std::queue<int> Compare::getColList(Sheet* sheet, std::wstring label)
{
	std::queue<int> colList;
	for (int col = sheet->firstFilledCol(); col < sheet->lastFilledCol(); col++)
	{
		if (sheet->cellType(headRow, col) == CELLTYPE_STRING)
		{
			std::wstring cellData(sheet->readStr(headRow, col));
			std::transform(cellData.begin(), cellData.end(), cellData.begin(),
				[](wchar_t c) { return tolower(c); });
			if (cellData.find(label) != std::wstring::npos)
				colList.push(col);
		}
	}
	return colList;
}