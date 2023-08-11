#ifndef PARSEMSG_H
#define PARSEMSG_H

// Copyright 2023-2023 Pasquale J. Rinaldi, Jr.
// Distributed under the terms of the GNU General Public License version 2

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <vector>
#include <cstdint>

#include "cfb.h"

std::string SenderName(std::string* mailboxpath);
std::string SenderAddress(std::string* mailboxpath);

#endif // PARSEMSG_H
