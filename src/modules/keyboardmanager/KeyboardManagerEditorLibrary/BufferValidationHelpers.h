#pragma once

#include <keyboardmanager/common/Shortcut.h>

#include "ShortcutErrorType.h"

namespace BufferValidationHelpers
{
    enum class DropDownAction
    {
        NoAction,
        AddDropDown,
        DeleteDropDown,
        ClearUnusedDropDowns
    };

    // Update the remap buffer based on the selected drop down index.
    static void UpdateRemapBuffer(RemapBuffer& remapBuffer, int rowIndex, int columnIndex, int dropDownIndex, DWORD keyCode)
    {
        remapBuffer.at(rowIndex).UpdateByDropDownSelection(columnIndex, dropDownIndex, keyCode);
    }

    static constexpr std::vector<ShortcutErrorType> ValidateRemapBuffer(const RemapBuffer& buffer);
}
