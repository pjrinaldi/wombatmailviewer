#include "viewer.h"

// Copyright 2013-2023 Pasquale J. Rinaldi, Jr.
// Distributed under the terms of the GNU General Public License version 2

FXIMPLEMENT(Viewer,FXDialogBox, NULL, 0)

Viewer::Viewer(FXWindow* parent, const FXString& title):FXDialogBox(parent, title, DECOR_TITLE|DECOR_RESIZE|DECOR_BORDER|DECOR_CLOSE, 0, 0, 400, 300, 0,0,0,0, 0, 0)
{
    plainfont = new FXFont(this->getApp(), "monospace");
    vframe = new FXVerticalFrame(this, LAYOUT_TOP|LAYOUT_FILL_X|LAYOUT_FILL_Y, 0, 0, 0, 0, 0, 0, 0, 0);
    textview = new FXText(vframe, NULL, 0, LAYOUT_FILL_X|LAYOUT_FILL_Y);
    textview->setFont(plainfont);
    textview->setEditable(false);
}

void Viewer::SetText(FXString txt)
{
    textview->setText(txt);
}
