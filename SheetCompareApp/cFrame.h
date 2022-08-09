/*
* Author: Iain Weissburg
* Date: 8/1/2022
*/
#pragma once

#include "wx/wx.h"
#include "wx/filepicker.h"
#include "wx/valnum.h"
#include "Compare.h"

class cFrame : public wxFrame
{
public:
	cFrame();
	~cFrame();

	wxButton* btn1 = nullptr;
	wxFilePickerCtrl* pastFile = nullptr;
	wxFilePickerCtrl* recentFile = nullptr;
	wxTextCtrl* rowInput = nullptr;
	//wxTextCtrl* output = nullptr;

	wxStaticText* pastText = nullptr;
	wxStaticText* recentText = nullptr;
	wxStaticText* rowText = nullptr;

	void PerformTransfer(wxCommandEvent& evt);
	void ResetButton(wxFileDirPickerEvent& evt);

	wxDECLARE_EVENT_TABLE();

};

