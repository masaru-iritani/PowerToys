#include "pch.h"
#include "BufferValidationHelpers.h"

#include <common/interop/shared_constants.h>
#include <keyboardmanager/common/Helpers.h>

#include "KeyboardManagerEditorStrings.h"
#include "KeyDropDownControl.h"
#include "EditorHelpers.h"
#include "EditorConstants.h"

static bool IsAppNameEqual(const std::wstring& appName1, const std::wstring& appName2)
{
    return _wcsicmp(appName1.c_str(), appName2.c_str()) == 0;
}

static const std::wstring NormalizeAppName(const std::wstring& appName)
{
    const std::wstring defaultAppName = KeyboardManagerEditorStrings::DefaultAppName();
    return IsAppNameEqual(appName, defaultAppName) ? L"" : appName;
}

// Helper function to verify if a key is being remapped to/from its combined key
constexpr static bool IsKeyRemappingToItsCombinedKey(DWORD keyCode1, DWORD keyCode2)
{
    return (keyCode1 == Helpers::GetCombinedKey(keyCode1) || keyCode2 == Helpers::GetCombinedKey(keyCode2)) &&
           Helpers::GetCombinedKey(keyCode1) == Helpers::GetCombinedKey(keyCode2);
}

// Validates a remap buffer column to warn when the key is an illegal shortcut (e.g., Windows + L, Ctrl + Alt + Delete).
constexpr static ShortcutErrorType ValidateKeyShortcutTextUnion(const KeyShortcutTextUnion& keyShortcutTextUnion)
{
    if (const Shortcut* shortcut = std::get_if<Shortcut>(&keyShortcutTextUnion))
    {
        return EditorHelpers::IsShortcutIllegal(*shortcut);
    }

    return ShortcutErrorType::NoError;
}

// Validates a remap buffer row to warn when a key code or a shortcut is mapped to itself.
constexpr static ShortcutErrorType ValidateRemapBufferRow(const RemapBufferRow& row)
{
    for (const KeyShortcutTextUnion& keyShortcutTextUnion : row.mapping)
    {
        const ShortcutErrorType errorType = ValidateKeyShortcutTextUnion(keyShortcutTextUnion);
        if (errorType != ShortcutErrorType::NoError)
        {
            return errorType;
        }
    }

    const auto& [original, remapped] = row.mapping;
    if (const DWORD* originalKeyCode = std::get_if<DWORD>(&original))
    {
        if (const DWORD* remappedKeyCode = std::get_if<DWORD>(&remapped))
        {
            if (*originalKeyCode == *remappedKeyCode)
            {
                return ShortcutErrorType::MapToSameKey;
            }
        }
    }
    else if (const Shortcut* originalShortcut = std::get_if<Shortcut>(&original))
    {
        if (const Shortcut* remappedShortcut = std::get_if<Shortcut>(&remapped))
        {
            if (*originalShortcut == *remappedShortcut)
            {
                return ShortcutErrorType::MapToSameShortcut;
            }
        }
    }

    return ShortcutErrorType::NoError;
}

// Validates two remap buffer rows to warn if both map the same or overlapped key.
constexpr static ShortcutErrorType ValidateRemapBufferRows(const RemapBufferRow& row1, const RemapBufferRow& row2)
{
    const auto& original1 = row1.mapping.at(0);
    const auto& original2 = row2.mapping.at(0);
    if (const DWORD* originalKeyCode1 = std::get_if<DWORD>(&original1))
    {
        if (const DWORD* originalKeyCode2 = std::get_if<DWORD>(&original2))
        {
            return EditorHelpers::DoKeysOverlap(*originalKeyCode1, *originalKeyCode2);
        }
    }
    else if (const Shortcut* originalShortcut1 = std::get_if<Shortcut>(&original1))
    {
        if (const Shortcut* originalShortcut2 = std::get_if<Shortcut>(&original2))
        {
            return EditorHelpers::DoShortcutsOverlap(*originalShortcut1, *originalShortcut2);
        }
    }

    return ShortcutErrorType::NoError;
}

namespace BufferValidationHelpers
{
    static constexpr std::vector<ShortcutErrorType> ValidateRemapBuffer(const RemapBuffer& buffer)
    {
        const size_t rowCount = buffer.size();
        auto errorTypes = std::vector<ShortcutErrorType>(rowCount);
        for (int i = 0; i < rowCount; ++i)
        {
            const RemapBufferRow& remapBufferRow1 = buffer.at(i);
            const std::wstring appName1 = NormalizeAppName(remapBufferRow1.appName);
            if (const ShortcutErrorType errorType = ValidateRemapBufferRow(remapBufferRow1); errorType != ShortcutErrorType::NoError)
            {
                errorTypes.at(i) = errorType;
            }

            for (int j = i + 1; i < rowCount; ++j)
            {
                const RemapBufferRow& remapBufferRow2 = buffer.at(j);
                const std::wstring appName2 = NormalizeAppName(remapBufferRow2.appName);
                if (!IsAppNameEqual(appName1, appName2))
                {
                    continue;
                }

                if (ShortcutErrorType errorType = ValidateRemapBufferRows(remapBufferRow1, remapBufferRow2); errorType != ShortcutErrorType::NoError)
                {
                    errorTypes.at(i) = errorType;
                    errorTypes.at(j) = errorType;
                }
            }
        }

        return errorTypes;
    }

    static void UpdateShortcutBufferElement(int rowIndex, int columnIndex, const std::vector<int32_t>& selectedCodes, bool isHybridControl)
    {
        KeyShortcutTextUnion tempShortcut;
        if (isHybridControl && KeyDropDownControl::GetNumberOfSelectedKeys(selectedCodes) == 1)
        {
            tempShortcut = (DWORD)*std::find_if(selectedCodes.begin(), selectedCodes.end(), [](int32_t a) { return a != -1 && a != 0; });
        }
        else
        {
            tempShortcut = Shortcut();
            std::get<Shortcut>(tempShortcut).SetKeyCodes(selectedCodes);
        }
    }

    static ShortcutErrorType ValidateSelectedKeyCodes(const std::vector<int32_t>& selectedCodes, uint32_t dropDownIndex, bool isHybridControl)
    {
        const size_t dropDownCount = selectedCodes.size();
        const int32_t selectedKeyCode = selectedCodes.at(dropDownIndex);
        if (selectedKeyCode != -1)
        {
            // If only 1 drop down and action key is chosen: Warn that a modifier must be chosen (if the drop down is not for a hybrid scenario)
            if (dropDownCount == 1 && !Helpers::IsModifierKey(selectedKeyCode) && !isHybridControl)
            {
                // warn and reset the drop down
                return ShortcutErrorType::ShortcutStartWithModifier;
            }
            else if (dropDownIndex == dropDownCount - 1)
            {
                // If it is the last drop down
                // If last drop down and a modifier is selected: add a new drop down (max drop down count should be enforced)
                if (Helpers::IsModifierKey(selectedKeyCode) && dropDownCount < EditorConstants::MaxShortcutSize)
                {
                    // If it matched any of the previous modifiers then reset that drop down
                    if (EditorHelpers::CheckRepeatedModifier(selectedCodes, selectedKeyCode))
                    {
                        // warn and reset the drop down
                        return ShortcutErrorType::ShortcutCannotHaveRepeatedModifier;
                    }
                }
                else if (Helpers::IsModifierKey(selectedKeyCode) && dropDownCount >= EditorConstants::MaxShortcutSize)
                {
                    // If last drop down and a modifier is selected but there are already max drop downs: warn the user
                    // warn and reset the drop down
                    return ShortcutErrorType::ShortcutMaxShortcutSizeOneActionKey;
                }
                else if (selectedKeyCode == 0)
                {
                    // If None is selected but it's the last index: warn
                    // If it is a hybrid control and there are 2 drop downs then deletion is allowed
                    if (isHybridControl && dropDownCount == EditorConstants::MinShortcutSize)
                    {
                        // set delete drop down flag
                        // dropDownAction = BufferValidationHelpers::DropDownAction::DeleteDropDown;
                        // do not delete the drop down now since there may be some other error which would cause the drop down to be invalid after removal
                    }
                    else
                    {
                        // warn and reset the drop down
                        return ShortcutErrorType::ShortcutOneActionKey;
                    }
                }
                else if (selectedKeyCode == CommonSharedConstants::VK_DISABLED && dropDownIndex)
                {
                    // Disable can not be selected if one modifier key has already been selected
                    return ShortcutErrorType::ShortcutDisableAsActionKey;
                }
                // If none of the above, then the action key will be set
            }
            else
            {
                // If it is not the last drop down
                if (Helpers::IsModifierKey(selectedKeyCode))
                {
                    // If it matched any of the previous modifiers then reset that drop down
                    if (EditorHelpers::CheckRepeatedModifier(selectedCodes, selectedKeyCode))
                    {
                        // warn and reset the drop down
                        return ShortcutErrorType::ShortcutCannotHaveRepeatedModifier;
                    }
                    // If not, the modifier key will be set
                }
                else if (selectedKeyCode == 0 && dropDownCount > EditorConstants::MinShortcutSize)
                {
                    // If None is selected and there are more than 2 drop downs
                    // set delete drop down flag
                    // dropDownAction = BufferValidationHelpers::DropDownAction::DeleteDropDown;
                    // do not delete the drop down now since there may be some other error which would cause the drop down to be invalid after removal
                }
                else if (selectedKeyCode == 0 && dropDownCount <= EditorConstants::MinShortcutSize)
                {
                    // If it is a hybrid control and there are 2 drop downs then deletion is allowed
                    if (isHybridControl && dropDownCount == EditorConstants::MinShortcutSize)
                    {
                        // set delete drop down flag
                        // dropDownAction = BufferValidationHelpers::DropDownAction::DeleteDropDown;
                        // do not delete the drop down now since there may be some other error which would cause the drop down to be invalid after removal
                    }
                    else
                    {
                        // warn and reset the drop down
                        return ShortcutErrorType::ShortcutAtleast2Keys;
                    }
                }
                else if (selectedKeyCode == CommonSharedConstants::VK_DISABLED && dropDownIndex)
                {
                    // Allow selection of VK_DISABLE only in first dropdown
                    return ShortcutErrorType::ShortcutDisableAsActionKey;
                }
                else if (dropDownIndex != 0 || isHybridControl)
                {
                    // If the user tries to set an action key check if all drop down menus after this are empty if it is not the first key.
                    // If it is a hybrid control, this can be done even on the first key
                    bool isClear = true;
                    for (size_t i = static_cast<size_t>(dropDownIndex) + 1; i < static_cast<int>(dropDownCount); i++)
                    {
                        if (selectedCodes[i] != -1)
                        {
                            isClear = false;
                            break;
                        }
                    }

                    if (isClear)
                    {
                        // dropDownAction = BufferValidationHelpers::DropDownAction::ClearUnusedDropDowns;
                    }
                    else
                    {
                        // this used to "warn and reset the drop down" but for now, since we will allow Chords, we do allow this
                        // leaving the here and commented out for posterity, for now.
                        // return ShortcutErrorType::ShortcutNotMoreThanOneActionKey;
                    }
                }
                else
                {
                    // If there an action key is chosen on the first drop down and there are more than one drop down menus
                    // warn and reset the drop down
                    return ShortcutErrorType::ShortcutStartWithModifier;
                }
            }
        }

        return ShortcutErrorType::NoError;
    }
}
