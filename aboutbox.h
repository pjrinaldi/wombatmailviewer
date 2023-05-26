#ifndef ABOUTBOX_H
#define ABOUTBOX_H

// Copyright 2013-2023 Pasquale J. Rinaldi, Jr.
// Distributed under the terms of the GNU General Public License version 2

#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>

#include "icons.h"

#include "fox-1.7/fx.h"

class AboutBox : public FXDialogBox
{
    FXDECLARE(AboutBox)

    private:
        FXVerticalFrame* mainframe;
	FXPNGImage* mainimage;
	FXImageFrame* imgframe;
        FXLabel* mainlabel;

        std::vector<std::string>* tags = NULL;

    protected:
        AboutBox() {}

    public:
        AboutBox(FXWindow* parent, const FXString& title);

};

#endif // ABOUTBOX_H
