#ifndef PARSEMSG_H
#define PARSEMSG_H

// Copyright 2023-2023 Pasquale J. Rinaldi, Jr.
// Distributed under the terms of the GNU General Public License version 2

#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdio.h>
#include <string>
#include <vector>
#include <cstdint>

#include "cfb.h"

std::string SenderName(std::string* mailboxpath);
std::string SenderAddress(std::string* mailboxpath);
void ReceiverNames(std::string* mailboxpath, std::vector<std::string>* receivernames);
void ReceiverAddresses(std::string* mailboxpath, std::vector<std::string>* receiveraddresses);

#endif // PARSEMSG_H
