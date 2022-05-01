#include "wombatregistry.h"

// Copyright 2022-2022 Pasquale J. Rinaldi, Jr.
// Distrubted under the terms of the GNU General Public License version 2

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/appicon"));
    WombatRegistry r;
    r.show();
    
    return a.exec();
}
