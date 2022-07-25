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
	output->save(destPath.replace(destPath.find(L".xls"), 5, L"_comparison.xlsx").c_str());
	std::wcout << "Output saved as: " << destPath << std::endl;

	output->release();
	src->release();
	dest->release();
}

void Compare::CompareBooks()
{
	std::unordered_map<std::wstring, int> srcMap;
	std::unordered_map<std::wstring, int> destMap;

	int srcSheetNum, destSheetNum, outSheetNum;
	for (srcSheetNum = 0; srcSheetNum < src->sheetCount(); srcSheetNum++) 
	{
		srcSheet = src->getSheet(srcSheetNum);
		destSheetNum = getSheet(dest, srcSheet->name());

		if (destSheetNum == -1)
			continue;

		destSheet = dest->getSheet(destSheetNum);
		outSheet = output->addSheet(srcSheet->name());

		int srcIDCol = getCol(srcSheet, L"unique");
		int destIDCol = getCol(destSheet, L"unique");

		srcMap.reserve(srcSheet->lastFilledRow());
		destMap.reserve(destSheet->lastFilledRow());

		//Add all rows to maps for faster search
		for (int row = headRow + 1; row < srcSheet->lastFilledRow(); row++)
		{
			std::wstring id = srcSheet->readStr(row, srcIDCol);
			srcMap[id] = row;
		}
		for (int row = headRow + 1; row < destSheet->lastFilledRow(); row++)
		{
			std::wstring id = destSheet->readStr(row, destIDCol);
			destMap[id] = row;
		}
		
		deletedRecords.reserve(srcMap.size());
		consistantRecords.reserve(std::min(srcMap.size(), destMap.size()));
		
		//Add all srcMap elements to deleted or consistant
		for (auto srcit = srcMap.begin(); srcit != srcMap.end(); srcit = srcMap.erase(srcit)) 
		{
			auto destit = destMap.find(srcit->first);
			if (destit == destMap.end())
				deletedRecords.insert(*srcit);
			else
			{
				consistantRecords[srcit->first] = std::make_pair(srcit->second, destit->second);
				destMap.erase(destit);
			}
		}
		srcMap.clear();
		deletedRecords.rehash(deletedRecords.size());
		consistantRecords.rehash(consistantRecords.size());

		addedRecords.reserve(destMap.size());
		addedRecords = destMap;
		destMap.clear();

		UpdateCols();
		CompareSheets();

		addedRecords.clear();
		deletedRecords.clear();
		consistantRecords.clear();
		allCols.clear();

	}

}

void Compare::UpdateCols()
{

	std::map<std::wstring, int> destMap;
	for (int col = destSheet->firstFilledCol(); col < destSheet->lastFilledCol(); col++)
	{
		std::wstring id = destSheet->readStr(headRow, col);
		destMap[id] = col;
	}

	for (int col = srcSheet->firstFilledCol(); col < srcSheet->lastFilledCol(); col++)
	{
		std::wstring id = srcSheet->readStr(headRow, col);
		auto destit = destMap.find(id);
		if (destit == destMap.end())
			allCols[id] = std::make_pair(col, -1);
		else
		{
			allCols[id] = std::make_pair(col, destit->second);
			destMap.erase(destit);
		}
	}
	for (auto destit = destMap.begin(); destit != destMap.end(); destit = destMap.erase(destit))
	{
		allCols[destit->first] = std::make_pair(-1, destit->second);
	}
	destMap.clear();
	
}

void Compare::CompareSheets()
{
	int row = 0, col = 0;
	
	Format* blank = output->addFormat();
	Format* header = output->addFormat();
	header->setFillPattern(FILLPATTERN_GRAY50);
	header->setPatternForegroundColor(COLOR_ICEBLUE_CF);
	header->setBorderLeft();
	header->setBorderRight();
	header->setBorderTop();
	header->setBorderBottom();
	header->setAlignH(ALIGNH_CENTER);
	header->setWrap(true);

	Format* modified = output->addFormat();
	modified->setFillPattern(FILLPATTERN_GRAY50);
	modified->setPatternForegroundColor(COLOR_LIGHTYELLOW);

	Format* added = output->addFormat();
	added->setFillPattern(FILLPATTERN_GRAY50);
	added->setPatternForegroundColor(COLOR_LIGHTGREEN);

	Format* deleted = output->addFormat();
	deleted->setFillPattern(FILLPATTERN_GRAY50);
	deleted->setPatternForegroundColor(COLOR_ROSE);

	for (auto colit = allCols.begin(); colit != allCols.end(); colit++, col++)
	{
		outSheet->writeStr(row, col, colit->first.c_str(), header);
		if (colit->second.first != -1)
			outSheet->setCol(col, col, srcSheet->colWidth(colit->second.first));
		else
			outSheet->setCol(col, col, destSheet->colWidth(colit->second.second));
	}
	outSheet->setRow(row, srcSheet->rowHeight(headRow));

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