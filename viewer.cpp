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

void Viewer::GetText(FXString* buf)
{
    textview->getText(*buf);
}

void Viewer::GenerateReport(FXArray<FXString> taggedlist, std::vector<std::string> tags)
{
    // PLACE TITLE
    textview->appendText("Wombat Mail Report\n-----------------\n\n");
    // PLACE CONTENTS HEADER
    textview->appendText("Contents\n--------\n\n");
    // GENERATE TAGS AND THEIR COUNT
    for(int j=0; j < tags.size(); j++)
    {
        int tagcnt = 0;
        for(int i=0; i < taggedlist.no(); i++)
        {
            std::size_t found = taggedlist.at(i).find("|");
            FXString itemtag = taggedlist.at(i).mid(0, found);
            if(FXString::compare(itemtag, FXString(tags.at(j).c_str())) == 0)
                tagcnt++;
        }
        textview->appendText(FXString(tags.at(j).c_str()) + " (" + FXString::value(tagcnt) + ")\n");
    }
    textview->appendText("\n\n");
    // GENERATE TAG HEADER AND CONTENT
    for(int i=0; i < tags.size(); i++)
    {
        textview->appendText(FXString(tags.at(i).c_str()) + "\n");
        for(int j=0; j < tags.at(i).size(); j++)
            textview->appendText("-");
        textview->appendText("\n\n");
        for(int j=0; j < taggedlist.no(); j++)
        {
            std::size_t found = taggedlist.at(j).find("|");
            std::size_t rfound = taggedlist.at(j).rfind("|");
            FXString itemtag = taggedlist.at(j).mid(0, found);
            FXString itemhdr = taggedlist.at(j).mid(found+1, rfound - found - 1);
            std::size_t hfind1 = itemhdr.find("\t");
            std::size_t hfind2 = itemhdr.find("\t", hfind1+1);
            std::size_t hfind3 = itemhdr.find("\t", hfind2+1);
            std::size_t hfind4 = itemhdr.find("\t", hfind3+1);
            FXString itemcon = taggedlist.at(j).mid(rfound+1, taggedlist.at(j).length() - rfound);
            if(itemtag == tags.at(i).c_str())
            {
                textview->appendText("Mail Item:\t" + itemhdr.mid(0, hfind1) + "\n");
                textview->appendText("Folder:\t\t" + itemhdr.mid(hfind1+1, hfind2 - hfind1 - 1) + "\n");
                textview->appendText("From:\t\t" + itemhdr.mid(hfind2+1, hfind3 - hfind2 - 1) + "\n");
                textview->appendText("Date:\t\t" + itemhdr.mid(hfind3+1, hfind4 - hfind3 - 1) + "\n");
                textview->appendText("Subject:\t" + itemhdr.mid(hfind4+1, itemhdr.length() - hfind4 - 1) + "\n\n");
                textview->appendText(itemcon + "\n");
                for(int k=0; k < 80; k++)
                    textview->appendText("-");
                textview->appendText("\n\n");
            }
        }
    }
}
