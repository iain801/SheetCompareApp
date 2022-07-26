#include "Compare.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <iomanip>

using namespace libxl;

Compare::Compare(std::wstring sourcePath, std::wstring recentinationPath, unsigned int headRow)
	: pastPath(sourcePath), recentPath(recentinationPath), headRow(headRow)
{
	if (pastPath.compare(recentPath) == 0) {
		std::wcout << "ERROR: Duplicate input paths" << std::endl;
		return;
	}

	if (pastPath.find(L".xlsx") != std::wstring::npos)
		past = xlCreateXMLBook();
	else if (pastPath.find(L".xls") != std::wstring::npos)
		past = xlCreateBook();
	else {
		std::wcout << "ERROR: Invaid source filetype" << std::endl;
		return;
	}
	past->setKey(L"Iain Weissburg", L"windows-2a242a0d01cfe90a6ab8666baft2map2");
	std::wcout << "Created source book" << std::endl;

	if (recentPath.find(L".xlsx") != std::wstring::npos)
		recent = xlCreateXMLBook();
	else if (recentPath.find(L".xls") != std::wstring::npos)
		recent = xlCreateBook();
	else {
		std::wcout << "ERROR: Invaid recentination filetype" << std::endl;
		return;
	}
	recent->setKey(L"Iain Weissburg", L"windows-2a242a0d01cfe90a6ab8666baft2map2");
	std::wcout << "Created recentination book" << std::endl;

	past->load(pastPath.c_str());
	recent->load(recentPath.c_str());
	std::wcout << "Loaded books" << std::endl;

	output = xlCreateXMLBook();
	output->setKey(L"Iain Weissburg", L"windows-2a242a0d01cfe90a6ab8666baft2map2");
}

Compare::~Compare() 
{
	output->save(recentPath.replace(recentPath.find(L".xls"), 5, L"_comparison.xlsx").c_str());
	std::wcout << "Output saved as: " << recentPath << std::endl;

	output->release();
	past->release();
	recent->release();
}

void Compare::CompareBooks()
{
	std::unordered_map<std::wstring, int> pastMap;
	std::unordered_map<std::wstring, int> recentMap;

	int pastSheetNum, recentSheetNum;
	for (pastSheetNum = 0; pastSheetNum < past->sheetCount(); pastSheetNum++) 
	{
		pastSheet = past->getSheet(pastSheetNum);
		recentSheetNum = getSheet(recent, pastSheet->name());

		if (recentSheetNum == -1)
			continue;

		recentSheet = recent->getSheet(recentSheetNum);
		outSheet = output->addSheet(pastSheet->name());

		int pastIDCol = getCol(pastSheet, L"unique");
		int recentIDCol = getCol(recentSheet, L"unique");

		pastMap.reserve(pastSheet->lastFilledRow());
		recentMap.reserve(recentSheet->lastFilledRow());

		//Add all rows to maps for faster search
		for (int row = headRow + 1; row < pastSheet->lastFilledRow(); row++)
		{
			std::wstring id = pastSheet->readStr(row, pastIDCol);
			pastMap[id] = row;
		}
		for (int row = headRow + 1; row < recentSheet->lastFilledRow(); row++)
		{
			std::wstring id = recentSheet->readStr(row, recentIDCol);
			recentMap[id] = row;
		}
		
		deletedRecords.reserve(pastMap.size());
		consistantRecords.reserve(std::min(pastMap.size(), recentMap.size()));
		
		//Add all pastMap elements to deleted or consistant
		for (auto pastit = pastMap.begin(); pastit != pastMap.end(); pastit = pastMap.erase(pastit)) 
		{
			auto recentit = recentMap.find(pastit->first);
			if (recentit == recentMap.end())
				deletedRecords.insert(*pastit);
			else
			{
				consistantRecords[pastit->first] = std::make_pair(pastit->second, recentit->second);
				recentMap.erase(recentit);
			}
		}
		pastMap.clear();
		deletedRecords.rehash(deletedRecords.size());
		consistantRecords.rehash(consistantRecords.size());

		addedRecords.reserve(recentMap.size());
		addedRecords = recentMap;
		recentMap.clear();

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

	std::unordered_map<std::wstring, int> recentMap;
	for (int col = recentSheet->firstFilledCol(); col < recentSheet->lastFilledCol(); col++)
	{
		std::wstring id = recentSheet->readStr(headRow, col);
		recentMap[id] = col;
	}

	for (int col = pastSheet->firstFilledCol(); col < pastSheet->lastFilledCol(); col++)
	{
		std::wstring id = pastSheet->readStr(headRow, col);
		auto recentit = recentMap.find(id);
		if (recentit == recentMap.end())
			allCols.push_back(cpastata(id, col, -1));
		else
		{
			allCols.push_back(cpastata(id, col, recentit->second));
			recentMap.erase(recentit);
		}
	}
	for (auto recentit = recentMap.begin(); recentit != recentMap.end(); recentit = recentMap.erase(recentit))
	{
		allCols.push_back(cpastata(recentit->first, -1, recentit->second));
	}
	recentMap.clear();
	
}

void Compare::CompareSheets()
{
	int row = 0, col = 0;
	
	Format* blank = output->addFormat();
	blank->setBorderLeft();
	blank->setBorderRight();
	blank->setBorderTop();
	blank->setBorderBottom();

	Format* header = output->addFormat();
	header->setFillPattern(FILLPATTERN_SOLID);
	header->setPatternForegroundColor(COLOR_ICEBLUE_CF);
	header->setBorderLeft(BORDERSTYLE_MEDIUM);
	header->setBorderRight(BORDERSTYLE_MEDIUM);
	header->setBorderTop(BORDERSTYLE_MEDIUM);
	header->setBorderBottom(BORDERSTYLE_MEDIUM);
	header->setAlignH(ALIGNH_CENTER);
	header->setWrap(true);

	Format* modified = output->addFormat();
	modified->setFillPattern(FILLPATTERN_SOLID);
	modified->setPatternForegroundColor(COLOR_LIGHTYELLOW);
	modified->setBorderLeft();
	modified->setBorderRight();
	modified->setBorderTop();
	modified->setBorderBottom();

	Format* added = output->addFormat();
	added->setFillPattern(FILLPATTERN_SOLID);
	added->setPatternForegroundColor(COLOR_LIGHTGREEN);
	added->setBorderLeft();
	added->setBorderRight();
	added->setBorderTop();
	added->setBorderBottom();

	Format* deleted = output->addFormat();
	deleted->setFillPattern(FILLPATTERN_SOLID);
	deleted->setPatternForegroundColor(COLOR_ROSE);
	deleted->setBorderLeft();
	deleted->setBorderRight();
	deleted->setBorderTop();
	deleted->setBorderBottom();

	for (auto colit = allCols.begin(); colit != allCols.end(); colit++, col++)
	{
		outSheet->writeStr(row, col, colit->label.c_str(), header);
		if (colit->past != -1)
			outSheet->setCol(col, col, pastSheet->colWidth(colit->past));
		else
			outSheet->setCol(col, col, recentSheet->colWidth(colit->recent));
	}
	outSheet->setRow(row, pastSheet->rowHeight(headRow));
	row++;

	int outIDCol = getCol(outSheet, L"unique");
	for (auto rowit = consistantRecords.begin(); rowit != consistantRecords.end(); rowit++)
	{
		col = 0;
		bool difference = false;
		for (auto colit = allCols.begin(); colit != allCols.end(); colit++, col++)
		{
			if (colit->past >= 0 && colit->recent >= 0) 
			{
				std::wstring pastText = getCellString(pastSheet, rowit->second.first, colit->past);
				std::wstring recentText = getCellString(recentSheet, rowit->second.second, colit->recent);

				if (pastText.compare(recentText) != 0)
				{
					difference = true;
					break;
				}
				
			}
		}
		if (difference)
		{
			col = 0;
			for (auto colit = allCols.begin(); colit != allCols.end(); colit++, col++)
			{
				if (colit->past >= 0 && colit->recent >= 0) 
				{
					CellType type = pastSheet->cellType(rowit->second.first, colit->past);
					if (pastSheet->isDate(rowit->second.first, colit->past))
						type = CELLTYPE_STRING;
					std::wstring pastText = getCellString(pastSheet, rowit->second.first, colit->past);
					std::wstring recentText = getCellString(recentSheet, rowit->second.second, colit->recent);

					if (pastText.compare(recentText) == 0)
						outSheet->writeStr(row, col, pastText.c_str(), blank, type);
					else
						outSheet->writeStr(row, col, pastText.append(L" to ").append(recentText).c_str(), modified);
				}
				else if (colit->past == -1)
				{
					CellType type = recentSheet->cellType(rowit->second.second, colit->recent);
					if (recentSheet->isDate(rowit->second.second, colit->past))
						type = CELLTYPE_STRING;
					std::wstring recentText = getCellString(recentSheet, rowit->second.second, colit->recent);
					outSheet->writeStr(row, col, recentText.c_str(), added, type);
				}
				else
				{
					CellType type = pastSheet->cellType(rowit->second.first, colit->past);
					if (pastSheet->isDate(rowit->second.first, colit->past))
						type = CELLTYPE_STRING;
					std::wstring pastText = getCellString(pastSheet, rowit->second.first, colit->past);
					outSheet->writeStr(row, col, pastText.c_str(), deleted, type);
				}
			}
			row++;
		}
	}
	row++;

	for (auto rowit = addedRecords.begin(); rowit != addedRecords.end(); rowit++)
	{
		col = 0;
		for (auto colit = allCols.begin(); colit != allCols.end(); colit++, col++)
		{
			CellType type = recentSheet->cellType(rowit->second, colit->recent);
			if (recentSheet->isDate(rowit->second, colit->past))
				type = CELLTYPE_STRING;
			std::wstring recentText = getCellString(recentSheet, rowit->second, colit->recent);
			outSheet->writeStr(row, col, recentText.c_str(), added, type);
		}
		row++;
	}
	row++;

	for (auto rowit = deletedRecords.begin(); rowit != deletedRecords.end(); rowit++)
	{
		col = 0;
		for (auto colit = allCols.begin(); colit != allCols.end(); colit++, col++)
		{
			CellType type = pastSheet->cellType(rowit->second, colit->past);
			if (pastSheet->isDate(rowit->second, colit->past))
				type = CELLTYPE_STRING;
			std::wstring pastText = getCellString(pastSheet, rowit->second, colit->past);
			outSheet->writeStr(row, col, pastText.c_str(), deleted, type);
		}
		row++;
	}
}


bool Compare::isID()
{
	return getCol(past->getSheet(0), L"unique") != -1
		&& getCol(recent->getSheet(0), L"unique") != -1;
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

std::deque<int> Compare::getColList(Sheet* sheet, std::wstring label)
{
	std::deque<int> colList;
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

std::wstring Compare::getCellString(libxl::Sheet* sheet, int row, int col)
{
	std::wstring text = L"";
	CellType type = sheet->cellType(row, col);
	if (sheet->isDate(row, col))
	{
		double val = sheet->readNum(row, col);
		int year, month, day, hour, min, sec, msec;
		std::unordered_map<int, std::wstring> month_map(
			{ {1,L"Jan"}, {2,L"Feb"}, {3,L"Mar"}, {4,L"Apr"}, {5,L"May"}, {6,L"Jun"},
			  {7,L"Jul"}, {8,L"Aug"}, {9,L"Sep"}, {10,L"Oct"}, {11,L"Nov"}, {12,L"Dec"} });
		output->dateUnpack(val, &year, &month, &day, &hour, &min, &sec, &msec);
		std::wstringstream ss;
		ss << std::setfill(L'0');
		ss << std::setw(2) << day << month_map[month] << year << L":" 
			<< hour << L":" << min << L":" << sec << L"." << msec;
		text = ss.str();
	}
	else if (type == CELLTYPE_BLANK || type == CELLTYPE_EMPTY)
		text = L"Blank";
	else
		text = sheet->readStr(row, col);
	return text;
}