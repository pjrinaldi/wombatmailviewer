/**************************************************************************
* Mail Archiver - A solution to store and manage offline e-mail files.    *
* Copyright (C) 2015-2016 Carlos Nihelton <carlosnsoliveira@gmail.com>    *
*                                                                         *
* This is a free software; you can redistribute it and/or                 *
* modify it under the terms of the GNU Library General Public             *
* License as published by the Free Software Foundation; either            *
* version 2 of the License, or (at your option) any later version.        *
*                                                                         *
* This software  is distributed in the hope that it will be useful,       *
* but WITHOUT ANY WARRANTY; without even the implied warranty of          *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
* GNU Library General Public License for more details.                    *
*                                                                         *
* You should have received a copy of the GNU Library General Public       *
* License along with this library; see the file COPYING.LIB. If not,      *
* write to the Free Software Foundation, Inc., 59 Temple Place,           *
* Suite 330, Boston, MA  02111-1307, USA                                  *
*                                                                         *
**************************************************************************/

#ifndef MAILARCHIVER_MSG_H
#define MAILARCHIVER_MSG_H

// std
#include <string>

// local
#include "pole.h"

namespace Core
{

class Msg
{
  private:
    POLE::Storage* m_File;
    bool m_Opened;
    std::string m_FileName;
    std::string m_SenderName, m_SenderAddress;
    std::string m_ReceiversNames, m_ReceiversAddresses;
    std::string m_Subject;
    std::string m_CC;
    std::string m_Bcc;
    std::string m_date;
    std::string m_body;
    std::string m_hash;
    bool m_hasAttachments;

  protected:
    const std::string getDateTimeFromStream(const char* stream);
    const std::string getStringFromStream(const char* stream);
    void visit(int indent, POLE::Storage* storage, std::string path);

  public:
    Msg();
    explicit Msg(const std::string& filename);

    ~Msg();

    bool open(const char* arg1);

    void loadBody();

    const std::string fileName();
    const std::string senderName();
    const std::string receiversNames();
    const std::string senderAddress();
    const std::string receiversAddresses();
    const std::string CCs();
    const std::string Bccs();
    const std::string subject();
    const std::string date();
    const std::string& body();
    const std::string hash();

    bool hasAttachments();

    void close();

    // No copy operators
    Msg(const Msg&) = delete;
    Msg operator=(const Msg&) = delete;

    // Move-only
    Msg(Msg&& rhs);
    Msg& operator=(Msg&& rhs);
};
}

#endif // MAILARCHIVER_MSG_H
