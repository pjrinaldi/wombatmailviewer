#ifndef VIEWER_H
#define VIEWER_H

// Copyright 2013-2023 Pasquale J. Rinaldi, Jr.
// Distributed under the terms of the GNU General Public License version 2

#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string>
#include <vector>

#include "icons.h"

#include "fox-1.7/fx.h"

class Viewer : public FXDialogBox
{
    FXDECLARE(Viewer)

    private:
        FXVerticalFrame* vframe;
        FXText* textview;
        FXFont* plainfont;

    protected:
        Viewer() {}

    public:
        Viewer(FXWindow* parent, const FXString& title);
        void GenerateReport(FXArray<FXString> taggedlist, std::vector<std::string> tags);
        void GetText(FXString* buf);
};

#endif // VIEWER_H
