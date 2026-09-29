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

#include <SFML/Window/Harmony/CancelableInitialization.hpp>
#include <SFML/Window/Harmony/ClipboardImpl.hpp>
#include <SFML/Window/Harmony/InputMethodImpl.hpp>
#include <SFML/Window/Harmony/NativeApp.hpp>
#include <SFML/Window/Harmony/NativeAppImpl.hpp>

#include <SFML/System/Err.hpp>
#include <SFML/System/String.hpp>

#include <algorithm>
#include <atomic>
#include <inputmethod/inputmethod_controller_capi.h>
#include <inputmethod/inputmethod_text_config_capi.h>
#include <limits>
#include <mutex>
#include <ostream>
#include <string>
#include <string_view>
#include <utility>


namespace
{
constexpr std::size_t maxTextRequestLength = 8 * 1024;


struct EditingState
{
    InputMethod_TextEditorProxy* editor{};
    std::u16string               text;
    std::size_t                  selectionStart{};
    std::size_t                  selectionEnd{};
    bool                         hasPreview{};
    std::size_t                  previewStart{};
    std::size_t                  previewEnd{};
    std::size_t                  pendingBackwardDeletions{};
    std::size_t                  pendingForwardDeletions{};
};


enum class KeyboardVisibility : std::int8_t
{
    Unknown = -1,
    Hidden,
    Shown
};


std::mutex                                  inputMethodMutex;
InputMethod_TextEditorProxy*                editorProxy{};
InputMethod_InputMethodProxy*               inputMethodProxy{};
sf::priv::Harmony::CancelableInitialization inputMethodInitialization;
std::atomic<std::int32_t>                   inputWindowId{};
std::atomic<KeyboardVisibility>             keyboardVisibility{KeyboardVisibility::Unknown};

std::mutex   editingStateMutex;
EditingState editingState;


[[nodiscard]] bool isHighSurrogate(char16_t value)
{
    return value >= 0xD800 && value <= 0xDBFF;
}


[[nodiscard]] bool isLowSurrogate(char16_t value)
{
    return value >= 0xDC00 && value <= 0xDFFF;
}


[[nodiscard]] bool isUtf16Boundary(std::u16string_view text, std::size_t index)
{
    return index <= text.size() &&
           !(index && index < text.size() && isHighSurrogate(text[index - 1]) && isLowSurrogate(text[index]));
}


[[nodiscard]] std::size_t floorUtf16Boundary(std::u16string_view text, std::size_t index)
{
    index = std::min(index, text.size());
    return isUtf16Boundary(text, index) ? index : index - 1;
}


[[nodiscard]] std::size_t ceilUtf16Boundary(std::u16string_view text, std::size_t index)
{
    index = std::min(index, text.size());
    return isUtf16Boundary(text, index) ? index : index + 1;
}


[[nodiscard]] std::size_t previousCodepoint(std::u16string_view text, std::size_t index)
{
    index = floorUtf16Boundary(text, index);
    if (!index)
        return 0;

    --index;
    if (index && isLowSurrogate(text[index]) && isHighSurrogate(text[index - 1]))
        --index;
    return index;
}


[[nodiscard]] std::size_t nextCodepoint(std::u16string_view text, std::size_t index)
{
    index = ceilUtf16Boundary(text, index);
    if (index >= text.size())
        return text.size();

    return index + ((isHighSurrogate(text[index]) && index + 1 < text.size() && isLowSurrogate(text[index + 1])) ? 2 : 1);
}


[[nodiscard]] bool isValidUtf16(const char16_t* text, std::size_t length)
{
    if (!text)
        return length == 0;

    for (std::size_t index = 0; index < length; ++index)
    {
        if (isHighSurrogate(text[index]))
        {
            if (index + 1 >= length || !isLowSurrogate(text[index + 1]))
                return false;
            ++index;
        }
        else if (isLowSurrogate(text[index]))
        {
            return false;
        }
    }
    return true;
}


[[nodiscard]] std::u16string sanitizeUtf16(const char16_t* text, std::size_t length)
{
    std::u16string sanitized;
    if (!text || !length)
        return sanitized;

    sanitized.reserve(length);
    for (std::size_t index = 0; index < length; ++index)
    {
        if (isHighSurrogate(text[index]))
        {
            if (index + 1 < length && isLowSurrogate(text[index + 1]))
            {
                sanitized.push_back(text[index]);
                sanitized.push_back(text[++index]);
            }
            else
            {
                sanitized.push_back(u'\uFFFD');
            }
        }
        else if (isLowSurrogate(text[index]))
        {
            sanitized.push_back(u'\uFFFD');
        }
        else
        {
            sanitized.push_back(text[index]);
        }
    }
    return sanitized;
}


[[nodiscard]] std::size_t countCodepoints(std::u16string_view text)
{
    std::size_t count{};
    for (std::size_t index = 0; index < text.size(); ++count)
        index = nextCodepoint(text, index);
    return count;
}


[[nodiscard]] std::int32_t toImeIndex(std::size_t index)
{
    constexpr auto maximum = static_cast<std::size_t>(std::numeric_limits<std::int32_t>::max());
    return static_cast<std::int32_t>(std::min(index, maximum));
}


void activateEditor(InputMethod_TextEditorProxy* editor)
{
    const std::lock_guard lock(editingStateMutex);
    editingState        = {};
    editingState.editor = editor;
    keyboardVisibility  = KeyboardVisibility::Unknown;
}


void deactivateEditor(InputMethod_TextEditorProxy* editor)
{
    const std::lock_guard lock(editingStateMutex);
    if (editingState.editor == editor)
    {
        editingState       = {};
        keyboardVisibility = KeyboardVisibility::Unknown;
    }
}


void submitCommittedText(std::u16string_view text)
{
    const auto committed = sf::String::fromUtf16(text.begin(), text.end(), U'\uFFFD');
    for (const char32_t codepoint : committed)
        sf::priv::Harmony::submitText(codepoint);
}


void pushEditingKey(sf::Keyboard::Key key, sf::Keyboard::Scancode scan)
{
    sf::priv::Harmony::enqueueEvent(sf::Event::KeyPressed{key, scan, false, false, false, false});
    sf::priv::Harmony::enqueueEvent(sf::Event::KeyReleased{key, scan, false, false, false, false});
}


void pushDeleteEvents(bool backward, std::size_t count)
{
    count = std::min(count, maxTextRequestLength);
    for (std::size_t index = 0; index < count; ++index)
    {
        if (backward)
        {
            pushEditingKey(sf::Keyboard::Key::Backspace, sf::Keyboard::Scan::Backspace);
            sf::priv::Harmony::submitText(U'\b');
        }
        else
        {
            pushEditingKey(sf::Keyboard::Key::Delete, sf::Keyboard::Scan::Delete);
        }
    }
}


[[nodiscard]] std::size_t mapPositionAfterErase(std::size_t position, std::size_t start, std::size_t end)
{
    if (position <= start)
        return position;
    if (position >= end)
        return position - (end - start);
    return start;
}


void eraseRange(EditingState& state, std::size_t start, std::size_t end)
{
    if (start >= end)
        return;

    if (state.hasPreview)
    {
        state.previewStart = mapPositionAfterErase(state.previewStart, start, end);
        state.previewEnd   = mapPositionAfterErase(state.previewEnd, start, end);
    }
    state.text.erase(start, end - start);
}


[[nodiscard]] std::size_t countCommittedCodepoints(const EditingState& state, std::size_t start, std::size_t end)
{
    const std::u16string_view text(state.text);
    if (!state.hasPreview)
        return countCodepoints(text.substr(start, end - start));

    std::size_t count{};
    if (start < state.previewStart)
    {
        const auto segmentEnd = std::min(end, state.previewStart);
        count += countCodepoints(text.substr(start, segmentEnd - start));
    }
    if (end > state.previewEnd)
    {
        const auto segmentStart = std::max(start, state.previewEnd);
        count += countCodepoints(text.substr(segmentStart, end - segmentStart));
    }
    return count;
}


void replaceWithCommittedText(InputMethod_TextEditorProxy* editor, const std::u16string& committed)
{
    std::size_t backwardDeletions{};
    std::size_t forwardDeletions{};
    {
        const std::lock_guard lock(editingStateMutex);
        if (editingState.editor != editor)
            return;

        const std::size_t start = editingState.hasPreview ? editingState.previewStart : editingState.selectionStart;
        const std::size_t end   = editingState.hasPreview ? editingState.previewEnd : editingState.selectionEnd;
        backwardDeletions       = editingState.hasPreview ? editingState.pendingBackwardDeletions
                                                          : countCommittedCodepoints(editingState, start, end);
        forwardDeletions        = editingState.hasPreview ? editingState.pendingForwardDeletions : 0;
        editingState.text.replace(start, end - start, committed);
        editingState.selectionStart           = start + committed.size();
        editingState.selectionEnd             = editingState.selectionStart;
        editingState.hasPreview               = false;
        editingState.previewStart             = 0;
        editingState.previewEnd               = 0;
        editingState.pendingBackwardDeletions = 0;
        editingState.pendingForwardDeletions  = 0;
    }

    // Calling back into the SFML event queue while holding editingStateMutex
    // can re-enter the IME through application code, so commits are forwarded
    // only after the editing state is internally consistent and unlocked.
    pushDeleteEvents(true, backwardDeletions);
    pushDeleteEvents(false, forwardDeletions);
    submitCommittedText(committed);
}


[[nodiscard]] std::size_t lineStart(std::u16string_view text, std::size_t position)
{
    position = floorUtf16Boundary(text, position);
    while (position)
    {
        const auto previous = previousCodepoint(text, position);
        if (text[previous] == u'\n' || text[previous] == u'\r')
            break;
        position = previous;
    }
    return position;
}


[[nodiscard]] std::size_t lineEnd(std::u16string_view text, std::size_t position)
{
    position = ceilUtf16Boundary(text, position);
    while (position < text.size() && text[position] != u'\n' && text[position] != u'\r')
        position = nextCodepoint(text, position);
    return position;
}


[[nodiscard]] std::size_t advanceCodepoints(std::u16string_view text, std::size_t begin, std::size_t end, std::size_t count)
{
    while (begin < end && count)
    {
        begin = std::min(nextCodepoint(text, begin), end);
        --count;
    }
    return begin;
}


[[nodiscard]] std::size_t moveVertically(std::u16string_view text, std::size_t position, bool up)
{
    const auto currentStart = lineStart(text, position);
    const auto column       = countCodepoints(text.substr(currentStart, position - currentStart));

    if (up)
    {
        if (!currentStart)
            return position;

        auto previousEnd = currentStart;
        while (previousEnd && (text[previousEnd - 1] == u'\n' || text[previousEnd - 1] == u'\r'))
            --previousEnd;
        const auto previousStart = lineStart(text, previousEnd);
        return advanceCodepoints(text, previousStart, previousEnd, column);
    }

    auto nextStart = lineEnd(text, position);
    if (nextStart == text.size())
        return position;
    while (nextStart < text.size() && (text[nextStart] == u'\n' || text[nextStart] == u'\r'))
        ++nextStart;
    return advanceCodepoints(text, nextStart, lineEnd(text, nextStart), column);
}


void getTextConfig(InputMethod_TextEditorProxy* editor, InputMethod_TextConfig* config)
{
    if (!config)
        return;

    std::int32_t selectionStart{};
    std::int32_t selectionEnd{};
    {
        const std::lock_guard lock(editingStateMutex);
        if (editingState.editor == editor)
        {
            selectionStart = toImeIndex(editingState.selectionStart);
            selectionEnd   = toImeIndex(editingState.selectionEnd);
        }
    }

    (void)OH_TextConfig_SetInputType(config, IME_TEXT_INPUT_TYPE_TEXT);
    (void)OH_TextConfig_SetEnterKeyType(config, IME_ENTER_KEY_NEWLINE);
    (void)OH_TextConfig_SetPreviewTextSupport(config, true);
    (void)OH_TextConfig_SetSelection(config, selectionStart, selectionEnd);
    (void)OH_TextConfig_SetWindowId(config, inputWindowId.load());
}


void insertText(InputMethod_TextEditorProxy* editor, const char16_t* text, std::size_t length)
{
    if (!text || !length)
        return;

    replaceWithCommittedText(editor, sanitizeUtf16(text, length));
}


void deleteText(InputMethod_TextEditorProxy* editor, std::int32_t length, bool backward)
{
    if (length <= 0)
        return;

    const auto  request = std::min(static_cast<std::size_t>(length), maxTextRequestLength);
    std::size_t eventCount{};
    {
        const std::lock_guard lock(editingStateMutex);
        if (editingState.editor != editor)
            return;

        const bool  hasSelection = editingState.selectionStart != editingState.selectionEnd;
        std::size_t start{};
        std::size_t end{};
        if (hasSelection)
        {
            start = editingState.selectionStart;
            end   = editingState.selectionEnd;
        }
        else if (backward)
        {
            end   = editingState.selectionEnd;
            start = floorUtf16Boundary(editingState.text, end > request ? end - request : 0);
        }
        else
        {
            start = editingState.selectionEnd;
            end   = ceilUtf16Boundary(editingState.text, start + std::min(request, editingState.text.size() - start));
        }

        eventCount = countCommittedCodepoints(editingState, start, end);
        if (!hasSelection && end - start < request)
            eventCount += request - (end - start);

        eraseRange(editingState, start, end);
        editingState.selectionStart = start;
        editingState.selectionEnd   = start;
    }

    pushDeleteEvents(backward, eventCount);
}


void deleteForward(InputMethod_TextEditorProxy* editor, std::int32_t length)
{
    deleteText(editor, length, false);
}


void deleteBackward(InputMethod_TextEditorProxy* editor, std::int32_t length)
{
    deleteText(editor, length, true);
}


void sendKeyboardStatus(InputMethod_TextEditorProxy* editor, InputMethod_KeyboardStatus status)
{
    const std::lock_guard lock(editingStateMutex);
    if (editingState.editor != editor)
        return;

    if (status == IME_KEYBOARD_STATUS_HIDE)
        keyboardVisibility = KeyboardVisibility::Hidden;
    else if (status == IME_KEYBOARD_STATUS_SHOW)
        keyboardVisibility = KeyboardVisibility::Shown;
}


void sendEnterKey(InputMethod_TextEditorProxy* editor, InputMethod_EnterKeyType)
{
    replaceWithCommittedText(editor, u"\r");
}


void moveCursor(InputMethod_TextEditorProxy* editor, InputMethod_Direction direction)
{
    sf::Keyboard::Key      key{sf::Keyboard::Key::Unknown};
    sf::Keyboard::Scancode scan{sf::Keyboard::Scan::Unknown};
    {
        const std::lock_guard lock(editingStateMutex);
        if (editingState.editor != editor)
            return;

        const bool hasSelection = editingState.selectionStart != editingState.selectionEnd;
        auto       position     = editingState.selectionEnd;
        switch (direction)
        {
            case IME_DIRECTION_LEFT:
                key      = sf::Keyboard::Key::Left;
                scan     = sf::Keyboard::Scan::Left;
                position = hasSelection ? editingState.selectionStart : previousCodepoint(editingState.text, position);
                break;
            case IME_DIRECTION_RIGHT:
                key      = sf::Keyboard::Key::Right;
                scan     = sf::Keyboard::Scan::Right;
                position = hasSelection ? editingState.selectionEnd : nextCodepoint(editingState.text, position);
                break;
            case IME_DIRECTION_UP:
                key  = sf::Keyboard::Key::Up;
                scan = sf::Keyboard::Scan::Up;
                position = moveVertically(editingState.text, hasSelection ? editingState.selectionStart : position, true);
                break;
            case IME_DIRECTION_DOWN:
                key  = sf::Keyboard::Key::Down;
                scan = sf::Keyboard::Scan::Down;
                position = moveVertically(editingState.text, hasSelection ? editingState.selectionEnd : position, false);
                break;
            default:
                return;
        }

        editingState.selectionStart = position;
        editingState.selectionEnd   = position;
    }

    pushEditingKey(key, scan);
}


void setSelection(InputMethod_TextEditorProxy* editor, std::int32_t start, std::int32_t end)
{
    const std::lock_guard lock(editingStateMutex);
    if (editingState.editor != editor)
        return;

    const auto clampIndex = [&](std::int32_t value)
    { return value <= 0 ? std::size_t{} : std::min(static_cast<std::size_t>(value), editingState.text.size()); };

    auto selectionStart = clampIndex(std::min(start, end));
    auto selectionEnd   = clampIndex(std::max(start, end));
    if (selectionStart == selectionEnd)
    {
        selectionStart = ceilUtf16Boundary(editingState.text, selectionStart);
        selectionEnd   = selectionStart;
    }
    else
    {
        selectionStart = floorUtf16Boundary(editingState.text, selectionStart);
        selectionEnd   = ceilUtf16Boundary(editingState.text, selectionEnd);
    }

    editingState.selectionStart = selectionStart;
    editingState.selectionEnd   = selectionEnd;
}


void extendAction(InputMethod_TextEditorProxy* editor, InputMethod_ExtendAction action)
{
    if (action == IME_EXTEND_ACTION_SELECT_ALL)
    {
        const std::lock_guard lock(editingStateMutex);
        if (editingState.editor == editor)
        {
            editingState.selectionStart = 0;
            editingState.selectionEnd   = editingState.text.size();
        }
        return;
    }

    if (action == IME_EXTEND_ACTION_PASTE)
    {
        const auto pasted = sf::priv::ClipboardImpl::getString().toUtf16(static_cast<std::uint16_t>(0xFFFD));
        if (!pasted.empty())
            replaceWithCommittedText(editor, pasted);
        return;
    }

    std::u16string selected;
    std::size_t    deletedCodepoints{};
    {
        const std::lock_guard lock(editingStateMutex);
        if (editingState.editor != editor || editingState.selectionStart == editingState.selectionEnd)
            return;

        selected = editingState.text.substr(editingState.selectionStart,
                                            editingState.selectionEnd - editingState.selectionStart);
        if (action == IME_EXTEND_ACTION_CUT)
        {
            deletedCodepoints = countCommittedCodepoints(editingState, editingState.selectionStart, editingState.selectionEnd);
            const auto cursor = editingState.selectionStart;
            eraseRange(editingState, editingState.selectionStart, editingState.selectionEnd);
            editingState.selectionStart = cursor;
            editingState.selectionEnd   = cursor;
        }
        else if (action != IME_EXTEND_ACTION_COPY)
        {
            return;
        }
    }

    sf::priv::ClipboardImpl::setString(sf::String::fromUtf16(selected.begin(), selected.end(), U'\uFFFD'));
    if (action == IME_EXTEND_ACTION_CUT)
        pushDeleteEvents(true, deletedCodepoints);
}


void getText(InputMethod_TextEditorProxy* editor, std::int32_t number, char16_t* text, std::size_t* length, bool left)
{
    if (!length)
        return;

    const std::size_t capacity = *length;
    *length                    = 0;
    if (!text || !capacity || number <= 0)
        return;

    std::u16string result;
    {
        const std::lock_guard lock(editingStateMutex);
        if (editingState.editor != editor)
            return;

        const auto request = std::min(static_cast<std::size_t>(number), maxTextRequestLength);
        if (left)
        {
            const auto end   = editingState.selectionStart;
            const auto start = ceilUtf16Boundary(editingState.text, end > request ? end - request : 0);
            result           = editingState.text.substr(start, end - start);
        }
        else
        {
            const auto start = editingState.selectionEnd;
            const auto end   = floorUtf16Boundary(editingState.text,
                                                start + std::min(request, editingState.text.size() - start));
            result           = editingState.text.substr(start, end - start);
        }
    }

    std::size_t copied = std::min(result.size(), capacity);
    copied             = floorUtf16Boundary(result, copied);
    std::copy_n(result.begin(), copied, text);
    if (copied < capacity)
        text[copied] = u'\0';
    *length = copied;
}


void getLeftText(InputMethod_TextEditorProxy* editor, std::int32_t number, char16_t* text, std::size_t* length)
{
    getText(editor, number, text, length, true);
}


void getRightText(InputMethod_TextEditorProxy* editor, std::int32_t number, char16_t* text, std::size_t* length)
{
    getText(editor, number, text, length, false);
}


std::int32_t getCursorIndex(InputMethod_TextEditorProxy* editor)
{
    const std::lock_guard lock(editingStateMutex);
    return editingState.editor == editor ? toImeIndex(editingState.selectionEnd) : 0;
}


std::int32_t receivePrivateCommand(InputMethod_TextEditorProxy*, InputMethod_PrivateCommand**, std::size_t)
{
    return IME_ERR_OK;
}


std::int32_t setPreviewText(InputMethod_TextEditorProxy* editor,
                            const char16_t*              text,
                            std::size_t                  length,
                            std::int32_t                 start,
                            std::int32_t                 end)
{
    if ((!text && length) || length > maxTextRequestLength)
        return text ? IME_ERR_PARAMCHECK : IME_ERR_NULL_POINTER;
    if (!isValidUtf16(text, length))
        return IME_ERR_PARAMCHECK;
    if ((start == -1) != (end == -1) || start < -1 || end < -1 || (start != -1 && start > end))
        return IME_ERR_PARAMCHECK;

    const std::u16string  preview(text ? text : u"", length);
    const std::lock_guard lock(editingStateMutex);
    if (editingState.editor != editor)
        return IME_ERR_DETACHED;

    std::size_t replaceStart{};
    std::size_t replaceEnd{};
    const bool  hasSelection = editingState.selectionStart != editingState.selectionEnd;
    if (hasSelection)
    {
        // OpenHarmony rejects an explicit preview range while text is selected.
        // The default range replaces the selected editor text.
        if (start != -1)
            return IME_ERR_PARAMCHECK;
        replaceStart = editingState.selectionStart;
        replaceEnd   = editingState.selectionEnd;
    }
    else if (start == -1)
    {
        replaceStart = editingState.hasPreview ? editingState.previewStart : editingState.selectionStart;
        replaceEnd   = editingState.hasPreview ? editingState.previewEnd : editingState.selectionEnd;
    }
    else
    {
        replaceStart = static_cast<std::size_t>(start);
        replaceEnd   = static_cast<std::size_t>(end);
        if (replaceEnd > editingState.text.size() || !isUtf16Boundary(editingState.text, replaceStart) ||
            !isUtf16Boundary(editingState.text, replaceEnd))
            return IME_ERR_PARAMCHECK;

        // Once previewing has started, explicit ranges are absolute UTF-16
        // editor ranges and must stay inside the active preview range.
        if (editingState.hasPreview && (replaceStart < editingState.previewStart || replaceEnd > editingState.previewEnd))
            return IME_ERR_PARAMCHECK;
    }

    const bool        hadPreview      = editingState.hasPreview;
    const std::size_t oldPreviewStart = editingState.previewStart;
    const std::size_t oldPreviewEnd   = editingState.previewEnd;
    const std::size_t removedLength   = replaceEnd - replaceStart;

    const auto addPendingDeletion = [&](bool backward, std::size_t count)
    {
        const auto pendingTotal = editingState.pendingBackwardDeletions + editingState.pendingForwardDeletions;
        count                   = std::min(count, maxTextRequestLength - pendingTotal);
        if (backward)
            editingState.pendingBackwardDeletions += count;
        else
            editingState.pendingForwardDeletions += count;
    };

    if (hasSelection)
    {
        addPendingDeletion(true, countCommittedCodepoints(editingState, replaceStart, replaceEnd));
    }
    else
    {
        const auto cursor     = editingState.selectionEnd;
        const auto leftEnd    = std::min(replaceEnd, cursor);
        const auto rightStart = std::max(replaceStart, cursor);
        if (replaceStart < leftEnd)
            addPendingDeletion(true, countCommittedCodepoints(editingState, replaceStart, leftEnd));
        if (rightStart < replaceEnd)
            addPendingDeletion(false, countCommittedCodepoints(editingState, rightStart, replaceEnd));
    }
    editingState.text.replace(replaceStart, removedLength, preview);

    if (hadPreview)
    {
        editingState.previewStart = oldPreviewStart < replaceStart ? oldPreviewStart : replaceStart;
        if (oldPreviewEnd > replaceEnd)
        {
            editingState.previewEnd = preview.size() >= removedLength ? oldPreviewEnd + (preview.size() - removedLength)
                                                                      : oldPreviewEnd - (removedLength - preview.size());
        }
        else
        {
            editingState.previewEnd = replaceStart + preview.size();
        }
        editingState.previewEnd = std::max(editingState.previewEnd, replaceStart + preview.size());
    }
    else
    {
        editingState.previewStart = replaceStart;
        editingState.previewEnd   = replaceStart + preview.size();
    }

    editingState.hasPreview     = true;
    editingState.selectionStart = editingState.previewEnd;
    editingState.selectionEnd   = editingState.previewEnd;

    // Preview text is deliberately kept out of SFML's TextEntered stream.
    // It becomes committed only through insertText() or finishTextPreview().
    return IME_ERR_OK;
}


void finishTextPreview(InputMethod_TextEditorProxy* editor)
{
    std::u16string committed;
    std::size_t    backwardDeletions{};
    std::size_t    forwardDeletions{};
    {
        const std::lock_guard lock(editingStateMutex);
        if (editingState.editor != editor || !editingState.hasPreview)
            return;

        committed = editingState.text.substr(editingState.previewStart, editingState.previewEnd - editingState.previewStart);
        backwardDeletions                     = editingState.pendingBackwardDeletions;
        forwardDeletions                      = editingState.pendingForwardDeletions;
        editingState.selectionStart           = editingState.previewEnd;
        editingState.selectionEnd             = editingState.previewEnd;
        editingState.hasPreview               = false;
        editingState.previewStart             = 0;
        editingState.previewEnd               = 0;
        editingState.pendingBackwardDeletions = 0;
        editingState.pendingForwardDeletions  = 0;
    }
    pushDeleteEvents(true, backwardDeletions);
    pushDeleteEvents(false, forwardDeletions);
    submitCommittedText(committed);
}


bool installCallbacks(InputMethod_TextEditorProxy* proxy)
{
    return OH_TextEditorProxy_SetGetTextConfigFunc(proxy, getTextConfig) == IME_ERR_OK &&
           OH_TextEditorProxy_SetInsertTextFunc(proxy, insertText) == IME_ERR_OK &&
           OH_TextEditorProxy_SetDeleteForwardFunc(proxy, deleteForward) == IME_ERR_OK &&
           OH_TextEditorProxy_SetDeleteBackwardFunc(proxy, deleteBackward) == IME_ERR_OK &&
           OH_TextEditorProxy_SetSendKeyboardStatusFunc(proxy, sendKeyboardStatus) == IME_ERR_OK &&
           OH_TextEditorProxy_SetSendEnterKeyFunc(proxy, sendEnterKey) == IME_ERR_OK &&
           OH_TextEditorProxy_SetMoveCursorFunc(proxy, moveCursor) == IME_ERR_OK &&
           OH_TextEditorProxy_SetHandleSetSelectionFunc(proxy, setSelection) == IME_ERR_OK &&
           OH_TextEditorProxy_SetHandleExtendActionFunc(proxy, extendAction) == IME_ERR_OK &&
           OH_TextEditorProxy_SetGetLeftTextOfCursorFunc(proxy, getLeftText) == IME_ERR_OK &&
           OH_TextEditorProxy_SetGetRightTextOfCursorFunc(proxy, getRightText) == IME_ERR_OK &&
           OH_TextEditorProxy_SetGetTextIndexAtCursorFunc(proxy, getCursorIndex) == IME_ERR_OK &&
           OH_TextEditorProxy_SetReceivePrivateCommandFunc(proxy, receivePrivateCommand) == IME_ERR_OK &&
           OH_TextEditorProxy_SetSetPreviewTextFunc(proxy, setPreviewText) == IME_ERR_OK &&
           OH_TextEditorProxy_SetFinishTextPreviewFunc(proxy, finishTextPreview) == IME_ERR_OK;
}


void destroyInputMethod(InputMethod_TextEditorProxy* editor, InputMethod_InputMethodProxy* inputMethod)
{
    deactivateEditor(editor);
    if (inputMethod)
        OH_InputMethodController_Detach(inputMethod);
    if (editor)
        OH_TextEditorProxy_Destroy(editor);
}


void abortInputMethodInitialization(sf::priv::Harmony::CancelableInitialization::Token token)
{
    const std::lock_guard lock(inputMethodMutex);
    inputMethodInitialization.abort(token);
}
} // namespace


namespace sf::priv::Harmony
{
bool initializeInputMethod(std::int32_t windowId)
{
    InputMethod_TextEditorProxy*    oldEditor{};
    InputMethod_InputMethodProxy*   oldInputMethod{};
    CancelableInitialization::Token initializationToken{};
    {
        const std::lock_guard lock(inputMethodMutex);
        initializationToken = inputMethodInitialization.begin();
        if (!initializationToken || !inputMethodInitialization.authorize(initializationToken))
            return false;

        inputWindowId  = windowId;
        oldEditor      = std::exchange(editorProxy, nullptr);
        oldInputMethod = std::exchange(inputMethodProxy, nullptr);
    }
    destroyInputMethod(oldEditor, oldInputMethod);

    InputMethod_TextEditorProxy* nextEditor = OH_TextEditorProxy_Create();
    if (!nextEditor || !installCallbacks(nextEditor))
    {
        if (nextEditor)
            OH_TextEditorProxy_Destroy(nextEditor);
        abortInputMethodInitialization(initializationToken);
        err() << "Failed to create the Harmony input method text editor proxy" << std::endl;
        return false;
    }

    // Attach may synchronously request the editor configuration, so publish
    // the callback state before entering the platform API.
    activateEditor(nextEditor);

    InputMethod_AttachOptions*    options         = OH_AttachOptions_Create(false);
    InputMethod_InputMethodProxy* nextInputMethod = nullptr;
    const auto result = options ? OH_InputMethodController_Attach(nextEditor, options, &nextInputMethod)
                                : IME_ERR_NULL_POINTER;
    if (options)
        OH_AttachOptions_Destroy(options);

    if (result != IME_ERR_OK || !nextInputMethod)
    {
        destroyInputMethod(nextEditor, nextInputMethod);
        abortInputMethodInitialization(initializationToken);
        err() << "Failed to attach the Harmony input method (error " << static_cast<int>(result) << ')' << std::endl;
        return false;
    }

    bool accepted = false;
    {
        const std::lock_guard lock(inputMethodMutex);
        if (inputMethodInitialization.commit(initializationToken))
        {
            editorProxy      = nextEditor;
            inputMethodProxy = nextInputMethod;
            accepted         = true;
        }
    }

    if (!accepted)
        destroyInputMethod(nextEditor, nextInputMethod);
    return accepted;
}


void shutdownInputMethod()
{
    InputMethod_TextEditorProxy*  oldEditor{};
    InputMethod_InputMethodProxy* oldInputMethod{};
    {
        const std::lock_guard lock(inputMethodMutex);
        inputMethodInitialization.shutdown();
        inputWindowId  = 0;
        oldEditor      = std::exchange(editorProxy, nullptr);
        oldInputMethod = std::exchange(inputMethodProxy, nullptr);
    }

    destroyInputMethod(oldEditor, oldInputMethod);
}


InputMethodVisibilityResult setInputMethodVisible(bool visible)
{
    const std::lock_guard lock(inputMethodMutex);
    if (!inputMethodProxy)
        return InputMethodVisibilityResult::Unavailable;

    const auto requested = visible ? KeyboardVisibility::Shown : KeyboardVisibility::Hidden;
    const auto previous  = keyboardVisibility.load();
    if (previous == requested)
        return InputMethodVisibilityResult::Applied;

    const auto result = visible ? OH_InputMethodProxy_ShowKeyboard(inputMethodProxy)
                                : OH_InputMethodProxy_HideKeyboard(inputMethodProxy);
    if (result == IME_ERR_OK)
    {
        auto unchanged = previous;
        (void)keyboardVisibility.compare_exchange_strong(unchanged, requested);
        return InputMethodVisibilityResult::Applied;
    }
    return InputMethodVisibilityResult::Rejected;
}


bool submitFallbackTextEdit(std::size_t backwardDeletions, std::size_t forwardDeletions, std::u32string_view insertedText)
{
    // Linearize the compatibility edit against native IME attachment. If a
    // native proxy exists, its editor callbacks are the only text source.
    // Otherwise the entire delete/insert transaction is queued while attach
    // is excluded, preventing duplicate or interleaved TextEntered events.
    const std::lock_guard lock(inputMethodMutex);
    if (inputMethodProxy || inputMethodInitialization.isActive())
        return false;

    submitTextEdit(backwardDeletions, forwardDeletions, insertedText);
    return true;
}

} // namespace sf::priv::Harmony
