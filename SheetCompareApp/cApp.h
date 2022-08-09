/*
* Author: Iain Weissburg
* Date: 8/1/2022
*/

#pragma once

#include "wx/wx.h"
#include "cFrame.h"

class cApp : public wxApp
{
public:
	cApp();
	~cApp();
	virtual bool OnInit();

private:
	cFrame* frame1 = nullptr;

};

