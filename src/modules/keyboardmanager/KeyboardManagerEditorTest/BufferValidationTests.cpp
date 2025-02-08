#include "pch.h"

// Suppressing 26466 - Don't use static_cast downcasts - in CppUnitTest.h
#pragma warning(push)
#pragma warning(disable : 26466)
#include "CppUnitTest.h"
#pragma warning(pop)

#include <keyboardmanager/KeyboardManagerEditorLibrary/BufferValidationHelpers.h>
#include <common/interop/keyboard_layout.h>
#include <common/interop/shared_constants.h>
#include <functional>
#include <keyboardmanager/KeyboardManagerEditorLibrary/ShortcutErrorType.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace RemappingUITests
{
    // Tests for methods in the BufferValidationHelpers namespace
    TEST_CLASS (BufferValidationTests)
    {
        std::wstring testApp1 = L"testprocess1.exe";
        std::wstring testApp2 = L"testprocess2.exe";
        LayoutMap keyboardLayout;

        struct ValidateAndUpdateKeyBufferElementArgs
        {
            int elementRowIndex;
            int elementColIndex;
            int selectedCodeFromDropDown;
        };

        struct ValidateShortcutBufferElementArgs
        {
            int elementRowIndex;
            int elementColIndex;
            uint32_t indexOfDropDownLastModified;
            std::vector<int32_t> selectedCodesOnDropDowns;
            std::wstring targetAppNameInTextBox;
            bool isHybridColumn;
            RemapBufferRow bufferRow;
        };

        void RunTestCases(const std::vector<ValidateShortcutBufferElementArgs>& testCases, std::function<void(const ValidateShortcutBufferElementArgs&)> testMethod)
        {
            for (int i = 0; i < testCases.size(); i++)
            {
                testMethod(testCases[i]);
            }
        }

    public:
        TEST_METHOD_INITIALIZE(InitializeTestEnv)
            {
            }

            // Test if the ValidateAndUpdateKeyBufferElement method is successful when setting a key to null in a new row
            TEST_METHOD (ValidateAndUpdateKeyBufferElement_ShouldUpdateAndReturnNoError_OnSettingKeyToNullInANewRow)
            {
                RemapBuffer remapBuffer;

                // Add 2 empty rows
                remapBuffer.push_back({ { VK_NULL, VK_NULL }, L"" });
                remapBuffer.push_back({ { VK_NULL, VK_NULL }, L"" });

                // Validate and update the element when VK_INVALID i.e. null selection is made on an empty row.
                ValidateAndUpdateKeyBufferElementArgs args = { 0, 0, VK_INVALID };
                ShortcutErrorType error = BufferValidationHelpers::ValidateAndUpdateKeyBufferElement(args.elementRowIndex, args.elementColIndex, args.selectedCodeFromDropDown, remapBuffer);

                // Assert that the element is validated and buffer is updated
                Assert::AreEqual(ShortcutErrorType::NoError, error);
                Assert::AreEqual(VK_NULL, std::get<DWORD>(remapBuffer[0].mapping[0]));
                Assert::AreEqual(VK_NULL, std::get<DWORD>(remapBuffer[0].mapping[1]));
                Assert::AreEqual(VK_NULL, std::get<DWORD>(remapBuffer[1].mapping[0]));
                Assert::AreEqual(VK_NULL, std::get<DWORD>(remapBuffer[1].mapping[1]));
            }

            // Test if the ValidateAndUpdateKeyBufferElement method is successful when setting a key to non-null in a new row
            TEST_METHOD (ValidateAndUpdateKeyBufferElement_ShouldUpdateAndReturnNoError_OnSettingKeyToNonNullInANewRow)
            {
                RemapBuffer remapBuffer;

                // Add an empty row
                remapBuffer.push_back({ { VK_NULL, VK_NULL }, L"" });

                // Validate and update the element when selecting B on an empty row
                ValidateAndUpdateKeyBufferElementArgs args = { 0, 0, 'B' };
                ShortcutErrorType error = BufferValidationHelpers::ValidateAndUpdateKeyBufferElement(args.elementRowIndex, args.elementColIndex, args.selectedCodeFromDropDown, remapBuffer);

                // Assert that the element is validated and buffer is updated
                Assert::AreEqual(ShortcutErrorType::NoError, error);
                Assert::AreEqual(static_cast<DWORD>('B'), std::get<DWORD>(remapBuffer[0].mapping[0]));
                Assert::AreEqual(VK_NULL, std::get<DWORD>(remapBuffer[0].mapping[1]));
            }

            // Test if the ValidateAndUpdateKeyBufferElement method is successful when setting a key to non-null in a valid key to key
            TEST_METHOD (ValidateAndUpdateKeyBufferElement_ShouldUpdateAndReturnNoError_OnSettingKeyToNonNullInAValidKeyToKeyRow)
            {
                RemapBuffer remapBuffer;

                // Add a row with A as the target
                remapBuffer.push_back({ { VK_NULL, 'A' }, L"" });

                // Validate and update the element when selecting B on a row
                ValidateAndUpdateKeyBufferElementArgs args = { 0, 0, 'B' };
                ShortcutErrorType error = BufferValidationHelpers::ValidateAndUpdateKeyBufferElement(args.elementRowIndex, args.elementColIndex, args.selectedCodeFromDropDown, remapBuffer);

                // Assert that the element is validated and buffer is updated
                Assert::AreEqual(ShortcutErrorType::NoError, error);
                Assert::AreEqual(static_cast<DWORD>('B'), std::get<DWORD>(remapBuffer[0].mapping[0]));
                Assert::AreEqual(static_cast<DWORD>('A'), std::get<DWORD>(remapBuffer[0].mapping[1]));
            }

            // Test if the ValidateAndUpdateKeyBufferElement method is successful when setting a key to non-null in a valid key to shortcut
            TEST_METHOD (ValidateAndUpdateKeyBufferElement_ShouldUpdateAndReturnNoError_OnSettingKeyToNonNullInAValidKeyToShortcutRow)
            {
                RemapBuffer remapBuffer;

                // Add a row with Ctrl+A as the target
                remapBuffer.push_back({ { VK_NULL, Shortcut{ VK_CONTROL, 'A' } }, L"" });

                // Validate and update the element when selecting B on a row
                ValidateAndUpdateKeyBufferElementArgs args = { 0, 0, 'B' };
                ShortcutErrorType error = BufferValidationHelpers::ValidateAndUpdateKeyBufferElement(args.elementRowIndex, args.elementColIndex, args.selectedCodeFromDropDown, remapBuffer);

                // Assert that the element is validated and buffer is updated
                Assert::AreEqual(ShortcutErrorType::NoError, error);
                Assert::AreEqual(static_cast<DWORD>('B'), std::get<DWORD>(remapBuffer[0].mapping[0]));
                Assert::AreEqual(std::get<Shortcut>(remapBuffer[0].mapping[1]), Shortcut{ VK_CONTROL, 'A' });
            }

            // Test if the ValidateAndUpdateKeyBufferElement method is unsuccessful when setting first column to the same value as the right column
            TEST_METHOD (ValidateAndUpdateKeyBufferElement_ShouldReturnMapToSameKeyError_OnSettingFirstColumnToSameValueAsRightColumn)
            {
                RemapBuffer remapBuffer;

                // Add a row with A as the target
                remapBuffer.push_back({ { VK_NULL, 'A' }, L"" });

                // Validate and update the element when selecting A on a row
                ValidateAndUpdateKeyBufferElementArgs args = { 0, 0, 'A' };
                ShortcutErrorType error = BufferValidationHelpers::ValidateAndUpdateKeyBufferElement(args.elementRowIndex, args.elementColIndex, args.selectedCodeFromDropDown, remapBuffer);

                // Assert that the element is invalid and buffer is not updated
                Assert::AreEqual(ShortcutErrorType::MapToSameKey, error);
                Assert::AreEqual(VK_NULL, std::get<DWORD>(remapBuffer[0].mapping[0]));
                Assert::AreEqual(static_cast<DWORD>('A'), std::get<DWORD>(remapBuffer[0].mapping[1]));
            }

            // Test if the ValidateAndUpdateKeyBufferElement method is unsuccessful when setting first column of a key to key row to the same value as in another row
            TEST_METHOD (ValidateAndUpdateKeyBufferElement_ShouldReturnSameKeyPreviouslyMappedError_OnSettingFirstColumnOfAKeyToKeyRowToSameValueAsInAnotherRow)
            {
                RemapBuffer remapBuffer;

                // Add a row from A->B and a row with C as target
                remapBuffer.push_back({ { 'A', 'B' }, L"" });
                remapBuffer.push_back({ { VK_NULL, 'C' }, L"" });

                // Validate and update the element when selecting A on second row
                ValidateAndUpdateKeyBufferElementArgs args = { 1, 0, 'A' };
                ShortcutErrorType error = BufferValidationHelpers::ValidateAndUpdateKeyBufferElement(args.elementRowIndex, args.elementColIndex, args.selectedCodeFromDropDown, remapBuffer);

                // Assert that the element is invalid and buffer is not updated
                Assert::AreEqual(ShortcutErrorType::SameKeyPreviouslyMapped, error);
                Assert::AreEqual(VK_NULL, std::get<DWORD>(remapBuffer[1].mapping[0]));
                Assert::AreEqual(static_cast<DWORD>('C'), std::get<DWORD>(remapBuffer[1].mapping[1]));
            }

            // Test if the ValidateAndUpdateKeyBufferElement method is unsuccessful when setting first column of a key to shortcut row to the same value as in another row
            TEST_METHOD (ValidateAndUpdateKeyBufferElement_ShouldReturnSameKeyPreviouslyMappedError_OnSettingFirstColumnOfAKeyToShortcutRowToSameValueAsInAnotherRow)
            {
                RemapBuffer remapBuffer;

                // Add a row from A->B and a row with Ctrl+A as target
                remapBuffer.push_back({ { 'A', 'B' }, L"" });
                remapBuffer.push_back({ { VK_NULL, Shortcut{ VK_CONTROL, 'A' } }, L"" });

                // Validate and update the element when selecting A on second row
                ValidateAndUpdateKeyBufferElementArgs args = { 1, 0, 'A' };
                ShortcutErrorType error = BufferValidationHelpers::ValidateAndUpdateKeyBufferElement(args.elementRowIndex, args.elementColIndex, args.selectedCodeFromDropDown, remapBuffer);

                // Assert that the element is invalid and buffer is not updated
                Assert::AreEqual(ShortcutErrorType::SameKeyPreviouslyMapped, error);
                Assert::AreEqual(VK_NULL, std::get<DWORD>(remapBuffer[1].mapping[0]));
                Assert::AreEqual(std::get<Shortcut>(remapBuffer[1].mapping[1]), Shortcut{ VK_CONTROL, 'A' });
            }

            // Test if the ValidateAndUpdateKeyBufferElement method is unsuccessful when setting first column of a key to key row to a conflicting modifier with another row
            TEST_METHOD (ValidateAndUpdateKeyBufferElement_ShouldReturnConflictingModifierKeyError_OnSettingFirstColumnOfAKeyToKeyRowToConflictingModifierWithAnotherRow)
            {
                RemapBuffer remapBuffer;

                // Add a row from Ctrl->B and a row with C as target
                remapBuffer.push_back({ { VK_CONTROL, 'B' }, L"" });
                remapBuffer.push_back({ { VK_NULL, 'C' }, L"" });

                // Validate and update the element when selecting LCtrl on second row
                ValidateAndUpdateKeyBufferElementArgs args = { 1, 0, VK_LCONTROL };
                ShortcutErrorType error = BufferValidationHelpers::ValidateAndUpdateKeyBufferElement(args.elementRowIndex, args.elementColIndex, args.selectedCodeFromDropDown, remapBuffer);

                // Assert that the element is invalid and buffer is not updated
                Assert::AreEqual(ShortcutErrorType::ConflictingModifierKey, error);
                Assert::AreEqual(VK_NULL, std::get<DWORD>(remapBuffer[1].mapping[0]));
                Assert::AreEqual(static_cast<DWORD>('C'), std::get<DWORD>(remapBuffer[1].mapping[1]));
            }

            // Test if the ValidateAndUpdateKeyBufferElement method is unsuccessful when setting first column of a key to shortcut row to a conflicting modifier with another row
            TEST_METHOD (ValidateAndUpdateKeyBufferElement_ShouldReturnConflictingModifierKeyError_OnSettingFirstColumnOfAKeyToShortcutRowToConflictingModifierWithAnotherRow)
            {
                RemapBuffer remapBuffer;

                // Add a row from Ctrl->B and a row with Ctrl+A as target
                remapBuffer.push_back({ { VK_CONTROL, 'B' }, L"" });
                remapBuffer.push_back({ { VK_NULL, Shortcut{ VK_CONTROL, 'A' } }, L"" });

                // Validate and update the element when selecting LCtrl on second row
                ValidateAndUpdateKeyBufferElementArgs args = { 1, 0, VK_LCONTROL };
                ShortcutErrorType error = BufferValidationHelpers::ValidateAndUpdateKeyBufferElement(args.elementRowIndex, args.elementColIndex, args.selectedCodeFromDropDown, remapBuffer);

                // Assert that the element is invalid and buffer is not updated
                Assert::AreEqual(ShortcutErrorType::ConflictingModifierKey, error);
                Assert::AreEqual(VK_NULL, std::get<DWORD>(remapBuffer[1].mapping[0]));
                Assert::AreEqual(std::get<Shortcut>(remapBuffer[1].mapping[1]), Shortcut{ VK_CONTROL, 'A' });
            }

            // Test if the ValidateShortcutBufferElement method is successful and no drop down action is required on setting a column to null in a new or valid row
            TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnNoErrorAndNoAction_OnSettingColumnToNullInANewOrValidRow)
            {
                std::vector<ValidateShortcutBufferElementArgs> testCases;
                // Case 1: Validate the element when making null-selection (VK_INVALID index) on first column of empty shortcut to shortcut row
                testCases.push_back({ 0, 0, 0, { VK_INVALID }, L"", false, { { Shortcut(), Shortcut() }, L"" } });
                // Case 2: Validate the element when making null-selection (VK_INVALID index) on second column of empty shortcut to shortcut row
                testCases.push_back({ 0, 1, 0, { VK_INVALID }, L"", false, { { Shortcut(), Shortcut() }, L"" } });
                // Case 3: Validate the element when making null-selection (VK_INVALID index) on first column of empty shortcut to key row
                testCases.push_back({ 0, 0, 0, { VK_INVALID }, L"", false, { { Shortcut(), VK_NULL }, L"" } });
                // Case 4: Validate the element when making null-selection (VK_INVALID index) on second column of empty shortcut to key row
                testCases.push_back({ 0, 1, 0, { VK_INVALID }, L"", true, { { Shortcut(), VK_NULL }, L"" } });
                // Case 5: Validate the element when making null-selection (VK_INVALID index) on first dropdown of first column of valid shortcut to shortcut row
                testCases.push_back({ 0, 0, 0, { VK_INVALID, 'C' }, L"", false, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut{ VK_CONTROL, 'A' } }, L"" } });
                // Case 6: Validate the element when making null-selection (VK_INVALID index) on first dropdown of second column of valid shortcut to shortcut row
                testCases.push_back({ 0, 1, 0, { VK_INVALID, 'A' }, L"", false, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut{ VK_CONTROL, 'A' } }, L"" } });
                // Case 7: Validate the element when making null-selection (VK_INVALID index) on first dropdown of second column of valid hybrid shortcut to shortcut row
                testCases.push_back({ 0, 1, 0, { VK_INVALID, 'A' }, L"", true, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut{ VK_CONTROL, 'A' } }, L"" } });
                // Case 8: Validate the element when making null-selection (VK_INVALID index) on second dropdown of first column of valid shortcut to shortcut row
                testCases.push_back({ 0, 0, 1, { VK_CONTROL, VK_INVALID }, L"", false, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut{ VK_CONTROL, 'A' } }, L"" } });
                // Case 9: Validate the element when making null-selection (VK_INVALID index) on second dropdown of second column of valid shortcut to shortcut row
                testCases.push_back({ 0, 1, 1, { VK_CONTROL, VK_INVALID }, L"", false, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut{ VK_CONTROL, 'A' } }, L"" } });
                // Case 10: Validate the element when making null-selection (VK_INVALID index) on second dropdown of second column of valid hybrid shortcut to shortcut row
                testCases.push_back({ 0, 1, 1, { VK_CONTROL, VK_INVALID }, L"", true, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut{ VK_CONTROL, 'A' } }, L"" } });
                // Case 11: Validate the element when making null-selection (VK_INVALID index) on first dropdown of first column of valid 3 key shortcut to shortcut row
                testCases.push_back({ 0, 0, 0, { VK_INVALID, VK_SHIFT, 'D' }, L"", false, { { Shortcut{ VK_CONTROL, VK_SHIFT, 'D' }, Shortcut{ VK_CONTROL, VK_SHIFT, 'B' } }, L"" } });
                // Case 12: Validate the element when making null-selection (VK_INVALID index) on first dropdown of second column of valid 3 key shortcut to shortcut row
                testCases.push_back({ 0, 1, 0, { VK_INVALID, VK_SHIFT, 'B' }, L"", false, { { Shortcut{ VK_CONTROL, VK_SHIFT, 'D' }, Shortcut{ VK_CONTROL, VK_SHIFT, 'B' } }, L"" } });
                // Case 13: Validate the element when making null-selection (VK_INVALID index) on first dropdown of second column of valid hybrid 3 key shortcut to shortcut row
                testCases.push_back({ 0, 1, 0, { VK_INVALID, VK_SHIFT, 'B' }, L"", true, { { Shortcut{ VK_CONTROL, VK_SHIFT, 'D' }, Shortcut{ VK_CONTROL, VK_SHIFT, 'B' } }, L"" } });
                // Case 14: Validate the element when making null-selection (VK_INVALID index) on second dropdown of first column of valid 3 key shortcut to shortcut row
                testCases.push_back({ 0, 0, 1, { VK_CONTROL, VK_INVALID, 'D' }, L"", false, { { Shortcut{ VK_CONTROL, VK_SHIFT, 'D' }, Shortcut{ VK_CONTROL, VK_SHIFT, 'B' } }, L"" } });
                // Case 15: Validate the element when making null-selection (VK_INVALID index) on second dropdown of second column of valid 3 key shortcut to shortcut row
                testCases.push_back({ 0, 1, 1, { VK_CONTROL, VK_INVALID, 'B' }, L"", false, { { Shortcut{ VK_CONTROL, VK_SHIFT, 'D' }, Shortcut{ VK_CONTROL, VK_SHIFT, 'B' } }, L"" } });
                // Case 16: Validate the element when making null-selection (VK_INVALID index) on second dropdown of second column of valid hybrid 3 key shortcut to shortcut row
                testCases.push_back({ 0, 1, 1, { VK_CONTROL, VK_INVALID, 'B' }, L"", true, { { Shortcut{ VK_CONTROL, VK_SHIFT, 'D' }, Shortcut{ VK_CONTROL, VK_SHIFT, 'B' } }, L"" } });
                // Case 17: Validate the element when making null-selection (VK_INVALID index) on third dropdown of first column of valid 3 key shortcut to shortcut row
                testCases.push_back({ 0, 0, 2, { VK_CONTROL, VK_SHIFT, VK_INVALID }, L"", false, { { Shortcut{ VK_CONTROL, VK_SHIFT, 'D' }, Shortcut{ VK_CONTROL, VK_SHIFT, 'B' } }, L"" } });
                // Case 18: Validate the element when making null-selection (VK_INVALID index) on third dropdown of second column of valid 3 key shortcut to shortcut row
                testCases.push_back({ 0, 1, 2, { VK_CONTROL, VK_SHIFT, VK_INVALID }, L"", false, { { Shortcut{ VK_CONTROL, VK_SHIFT, 'D' }, Shortcut{ VK_CONTROL, VK_SHIFT, 'B' } }, L"" } });
                // Case 19: Validate the element when making null-selection (VK_INVALID index) on third dropdown of second column of valid hybrid 3 key shortcut to shortcut row
                testCases.push_back({ 0, 1, 2, { VK_CONTROL, VK_SHIFT, VK_INVALID }, L"", true, { { Shortcut{ VK_CONTROL, VK_SHIFT, 'D' }, Shortcut{ VK_CONTROL, VK_SHIFT, 'B' } }, L"" } });
                // Case 20: Validate the element when making null-selection (VK_INVALID index) on fourth dropdown of first column of valid 4 key shortcut to shortcut row
                testCases.push_back({ 0, 0, 3, { VK_CONTROL, VK_SHIFT, VK_MENU, VK_INVALID }, L"", false, { { Shortcut{ VK_CONTROL, VK_SHIFT, VK_MENU, 'D' }, Shortcut{ VK_CONTROL, VK_SHIFT, VK_MENU, 'B' } }, L"" } });
                // Case 21: Validate the element when making null-selection (VK_INVALID index) on fourth dropdown of second column of valid 4 key shortcut to shortcut row
                testCases.push_back({ 0, 1, 3, { VK_CONTROL, VK_SHIFT, VK_MENU, VK_INVALID }, L"", false, { { Shortcut{ VK_CONTROL, VK_SHIFT, VK_MENU, 'D' }, Shortcut{ VK_CONTROL, VK_SHIFT, VK_MENU, 'B' } }, L"" } });
                // Case 22: Validate the element when making null-selection (VK_INVALID index) on fourth dropdown of second column of valid hybrid 4 key shortcut to shortcut row
                testCases.push_back({ 0, 1, 3, { VK_CONTROL, VK_SHIFT, VK_MENU, VK_INVALID }, L"", true, { { Shortcut{ VK_CONTROL, VK_SHIFT, VK_MENU, 'D' }, Shortcut{ VK_CONTROL, VK_SHIFT, VK_MENU, 'B' } }, L"" } });
                // Case 23: Validate the element when making null-selection (VK_INVALID index) on fifth dropdown of first column of valid 5 key shortcut to shortcut row
                testCases.push_back({ 0, 0, 3, { VK_CONTROL, VK_SHIFT, VK_MENU, VK_LWIN, VK_INVALID }, L"", false, { { Shortcut{ VK_CONTROL, VK_SHIFT, VK_MENU, VK_LWIN, 'D' }, Shortcut{ VK_CONTROL, VK_SHIFT, VK_MENU, VK_LWIN, 'B' } }, L"" } });
                // Case 24: Validate the element when making null-selection (VK_INVALID index) on fifth dropdown of second column of valid 5 key shortcut to shortcut row
                testCases.push_back({ 0, 1, 3, { VK_CONTROL, VK_SHIFT, VK_MENU, VK_LWIN, VK_INVALID }, L"", false, { { Shortcut{ VK_CONTROL, VK_SHIFT, VK_MENU, VK_LWIN, 'D' }, Shortcut{ VK_CONTROL, VK_SHIFT, VK_MENU, VK_LWIN, 'B' } }, L"" } });
                // Case 25: Validate the element when making null-selection (VK_INVALID index) on fifth dropdown of second column of valid hybrid 5 key shortcut to shortcut row
                testCases.push_back({ 0, 1, 3, { VK_CONTROL, VK_SHIFT, VK_MENU, VK_LWIN, VK_INVALID }, L"", true, { { Shortcut{ VK_CONTROL, VK_SHIFT, VK_MENU, VK_LWIN, 'D' }, Shortcut{ VK_CONTROL, VK_SHIFT, VK_MENU, VK_LWIN, 'B' } }, L"" } });

                RunTestCases(testCases, [this](const ValidateShortcutBufferElementArgs& testCase) {
                    // Arrange
                    RemapBuffer remapBuffer;
                    remapBuffer.push_back(testCase.bufferRow);

                    // Act
                    std::pair<ShortcutErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedCodesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, remapBuffer, true);

                    // Assert that the element is valid and no drop down action is required
                    Assert::AreEqual(ShortcutErrorType::NoError, result.first);
                    Assert::AreEqual(BufferValidationHelpers::DropDownAction::NoAction, result.second);
                });
            }

            // Test if the ValidateShortcutBufferElement method returns ShortcutStartWithModifier error and no drop down action is required on setting first drop down to an action key on a non-hybrid control column
            TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnShortcutStartWithModifierErrorAndNoAction_OnSettingFirstDropDownToActionKeyOnANonHybridColumn)
            {
                std::vector<ValidateShortcutBufferElementArgs> testCases;
                // Case 1: Validate the element when selecting A ('A') on first dropdown of first column of empty shortcut to shortcut row
                testCases.push_back({ 0, 0, 0, { 'A' }, L"", false, { { Shortcut(), Shortcut() }, L"" } });
                // Case 2: Validate the element when selecting A ('A') on first dropdown of second column of empty shortcut to shortcut row
                testCases.push_back({ 0, 1, 0, { 'A' }, L"", false, { { Shortcut(), Shortcut() }, L"" } });
                // Case 3: Validate the element when selecting A ('A') on first dropdown of first column of empty shortcut to key row
                testCases.push_back({ 0, 0, 0, { 'A' }, L"", false, { { Shortcut(), VK_NULL }, L"" } });
                // Case 4: Validate the element when selecting A ('A') on first dropdown of first column of valid shortcut to shortcut row
                testCases.push_back({ 0, 0, 0, { 'A', 'C' }, L"", false, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut{ VK_CONTROL, 'A' } }, L"" } });

                RunTestCases(testCases, [this](const ValidateShortcutBufferElementArgs& testCase) {
                    // Arrange
                    RemapBuffer remapBuffer;
                    remapBuffer.push_back(testCase.bufferRow);

                    // Act
                    std::pair<ShortcutErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedCodesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, remapBuffer, true);

                    // Assert that the element is invalid and no drop down action is required
                    Assert::AreEqual(ShortcutErrorType::ShortcutStartWithModifier, result.first);
                    Assert::AreEqual(BufferValidationHelpers::DropDownAction::NoAction, result.second);
                });
            }

            // Test if the ValidateShortcutBufferElement method returns no error and no drop down action is required on setting first drop down to an action key on an empty hybrid control column
            TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnNoErrorAndNoAction_OnSettingFirstDropDownToActionKeyOnAnEmptyHybridColumn)
            {
                std::vector<ValidateShortcutBufferElementArgs> testCases;
                // Case 1: Validate the element when selecting A ('A') on first dropdown of second column of empty shortcut to shortcut row
                testCases.push_back({ 0, 1, 0, { 'A' }, L"", true, { { Shortcut(), Shortcut() }, L"" } });
                // Case 2: Validate the element when selecting A ('A') on first dropdown of second column of empty shortcut to key row
                testCases.push_back({ 0, 1, 0, { 'A' }, L"", true, { { Shortcut(), VK_NULL }, L"" } });

                RunTestCases(testCases, [this](const ValidateShortcutBufferElementArgs& testCase) {
                    // Arrange
                    RemapBuffer remapBuffer;
                    remapBuffer.push_back(testCase.bufferRow);

                    // Act
                    std::pair<ShortcutErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedCodesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, remapBuffer, true);

                    // Assert that the element is valid and no drop down action is required
                    Assert::AreEqual(ShortcutErrorType::NoError, result.first);
                    Assert::AreEqual(BufferValidationHelpers::DropDownAction::NoAction, result.second);
                });
            }

            // Test if the ValidateShortcutBufferElement method returns ShortcutNotMoreThanOneActionKey error and no drop down action is required on setting first drop down to an action key on a hybrid control column with full shortcut
            // Changed because we can have more than one key here now that we allow key-chords
            // old name: ValidateShortcutBufferElement_ShouldReturnShortcutNotMoreThanOneActionKeyAndNoAction_OnSettingNonLastDropDownToActionKeyOnAHybridColumnWithFullShortcut
            TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnNoErrorAndNoAction_OnSettingNonLastDropDownToActionKeyOnAHybridColumnWithFullShortcut)
            {
                std::vector<ValidateShortcutBufferElementArgs> testCases;
                // Case 1: Validate the element when selecting A ('A') on first dropdown of second column of hybrid shortcut to shortcut row
                testCases.push_back({ 0, 1, 0, { 'A', 'C' }, L"", true, { { Shortcut(), Shortcut{ VK_CONTROL, 'C' } }, L"" } });
                // Case 2: Validate the element when selecting A ('A') on second dropdown of second column of hybrid shortcut to shortcut row
                testCases.push_back({ 0, 1, 1, { VK_CONTROL, 'A', 'B' }, L"", true, { { Shortcut(), Shortcut{ VK_CONTROL, VK_SHIFT, 'C' } }, L"" } });

                RunTestCases(testCases, [this](const ValidateShortcutBufferElementArgs& testCase) {
                    // Arrange
                    RemapBuffer remapBuffer;
                    remapBuffer.push_back(testCase.bufferRow);

                    // Act
                    std::pair<ShortcutErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedCodesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, remapBuffer, true);

                    // Assert that the element is invalid and no drop down action is required
                    Assert::AreEqual(ShortcutErrorType::NoError, result.first);
                    Assert::AreEqual(BufferValidationHelpers::DropDownAction::NoAction, result.second);
                });
            }

            // Test if the ValidateShortcutBufferElement method returns ShortcutNotMoreThanOneActionKey error and no drop down action is required on setting non first non last drop down to an action key on a non hybrid control column with full shortcut
            // This test was changed since there is now NoError since more than one shortcut can have the same first key, since we now allow key-chords
            // old name: ValidateShortcutBufferElement_ShouldReturnShortcutNotMoreThanOneActionKeyAndNoAction_OnSettingNonFirstNonLastDropDownToActionKeyOnANonHybridColumnWithFullShortcut
            TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnNoErrorAndNoAction_OnSettingNonFirstNonLastDropDownToActionKeyOnANonHybridColumnWithFullShortcut)
            {
                std::vector<ValidateShortcutBufferElementArgs> testCases;
                // Case 1: Validate the element when selecting A ('A') on second dropdown of first column of shortcut to shortcut row
                testCases.push_back({ 0, 0, 1, { VK_CONTROL, 'A', 'C' }, L"", false, { { Shortcut{ VK_CONTROL, VK_SHIFT, 'C' }, Shortcut{ VK_CONTROL, VK_SHIFT, 'B' } }, L"" } });
                // Case 2: Validate the element when selecting A  ('A')on second dropdown of second column of shortcut to shortcut row
                testCases.push_back({ 0, 1, 1, { VK_CONTROL, 'A', 'B' }, L"", false, { { Shortcut{ VK_CONTROL, VK_SHIFT, 'C' }, Shortcut{ VK_CONTROL, VK_SHIFT, 'B' } }, L"" } });

                RunTestCases(testCases, [this](const ValidateShortcutBufferElementArgs& testCase) {
                    // Arrange
                    RemapBuffer remapBuffer;
                    remapBuffer.push_back(testCase.bufferRow);

                    // Act
                    std::pair<ShortcutErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedCodesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, remapBuffer, true);

                    // Assert that the element is invalid and no drop down action is required
                    Assert::AreEqual(ShortcutErrorType::NoError, result.first);
                    Assert::AreEqual(BufferValidationHelpers::DropDownAction::NoAction, result.second);
                });
            }

            // Test if the ValidateShortcutBufferElement method returns no error and no drop down action is required on setting last drop down to an action key on a column with atleast two drop downs
            TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnNoErrorAndNoAction_OnSettingLastDropDownToActionKeyOnAColumnWithAtleastTwoDropDowns)
            {
                std::vector<ValidateShortcutBufferElementArgs> testCases;
                // Case 1: Validate the element when selecting A ('A') on last dropdown of first column of three key shortcut to shortcut row
                testCases.push_back({ 0, 0, 2, { VK_CONTROL, VK_SHIFT, 'A' }, L"", false, { { Shortcut{ VK_CONTROL, VK_SHIFT, 'C' }, Shortcut{ VK_CONTROL, VK_SHIFT, 'B' } }, L"" } });
                // Case 2: Validate the element when selecting A ('A') on last dropdown of second column of three key shortcut to shortcut row
                testCases.push_back({ 0, 1, 2, { VK_CONTROL, VK_SHIFT, 'A' }, L"", false, { { Shortcut{ VK_CONTROL, VK_SHIFT, 'C' }, Shortcut{ VK_CONTROL, VK_SHIFT, 'B' } }, L"" } });
                // Case 3: Validate the element when selecting A ('A') on last dropdown of hybrid second column of three key shortcut to shortcut row
                testCases.push_back({ 0, 1, 2, { VK_CONTROL, VK_SHIFT, 'A' }, L"", true, { { Shortcut{ VK_CONTROL, VK_SHIFT, 'C' }, Shortcut{ VK_CONTROL, VK_SHIFT, 'B' } }, L"" } });
                // Case 4: Validate the element when selecting A ('A') on last dropdown of first column of two key shortcut to shortcut row
                testCases.push_back({ 0, 0, 1, { VK_CONTROL, 'A' }, L"", false, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut{ VK_CONTROL, 'B' } }, L"" } });
                // Case 5: Validate the element when selecting A ('A') on last dropdown of second column of two key shortcut to shortcut row
                testCases.push_back({ 0, 1, 1, { VK_CONTROL, 'A' }, L"", false, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut{ VK_CONTROL, 'B' } }, L"" } });
                // Case 6: Validate the element when selecting A ('A') on last dropdown of hybrid second column of two key shortcut to shortcut row
                testCases.push_back({ 0, 1, 1, { VK_CONTROL, 'A' }, L"", true, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut{ VK_CONTROL, 'B' } }, L"" } });

                RunTestCases(testCases, [this](const ValidateShortcutBufferElementArgs& testCase) {
                    // Arrange
                    RemapBuffer remapBuffer;
                    remapBuffer.push_back(testCase.bufferRow);

                    // Act
                    std::pair<ShortcutErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedCodesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, remapBuffer, true);

                    // Assert that the element is valid and no drop down action is required
                    Assert::AreEqual(ShortcutErrorType::NoError, result.first);
                    Assert::AreEqual(BufferValidationHelpers::DropDownAction::NoAction, result.second);
                });
            }

            // Test if the ValidateShortcutBufferElement method returns no error and ClearUnusedDropDowns action is required on setting non first drop down to an action key on a column if all the drop downs after it are empty
            TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnNoErrorAndClearUnusedDropDownsAction_OnSettingNonFirstDropDownToActionKeyOnAColumnIfAllTheDropDownsAfterItAreEmpty)
            {
                std::vector<ValidateShortcutBufferElementArgs> testCases;
                // Case 1: Validate the element when selecting A ('A') on second dropdown of first column of 3 dropdown shortcut to shortcut row
                testCases.push_back({ 0, 0, 1, { VK_CONTROL, 'A', VK_INVALID }, L"", false, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut{ VK_CONTROL, 'B' } }, L"" } });
                // Case 2: Validate the element when selecting A ('A') on second dropdown of second column of 3 dropdown shortcut to shortcut row
                testCases.push_back({ 0, 1, 1, { VK_CONTROL, 'A', VK_INVALID }, L"", false, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut{ VK_CONTROL, 'B' } }, L"" } });
                // Case 3: Validate the element when selecting A ('A') on second dropdown of second column of 3 dropdown hybrid shortcut to shortcut row
                testCases.push_back({ 0, 1, 1, { VK_CONTROL, 'A', VK_INVALID }, L"", true, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut{ VK_CONTROL, 'B' } }, L"" } });
                // Case 4: Validate the element when selecting A ('A') on second dropdown of first column of empty 3 dropdown shortcut to shortcut row
                testCases.push_back({ 0, 0, 1, { VK_INVALID, 'A', VK_INVALID }, L"", false, { { Shortcut(), Shortcut() }, L"" } });
                // Case 5: Validate the element when selecting A ('A') on second dropdown of second column of empty 3 dropdown shortcut to shortcut row
                testCases.push_back({ 0, 1, 1, { VK_INVALID, 'A', VK_INVALID }, L"", false, { { Shortcut(), Shortcut() }, L"" } });
                // Case 6: Validate the element when selecting A ('A') on second dropdown of second column of empty 3 dropdown hybrid shortcut to shortcut row
                testCases.push_back({ 0, 1, 1, { VK_INVALID, 'A', VK_INVALID }, L"", true, { { Shortcut(), Shortcut() }, L"" } });

                RunTestCases(testCases, [this](const ValidateShortcutBufferElementArgs& testCase) {
                    // Arrange
                    RemapBuffer remapBuffer;
                    remapBuffer.push_back(testCase.bufferRow);

                    // Act
                    std::pair<ShortcutErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedCodesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, remapBuffer, true);

                    // Assert that the element is valid and ClearUnusedDropDowns action is required
                    Assert::AreEqual(ShortcutErrorType::NoError, result.first);
                    Assert::AreEqual(BufferValidationHelpers::DropDownAction::ClearUnusedDropDowns, result.second);
                });
            }

            // Test if the ValidateShortcutBufferElement method returns no error and ClearUnusedDropDowns action is required on setting first drop down to an action key on a hybrid column if all the drop downs after it are empty
            TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnNoErrorAndClearUnusedDropDownsAction_OnSettingFirstDropDownToActionKeyOnAHybridColumnIfAllTheDropDownsAfterItAreEmpty)
            {
                std::vector<ValidateShortcutBufferElementArgs> testCases;
                // Case 1: Validate the element when selecting A ('A') on first dropdown of second column of empty 3 dropdown hybrid shortcut to shortcut row
                testCases.push_back({ 0, 1, 0, { 'A', VK_INVALID, VK_INVALID }, L"", true, { { Shortcut(), Shortcut() }, L"" } });

                RunTestCases(testCases, [this](const ValidateShortcutBufferElementArgs& testCase) {
                    // Arrange
                    RemapBuffer remapBuffer;
                    remapBuffer.push_back(testCase.bufferRow);

                    // Act
                    std::pair<ShortcutErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedCodesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, remapBuffer, true);

                    // Assert that the element is valid and ClearUnusedDropDowns action is required
                    Assert::AreEqual(ShortcutErrorType::NoError, result.first);
                    Assert::AreEqual(BufferValidationHelpers::DropDownAction::ClearUnusedDropDowns, result.second);
                });
            }

            // Test if the ValidateShortcutBufferElement method returns no error and AddDropDown action is required on setting last drop down to a non-repeated modifier key on a column there are less than 3 drop downs
            TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnNoErrorAndAddDropDownAction_OnSettingLastDropDownToNonRepeatedModifierKeyOnAColumnIfThereAreLessThan3DropDowns)
            {
                std::vector<ValidateShortcutBufferElementArgs> testCases;
                // Case 1: Validate the element when selecting Shift (VK_SHIFT) on second dropdown of first column of 2 dropdown shortcut to shortcut row
                testCases.push_back({ 0, 0, 1, { VK_CONTROL, VK_SHIFT }, L"", false, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut{ VK_CONTROL, 'B' } }, L"" } });
                // Case 2: Validate the element when selecting Shift (VK_SHIFT) on second dropdown of second column of 2 dropdown shortcut to shortcut row
                testCases.push_back({ 0, 1, 1, { VK_CONTROL, VK_SHIFT }, L"", false, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut{ VK_CONTROL, 'B' } }, L"" } });
                // Case 3: Validate the element when selecting Shift (VK_SHIFT) on second dropdown of second column of 2 dropdown hybrid shortcut to shortcut row
                testCases.push_back({ 0, 1, 1, { VK_CONTROL, VK_SHIFT }, L"", true, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut{ VK_CONTROL, 'B' } }, L"" } });
                // Case 4: Validate the element when selecting Shift (VK_SHIFT) on first dropdown of first column of 1 dropdown shortcut to shortcut row
                testCases.push_back({ 0, 0, 0, { VK_SHIFT }, L"", false, { { Shortcut(), Shortcut() }, L"" } });
                // Case 5: Validate the element when selecting Shift (VK_SHIFT) on first dropdown of second column of 1 dropdown shortcut to shortcut row
                testCases.push_back({ 0, 1, 0, { VK_SHIFT }, L"", false, { { Shortcut(), Shortcut() }, L"" } });
                // Case 6: Validate the element when selecting Shift (VK_SHIFT) on first dropdown of second column of 1 dropdown hybrid shortcut to shortcut row
                testCases.push_back({ 0, 1, 0, { VK_SHIFT }, L"", true, { { Shortcut(), Shortcut() }, L"" } });
                // Case 7: Validate the element when selecting Shift (VK_SHIFT) on first dropdown of second column of 1 dropdown hybrid shortcut to key row with an action key selected
                testCases.push_back({ 0, 1, 0, { VK_SHIFT }, L"", true, { { Shortcut(), 'D' }, L"" } });

                RunTestCases(testCases, [this](const ValidateShortcutBufferElementArgs& testCase) {
                    // Arrange
                    RemapBuffer remapBuffer;
                    remapBuffer.push_back(testCase.bufferRow);

                    // Act
                    std::pair<ShortcutErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedCodesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, remapBuffer, true);

                    // Assert that the element is valid and AddDropDown action is required
                    Assert::AreEqual(ShortcutErrorType::NoError, result.first);
                    Assert::AreEqual(BufferValidationHelpers::DropDownAction::AddDropDown, result.second);
                });
            }

            // Test if the ValidateShortcutBufferElement method returns ShortcutCannotHaveRepeatedModifier error and no action is required on setting last drop down to a repeated modifier key on a column there are less than 3 drop downs
            TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnShortcutCannotHaveRepeatedModifierErrorAndNoAction_OnSettingLastDropDownToRepeatedModifierKeyOnAColumnIfThereAreLessThan3DropDowns)
            {
                std::vector<ValidateShortcutBufferElementArgs> testCases;
                // Case 1: Validate the element when selecting LCtrl (VK_LCONTROL) on second dropdown of first column of 2 dropdown shortcut to shortcut row
                testCases.push_back({ 0, 0, 1, { VK_CONTROL, VK_LCONTROL }, L"", false, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut{ VK_CONTROL, 'B' } }, L"" } });
                // Case 2: Validate the element when selecting LCtrl (VK_LCONTROL) on second dropdown of second column of 2 dropdown hybrid shortcut to shortcut row
                testCases.push_back({ 0, 1, 1, { VK_CONTROL, VK_LCONTROL }, L"", false, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut{ VK_CONTROL, 'B' } }, L"" } });
                // Case 3: Validate the element when selecting LCtrl (VK_LCONTROL) on second dropdown of second column of 2 dropdown hybrid shortcut to shortcut row
                testCases.push_back({ 0, 1, 1, { VK_CONTROL, VK_LCONTROL }, L"", true, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut{ VK_CONTROL, 'B' } }, L"" } });

                RunTestCases(testCases, [this](const ValidateShortcutBufferElementArgs& testCase) {
                    // Arrange
                    RemapBuffer remapBuffer;
                    remapBuffer.push_back(testCase.bufferRow);

                    // Act
                    std::pair<ShortcutErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedCodesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, remapBuffer, true);

                    // Assert that the element is invalid and no action is required
                    Assert::AreEqual(ShortcutErrorType::ShortcutCannotHaveRepeatedModifier, result.first);
                    Assert::AreEqual(BufferValidationHelpers::DropDownAction::NoAction, result.second);
                });
            }

            // Test if the ValidateShortcutBufferElement method returns ShortcutMaxShortcutSizeOneActionKey error and no action is required on setting last drop down to a non repeated modifier key on a column there 3 or more drop downs
            TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnShortcutMaxShortcutSizeOneActionKeyErrorAndNoAction_OnSettingLastDropDownToNonRepeatedModifierKeyOnAColumnIfThereAre3OrMoreDropDowns)
            {
                std::vector<ValidateShortcutBufferElementArgs> testCases;
                // Case 1: Validate the element when selecting Shift (VK_SHIFT) on last dropdown of first column of 5 dropdown shortcut to shortcut row with middle empty
                testCases.push_back({ 0, 0, 4, { VK_CONTROL, VK_INVALID, VK_INVALID, VK_INVALID, VK_SHIFT }, L"", false, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut{ VK_CONTROL, 'B' } }, L"" } });
                // Case 2: Validate the element when selecting Shift (VK_SHIFT) on last dropdown of second column of 5 dropdown shortcut to shortcut row with middle empty
                testCases.push_back({ 0, 1, 4, { VK_CONTROL, VK_INVALID, VK_INVALID, VK_INVALID, VK_SHIFT }, L"", false, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut{ VK_CONTROL, 'B' } }, L"" } });
                // Case 3: Validate the element when selecting Shift (VK_SHIFT) on last dropdown of second column of 5 dropdown hybrid shortcut to shortcut row with middle empty
                testCases.push_back({ 0, 1, 4, { VK_CONTROL, VK_INVALID, VK_INVALID, VK_INVALID, VK_SHIFT }, L"", true, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut{ VK_CONTROL, 'B' } }, L"" } });
                // Case 4: Validate the element when selecting Shift (VK_SHIFT) on last dropdown of first column of 5 dropdown shortcut to shortcut row with first four empty
                testCases.push_back({ 0, 0, 4, { VK_INVALID, VK_INVALID, VK_INVALID, VK_INVALID, VK_SHIFT }, L"", false, { { Shortcut(), Shortcut() }, L"" } });
                // Case 5: Validate the element when selecting Shift (VK_SHIFT) on last dropdown of second column of 5 dropdown shortcut to shortcut row with first four empty
                testCases.push_back({ 0, 1, 4, { VK_INVALID, VK_INVALID, VK_INVALID, VK_INVALID, VK_SHIFT }, L"", false, { { Shortcut(), Shortcut() }, L"" } });
                // Case 6: Validate the element when selecting Shift (VK_SHIFT) on last dropdown of second column of 5 dropdown hybrid shortcut to shortcut row with first four empty
                testCases.push_back({ 0, 1, 4, { VK_INVALID, VK_INVALID, VK_INVALID, VK_INVALID, VK_SHIFT }, L"", true, { { Shortcut(), Shortcut() }, L"" } });
                // Case 7: Validate the element when selecting Shift (VK_SHIFT) on last dropdown of first column of 5 dropdown shortcut to shortcut row
                testCases.push_back({ 0, 0, 4, { VK_CONTROL, VK_MENU, VK_LWIN, VK_RWIN, VK_SHIFT }, L"", false, { { Shortcut{ VK_CONTROL, VK_MENU, VK_LWIN, VK_RWIN, 'C' }, Shortcut{ VK_CONTROL, VK_MENU, VK_LWIN, VK_RWIN, 'B' } }, L"" } });
                // Case 8: Validate the element when selecting Shift (VK_SHIFT) on last dropdown of second column of 5 dropdown shortcut to shortcut row
                testCases.push_back({ 0, 1, 4, { VK_CONTROL, VK_MENU, VK_LWIN, VK_RWIN, VK_SHIFT }, L"", false, { { Shortcut{ VK_CONTROL, VK_MENU, VK_LWIN, VK_RWIN, 'C' }, Shortcut{ VK_CONTROL, VK_MENU, VK_LWIN, VK_RWIN, 'B' } }, L"" } });
                // Case 9: Validate the element when selecting Shift (VK_SHIFT) on last dropdown of second column of 5 dropdown hybrid shortcut to shortcut row
                testCases.push_back({ 0, 1, 4, { VK_CONTROL, VK_MENU, VK_LWIN, VK_RWIN, VK_SHIFT }, L"", true, { { Shortcut{ VK_CONTROL, VK_MENU, VK_LWIN, VK_RWIN, 'C' }, Shortcut{ VK_CONTROL, VK_MENU, VK_LWIN, VK_RWIN, 'B' } }, L"" } });

                RunTestCases(testCases, [this](const ValidateShortcutBufferElementArgs& testCase) {
                    // Arrange
                    RemapBuffer remapBuffer;
                    remapBuffer.push_back(testCase.bufferRow);

                    // Act
                    std::pair<ShortcutErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedCodesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, remapBuffer, true);

                    // Assert that the element is invalid and no action is required
                    Assert::AreEqual(ShortcutErrorType::ShortcutMaxShortcutSizeOneActionKey, result.first);
                    Assert::AreEqual(BufferValidationHelpers::DropDownAction::NoAction, result.second);
                });
            }

            // Test if the ValidateShortcutBufferElement method returns ShortcutMaxShortcutSizeOneActionKey error and no action is required on setting last drop down to a repeated modifier key on a column there 3 or more drop downs
            TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnShortcutMaxShortcutSizeOneActionKeyErrorAndNoAction_OnSettingLastDropDownToRepeatedModifierKeyOnAColumnIfThereAre3OrMoreDropDowns)
            {
                std::vector<ValidateShortcutBufferElementArgs> testCases;
                // Case 1: Validate the element when selecting Ctrl (VK_CONTROL) on last dropdown of first column of 5 dropdown shortcut to shortcut row with middle empty
                testCases.push_back({ 0, 0, 4, { VK_CONTROL, VK_INVALID, VK_INVALID, VK_INVALID, VK_CONTROL }, L"", false, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut{ VK_CONTROL, 'B' } }, L"" } });
                // Case 2: Validate the element when selecting Ctrl (VK_CONTROL) on last dropdown of second column of 5 dropdown shortcut to shortcut row with middle empty
                testCases.push_back({ 0, 1, 4, { VK_CONTROL, VK_INVALID, VK_INVALID, VK_INVALID, VK_CONTROL }, L"", false, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut{ VK_CONTROL, 'B' } }, L"" } });
                // Case 3: Validate the element when selecting Ctrl (VK_CONTROL) on last dropdown of second column of 5 dropdown hybrid shortcut to shortcut row with middle empty
                testCases.push_back({ 0, 1, 4, { VK_CONTROL, VK_INVALID, VK_INVALID, VK_INVALID, VK_CONTROL }, L"", true, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut{ VK_CONTROL, 'B' } }, L"" } });
                // Case 4: Validate the element when selecting Ctrl (VK_CONTROL) on last dropdown of first column of 5 dropdown shortcut to shortcut row
                testCases.push_back({ 0, 0, 4, { VK_CONTROL, VK_MENU, VK_LWIN, VK_SHIFT, VK_CONTROL }, L"", false, { { Shortcut{ VK_CONTROL, VK_MENU, VK_LWIN, VK_SHIFT, 'C' }, Shortcut{ VK_CONTROL, VK_MENU, VK_LWIN, VK_SHIFT, 'B' } }, L"" } });
                // Case 5: Validate the element when selecting Ctrl (VK_CONTROL) on last dropdown of second column of 5 dropdown shortcut to shortcut row
                testCases.push_back({ 0, 1, 4, { VK_CONTROL, VK_MENU, VK_LWIN, VK_SHIFT, VK_CONTROL }, L"", false, { { Shortcut{ VK_CONTROL, VK_MENU, VK_LWIN, VK_SHIFT, 'C' }, Shortcut{ VK_CONTROL, VK_MENU, VK_LWIN, VK_SHIFT, 'B' } }, L"" } });
                // Case 6: Validate the element when selecting Ctrl (VK_CONTROL) on last dropdown of second column of 5 dropdown hybrid shortcut to shortcut row
                testCases.push_back({ 0, 1, 4, { VK_CONTROL, VK_MENU, VK_LWIN, VK_SHIFT, VK_CONTROL }, L"", true, { { Shortcut{ VK_CONTROL, VK_MENU, VK_LWIN, VK_SHIFT, 'C' }, Shortcut{ VK_CONTROL, VK_MENU, VK_LWIN, VK_SHIFT, 'B' } }, L"" } });

                RunTestCases(testCases, [this](const ValidateShortcutBufferElementArgs& testCase) {
                    // Arrange
                    RemapBuffer remapBuffer;
                    remapBuffer.push_back(testCase.bufferRow);

                    // Act
                    std::pair<ShortcutErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedCodesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, remapBuffer, true);

                    // Assert that the element is invalid and no action is required
                    Assert::AreEqual(ShortcutErrorType::ShortcutMaxShortcutSizeOneActionKey, result.first);
                    Assert::AreEqual(BufferValidationHelpers::DropDownAction::NoAction, result.second);
                });
            }

            // Test if the ValidateShortcutBufferElement method returns no error and no action is required on setting non-last drop down to a non repeated modifier key on a column
            TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnNoErrorAndNoAction_OnSettingNonLastDropDownToNonRepeatedModifierKeyOnAColumn)
            {
                std::vector<ValidateShortcutBufferElementArgs> testCases;
                // Case 1: Validate the element when selecting Shift (VK_SHIFT) on first dropdown of first column of 2 dropdown shortcut to shortcut row
                testCases.push_back({ 0, 0, 0, { VK_CONTROL, VK_SHIFT }, L"", false, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut{ VK_CONTROL, 'B' } }, L"" } });
                // Case 2: Validate the element when selecting Shift (VK_SHIFT) on first dropdown of second column of 2 dropdown shortcut to shortcut row
                testCases.push_back({ 0, 1, 0, { VK_CONTROL, VK_SHIFT }, L"", false, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut{ VK_CONTROL, 'B' } }, L"" } });
                // Case 3: Validate the element when selecting Shift (VK_SHIFT) on first dropdown of second column of 2 dropdown hybrid shortcut to shortcut row
                testCases.push_back({ 0, 1, 0, { VK_CONTROL, VK_SHIFT }, L"", true, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut{ VK_CONTROL, 'B' } }, L"" } });
                // Case 4: Validate the element when selecting Shift (VK_SHIFT) on first dropdown of first column of 3 dropdown shortcut to shortcut row
                testCases.push_back({ 0, 0, 0, { VK_SHIFT, VK_MENU, 'C' }, L"", false, { { Shortcut{ VK_CONTROL, VK_MENU, 'C' }, Shortcut{ VK_CONTROL, VK_MENU, 'B' } }, L"" } });
                // Case 5: Validate the element when selecting Shift (VK_SHIFT) on first dropdown of second column of 3 dropdown shortcut to shortcut row
                testCases.push_back({ 0, 1, 0, { VK_SHIFT, VK_MENU, 'B' }, L"", false, { { Shortcut{ VK_CONTROL, VK_MENU, 'C' }, Shortcut{ VK_CONTROL, VK_MENU, 'B' } }, L"" } });
                // Case 6: Validate the element when selecting Shift (VK_SHIFT) on first dropdown of second column of 3 dropdown hybrid shortcut to shortcut row
                testCases.push_back({ 0, 1, 0, { VK_SHIFT, VK_MENU, 'B' }, L"", true, { { Shortcut{ VK_CONTROL, VK_MENU, 'C' }, Shortcut{ VK_CONTROL, VK_MENU, 'B' } }, L"" } });
                // Case 7: Validate the element when selecting Shift (VK_SHIFT) on second dropdown of first column of 3 dropdown shortcut to shortcut row
                testCases.push_back({ 0, 0, 1, { VK_CONTROL, VK_SHIFT, 'C' }, L"", false, { { Shortcut{ VK_CONTROL, VK_MENU, 'C' }, Shortcut{ VK_CONTROL, VK_MENU, 'B' } }, L"" } });
                // Case 8: Validate the element when selecting Shift (VK_SHIFT) on second dropdown of second column of 3 dropdown shortcut to shortcut row
                testCases.push_back({ 0, 1, 1, { VK_CONTROL, VK_SHIFT, 'B' }, L"", false, { { Shortcut{ VK_CONTROL, VK_MENU, 'C' }, Shortcut{ VK_CONTROL, VK_MENU, 'B' } }, L"" } });
                // Case 9: Validate the element when selecting Shift (VK_SHIFT) on second dropdown of second column of 3 dropdown hybrid shortcut to shortcut row
                testCases.push_back({ 0, 1, 1, { VK_CONTROL, VK_SHIFT, 'B' }, L"", true, { { Shortcut{ VK_CONTROL, VK_MENU, 'C' }, Shortcut{ VK_CONTROL, VK_MENU, 'B' } }, L"" } });

                RunTestCases(testCases, [this](const ValidateShortcutBufferElementArgs& testCase) {
                    // Arrange
                    RemapBuffer remapBuffer;
                    remapBuffer.push_back(testCase.bufferRow);

                    // Act
                    std::pair<ShortcutErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedCodesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, remapBuffer, true);

                    // Assert that the element is valid and no action is required
                    Assert::AreEqual(ShortcutErrorType::NoError, result.first);
                    Assert::AreEqual(BufferValidationHelpers::DropDownAction::NoAction, result.second);
                });
            }

            // Test if the ValidateShortcutBufferElement method returns ShortcutCannotHaveRepeatedModifier error and no action is required on setting non-last drop down to a repeated modifier key on a column
            TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnShortcutCannotHaveRepeatedModifierErrorAndNoAction_OnSettingNonLastDropDownToRepeatedModifierKeyOnAColumn)
            {
                std::vector<ValidateShortcutBufferElementArgs> testCases;
                // Case 1: Validate the element when selecting Ctrl (VK_CONTROL) on first dropdown of first column of 3 dropdown shortcut to shortcut row with first empty
                testCases.push_back({ 0, 0, 0, { VK_CONTROL, VK_CONTROL, 'C' }, L"", false, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut{ VK_CONTROL, 'B' } }, L"" } });
                // Case 2: Validate the element when selecting Ctrl (VK_CONTROL) on first dropdown of second column of 3 dropdown shortcut to shortcut row with first empty
                testCases.push_back({ 0, 1, 0, { VK_CONTROL, VK_CONTROL, 'B' }, L"", false, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut{ VK_CONTROL, 'B' } }, L"" } });
                // Case 3: Validate the element when selecting Ctrl (VK_CONTROL) on first dropdown of second column of 3 dropdown hybrid shortcut to shortcut row with first empty
                testCases.push_back({ 0, 1, 0, { VK_CONTROL, VK_CONTROL, 'B' }, L"", true, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut{ VK_CONTROL, 'B' } }, L"" } });
                // Case 4: Validate the element when selecting Alt (VK_MENU) on first dropdown of first column of 3 dropdown shortcut to shortcut row
                testCases.push_back({ 0, 0, 0, { VK_MENU, VK_MENU, 'C' }, L"", false, { { Shortcut{ VK_CONTROL, VK_MENU, 'C' }, Shortcut{ VK_CONTROL, VK_MENU, 'B' } }, L"" } });
                // Case 5: Validate the element when selecting Alt (VK_MENU) on first dropdown of second column of 3 dropdown shortcut to shortcut row
                testCases.push_back({ 0, 1, 0, { VK_MENU, VK_MENU, 'B' }, L"", false, { { Shortcut{ VK_CONTROL, VK_MENU, 'C' }, Shortcut{ VK_CONTROL, VK_MENU, 'B' } }, L"" } });
                // Case 6: Validate the element when selecting Alt (VK_MENU) on first dropdown of second column of 3 dropdown hybrid shortcut to shortcut row
                testCases.push_back({ 0, 1, 0, { VK_MENU, VK_MENU, 'B' }, L"", true, { { Shortcut{ VK_CONTROL, VK_MENU, 'C' }, Shortcut{ VK_CONTROL, VK_MENU, 'B' } }, L"" } });
                // Case 7: Validate the element when selecting Ctrl (VK_CONTROL) on second dropdown of first column of 3 dropdown shortcut to shortcut row
                testCases.push_back({ 0, 0, 1, { VK_CONTROL, VK_CONTROL, 'C' }, L"", false, { { Shortcut{ VK_CONTROL, VK_MENU, 'C' }, Shortcut{ VK_CONTROL, VK_MENU, 'B' } }, L"" } });
                // Case 8: Validate the element when selecting Ctrl (VK_CONTROL) on second dropdown of second column of 3 dropdown shortcut to shortcut row
                testCases.push_back({ 0, 1, 1, { VK_CONTROL, VK_CONTROL, 'B' }, L"", false, { { Shortcut{ VK_CONTROL, VK_MENU, 'C' }, Shortcut{ VK_CONTROL, VK_MENU, 'B' } }, L"" } });
                // Case 9: Validate the element when selecting Ctrl (VK_CONTROL) on second dropdown of second column of 3 dropdown hybrid shortcut to shortcut row
                testCases.push_back({ 0, 1, 1, { VK_CONTROL, VK_CONTROL, 'B' }, L"", true, { { Shortcut{ VK_CONTROL, VK_MENU, 'C' }, Shortcut{ VK_CONTROL, VK_MENU, 'B' } }, L"" } });

                RunTestCases(testCases, [this](const ValidateShortcutBufferElementArgs& testCase) {
                    // Arrange
                    RemapBuffer remapBuffer;
                    remapBuffer.push_back(testCase.bufferRow);

                    // Act
                    std::pair<ShortcutErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedCodesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, remapBuffer, true);

                    // Assert that the element is invalid and no action is required
                    Assert::AreEqual(ShortcutErrorType::ShortcutCannotHaveRepeatedModifier, result.first);
                    Assert::AreEqual(BufferValidationHelpers::DropDownAction::NoAction, result.second);
                });
            }

            // Test if the ValidateShortcutBufferElement method returns ShortcutStartWithModifier error and no action is required on setting first drop down to None on a non-hybrid column with one drop down
            TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnShortcutStartWithModifierErrorAndNoAction_OnSettingFirstDropDownToNoneOnNonHybridColumnWithOneDropDown)
            {
                std::vector<ValidateShortcutBufferElementArgs> testCases;
                // Case 1: Validate the element when selecting None (VK_NULL) on first dropdown of first column of 1 dropdown shortcut to shortcut row
                testCases.push_back({ 0, 0, 0, { VK_NULL }, L"", false, { { Shortcut(), Shortcut() }, L"" } });
                // Case 2: Validate the element when selecting None (VK_NULL) on first dropdown of second column of 1 dropdown shortcut to shortcut row
                testCases.push_back({ 0, 1, 0, { VK_NULL }, L"", false, { { Shortcut(), Shortcut() }, L"" } });

                RunTestCases(testCases, [this](const ValidateShortcutBufferElementArgs& testCase) {
                    // Arrange
                    RemapBuffer remapBuffer;
                    remapBuffer.push_back(testCase.bufferRow);

                    // Act
                    std::pair<ShortcutErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedCodesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, remapBuffer, true);

                    // Assert that the element is invalid and no action is required
                    Assert::AreEqual(ShortcutErrorType::ShortcutStartWithModifier, result.first);
                    Assert::AreEqual(BufferValidationHelpers::DropDownAction::NoAction, result.second);
                });
            }

            // Test if the ValidateShortcutBufferElement method returns ShortcutOneActionKey error and no action is required on setting first drop down to None on a hybrid column with one drop down
            TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnShortcutOneActionKeyErrorAndNoAction_OnSettingFirstDropDownToNoneOnHybridColumnWithOneDropDown)
            {
                std::vector<ValidateShortcutBufferElementArgs> testCases;
                // Case 1: Validate the element when selecting None (VK_NULL) on first dropdown of first column of 1 dropdown hybrid shortcut to shortcut row
                testCases.push_back({ 0, 1, 0, { VK_NULL }, L"", true, { { Shortcut(), Shortcut() }, L"" } });

                RunTestCases(testCases, [this](const ValidateShortcutBufferElementArgs& testCase) {
                    // Arrange
                    RemapBuffer remapBuffer;
                    remapBuffer.push_back(testCase.bufferRow);

                    // Act
                    std::pair<ShortcutErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedCodesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, remapBuffer, true);

                    // Assert that the element is invalid and no action is required
                    Assert::AreEqual(ShortcutErrorType::ShortcutOneActionKey, result.first);
                    Assert::AreEqual(BufferValidationHelpers::DropDownAction::NoAction, result.second);
                });
            }

            // Test if the ValidateShortcutBufferElement method returns ShortcutAtleast2Keys error and no action is required on setting first drop down to None on a non-hybrid column with two drop down
            TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnShortcutAtleast2KeysAndNoAction_OnSettingFirstDropDownToNoneOnNonHybridColumnWithTwoDropDowns)
            {
                std::vector<ValidateShortcutBufferElementArgs> testCases;
                // Case 1: Validate the element when selecting None (VK_NULL) on first dropdown of first column of 2 dropdown empty shortcut to shortcut row
                testCases.push_back({ 0, 0, 0, { VK_NULL, VK_INVALID }, L"", false, { { Shortcut(), Shortcut() }, L"" } });
                // Case 2: Validate the element when selecting None (VK_NULL) on first dropdown of second column of 2 dropdown empty shortcut to shortcut row
                testCases.push_back({ 0, 1, 0, { VK_NULL, VK_INVALID }, L"", false, { { Shortcut(), Shortcut() }, L"" } });
                // Case 3: Validate the element when selecting None (VK_NULL) on first dropdown of second column of 2 dropdown valid shortcut to shortcut row
                testCases.push_back({ 0, 0, 0, { VK_NULL, 'C' }, L"", false, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut{ VK_CONTROL, 'B' } }, L"" } });
                // Case 4: Validate the element when selecting None (VK_NULL) on first dropdown of second column of 2 dropdown valid shortcut to shortcut row
                testCases.push_back({ 0, 1, 0, { VK_NULL, 'B' }, L"", false, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut{ VK_CONTROL, 'B' } }, L"" } });

                RunTestCases(testCases, [this](const ValidateShortcutBufferElementArgs& testCase) {
                    // Arrange
                    RemapBuffer remapBuffer;
                    remapBuffer.push_back(testCase.bufferRow);

                    // Act
                    std::pair<ShortcutErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedCodesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, remapBuffer, true);

                    // Assert that the element is invalid and no action is required
                    Assert::AreEqual(ShortcutErrorType::ShortcutAtleast2Keys, result.first);
                    Assert::AreEqual(BufferValidationHelpers::DropDownAction::NoAction, result.second);
                });
            }

            // Test if the ValidateShortcutBufferElement method returns ShortcutOneActionKey error and no action is required on setting second drop down to None on a non-hybrid column with two drop down
            TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnShortcutOneActionKeyAndNoAction_OnSettingSecondDropDownToNoneOnNonHybridColumnWithTwoDropDowns)
            {
                std::vector<ValidateShortcutBufferElementArgs> testCases;
                // Case 1: Validate the element when selecting None (VK_NULL) on second dropdown of first column of 2 dropdown empty shortcut to shortcut row
                testCases.push_back({ 0, 0, 1, { VK_INVALID, VK_NULL }, L"", false, { { Shortcut(), Shortcut() }, L"" } });
                // Case 2: Validate the element when selecting None (VK_NULL) on second dropdown of second column of 2 dropdown empty shortcut to shortcut row
                testCases.push_back({ 0, 1, 1, { VK_INVALID, VK_NULL }, L"", false, { { Shortcut(), Shortcut() }, L"" } });
                // Case 3: Validate the element when selecting None (VK_NULL) on second dropdown of second column of 2 dropdown valid shortcut to shortcut row
                testCases.push_back({ 0, 0, 1, { VK_CONTROL, VK_NULL }, L"", false, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut{ VK_CONTROL, 'B' } }, L"" } });
                // Case 4: Validate the element when selecting None (VK_NULL) on second dropdown of second column of 2 dropdown valid shortcut to shortcut row
                testCases.push_back({ 0, 1, 1, { VK_CONTROL, VK_NULL }, L"", false, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut{ VK_CONTROL, 'B' } }, L"" } });

                RunTestCases(testCases, [this](const ValidateShortcutBufferElementArgs& testCase) {
                    // Arrange
                    RemapBuffer remapBuffer;
                    remapBuffer.push_back(testCase.bufferRow);

                    // Act
                    std::pair<ShortcutErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedCodesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, remapBuffer, true);

                    // Assert that the element is invalid and no action is required
                    Assert::AreEqual(ShortcutErrorType::ShortcutOneActionKey, result.first);
                    Assert::AreEqual(BufferValidationHelpers::DropDownAction::NoAction, result.second);
                });
            }

            // Test if the ValidateShortcutBufferElement method returns no error and DeleteDropDown action is required on setting drop down to None on a hybrid column with two drop down
            TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnNoErrorAndDeleteDropDownAction_OnSettingDropDownToNoneOnHybridColumnWithTwoDropDowns)
            {
                std::vector<ValidateShortcutBufferElementArgs> testCases;
                // Case 1: Validate the element when selecting None (VK_NULL) on first dropdown of second column of 2 dropdown empty hybrid shortcut to shortcut row
                testCases.push_back({ 0, 1, 0, { VK_NULL, VK_INVALID }, L"", true, { { Shortcut(), Shortcut() }, L"" } });
                // Case 2: Validate the element when selecting None (VK_NULL) on second dropdown of second column of 2 dropdown empty hybrid shortcut to shortcut row
                testCases.push_back({ 0, 1, 1, { VK_INVALID, VK_NULL }, L"", true, { { Shortcut(), Shortcut() }, L"" } });
                // Case 3: Validate the element when selecting None (VK_NULL) on first dropdown of second column of 2 dropdown valid hybrid shortcut to shortcut row
                testCases.push_back({ 0, 1, 0, { VK_NULL, VK_CONTROL }, L"", true, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut{ VK_CONTROL, 'B' } }, L"" } });
                // Case 4: Validate the element when selecting None (VK_NULL) on second dropdown of second column of 2 dropdown valid hybrid shortcut to shortcut row
                testCases.push_back({ 0, 1, 1, { VK_CONTROL, VK_NULL }, L"", true, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut{ VK_CONTROL, 'B' } }, L"" } });

                RunTestCases(testCases, [this](const ValidateShortcutBufferElementArgs& testCase) {
                    // Arrange
                    RemapBuffer remapBuffer;
                    remapBuffer.push_back(testCase.bufferRow);

                    // Act
                    std::pair<ShortcutErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedCodesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, remapBuffer, true);

                    // Assert that the element is valid and DeleteDropDown action is required
                    Assert::AreEqual(ShortcutErrorType::NoError, result.first);
                    Assert::AreEqual(BufferValidationHelpers::DropDownAction::DeleteDropDown, result.second);
                });
            }

            // Test if the ValidateShortcutBufferElement method returns no error and DeleteDropDown action is required on setting non last drop down to None on a column with three drop down
            TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnNoErrorAndDeleteDropDownAction_OnSettingNonLastDropDownToNoneOnColumnWithThreeDropDowns)
            {
                std::vector<ValidateShortcutBufferElementArgs> testCases;
                // Case 1: Validate the element when selecting None (VK_NULL) on first dropdown of first column of 3 dropdown empty shortcut to shortcut row
                testCases.push_back({ 0, 0, 0, { VK_NULL, VK_INVALID, VK_INVALID }, L"", false, { { Shortcut(), Shortcut() }, L"" } });
                // Case 2: Validate the element when selecting None (VK_NULL) on first dropdown of second column of 3 dropdown empty shortcut to shortcut row
                testCases.push_back({ 0, 1, 0, { VK_NULL, VK_INVALID, VK_INVALID }, L"", false, { { Shortcut(), Shortcut() }, L"" } });
                // Case 3: Validate the element when selecting None (VK_NULL) on first dropdown of second column of 3 dropdown empty hybrid shortcut to shortcut row
                testCases.push_back({ 0, 1, 0, { VK_NULL, VK_INVALID, VK_INVALID }, L"", true, { { Shortcut(), Shortcut() }, L"" } });
                // Case 4: Validate the element when selecting None (VK_NULL) on second dropdown of first column of 3 dropdown empty shortcut to shortcut row
                testCases.push_back({ 0, 0, 1, { VK_INVALID, 0, VK_INVALID }, L"", false, { { Shortcut(), Shortcut() }, L"" } });
                // Case 5: Validate the element when selecting None (VK_NULL) on second dropdown of second column of 3 dropdown empty shortcut to shortcut row
                testCases.push_back({ 0, 1, 1, { VK_INVALID, 0, VK_INVALID }, L"", false, { { Shortcut(), Shortcut() }, L"" } });
                // Case 6: Validate the element when selecting None (VK_NULL) on second dropdown of second column of 3 dropdown empty hybrid shortcut to shortcut row
                testCases.push_back({ 0, 1, 1, { VK_INVALID, 0, VK_INVALID }, L"", true, { { Shortcut(), Shortcut() }, L"" } });
                // Case 7: Validate the element when selecting None (VK_NULL) on first dropdown of first column of 3 dropdown valid shortcut to shortcut row
                testCases.push_back({ 0, 0, 0, { VK_NULL, VK_MENU, 'C' }, L"", false, { { Shortcut{ VK_CONTROL, VK_MENU, 'C' }, Shortcut{ VK_CONTROL, VK_MENU, 'B' } }, L"" } });
                // Case 8: Validate the element when selecting None (VK_NULL) on first dropdown of second column of 3 dropdown valid shortcut to shortcut row
                testCases.push_back({ 0, 1, 0, { VK_NULL, VK_MENU, 'B' }, L"", false, { { Shortcut{ VK_CONTROL, VK_MENU, 'C' }, Shortcut{ VK_CONTROL, VK_MENU, 'B' } }, L"" } });
                // Case 9: Validate the element when selecting None (VK_NULL) on first dropdown of second column of 3 dropdown valid hybrid shortcut to shortcut row
                testCases.push_back({ 0, 1, 0, { VK_NULL, VK_MENU, 'B' }, L"", true, { { Shortcut{ VK_CONTROL, VK_MENU, 'C' }, Shortcut{ VK_CONTROL, VK_MENU, 'B' } }, L"" } });
                // Case 10: Validate the element when selecting None (VK_NULL) on first dropdown of first column of 3 dropdown valid shortcut to shortcut row
                testCases.push_back({ 0, 0, 1, { VK_CONTROL, VK_NULL, 'C' }, L"", false, { { Shortcut{ VK_CONTROL, VK_MENU, 'C' }, Shortcut{ VK_CONTROL, VK_MENU, 'B' } }, L"" } });
                // Case 11: Validate the element when selecting None (VK_NULL) on first dropdown of second column of 3 dropdown valid shortcut to shortcut row
                testCases.push_back({ 0, 1, 1, { VK_CONTROL, VK_NULL, 'B' }, L"", false, { { Shortcut{ VK_CONTROL, VK_MENU, 'C' }, Shortcut{ VK_CONTROL, VK_MENU, 'B' } }, L"" } });
                // Case 12: Validate the element when selecting None (VK_NULL) on first dropdown of second column of 3 dropdown valid hybrid shortcut to shortcut row
                testCases.push_back({ 0, 1, 1, { VK_CONTROL, VK_NULL, 'B' }, L"", true, { { Shortcut{ VK_CONTROL, VK_MENU, 'C' }, Shortcut{ VK_CONTROL, VK_MENU, 'B' } }, L"" } });

                RunTestCases(testCases, [this](const ValidateShortcutBufferElementArgs& testCase) {
                    // Arrange
                    RemapBuffer remapBuffer;
                    remapBuffer.push_back(testCase.bufferRow);

                    // Act
                    std::pair<ShortcutErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedCodesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, remapBuffer, true);

                    // Assert that the element is valid and DeleteDropDown action is required
                    Assert::AreEqual(ShortcutErrorType::NoError, result.first);
                    Assert::AreEqual(BufferValidationHelpers::DropDownAction::DeleteDropDown, result.second);
                });
            }

            // Test if the ValidateShortcutBufferElement method returns ShortcutOneActionKey error and no action is required on setting last drop down to None on a column with three drop down
            TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnShortcutOneActionKeyErrorAndNoAction_OnSettingLastDropDownToNoneOnColumnWithThreeDropDowns)
            {
                std::vector<ValidateShortcutBufferElementArgs> testCases;
                // Case 1: Validate the element when selecting None (VK_NULL) on first dropdown of first column of 3 dropdown empty shortcut to shortcut row
                testCases.push_back({ 0, 0, 2, { VK_INVALID, VK_INVALID, VK_NULL }, L"", false, { { Shortcut(), Shortcut() }, L"" } });
                // Case 2: Validate the element when selecting None (VK_NULL) on first dropdown of second column of 3 dropdown empty shortcut to shortcut row
                testCases.push_back({ 0, 1, 2, { VK_INVALID, VK_INVALID, VK_NULL }, L"", false, { { Shortcut(), Shortcut() }, L"" } });
                // Case 3: Validate the element when selecting None (VK_NULL) on first dropdown of second column of 3 dropdown empty hybrid shortcut to shortcut row
                testCases.push_back({ 0, 1, 2, { VK_INVALID, VK_INVALID, VK_NULL }, L"", true, { { Shortcut(), Shortcut() }, L"" } });
                // Case 4: Validate the element when selecting None (VK_NULL) on first dropdown of first column of 3 dropdown valid shortcut to shortcut row
                testCases.push_back({ 0, 0, 2, { VK_CONTROL, VK_MENU, VK_NULL }, L"", false, { { Shortcut{ VK_CONTROL, VK_MENU, 'C' }, Shortcut{ VK_CONTROL, VK_MENU, 'B' } }, L"" } });
                // Case 5: Validate the element when selecting None (VK_NULL) on first dropdown of second column of 3 dropdown valid shortcut to shortcut row
                testCases.push_back({ 0, 1, 2, { VK_CONTROL, VK_MENU, VK_NULL }, L"", false, { { Shortcut{ VK_CONTROL, VK_MENU, 'C' }, Shortcut{ VK_CONTROL, VK_MENU, 'B' } }, L"" } });
                // Case 6: Validate the element when selecting None (VK_NULL) on first dropdown of second column of 3 dropdown valid hybrid shortcut to shortcut row
                testCases.push_back({ 0, 1, 2, { VK_CONTROL, VK_MENU, VK_NULL }, L"", true, { { Shortcut{ VK_CONTROL, VK_MENU, 'C' }, Shortcut{ VK_CONTROL, VK_MENU, 'B' } }, L"" } });

                RunTestCases(testCases, [this](const ValidateShortcutBufferElementArgs& testCase) {
                    // Arrange
                    RemapBuffer remapBuffer;
                    remapBuffer.push_back(testCase.bufferRow);

                    // Act
                    std::pair<ShortcutErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedCodesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, remapBuffer, true);

                    // Assert that the element is invalid and no action is required
                    Assert::AreEqual(ShortcutErrorType::ShortcutOneActionKey, result.first);
                    Assert::AreEqual(BufferValidationHelpers::DropDownAction::NoAction, result.second);
                });
            }

            // Test if the ValidateShortcutBufferElement method returns WinL error on setting a drop down to Win or L on a column resulting in Win+L
            TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnWinLError_OnSettingDropDownToWinOrLOnColumnResultingInWinL)
            {
                std::vector<ValidateShortcutBufferElementArgs> testCases;
                // Case 1: Validate the element when selecting L ('L') on second dropdown of first column of LWin+Empty shortcut
                testCases.push_back({ 0, 0, 1, { VK_LWIN, 'L' }, L"", false, { { Shortcut{ VK_LWIN }, Shortcut() }, L"" } });
                // Case 2: Validate the element when selecting L ('L') on second dropdown of second column of LWin+Empty shortcut
                testCases.push_back({ 0, 1, 1, { VK_LWIN, 'L' }, L"", false, { { Shortcut(), Shortcut{ VK_LWIN } }, L"" } });
                // Case 3: Validate the element when selecting L ('L') on second dropdown of second column of hybrid LWin+Empty shortcut
                testCases.push_back({ 0, 1, 1, { VK_LWIN, 'L' }, L"", true, { { Shortcut(), Shortcut{ VK_LWIN } }, L"" } });
                // Case 4: Validate the element when selecting L ('L') on second dropdown of first column of Win+Empty shortcut
                testCases.push_back({ 0, 0, 1, { CommonSharedConstants::VK_WIN_BOTH, 'L' }, L"", false, { { Shortcut{ CommonSharedConstants::VK_WIN_BOTH }, Shortcut() }, L"" } });
                // Case 5: Validate the element when selecting L ('L') on second dropdown of second column of Win+Empty shortcut
                testCases.push_back({ 0, 1, 1, { CommonSharedConstants::VK_WIN_BOTH, 'L' }, L"", false, { { Shortcut(), Shortcut{ CommonSharedConstants::VK_WIN_BOTH } }, L"" } });
                // Case 6: Validate the element when selecting L ('L') on second dropdown of second column of hybrid Win+Empty shortcut
                testCases.push_back({ 0, 1, 1, { CommonSharedConstants::VK_WIN_BOTH, 'L' }, L"", true, { { Shortcut(), Shortcut{ CommonSharedConstants::VK_WIN_BOTH } }, L"" } });
                // Case 7: Validate the element when selecting LWin (VK_LWIN) on first dropdown of first column of Empty+L shortcut
                testCases.push_back({ 0, 0, 0, { VK_LWIN, 'L' }, L"", false, { { Shortcut{ 'L' }, Shortcut() }, L"" } });
                // Case 8: Validate the element when selecting LWin (VK_LWIN) on first dropdown of second column of Empty+L shortcut
                testCases.push_back({ 0, 1, 0, { VK_LWIN, 'L' }, L"", false, { { Shortcut(), Shortcut{ 'L' } }, L"" } });
                // Case 9: Validate the element when selecting LWin (VK_LWIN) on first dropdown of second column of hybrid Empty+L shortcut
                testCases.push_back({ 0, 1, 0, { VK_LWIN, 'L' }, L"", true, { { Shortcut(), Shortcut{ 'L' } }, L"" } });

                RunTestCases(testCases, [this](const ValidateShortcutBufferElementArgs& testCase) {
                    // Arrange
                    RemapBuffer remapBuffer;
                    remapBuffer.push_back(testCase.bufferRow);

                    // Act
                    std::pair<ShortcutErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedCodesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, remapBuffer, true);

                    // Assert that the element is invalid
                    Assert::AreEqual(ShortcutErrorType::WinL, result.first);
                });
            }

            // Test if the ValidateShortcutBufferElement method returns WinL error on setting a drop down to null or none on a column resulting in Win+L
            TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnWinLError_OnSettingDropDownToNullOrNoneOnColumnResultingInWinL)
            {
                std::vector<ValidateShortcutBufferElementArgs> testCases;
                // Case 1: Validate the element when selecting Null (VK_INVALID) on second dropdown of first column of LWin + Ctrl + L shortcut
                testCases.push_back({ 0, 0, 2, { VK_LWIN, VK_INVALID, 'L' }, L"", false, { { Shortcut{ VK_LWIN, VK_CONTROL, 'L' }, Shortcut() }, L"" } });
                // Case 2: Validate the element when selecting Null (VK_INVALID) on second dropdown of second column of LWin + Ctrl + L shortcut
                testCases.push_back({ 0, 1, 2, { VK_LWIN, VK_INVALID, 'L' }, L"", false, { { Shortcut(), Shortcut{ VK_LWIN, VK_CONTROL, 'L' } }, L"" } });
                // Case 3: Validate the element when selecting Null (VK_INVALID) on second dropdown of second column of hybrid LWin + Ctrl + L shortcut
                testCases.push_back({ 0, 1, 2, { VK_LWIN, VK_INVALID, 'L' }, L"", true, { { Shortcut(), Shortcut{ VK_LWIN, VK_CONTROL, 'L' } }, L"" } });
                // Case 4: Validate the element when selecting None (VK_NULL) on second dropdown of first column of LWin + Ctrl + L shortcut
                testCases.push_back({ 0, 0, 2, { VK_LWIN, VK_NULL, 'L' }, L"", false, { { Shortcut{ VK_LWIN, VK_CONTROL, 'L' }, Shortcut() }, L"" } });
                // Case 5: Validate the element when selecting None (VK_NULL) on second dropdown of second column of LWin + Ctrl + L shortcut
                testCases.push_back({ 0, 1, 2, { VK_LWIN, VK_NULL, 'L' }, L"", false, { { Shortcut(), Shortcut{ VK_LWIN, VK_CONTROL, 'L' } }, L"" } });
                // Case 6: Validate the element when selecting None (VK_NULL) on second dropdown of second column of hybrid LWin + Ctrl + L shortcut
                testCases.push_back({ 0, 1, 2, { VK_LWIN, VK_NULL, 'L' }, L"", true, { { Shortcut(), Shortcut{ VK_LWIN, VK_CONTROL, 'L' } }, L"" } });

                RunTestCases(testCases, [this](const ValidateShortcutBufferElementArgs& testCase) {
                    // Arrange
                    RemapBuffer remapBuffer;
                    remapBuffer.push_back(testCase.bufferRow);

                    // Act
                    std::pair<ShortcutErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedCodesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, remapBuffer, true);

                    // Assert that the element is invalid
                    Assert::AreEqual(ShortcutErrorType::WinL, result.first);
                });
            }

            // Test if the ValidateShortcutBufferElement method returns CtrlAltDel error on setting a drop down to Ctrl, Alt or Del on a column resulting in Ctrl+Alt+Del
            TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnCtrlAltDelError_OnSettingDropDownToCtrlAltOrDelOnColumnResultingInCtrlAltDel)
            {
                std::vector<ValidateShortcutBufferElementArgs> testCases;
                // Case 1: Validate the element when selecting Del (VK_DELETE) on third dropdown of first column of Ctrl+Alt+Empty shortcut
                testCases.push_back({ 0, 0, 2, { VK_CONTROL, VK_MENU, VK_DELETE }, L"", false, { { Shortcut{ VK_CONTROL, VK_MENU }, Shortcut() }, L"" } });
                // Case 2: Validate the element when selecting Del (VK_DELETE) on third dropdown of second column of Ctrl+Alt+Empty shortcut
                testCases.push_back({ 0, 1, 2, { VK_CONTROL, VK_MENU, VK_DELETE }, L"", false, { { Shortcut(), Shortcut{ VK_CONTROL, VK_MENU } }, L"" } });
                // Case 3: Validate the element when selecting Del (VK_DELETE) on third dropdown of second column of hybrid Ctrl+Alt+Empty shortcut
                testCases.push_back({ 0, 1, 2, { VK_CONTROL, VK_MENU, VK_DELETE }, L"", true, { { Shortcut(), Shortcut{ VK_CONTROL, VK_MENU } }, L"" } });
                // Case 4: Validate the element when selecting Alt (VK_MENU) on second dropdown of first column of Ctrl+Empty+Del shortcut
                testCases.push_back({ 0, 0, 1, { VK_CONTROL, VK_MENU, VK_DELETE }, L"", false, { { Shortcut{ VK_CONTROL, VK_DELETE }, Shortcut() }, L"" } });
                // Case 5: Validate the element when selecting Alt (VK_MENU) on second dropdown of second column of Ctrl+Empty+Del shortcut
                testCases.push_back({ 0, 1, 1, { VK_CONTROL, VK_MENU, VK_DELETE }, L"", false, { { Shortcut(), Shortcut{ VK_CONTROL, VK_DELETE } }, L"" } });
                // Case 6: Validate the element when selecting Alt (VK_MENU) on second dropdown of second column of hybrid Ctrl+Empty+Del shortcut
                testCases.push_back({ 0, 1, 1, { VK_CONTROL, VK_MENU, VK_DELETE }, L"", true, { { Shortcut(), Shortcut{ VK_CONTROL, VK_DELETE } }, L"" } });

                RunTestCases(testCases, [this](const ValidateShortcutBufferElementArgs& testCase) {
                    // Arrange
                    RemapBuffer remapBuffer;
                    remapBuffer.push_back(testCase.bufferRow);

                    // Act
                    std::pair<ShortcutErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedCodesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, remapBuffer, true);

                    // Assert that the element is invalid
                    Assert::AreEqual(ShortcutErrorType::CtrlAltDel, result.first);
                });
            }

            // Test if the ValidateShortcutBufferElement method returns MapToSameKey error on setting hybrid second column to match first column in a remap keys table
            TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnMapToSameKeyError_OnSettingHybridSecondColumnToFirstColumnInKeyTable)
            {
                std::vector<ValidateShortcutBufferElementArgs> testCases;
                // Case 1: Validate the element when selecting A ('A') on first dropdown of empty hybrid second column
                testCases.push_back({ 0, 1, 0, { 'A', VK_INVALID, VK_INVALID }, L"", true, { { 'A', VK_NULL }, L"" } });
                // Case 2: Validate the element when selecting A ('A') on second dropdown of empty hybrid second column
                testCases.push_back({ 0, 1, 1, { VK_INVALID, 'A', VK_INVALID }, L"", true, { { 'A', VK_NULL }, L"" } });
                // Case 3: Validate the element when selecting A ('A') on third dropdown of empty hybrid second column
                testCases.push_back({ 0, 1, 2, { VK_INVALID, VK_INVALID, 'A' }, L"", true, { { 'A', VK_NULL }, L"" } });
                // Case 4: Validate the element when selecting A ('A') on first dropdown of hybrid second column with key
                testCases.push_back({ 0, 1, 0, { 'A' }, L"", true, { { 'A', 'C' }, L"" } });
                // Case 5: Validate the element when selecting Null (VK_INVALID) on first dropdown of hybrid second column with shortcut
                testCases.push_back({ 0, 1, 0, { VK_INVALID, 'A' }, L"", true, { { 'A', Shortcut{ VK_CONTROL, 'A' } }, L"" } });
                // Case 6: Validate the element when selecting None (VK_NULL) on first dropdown of hybrid second column with shortcut
                testCases.push_back({ 0, 1, 0, { VK_NULL, 'A' }, L"", true, { { 'A', Shortcut{ VK_CONTROL, 'A' } }, L"" } });
                // Case 7: Validate the element when selecting Null (VK_INVALID) on second dropdown of hybrid second column with shortcut
                testCases.push_back({ 0, 1, 1, { VK_INVALID, VK_CONTROL }, L"", true, { { VK_CONTROL, Shortcut{ VK_CONTROL, 'A' } }, L"" } });
                // Case 8: Validate the element when selecting None (VK_NULL) on second dropdown of hybrid second column with shortcut
                testCases.push_back({ 0, 1, 1, { VK_NULL, VK_CONTROL }, L"", true, { { VK_CONTROL, Shortcut{ VK_CONTROL, 'A' } }, L"" } });

                RunTestCases(testCases, [this](const ValidateShortcutBufferElementArgs& testCase) {
                    // Arrange
                    RemapBuffer remapBuffer;
                    remapBuffer.push_back(testCase.bufferRow);

                    // Act
                    std::pair<ShortcutErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedCodesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, remapBuffer, true);

                    // Assert that the element is invalid
                    Assert::AreEqual(ShortcutErrorType::MapToSameKey, result.first);
                });
            }

            // Test if the ValidateShortcutBufferElement method returns MapToSameShortcut error on setting one column to match the other and both are valid 3 key shortcuts
            TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnMapToSameShortcutError_OnSettingOneColumnToTheOtherAndBothAreValid3KeyShortcuts)
            {
                std::vector<ValidateShortcutBufferElementArgs> testCases;
                // Case 1 : Validate the element when selecting C ('C') on third dropdown of first column with Ctrl+Shift+Empty
                testCases.push_back({ 0, 0, 2, { VK_CONTROL, VK_SHIFT, 'C' }, L"", false, { { Shortcut{ VK_CONTROL, VK_SHIFT }, Shortcut{ VK_CONTROL, VK_SHIFT, 'C' } }, L"" } });
                // Case 2 : Validate the element when selecting C ('C') on third dropdown of second column with Ctrl+Shift+Empty
                testCases.push_back({ 0, 1, 2, { VK_CONTROL, VK_SHIFT, 'C' }, L"", false, { { Shortcut{ VK_CONTROL, VK_SHIFT, 'C' }, Shortcut{ VK_CONTROL, VK_SHIFT } }, L"" } });
                // Case 3 : Validate the element when selecting C ('C') on third dropdown of second column with hybrid Ctrl+Shift+Empty
                testCases.push_back({ 0, 1, 2, { VK_CONTROL, VK_SHIFT, 'C' }, L"", true, { { Shortcut{ VK_CONTROL, VK_SHIFT, 'C' }, Shortcut{ VK_CONTROL, VK_SHIFT } }, L"" } });
                // Case 4 : Validate the element when selecting Shift (VK_SHIFT) on second dropdown of first column with Ctrl+Empty+C
                testCases.push_back({ 0, 0, 1, { VK_CONTROL, VK_SHIFT, 'C' }, L"", false, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut{ VK_CONTROL, VK_SHIFT, 'C' } }, L"" } });
                // Case 5 : Validate the element when selecting Shift (VK_SHIFT) on second dropdown of second column with Ctrl+Empty+C
                testCases.push_back({ 0, 1, 1, { VK_CONTROL, VK_SHIFT, 'C' }, L"", false, { { Shortcut{ VK_CONTROL, VK_SHIFT, 'C' }, Shortcut{ VK_CONTROL, 'C' } }, L"" } });
                // Case 6 : Validate the element when selecting Shift (VK_SHIFT) on second dropdown of second column with hybrid Ctrl+Empty+C
                testCases.push_back({ 0, 1, 1, { VK_CONTROL, VK_SHIFT, 'C' }, L"", true, { { Shortcut{ VK_CONTROL, VK_SHIFT, 'C' }, Shortcut{ VK_CONTROL, 'C' } }, L"" } });
                // Case 7 : Validate the element when selecting Shift (VK_SHIFT) on first dropdown of first column with Empty+Ctrl+C
                testCases.push_back({ 0, 0, 0, { VK_SHIFT, VK_CONTROL, 'C' }, L"", false, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut{ VK_CONTROL, VK_SHIFT, 'C' } }, L"" } });
                // Case 8 : Validate the element when selecting Shift (VK_SHIFT) on first dropdown of second column with Empty+Ctrl+C
                testCases.push_back({ 0, 1, 0, { VK_SHIFT, VK_CONTROL, 'C' }, L"", false, { { Shortcut{ VK_CONTROL, VK_SHIFT, 'C' }, Shortcut{ VK_CONTROL, 'C' } }, L"" } });
                // Case 9 : Validate the element when selecting Shift (VK_SHIFT) on first dropdown of second column with hybrid Empty+Ctrl+C
                testCases.push_back({ 0, 1, 0, { VK_SHIFT, VK_CONTROL, 'C' }, L"", true, { { Shortcut{ VK_CONTROL, VK_SHIFT, 'C' }, Shortcut{ VK_CONTROL, 'C' } }, L"" } });

                RunTestCases(testCases, [this](const ValidateShortcutBufferElementArgs& testCase) {
                    // Arrange
                    RemapBuffer remapBuffer;
                    remapBuffer.push_back(testCase.bufferRow);

                    // Act
                    std::pair<ShortcutErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedCodesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, remapBuffer, true);

                    // Assert that the element is invalid
                    Assert::AreEqual(ShortcutErrorType::MapToSameShortcut, result.first);
                });
            }

            // Test if the ValidateShortcutBufferElement method returns MapToSameShortcut error on setting one column to match the other and both are valid 2 key shortcuts
            TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnMapToSameShortcutError_OnSettingOneColumnToTheOtherAndBothAreValid2KeyShortcuts)
            {
                std::vector<ValidateShortcutBufferElementArgs> testCases;
                // Case 1 : Validate the element when selecting C ('C') on third dropdown of first column with Ctrl+Empty+Empty
                testCases.push_back({ 0, 0, 2, { VK_CONTROL, VK_INVALID, 'C' }, L"", false, { { Shortcut{ VK_CONTROL }, Shortcut{ VK_CONTROL, 'C' } }, L"" } });
                // Case 2 : Validate the element when selecting C ('C') on third dropdown of second column with Ctrl+Empty+Empty
                testCases.push_back({ 0, 1, 2, { VK_CONTROL, VK_INVALID, 'C' }, L"", false, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut{ VK_CONTROL } }, L"" } });
                // Case 3 : Validate the element when selecting C ('C') on third dropdown of second column with hybrid Ctrl+Empty+Empty
                testCases.push_back({ 0, 1, 2, { VK_CONTROL, VK_INVALID, 'C' }, L"", true, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut{ VK_CONTROL } }, L"" } });
                // Case 4 : Validate the element when selecting C ('C') on second dropdown of first column with Ctrl+Empty+Empty
                testCases.push_back({ 0, 0, 1, { VK_CONTROL, 'C', VK_INVALID }, L"", false, { { Shortcut{ VK_CONTROL }, Shortcut{ VK_CONTROL, 'C' } }, L"" } });
                // Case 5 : Validate the element when selecting C ('C') on second dropdown of second column with Ctrl+Empty+Empty
                testCases.push_back({ 0, 1, 1, { VK_CONTROL, 'C', VK_INVALID }, L"", false, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut{ VK_CONTROL } }, L"" } });
                // Case 6 : Validate the element when selecting C ('C') on second dropdown of second column with hybrid Ctrl+Empty+Empty
                testCases.push_back({ 0, 1, 1, { VK_CONTROL, 'C', VK_INVALID }, L"", true, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut{ VK_CONTROL } }, L"" } });
                // Case 7 : Validate the element when selecting Ctrl (VK_CONTROL) on first dropdown of first column with Empty+Empty+C
                testCases.push_back({ 0, 0, 0, { VK_CONTROL, VK_INVALID, 'C' }, L"", false, { { Shortcut{ 'C' }, Shortcut{ VK_CONTROL, 'C' } }, L"" } });
                // Case 8 : Validate the element when selecting Ctrl (VK_CONTROL) on first dropdown of second column with Empty+Empty+C
                testCases.push_back({ 0, 1, 0, { VK_CONTROL, VK_INVALID, 'C' }, L"", false, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut{ 'C' } }, L"" } });
                // Case 9 : Validate the element when selecting Ctrl (VK_CONTROL) on first dropdown of second column with hybrid Empty+Empty+C
                testCases.push_back({ 0, 1, 0, { VK_CONTROL, VK_INVALID, 'C' }, L"", true, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut{ 'C' } }, L"" } });
                // Case 10 : Validate the element when selecting Ctrl (VK_CONTROL) on second dropdown of first column with Empty+Empty+C
                testCases.push_back({ 0, 0, 1, { VK_INVALID, VK_CONTROL, 'C' }, L"", false, { { Shortcut{ 'C' }, Shortcut{ VK_CONTROL, 'C' } }, L"" } });
                // Case 11 : Validate the element when selecting Ctrl (VK_CONTROL) on second dropdown of second column with Empty+Empty+C
                testCases.push_back({ 0, 1, 1, { VK_INVALID, VK_CONTROL, 'C' }, L"", false, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut{ 'C' } }, L"" } });
                // Case 12 : Validate the element when selecting Ctrl (VK_CONTROL) on second dropdown of second column with hybrid Empty+Empty+C
                testCases.push_back({ 0, 1, 1, { VK_INVALID, VK_CONTROL, 'C' }, L"", true, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut{ 'C' } }, L"" } });
                // Case 13 : Validate the element when selecting C ('C') on second dropdown of first column with Ctrl+A
                testCases.push_back({ 0, 0, 1, { VK_CONTROL, 'C' }, L"", false, { { Shortcut{ VK_CONTROL, 'A' }, Shortcut{ VK_CONTROL, 'C' } }, L"" } });
                // Case 14 : Validate the element when selecting C ('C') on second dropdown of second column with Ctrl+A
                testCases.push_back({ 0, 1, 1, { VK_CONTROL, 'C' }, L"", false, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut{ VK_CONTROL, 'A' } }, L"" } });
                // Case 15 : Validate the element when selecting C ('C') on second dropdown of second column with hybrid Ctrl+A
                testCases.push_back({ 0, 1, 1, { VK_CONTROL, 'C' }, L"", true, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut{ VK_CONTROL, 'A' } }, L"" } });
                // Case 16 : Validate the element when selecting Ctrl (VK_CONTROL) on first dropdown of first column with Alt+C
                testCases.push_back({ 0, 0, 1, { VK_CONTROL, 'C' }, L"", false, { { Shortcut{ VK_MENU, 'C' }, Shortcut{ VK_CONTROL, 'C' } }, L"" } });
                // Case 17 : Validate the element when selecting Ctrl (VK_CONTROL) on first dropdown of second column with Alt+C
                testCases.push_back({ 0, 1, 1, { VK_CONTROL, 'C' }, L"", false, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut{ VK_MENU, 'C' } }, L"" } });
                // Case 18 : Validate the element when selecting Ctrl (VK_CONTROL) on first dropdown of second column with hybrid Alt+C
                testCases.push_back({ 0, 1, 1, { VK_CONTROL, 'C' }, L"", true, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut{ VK_MENU, 'C' } }, L"" } });
                // Case 19 : Validate the element when selecting Null (VK_INVALID)  on second dropdown of first column with Ctrl+Shift+C
                testCases.push_back({ 0, 0, 1, { VK_CONTROL, VK_INVALID, 'C' }, L"", false, { { Shortcut{ VK_CONTROL, VK_SHIFT, 'C' }, Shortcut{ VK_CONTROL, 'C' } }, L"" } });
                // Case 20 : Validate the element when selecting Null (VK_INVALID)  on second dropdown of second column with Ctrl+Shift+C
                testCases.push_back({ 0, 1, 1, { VK_CONTROL, VK_INVALID, 'C' }, L"", false, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut{ VK_CONTROL, VK_SHIFT, 'C' } }, L"" } });
                // Case 21 : Validate the element when selecting Null (VK_INVALID)  on second dropdown of second column with hybrid Ctrl+Shift+C
                testCases.push_back({ 0, 1, 1, { VK_CONTROL, VK_INVALID, 'C' }, L"", true, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut{ VK_CONTROL, VK_SHIFT, 'C' } }, L"" } });
                // Case 22 : Validate the element when selecting None (VK_NULL) on second dropdown of first column with Ctrl+Shift+C
                testCases.push_back({ 0, 0, 1, { VK_CONTROL, VK_NULL, 'C' }, L"", false, { { Shortcut{ VK_CONTROL, VK_SHIFT, 'C' }, Shortcut{ VK_CONTROL, 'C' } }, L"" } });
                // Case 23 : Validate the element when selecting None (VK_NULL)  on second dropdown of second column with Ctrl+Shift+C
                testCases.push_back({ 0, 1, 1, { VK_CONTROL, VK_NULL, 'C' }, L"", false, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut{ VK_CONTROL, VK_SHIFT, 'C' } }, L"" } });
                // Case 24 : Validate the element when selecting None (VK_NULL)  on second dropdown of second column with hybrid Ctrl+Shift+C
                testCases.push_back({ 0, 1, 1, { VK_CONTROL, VK_NULL, 'C' }, L"", true, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut{ VK_CONTROL, VK_SHIFT, 'C' } }, L"" } });
                // Case 25 : Validate the element when selecting Null (VK_INVALID)  on first dropdown of first column with Shift+Ctrl+C
                testCases.push_back({ 0, 0, 0, { VK_INVALID, VK_CONTROL, 'C' }, L"", false, { { Shortcut{ VK_SHIFT, VK_CONTROL, 'C' }, Shortcut{ VK_CONTROL, 'C' } }, L"" } });
                // Case 26 : Validate the element when selecting Null (VK_INVALID)  on first dropdown of second column with Shift+Ctrl+C
                testCases.push_back({ 0, 1, 0, { VK_INVALID, VK_CONTROL, 'C' }, L"", false, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut{ VK_SHIFT, VK_CONTROL, 'C' } }, L"" } });
                // Case 27 : Validate the element when selecting Null (VK_INVALID)  on first dropdown of second column with hybrid Shift+Ctrl+C
                testCases.push_back({ 0, 1, 0, { VK_INVALID, VK_CONTROL, 'C' }, L"", true, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut{ VK_SHIFT, VK_CONTROL, 'C' } }, L"" } });
                // Case 28 : Validate the element when selecting None (VK_NULL) on first dropdown of first column with Shift+Ctrl+C
                testCases.push_back({ 0, 0, 0, { VK_NULL, VK_CONTROL, 'C' }, L"", false, { { Shortcut{ VK_SHIFT, VK_CONTROL, 'C' }, Shortcut{ VK_CONTROL, 'C' } }, L"" } });
                // Case 29 : Validate the element when selecting None (VK_NULL) on first dropdown of second column with Shift+Ctrl+C
                testCases.push_back({ 0, 1, 0, { VK_NULL, VK_CONTROL, 'C' }, L"", false, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut{ VK_SHIFT, VK_CONTROL, 'C' } }, L"" } });
                // Case 30 : Validate the element when selecting None (VK_NULL) on first dropdown of second column with hybrid Shift+Ctrl+C
                testCases.push_back({ 0, 1, 0, { VK_NULL, VK_CONTROL, 'C' }, L"", true, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut{ VK_SHIFT, VK_CONTROL, 'C' } }, L"" } });

                RunTestCases(testCases, [this](const ValidateShortcutBufferElementArgs& testCase) {
                    // Arrange
                    RemapBuffer remapBuffer;
                    remapBuffer.push_back(testCase.bufferRow);

                    // Act
                    std::pair<ShortcutErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedCodesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, remapBuffer, true);

                    // Assert that the element is invalid
                    Assert::AreEqual(ShortcutErrorType::MapToSameShortcut, result.first);
                });
            }

            // Test if the ValidateShortcutBufferElement method returns SameShortcutPreviouslyMapped error on setting first column to match first column in another row with same target app and both are valid 3 key shortcuts
            TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnSameShortcutPreviouslyMappedError_OnSettingFirstColumnToFirstColumnInAnotherRowWithSameTargetAppAndBothAreValid3KeyShortcuts)
            {
                std::vector<ValidateShortcutBufferElementArgs> testCases;
                // Case 1 : Validate the element when selecting C ('C') on third dropdown of first column with Ctrl+Shift+Empty
                testCases.push_back({ 1, 0, 2, { VK_CONTROL, VK_SHIFT, 'C' }, L"", false, { { Shortcut{ VK_CONTROL, VK_SHIFT }, Shortcut() }, L"" } });
                // Case 2 : Validate the element when selecting Shift (VK_SHIFT) on second dropdown of first column with Ctrl+Empty+C
                testCases.push_back({ 1, 0, 1, { VK_CONTROL, VK_SHIFT, 'C' }, L"", false, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut() }, L"" } });
                // Case 3 : Validate the element when selecting Shift (VK_SHIFT) on first dropdown of first column with Empty+Ctrl+C
                testCases.push_back({ 1, 0, 0, { VK_SHIFT, VK_CONTROL, 'C' }, L"", false, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut() }, L"" } });

                RunTestCases(testCases, [this](const ValidateShortcutBufferElementArgs& testCase) {
                    // Arrange
                    RemapBuffer remapBuffer;
                    // Ctrl+Shift+C remapped
                    remapBuffer.push_back({ { Shortcut{ VK_CONTROL, VK_SHIFT, 'C' }, Shortcut() }, L"" });
                    remapBuffer.push_back(testCase.bufferRow);

                    // Act
                    std::pair<ShortcutErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedCodesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, remapBuffer, true);

                    // Assert that the element is invalid
                    Assert::AreEqual(ShortcutErrorType::SameShortcutPreviouslyMapped, result.first);
                });
            }

            // Test if the ValidateShortcutBufferElement method returns no error on setting first column to match first column in another row with different target app and both are valid 3 key shortcuts
            TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnNoError_OnSettingFirstColumnToFirstColumnInAnotherRowWithDifferentTargetAppAndBothAreValid3KeyShortcuts)
            {
                std::vector<ValidateShortcutBufferElementArgs> testCases;
                // Case 1 : Validate the element when selecting C ('C') on third dropdown of first column with Ctrl+Shift+Empty for testApp2
                testCases.push_back({ 1, 0, 2, { VK_CONTROL, VK_SHIFT, 'C' }, testApp2, false, { { Shortcut{ VK_CONTROL, VK_SHIFT }, Shortcut() }, testApp2 } });
                // Case 2 : Validate the element when selecting Shift (VK_SHIFT) on second dropdown of first column with Ctrl+Empty+C for testApp2
                testCases.push_back({ 1, 0, 1, { VK_CONTROL, VK_SHIFT, 'C' }, testApp2, false, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut() }, testApp2 } });
                // Case 3 : Validate the element when selecting Shift (VK_SHIFT) on first dropdown of first column with Empty+Ctrl+C for testApp2
                testCases.push_back({ 1, 0, 0, { VK_SHIFT, VK_CONTROL, 'C' }, testApp2, false, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut() }, testApp2 } });

                RunTestCases(testCases, [this](const ValidateShortcutBufferElementArgs& testCase) {
                    // Arrange
                    RemapBuffer remapBuffer;
                    // Ctrl+Shift+C remapped for testApp1
                    remapBuffer.push_back({ { Shortcut{ VK_CONTROL, VK_SHIFT, 'C' }, Shortcut() }, testApp1 });
                    remapBuffer.push_back(testCase.bufferRow);

                    // Act
                    std::pair<ShortcutErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedCodesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, remapBuffer, true);

                    // Assert that the element is valid
                    Assert::AreEqual(ShortcutErrorType::NoError, result.first);
                });
            }

            // Test if the ValidateShortcutBufferElement method returns ConflictingModifierShortcut error on setting first column to conflict with first column in another row with same target app and both are valid 3 key shortcuts
            TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnConflictingModifierShortcutError_OnSettingFirstColumnToConflictWithFirstColumnInAnotherRowWithSameTargetAppAndBothAreValid3KeyShortcuts)
            {
                std::vector<ValidateShortcutBufferElementArgs> testCases;
                // Case 1 : Validate the element when selecting C ('C') on third dropdown of first column with LCtrl+Shift+Empty
                testCases.push_back({ 1, 0, 2, { VK_LCONTROL, VK_SHIFT, 'C' }, L"", false, { { Shortcut{ VK_LCONTROL, VK_SHIFT }, Shortcut() }, L"" } });
                // Case 2 : Validate the element when selecting Shift (VK_SHIFT) on second dropdown of first column with LCtrl+Empty+C
                testCases.push_back({ 1, 0, 1, { VK_LCONTROL, VK_SHIFT, 'C' }, L"", false, { { Shortcut{ VK_LCONTROL, 'C' }, Shortcut() }, L"" } });
                // Case 3 : Validate the element when selecting LShift (VK_LSHIFT) on first dropdown of first column with Empty+Ctrl+C
                testCases.push_back({ 1, 0, 0, { VK_LSHIFT, VK_CONTROL, 'C' }, L"", false, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut() }, L"" } });

                RunTestCases(testCases, [this](const ValidateShortcutBufferElementArgs& testCase) {
                    // Arrange
                    RemapBuffer remapBuffer;
                    // Ctrl+Shift+C remapped
                    remapBuffer.push_back({ { Shortcut{ VK_CONTROL, VK_SHIFT, 'C' }, Shortcut() }, L"" });
                    remapBuffer.push_back(testCase.bufferRow);

                    // Act
                    std::pair<ShortcutErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedCodesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, remapBuffer, true);

                    // Assert that the element is invalid
                    Assert::AreEqual(ShortcutErrorType::ConflictingModifierShortcut, result.first);
                });
            }

            // Test if the ValidateShortcutBufferElement method returns no error on setting first column to conflict with first column in another row with different target app and both are valid 3 key shortcuts
            TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnNoError_OnSettingFirstColumnToConflictWithFirstColumnInAnotherRowWithDifferentTargetAppAndBothAreValid3KeyShortcuts)
            {
                std::vector<ValidateShortcutBufferElementArgs> testCases;
                // Case 1 : Validate the element when selecting C ('C') on third dropdown of first column with LCtrl+Shift+Empty for testApp2
                testCases.push_back({ 1, 0, 2, { VK_LCONTROL, VK_SHIFT, 'C' }, testApp2, false, { { Shortcut{ VK_LCONTROL, VK_SHIFT }, Shortcut() }, testApp2 } });
                // Case 2 : Validate the element when selecting Shift (VK_SHIFT) on second dropdown of first column with LCtrl+Empty+C for testApp2
                testCases.push_back({ 1, 0, 1, { VK_LCONTROL, VK_SHIFT, 'C' }, testApp2, false, { { Shortcut{ VK_LCONTROL, 'C' }, Shortcut() }, testApp2 } });
                // Case 3 : Validate the element when selecting LShift (VK_LSHIFT) on first dropdown of first column with Empty+Ctrl+C for testApp2
                testCases.push_back({ 1, 0, 0, { VK_LSHIFT, VK_CONTROL, 'C' }, testApp2, false, { { Shortcut{ VK_CONTROL, 'C' }, Shortcut() }, testApp2 } });

                RunTestCases(testCases, [this](const ValidateShortcutBufferElementArgs& testCase) {
                    // Arrange
                    RemapBuffer remapBuffer;
                    // Ctrl+Shift+C remapped for testApp1
                    remapBuffer.push_back({ { Shortcut{ VK_CONTROL, VK_SHIFT, 'C' }, Shortcut() }, testApp1 });
                    remapBuffer.push_back(testCase.bufferRow);

                    // Act
                    std::pair<ShortcutErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedCodesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, remapBuffer, true);

                    // Assert that the element is valid
                    Assert::AreEqual(ShortcutErrorType::NoError, result.first);
                });
            }

            // Test if the ValidateShortcutBufferElement method returns SameShortcutPreviouslyMapped error on setting first column to match first column in another row with same target app and both are valid 2 key shortcuts
            TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnSameShortcutPreviouslyMappedError_OnSettingFirstColumnToFirstColumnInAnotherRowWithSameTargetAppAndBothAreValid2KeyShortcuts)
            {
                std::vector<ValidateShortcutBufferElementArgs> testCases;
                // Case 1 : Validate the element when selecting C ('C') on second dropdown of first column with Ctrl+Empty
                testCases.push_back({ 1, 0, 1, { VK_CONTROL, 'C' }, L"", false, { { Shortcut{ VK_CONTROL }, Shortcut() }, L"" } });
                // Case 2 : Validate the element when selecting C ('C') on third dropdown of first column with Ctrl+Empty+Empty
                testCases.push_back({ 1, 0, 2, { VK_CONTROL, VK_INVALID, 'C' }, L"", false, { { Shortcut{ VK_CONTROL }, Shortcut() }, L"" } });
                // Case 3 : Validate the element when selecting C ('C') on second dropdown of first column with Ctrl+Empty+Empty
                testCases.push_back({ 1, 0, 1, { VK_CONTROL, 'C', VK_INVALID }, L"", false, { { Shortcut{ VK_CONTROL }, Shortcut() }, L"" } });
                // Case 4 : Validate the element when selecting Ctrl (VK_CONTROL) on first dropdown of first column with Empty+C
                testCases.push_back({ 1, 0, 0, { VK_CONTROL, 'C' }, L"", false, { { Shortcut{ 'C' }, Shortcut() }, L"" } });
                // Case 5 : Validate the element when selecting Ctrl (VK_CONTROL) on first dropdown of first column with Empty+Empty+C
                testCases.push_back({ 1, 0, 0, { VK_CONTROL, VK_INVALID, 'C' }, L"", false, { { Shortcut{ 'C' }, Shortcut() }, L"" } });
                // Case 6 : Validate the element when selecting Ctrl (VK_CONTROL) on second dropdown of first column with Empty+Empty+C
                testCases.push_back({ 1, 0, 1, { VK_INVALID, VK_CONTROL, 'C' }, L"", false, { { Shortcut{ 'C' }, Shortcut() }, L"" } });
                // Case 7 : Validate the element when selecting Null (VK_INVALID) on second dropdown of first column with Ctrl+Shift+C
                testCases.push_back({ 1, 0, 1, { VK_CONTROL, VK_INVALID, 'C' }, L"", false, { { Shortcut{ VK_CONTROL, VK_SHIFT, 'C' }, Shortcut() }, L"" } });
                // Case 8 : Validate the element when selecting Null (VK_INVALID) on first dropdown of first column with Shift+Ctrl+C
                testCases.push_back({ 1, 0, 0, { VK_INVALID, VK_CONTROL, 'C' }, L"", false, { { Shortcut{ VK_SHIFT, VK_CONTROL, 'C' }, Shortcut() }, L"" } });
                // Case 9 : Validate the element when selecting None (VK_NULL) on second dropdown of first column with Ctrl+Shift+C
                testCases.push_back({ 1, 0, 1, { VK_CONTROL, VK_NULL, 'C' }, L"", false, { { Shortcut{ VK_CONTROL, VK_SHIFT, 'C' }, Shortcut() }, L"" } });
                // Case 10 : Validate the element when selecting None (VK_NULL) on first dropdown of first column with Shift+Ctrl+C
                testCases.push_back({ 1, 0, 0, { VK_NULL, VK_CONTROL, 'C' }, L"", false, { { Shortcut{ VK_SHIFT, VK_CONTROL, 'C' }, Shortcut() }, L"" } });

                RunTestCases(testCases, [this](const ValidateShortcutBufferElementArgs& testCase) {
                    // Arrange
                    RemapBuffer remapBuffer;
                    // Ctrl+C remapped
                    remapBuffer.push_back({ { Shortcut{ VK_CONTROL, 'C' }, Shortcut() }, L"" });
                    remapBuffer.push_back(testCase.bufferRow);

                    // Act
                    std::pair<ShortcutErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedCodesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, remapBuffer, true);

                    // Assert that the element is invalid
                    Assert::AreEqual(ShortcutErrorType::SameShortcutPreviouslyMapped, result.first);
                });
            }

            // Test if the ValidateShortcutBufferElement method returns no error on setting first column to match first column in another row with different target app and both are valid 2 key shortcuts
            TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnNoError_OnSettingFirstColumnToFirstColumnInAnotherRowWithDifferentTargetAppAndBothAreValid2KeyShortcuts)
            {
                std::vector<ValidateShortcutBufferElementArgs> testCases;
                // Case 1 : Validate the element when selecting C ('C') on second dropdown of first column with Ctrl+Empty for testApp2
                testCases.push_back({ 1, 0, 1, { VK_CONTROL, 'C' }, testApp2, false, { { Shortcut{ VK_CONTROL }, Shortcut() }, testApp2 } });
                // Case 2 : Validate the element when selecting C ('C') on third dropdown of first column with Ctrl+Empty+Empty for testApp2
                testCases.push_back({ 1, 0, 2, { VK_CONTROL, VK_INVALID, 'C' }, testApp2, false, { { Shortcut{ VK_CONTROL }, Shortcut() }, testApp2 } });
                // Case 3 : Validate the element when selecting C ('C') on second dropdown of first column with Ctrl+Empty+Empty for testApp2
                testCases.push_back({ 1, 0, 1, { VK_CONTROL, 'C', VK_INVALID }, testApp2, false, { { Shortcut{ VK_CONTROL }, Shortcut() }, testApp2 } });
                // Case 4 : Validate the element when selecting Ctrl (VK_CONTROL) on first dropdown of first column with Empty+C for testApp2
                testCases.push_back({ 1, 0, 0, { VK_CONTROL, 'C' }, testApp2, false, { { Shortcut{ 'C' }, Shortcut() }, testApp2 } });
                // Case 5 : Validate the element when selecting Ctrl (VK_CONTROL) on first dropdown of first column with Empty+Empty+C for testApp2
                testCases.push_back({ 1, 0, 0, { VK_CONTROL, VK_INVALID, 'C' }, testApp2, false, { { Shortcut{ 'C' }, Shortcut() }, testApp2 } });
                // Case 6 : Validate the element when selecting Ctrl (VK_CONTROL) on second dropdown of first column with Empty+Empty+C for testApp2
                testCases.push_back({ 1, 0, 1, { VK_INVALID, VK_CONTROL, 'C' }, testApp2, false, { { Shortcut{ 'C' }, Shortcut() }, testApp2 } });
                // Case 7 : Validate the element when selecting Null (VK_INVALID) on second dropdown of first column with Ctrl+Shift+C for testApp2
                testCases.push_back({ 1, 0, 1, { VK_CONTROL, VK_INVALID, 'C' }, testApp2, false, { { Shortcut{ VK_CONTROL, VK_SHIFT, 'C' }, Shortcut() }, testApp2 } });
                // Case 8 : Validate the element when selecting Null (VK_INVALID) on first dropdown of first column with Shift+Ctrl+C for testApp2
                testCases.push_back({ 1, 0, 0, { VK_INVALID, VK_CONTROL, 'C' }, testApp2, false, { { Shortcut{ VK_SHIFT, VK_CONTROL, 'C' }, Shortcut() }, testApp2 } });
                // Case 9 : Validate the element when selecting None (VK_NULL) on second dropdown of first column with Ctrl+Shift+C for testApp2
                testCases.push_back({ 1, 0, 1, { VK_CONTROL, VK_NULL, 'C' }, testApp2, false, { { Shortcut{ VK_CONTROL, VK_SHIFT, 'C' }, Shortcut() }, testApp2 } });
                // Case 10 : Validate the element when selecting None (VK_NULL) on first dropdown of first column with Shift+Ctrl+C for testApp2
                testCases.push_back({ 1, 0, 0, { VK_NULL, VK_CONTROL, 'C' }, testApp2, false, { { Shortcut{ VK_SHIFT, VK_CONTROL, 'C' }, Shortcut() }, testApp2 } });

                RunTestCases(testCases, [this](const ValidateShortcutBufferElementArgs& testCase) {
                    // Arrange
                    RemapBuffer remapBuffer;
                    // Ctrl+C remapped for testApp1
                    remapBuffer.push_back({ { Shortcut{ VK_CONTROL, 'C' }, Shortcut() }, testApp1 });
                    remapBuffer.push_back(testCase.bufferRow);

                    // Act
                    std::pair<ShortcutErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedCodesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, remapBuffer, true);

                    // Assert that the element is valid
                    Assert::AreEqual(ShortcutErrorType::NoError, result.first);
                });
            }

            // Test if the ValidateShortcutBufferElement method returns ConflictingModifierShortcut error on setting first column to conflict with first column in another row with same target app and both are valid 2 key shortcuts
            TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnConflictingModifierShortcutError_OnSettingFirstColumnToConflictWithFirstColumnInAnotherRowWithSameTargetAppAndBothAreValid2KeyShortcuts)
            {
                std::vector<ValidateShortcutBufferElementArgs> testCases;
                // Case 1 : Validate the element when selecting C ('C') on second dropdown of first column with LCtrl+Empty
                testCases.push_back({ 1, 0, 1, { VK_LCONTROL, 'C' }, L"", false, { { Shortcut{ VK_LCONTROL }, Shortcut() }, L"" } });
                // Case 2 : Validate the element when selecting C ('C') on third dropdown of first column with LCtrl+Empty+Empty
                testCases.push_back({ 1, 0, 2, { VK_LCONTROL, VK_INVALID, 'C' }, L"", false, { { Shortcut{ VK_LCONTROL }, Shortcut() }, L"" } });
                // Case 3 : Validate the element when selecting C ('C') on second dropdown of first column with LCtrl+Empty+Empty
                testCases.push_back({ 1, 0, 1, { VK_LCONTROL, 'C', VK_INVALID }, L"", false, { { Shortcut{ VK_LCONTROL }, Shortcut() }, L"" } });
                // Case 4 : Validate the element when selecting LCtrl (VK_LCONTROL) on first dropdown of first column with Empty+C
                testCases.push_back({ 1, 0, 0, { VK_LCONTROL, 'C' }, L"", false, { { Shortcut{ 'C' }, Shortcut() }, L"" } });
                // Case 5 : Validate the element when selecting LCtrl (VK_LCONTROL) on first dropdown of first column with Empty+Empty+C
                testCases.push_back({ 1, 0, 0, { VK_LCONTROL, VK_INVALID, 'C' }, L"", false, { { Shortcut{ 'C' }, Shortcut() }, L"" } });
                // Case 6 : Validate the element when selecting LCtrl (VK_LCONTROL) on second dropdown of first column with Empty+Empty+C
                testCases.push_back({ 1, 0, 1, { VK_INVALID, VK_LCONTROL, 'C' }, L"", false, { { Shortcut{ 'C' }, Shortcut() }, L"" } });
                // Case 7 : Validate the element when selecting Null (VK_INVALID) on second dropdown of first column with LCtrl+Shift+C
                testCases.push_back({ 1, 0, 1, { VK_LCONTROL, VK_INVALID, 'C' }, L"", false, { { Shortcut{ VK_LCONTROL, VK_SHIFT, 'C' }, Shortcut() }, L"" } });
                // Case 8 : Validate the element when selecting Null (VK_INVALID) on first dropdown of first column with Shift+LCtrl+C
                testCases.push_back({ 1, 0, 0, { VK_INVALID, VK_LCONTROL, 'C' }, L"", false, { { Shortcut{ VK_SHIFT, VK_LCONTROL, 'C' }, Shortcut() }, L"" } });
                // Case 9 : Validate the element when selecting None (VK_NULL) on second dropdown of first column with LCtrl+Shift+C
                testCases.push_back({ 1, 0, 1, { VK_LCONTROL, VK_NULL, 'C' }, L"", false, { { Shortcut{ VK_LCONTROL, VK_SHIFT, 'C' }, Shortcut() }, L"" } });
                // Case 10 : Validate the element when selecting None (VK_NULL) on first dropdown of first column with Shift+LCtrl+C
                testCases.push_back({ 1, 0, 0, { VK_NULL, VK_LCONTROL, 'C' }, L"", false, { { Shortcut{ VK_SHIFT, VK_LCONTROL, 'C' }, Shortcut() }, L"" } });

                RunTestCases(testCases, [this](const ValidateShortcutBufferElementArgs& testCase) {
                    // Arrange
                    RemapBuffer remapBuffer;
                    // Ctrl+C remapped
                    remapBuffer.push_back({ { Shortcut{ VK_CONTROL, 'C' }, Shortcut() }, L"" });
                    remapBuffer.push_back(testCase.bufferRow);

                    // Act
                    std::pair<ShortcutErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedCodesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, remapBuffer, true);

                    // Assert that the element is invalid
                    Assert::AreEqual(ShortcutErrorType::ConflictingModifierShortcut, result.first);
                });
            }

            // Test if the ValidateShortcutBufferElement method returns no error on setting first column to conflict with first column in another row with different target app and both are valid 2 key shortcuts
            TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnNoError_OnSettingFirstColumnToConflictWithFirstColumnInAnotherRowWithDifferentTargetAppAndBothAreValid2KeyShortcuts)
            {
                std::vector<ValidateShortcutBufferElementArgs> testCases;
                // Case 1 : Validate the element when selecting C ('C') on second dropdown of first column with LCtrl+Empty for testApp2
                testCases.push_back({ 1, 0, 1, { VK_LCONTROL, 'C' }, testApp2, false, { { Shortcut{ VK_LCONTROL }, Shortcut() }, testApp2 } });
                // Case 2 : Validate the element when selecting C ('C') on third dropdown of first column with LCtrl+Empty+Empty for testApp2
                testCases.push_back({ 1, 0, 2, { VK_LCONTROL, VK_INVALID, 'C' }, testApp2, false, { { Shortcut{ VK_LCONTROL }, Shortcut() }, testApp2 } });
                // Case 3 : Validate the element when selecting C ('C') on second dropdown of first column with LCtrl+Empty+Empty for testApp2
                testCases.push_back({ 1, 0, 1, { VK_LCONTROL, 'C', VK_INVALID }, testApp2, false, { { Shortcut{ VK_LCONTROL }, Shortcut() }, testApp2 } });
                // Case 4 : Validate the element when selecting LCtrl (VK_LCONTROL) on first dropdown of first column with Empty+C for testApp2
                testCases.push_back({ 1, 0, 0, { VK_LCONTROL, 'C' }, testApp2, false, { { Shortcut{ 'C' }, Shortcut() }, testApp2 } });
                // Case 5 : Validate the element when selecting LCtrl (VK_LCONTROL) on first dropdown of first column with Empty+Empty+C for testApp2
                testCases.push_back({ 1, 0, 0, { VK_LCONTROL, VK_INVALID, 'C' }, testApp2, false, { { Shortcut{ 'C' }, Shortcut() }, testApp2 } });
                // Case 6 : Validate the element when selecting LCtrl (VK_LCONTROL) on second dropdown of first column with Empty+Empty+C for testApp2
                testCases.push_back({ 1, 0, 1, { VK_INVALID, VK_LCONTROL, 'C' }, testApp2, false, { { Shortcut{ 'C' }, Shortcut() }, testApp2 } });
                // Case 7 : Validate the element when selecting Null (VK_INVALID) on second dropdown of first column with LCtrl+Shift+C for testApp2
                testCases.push_back({ 1, 0, 1, { VK_LCONTROL, VK_INVALID, 'C' }, testApp2, false, { { Shortcut{ VK_LCONTROL, VK_SHIFT, 'C' }, Shortcut() }, testApp2 } });
                // Case 8 : Validate the element when selecting Null (VK_INVALID) on first dropdown of first column with Shift+LCtrl+C for testApp2
                testCases.push_back({ 1, 0, 0, { VK_INVALID, VK_LCONTROL, 'C' }, testApp2, false, { { Shortcut{ VK_SHIFT, VK_LCONTROL, 'C' }, Shortcut() }, testApp2 } });
                // Case 9 : Validate the element when selecting None (VK_NULL) on second dropdown of first column with LCtrl+Shift+C for testApp2
                testCases.push_back({ 1, 0, 1, { VK_LCONTROL, VK_NULL, 'C' }, testApp2, false, { { Shortcut{ VK_LCONTROL, VK_SHIFT, 'C' }, Shortcut() }, testApp2 } });
                // Case 10 : Validate the element when selecting None (VK_NULL) on first dropdown of first column with Shift+LCtrl+C for testApp2
                testCases.push_back({ 1, 0, 0, { VK_NULL, VK_LCONTROL, 'C' }, testApp2, false, { { Shortcut{ VK_SHIFT, VK_LCONTROL, 'C' }, Shortcut() }, testApp2 } });

                RunTestCases(testCases, [this](const ValidateShortcutBufferElementArgs& testCase) {
                    // Arrange
                    RemapBuffer remapBuffer;
                    // Ctrl+C remapped for testApp1
                    remapBuffer.push_back({ { Shortcut{ VK_CONTROL, 'C' }, Shortcut() }, testApp1 });
                    remapBuffer.push_back(testCase.bufferRow);

                    // Act
                    std::pair<ShortcutErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(testCase.elementRowIndex, testCase.elementColIndex, testCase.indexOfDropDownLastModified, testCase.selectedCodesOnDropDowns, testCase.targetAppNameInTextBox, testCase.isHybridColumn, remapBuffer, true);

                    // Assert that the element is valid
                    Assert::AreEqual(ShortcutErrorType::NoError, result.first);
                });
            }

            // Return error on Disable as second modifier key or action key
            TEST_METHOD (ValidateShortcutBufferElement_ShouldReturnDisableAsActionKeyError_OnSettingSecondDropdownAsDisable)
            {
                // Arrange
                RemapBuffer remapBuffer;
                remapBuffer.push_back({ { Shortcut{ VK_SHIFT, CommonSharedConstants::VK_DISABLED }, Shortcut() }, testApp1 });
                std::vector<int32_t> selectedCodes = {
                    VK_SHIFT,
                    CommonSharedConstants::VK_DISABLED
                };

                // Act
                std::pair<ShortcutErrorType, BufferValidationHelpers::DropDownAction> result = BufferValidationHelpers::ValidateShortcutBufferElement(0, 1, 1, selectedCodes, testApp1, true, remapBuffer, true);

                // Assert
                Assert::AreEqual(ShortcutErrorType::ShortcutDisableAsActionKey, result.first);
                Assert::AreEqual(BufferValidationHelpers::DropDownAction::NoAction, result.second);
            }
    };
}
