////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2026 Laurent Gomila (laurent@sfml-dev.org)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

#include <SFML/Window/Harmony/ClipboardImpl.hpp>

#include <SFML/System/Err.hpp>
#include <SFML/System/String.hpp>

#include <database/pasteboard/oh_pasteboard.h>
#include <database/pasteboard/oh_pasteboard_err_code.h>
#include <database/udmf/udmf.h>
#include <database/udmf/udmf_err_code.h>
#include <database/udmf/uds.h>
#include <memory>
#include <ostream>


namespace sf::priv
{
namespace
{
using PasteboardPtr = std::unique_ptr<OH_Pasteboard, decltype(&OH_Pasteboard_Destroy)>;
using DataPtr       = std::unique_ptr<OH_UdmfData, decltype(&OH_UdmfData_Destroy)>;
using RecordPtr     = std::unique_ptr<OH_UdmfRecord, decltype(&OH_UdmfRecord_Destroy)>;
using PlainTextPtr  = std::unique_ptr<OH_UdsPlainText, decltype(&OH_UdsPlainText_Destroy)>;
} // namespace


String ClipboardImpl::getString()
{
    const PasteboardPtr pasteboard(OH_Pasteboard_Create(), OH_Pasteboard_Destroy);
    if (!pasteboard)
        return {};

    int           status = ERR_INNER_ERROR;
    const DataPtr data(OH_Pasteboard_GetData(pasteboard.get(), &status), OH_UdmfData_Destroy);
    if (!data || status != ERR_OK)
    {
        if (status != ERR_PERMISSION_ERROR)
            err() << "Failed to read the Harmony pasteboard (error " << status << ')' << std::endl;
        return {};
    }

    const PlainTextPtr plainText(OH_UdsPlainText_Create(), OH_UdsPlainText_Destroy);
    if (!plainText || OH_UdmfData_GetPrimaryPlainText(data.get(), plainText.get()) != UDMF_E_OK)
        return {};

    const char* content = OH_UdsPlainText_GetContent(plainText.get());
    if (!content)
        return {};

    const auto* const end = content + std::char_traits<char>::length(content);
    return String::fromUtf8(content, end);
}


void ClipboardImpl::setString(const String& text)
{
    const PasteboardPtr pasteboard(OH_Pasteboard_Create(), OH_Pasteboard_Destroy);
    const DataPtr       data(OH_UdmfData_Create(), OH_UdmfData_Destroy);
    const RecordPtr     record(OH_UdmfRecord_Create(), OH_UdmfRecord_Destroy);
    const PlainTextPtr  plainText(OH_UdsPlainText_Create(), OH_UdsPlainText_Destroy);
    if (!pasteboard || !data || !record || !plainText)
    {
        err() << "Failed to allocate Harmony pasteboard data" << std::endl;
        return;
    }

    const auto utf8 = text.toUtf8();
    if (OH_UdsPlainText_SetContent(plainText.get(), reinterpret_cast<const char*>(utf8.c_str())) != UDMF_E_OK ||
        OH_UdmfRecord_AddPlainText(record.get(), plainText.get()) != UDMF_E_OK ||
        OH_UdmfData_AddRecord(data.get(), record.get()) != UDMF_E_OK)
    {
        err() << "Failed to create Harmony plain-text pasteboard data" << std::endl;
        return;
    }

    const int status = OH_Pasteboard_SetData(pasteboard.get(), data.get());
    if (status != ERR_OK && status != ERR_PERMISSION_ERROR)
        err() << "Failed to write the Harmony pasteboard (error " << status << ')' << std::endl;
}

} // namespace sf::priv
