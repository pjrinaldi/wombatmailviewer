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

// std
#include <iostream>
#include <chrono>
#include <ctime>
#include <cstdint>
#include <iomanip>
#include <sstream>
#include <cstring> //memset
#include <stdexcept>
#include <algorithm>
#include <codecvt>

// local
#include "msg.h"
#include "md5.hh"

namespace Core
{

// Cosntructors:
Msg::Msg() : m_File(nullptr), m_Opened(false)
{
}

Msg::Msg(const std::string& filename) : m_File(nullptr), m_Opened(false), m_FileName(filename)
{
    open(filename.c_str());
}

// Getters
const std::string Msg::fileName()
{
    return m_FileName;
}

const std::string Msg::senderName()
{
    return m_SenderName;
}

const std::string Msg::senderAddress()
{
    return m_SenderAddress;
}

const std::string Msg::receiversNames()
{
    return m_ReceiversNames;
}

const std::string Msg::receiversAddresses()
{
    return m_ReceiversAddresses;
}

const std::string Msg::CCs()
{
    return m_CC;
}

const std::string Msg::Bccs()
{
    return m_Bcc;
}

const std::string Msg::subject()
{
    return m_Subject;
}

const std::string Msg::date()
{
    return m_date;
}

void Msg::loadBody()
{
    if (m_body.empty() && m_Opened) {
        m_body = getStringFromStream("__substg1.0_1000001F");
    }
}

const std::string& Msg::body()
{
    loadBody();
    return m_body;
}

const std::string Msg::hash()
{
    if (m_hash.empty()) {
        MD5 md5(m_File->internalFile());
        m_hash.assign(md5.hex_digest());
    }
    return m_hash;
}

bool Msg::hasAttachments()
{
    return m_hasAttachments;
}

// Protected
void Msg::visit(int indent, POLE::Storage* storage, std::string path)
{
    std::list<std::string> entries;
    entries = storage->entries(path);

    std::list<std::string>::iterator it;
    for (it = entries.begin(); it != entries.end(); ++it) {
        std::string name     = *it;
        std::string fullname = path + name;
        for (int j = 0; j < indent; j++) std::cout << "    ";
        POLE::Stream* ss = new POLE::Stream(storage, fullname);
        std::cout << name;
        if (ss)
            if (!ss->fail())
                std::cout << "  (" << ss->size() << ")";
        std::cout << std::endl;
        delete ss;

        if (storage->isDirectory(fullname))
            visit(indent + 1, storage, fullname + "/");
    }
}

// Open
bool Msg::open(const char* arg1)
{
    if (m_Opened) {
        close();
    }

    m_File   = new POLE::Storage(arg1);
    m_Opened = m_File->open();
    if (m_Opened) {
        // Look for Sender Name
        m_SenderName = getStringFromStream("__substg1.0_0C1A001F");
        if (m_SenderName.empty())
            m_SenderName = getStringFromStream("__substg1.0_3FFA001F");
        if (m_SenderName.empty())
            m_SenderName = getStringFromStream("__substg1.0_0042001F");

        // Sender Address
        m_SenderAddress = getStringFromStream("__substg1.0_0065001F");
        if (m_SenderAddress.empty())
            m_SenderAddress = getStringFromStream("__substg1.0_0C1F001F");
        if (m_SenderAddress.empty())
            m_SenderAddress = getStringFromStream("__substg1.0_800B001F");
        if (m_SenderAddress.empty())
            m_SenderAddress = getStringFromStream("__substg1.0_3FFA001F");
        if (m_SenderAddress.empty())
            m_SenderAddress = getStringFromStream("__substg1.0_5D01001F");
        if (m_SenderAddress.empty())
            m_SenderAddress = getStringFromStream("__substg1.0_5D02001F");

        // Subject
        m_Subject = getStringFromStream("__substg1.0_0070001F");
        if (m_Subject.empty())
            m_Subject = getStringFromStream("__substg1.0_0E1D001F");
        if (m_Subject.empty())
            m_Subject = getStringFromStream("__substg1.0_0037001F");

        std::transform(m_Subject.begin(), m_Subject.end(), m_Subject.begin(), [](char c) -> char {
            if (c == '\'')
                return '\"';
            else
                return c;
        });

        // BCC
        m_Bcc = getStringFromStream("__substg1.0_0E02001F");
        // CC
        m_CC = getStringFromStream("__substg1.0_0E03001F");

        // Receivers Names
        m_ReceiversNames = getStringFromStream("__substg1.0_0E04001F");

        // Receivers Addresses
        m_ReceiversAddresses = getStringFromStream("__substg1.0_5D01001F");
        if (m_ReceiversAddresses.empty())
            m_ReceiversAddresses = getStringFromStream("__substg1.0_5D09001F");

        // Sent date
        m_date = getDateTimeFromStream("__properties_version1.0");

        // If has attachments.
        m_hasAttachments = m_File->exists("__attach_version1.0_#00000000");
    }
    return m_Opened;
}

// Close
void Msg::close()
{
    m_File->close();
    m_SenderName.clear();
    m_SenderAddress.clear();
    m_ReceiversNames.clear();
    m_ReceiversAddresses.clear();
    m_Subject.clear();
    m_CC.clear();
    m_Bcc.clear();
    m_date.clear();
    m_body.clear();
    m_hasAttachments = false;
    m_Opened         = false;

    delete m_File;
}

const std::string Msg::getDateTimeFromStream(const char* stream)
{
    std::stringstream ss;
    POLE::Stream requested_stream(m_File, stream);
    if (requested_stream.fail())
        std::cout << "Failed to obtain stream: " << stream << '\n';
    else {
        uint64_t microt;
        uint32_t address = 0;
        int read;
        do {
            read = requested_stream.read(reinterpret_cast<unsigned char*>(&address), 4);
        } while (read > 0 && address != 0x00390040u && address != 0x0E060040u && address != 0x80080040u);

        if (address == 0x00390040u || address == 0x0E060040u || address == 0x80080040u) {
            requested_stream.read(reinterpret_cast<unsigned char*>(&address), 4);
            requested_stream.read(reinterpret_cast<unsigned char*>(&microt), 8);

            microt = microt / 10;

            std::tm t;
            t.tm_year = 1601 - 1900;
            t.tm_mday = 1;
            t.tm_mon  = 0;
            t.tm_hour = 0;
            t.tm_min  = 0;
            t.tm_sec  = 0;

            std::time_t pTypTime = std::mktime(&t); // PTypTime() ==
                                                    // 01/01/1601
                                                    // 00:00:00.

            std::chrono::system_clock::time_point tpDate =
                (std::chrono::system_clock::from_time_t(pTypTime) + std::chrono::microseconds(microt));
            std::time_t ttDate = std::chrono::system_clock::to_time_t(tpDate);

            t = *(std::localtime(&ttDate));

            ss << std::setw(4) << std::setfill('0') << t.tm_year + 1900 << '-' << std::setw(2)
               << std::setfill('0') << t.tm_mon + 1 << '-' << std::setw(2) << std::setfill('0') << t.tm_mday;
        }
    }
    return ss.str();
}

const std::string Msg::getStringFromStream(const char* stream)
{
    std::u16string helper;
    std::string ret;
    unsigned char buffer[16];
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;

    POLE::Stream requested_stream(m_File, stream);
    if (!requested_stream.fail()) {
        int read;
        ret.reserve(requested_stream.size() / 2);
        helper.reserve(requested_stream.size() / 2);

        do {
            read = requested_stream.read(buffer, sizeof(buffer));
            helper.insert(helper.length(), reinterpret_cast<char16_t*>(buffer), read / 2);
        } while (read >= sizeof(buffer));
        if (helper.length() > 0)
            if (helper.at(helper.length() - 1) == (char16_t)0)
                helper.resize(helper.length() - 1);
        ret = converter.to_bytes(helper);
    }
    return ret;
}

// Destructor
Msg::~Msg()
{
    close();
}

// Move semantics
Msg::Msg(Msg&& rhs)
    : m_Opened(std::move(rhs.m_Opened)), m_FileName(std::move(rhs.m_FileName)),
      m_SenderName(std::move(rhs.m_SenderName)), m_SenderAddress(std::move(rhs.m_SenderAddress)),
      m_ReceiversNames(std::move(rhs.m_ReceiversNames)),
      m_ReceiversAddresses(std::move(rhs.m_ReceiversAddresses)), m_Subject(std::move(rhs.m_Subject)),
      m_CC(std::move(rhs.m_CC)), m_Bcc(std::move(rhs.m_Bcc)), m_date(std::move(rhs.m_date)),
      m_body(std::move(rhs.m_body)), m_hash(std::move(rhs.m_hash)),
      m_hasAttachments(std::move(rhs.m_hasAttachments))
{
    m_File     = rhs.m_File;
    rhs.m_File = nullptr;
}

Msg& Msg::operator=(Msg&& rhs)
{
    if (this != &rhs) {
        m_Opened             = std::move(rhs.m_Opened);
        m_FileName           = std::move(rhs.m_FileName);
        m_SenderName         = std::move(rhs.m_SenderName);
        m_SenderAddress      = std::move(rhs.m_SenderAddress);
        m_ReceiversNames     = std::move(rhs.m_ReceiversNames);
        m_Subject            = std::move(rhs.m_Subject);
        m_ReceiversAddresses = std::move(rhs.m_ReceiversAddresses);
        m_CC                 = std::move(rhs.m_CC);
        m_Bcc                = std::move(rhs.m_Bcc);
        m_date               = std::move(rhs.m_date);
        m_body               = std::move(rhs.m_body);
        m_hash               = std::move(rhs.m_hash);
        m_hasAttachments     = std::move(rhs.m_hasAttachments);
        m_File               = rhs.m_File;
        rhs.m_File           = nullptr;
    }
    return *this;
}
}
