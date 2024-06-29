#include "pch.h"

// Suppressing 26466 - Don't use static_cast downcasts - in CppUnitTest.h
#pragma warning(push)
#pragma warning(disable : 26466)
#include "CppUnitTest.h"
#pragma warning(pop)

#include <keyboardmanager/common/MappingConfiguration.h>
#include <keyboardmanager/KeyboardManagerEditorLibrary/LoadingAndSavingRemappingHelper.h>
#include <common/interop/shared_constants.h>
#include <keyboardmanager/KeyboardManagerEditorLibrary/ShortcutErrorType.h>
#include <keyboardmanager/KeyboardManagerEditorLibrary/RemapBuffer.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace RemappingUITests
{
    // Tests for methods in the LoadingAndSavingRemappingHelper namespace
    TEST_CLASS (LoadingAndSavingRemappingTests)
    {
        std::wstring testApp1 = L"testprocess1.exe";
        std::wstring testApp2 = L"testprocess2.exe";

    public:
        TEST_METHOD_INITIALIZE(InitializeTestEnv)
        {
        }

        // Test if the CheckIfRemappingsAreValid method is successful when no remaps are passed
        TEST_METHOD (CheckIfRemappingsAreValid_ShouldReturnNoError_OnPassingNoRemaps)
        {
            RemapBuffer remapBuffer{};

            // Assert that remapping set is valid
            Assert::AreEqual(ShortcutErrorType::NoError, LoadingAndSavingRemappingHelper::CheckIfRemappingsAreValid(remapBuffer));
        }

        // Test if the CheckIfRemappingsAreValid method is successful when valid key to key remaps are passed
        TEST_METHOD (CheckIfRemappingsAreValid_ShouldReturnNoError_OnPassingValidKeyToKeyRemaps)
        {
            // Remap A to B and B to C
            RemapBuffer remapBuffer{
                RemapBufferRow{ RemapBufferItem({ 'A', 'B' }), std::wstring() },
                RemapBufferRow{ RemapBufferItem({ 'B', 'C' }), std::wstring() },
            };

            // Assert that remapping set is valid
            Assert::AreEqual(ShortcutErrorType::NoError, LoadingAndSavingRemappingHelper::CheckIfRemappingsAreValid(remapBuffer));
        }

        // Test if the CheckIfRemappingsAreValid method is successful when valid key to shortcut remaps are passed
        TEST_METHOD (CheckIfRemappingsAreValid_ShouldReturnNoError_OnPassingValidKeyToShortcutRemaps)
        {
            // Remap A to Ctrl+V and B to Alt+Tab
            Shortcut ctrlV;
            ctrlV.SetKey(VK_CONTROL);
            ctrlV.SetKey('V');
            Shortcut altTab;
            altTab.SetKey(VK_MENU);
            altTab.SetKey(VK_TAB);
            RemapBuffer remapBuffer{
                RemapBufferRow{ RemapBufferItem({ 'A', ctrlV }), std::wstring() },
                RemapBufferRow{ RemapBufferItem({ 'B', altTab }), std::wstring() },
            };

            // Assert that remapping set is valid
            Assert::AreEqual(ShortcutErrorType::NoError, LoadingAndSavingRemappingHelper::CheckIfRemappingsAreValid(remapBuffer));
        }

        // Test if the CheckIfRemappingsAreValid method is successful when valid shortcut to key remaps are passed
        TEST_METHOD (CheckIfRemappingsAreValid_ShouldReturnNoError_OnPassingValidShortcutToKeyRemaps)
        {
            // Remap Ctrl+V to A and Alt+Tab to B
            Shortcut ctrlV;
            ctrlV.SetKey(VK_CONTROL);
            ctrlV.SetKey('V');
            Shortcut altTab;
            altTab.SetKey(VK_MENU);
            altTab.SetKey(VK_TAB);
            RemapBuffer remapBuffer{
                RemapBufferRow{ RemapBufferItem({ ctrlV, 'A' }), std::wstring() },
                RemapBufferRow{ RemapBufferItem({ altTab, 'B' }), std::wstring() },
            };

            // Assert that remapping set is valid
            Assert::AreEqual(ShortcutErrorType::NoError, LoadingAndSavingRemappingHelper::CheckIfRemappingsAreValid(remapBuffer));
        }

        // Test if the CheckIfRemappingsAreValid method is successful when valid shortcut to shortcut remaps are passed
        TEST_METHOD (CheckIfRemappingsAreValid_ShouldReturnNoError_OnPassingValidShortcutToShortcutRemaps)
        {
            // Remap Ctrl+V to Ctrl+D and Alt+Tab to Win+A
            Shortcut ctrlV;
            ctrlV.SetKey(VK_CONTROL);
            ctrlV.SetKey('V');
            Shortcut ctrlD;
            ctrlD.SetKey(VK_CONTROL);
            ctrlD.SetKey('D');
            Shortcut altTab;
            altTab.SetKey(VK_MENU);
            altTab.SetKey(VK_TAB);
            Shortcut winA;
            winA.SetKey(CommonSharedConstants::VK_WIN_BOTH);
            winA.SetKey('A');
            RemapBuffer remapBuffer{
                RemapBufferRow{ RemapBufferItem({ ctrlV, ctrlD }), std::wstring() },
                RemapBufferRow{ RemapBufferItem({ altTab, winA }), std::wstring() },
            };

            // Assert that remapping set is valid
            Assert::AreEqual(ShortcutErrorType::NoError, LoadingAndSavingRemappingHelper::CheckIfRemappingsAreValid(remapBuffer));
        }

        // Test if the CheckIfRemappingsAreValid method is successful when valid remaps are passed
        TEST_METHOD (CheckIfRemappingsAreValid_ShouldReturnNoError_OnPassingValidRemapsOfAllTypes)
        {
            // Remap Ctrl+V to Ctrl+D, Alt+Tab to A, A to B and B to Win+A
            Shortcut ctrlV;
            ctrlV.SetKey(VK_CONTROL);
            ctrlV.SetKey('V');
            Shortcut ctrlD;
            ctrlD.SetKey(VK_CONTROL);
            ctrlD.SetKey('D');
            Shortcut altTab;
            altTab.SetKey(VK_MENU);
            altTab.SetKey(VK_TAB);
            Shortcut winA;
            winA.SetKey(CommonSharedConstants::VK_WIN_BOTH);
            winA.SetKey('A');
            RemapBuffer remapBuffer{
                RemapBufferRow{ RemapBufferItem({ ctrlV, ctrlD }), std::wstring() },
                RemapBufferRow{ RemapBufferItem({ altTab, 'A' }), std::wstring() },
                RemapBufferRow{ RemapBufferItem({ 'A', 'B' }), std::wstring() },
                RemapBufferRow{ RemapBufferItem({ 'B', winA }), std::wstring() },
            };

            // Assert that remapping set is valid
            Assert::AreEqual(ShortcutErrorType::NoError, LoadingAndSavingRemappingHelper::CheckIfRemappingsAreValid(remapBuffer));
        }

        // Test if the CheckIfRemappingsAreValid method is unsuccessful when remaps with null keys are passed
        TEST_METHOD (CheckIfRemappingsAreValid_ShouldReturnRemapUnsuccessful_OnPassingRemapsWithNullKeys)
        {
            // Remap A to NULL
            RemapBuffer remapBuffer{
                RemapBufferRow{ RemapBufferItem({ 'A', NULL }), std::wstring() },
            };

            // Assert that remapping set is invalid
            Assert::AreEqual(ShortcutErrorType::RemapUnsuccessful, LoadingAndSavingRemappingHelper::CheckIfRemappingsAreValid(remapBuffer));
        }

        // Test if the CheckIfRemappingsAreValid method is unsuccessful when remaps with invalid shortcuts are passed
        TEST_METHOD (CheckIfRemappingsAreValid_ShouldReturnRemapUnsuccessful_OnPassingRemapsWithInvalidShortcut)
        {
            // Remap A to incomplete shortcut (Ctrl)
            Shortcut ctrl;
            ctrl.SetKey(VK_CONTROL);
            RemapBuffer remapBuffer{
                RemapBufferRow{ RemapBufferItem({ 'A', ctrl }), std::wstring() },
            };

            // Assert that remapping set is invalid
            Assert::AreEqual(ShortcutErrorType::RemapUnsuccessful, LoadingAndSavingRemappingHelper::CheckIfRemappingsAreValid(remapBuffer));
        }

        // Test if the CheckIfRemappingsAreValid method is unsuccessful when remaps with the same key remapped twice are passed
        TEST_METHOD (CheckIfRemappingsAreValid_ShouldReturnRemapUnsuccessful_OnPassingRemapsWithSameKeyRemappedTwice)
        {
            // Remap A to B and A to Ctrl+C
            Shortcut ctrlC;
            ctrlC.SetKey(VK_CONTROL);
            ctrlC.SetKey('C');
            RemapBuffer remapBuffer{
                RemapBufferRow{ RemapBufferItem({ 'A', 'B' }), std::wstring() },
                RemapBufferRow{ RemapBufferItem({ 'A', ctrlC }), std::wstring() },
            };

            // Assert that remapping set is invalid
            Assert::AreEqual(ShortcutErrorType::RemapUnsuccessful, LoadingAndSavingRemappingHelper::CheckIfRemappingsAreValid(remapBuffer));
        }

        // Test if the CheckIfRemappingsAreValid method is unsuccessful when remaps with the same shortcut remapped twice are passed
        TEST_METHOD (CheckIfRemappingsAreValid_ShouldReturnRemapUnsuccessful_OnPassingRemapsWithSameShortcutRemappedTwice)
        {
            // Remap Ctrl+A to B and Ctrl+A to Ctrl+V
            Shortcut ctrlA;
            ctrlA.SetKey(VK_CONTROL);
            ctrlA.SetKey('A');
            Shortcut ctrlV;
            ctrlV.SetKey(VK_CONTROL);
            ctrlV.SetKey('V');
            RemapBuffer remapBuffer{
                RemapBufferRow{ RemapBufferItem({ ctrlA, 'B' }), std::wstring() },
                RemapBufferRow{ RemapBufferItem({ ctrlA, ctrlV }), std::wstring() },
            };

            // Assert that remapping set is invalid
            Assert::AreEqual(ShortcutErrorType::RemapUnsuccessful, LoadingAndSavingRemappingHelper::CheckIfRemappingsAreValid(remapBuffer));
        }

        // Test if the CheckIfRemappingsAreValid method is unsuccessful when app specific remaps with the same shortcut remapped twice for the same target app are passed
        TEST_METHOD (CheckIfRemappingsAreValid_ShouldReturnRemapUnsuccessful_OnPassingAppSpecificRemapsWithSameShortcutRemappedTwiceForTheSameTargetApp)
        {
            // Remap Ctrl+A to B and Ctrl+A to Ctrl+V for testApp1
            Shortcut ctrlA;
            ctrlA.SetKey(VK_CONTROL);
            ctrlA.SetKey('A');
            Shortcut ctrlV;
            ctrlV.SetKey(VK_CONTROL);
            ctrlV.SetKey('V');
            RemapBuffer remapBuffer{
                RemapBufferRow{ RemapBufferItem({ ctrlA, 'B' }), testApp1 },
                RemapBufferRow{ RemapBufferItem({ ctrlA, ctrlV }), testApp1 },
            };

            // Assert that remapping set is invalid
            Assert::AreEqual(ShortcutErrorType::RemapUnsuccessful, LoadingAndSavingRemappingHelper::CheckIfRemappingsAreValid(remapBuffer));
        }

        // Test if the CheckIfRemappingsAreValid method is successful when app specific remaps with the same shortcut remapped twice for different target apps are passed
        TEST_METHOD (CheckIfRemappingsAreValid_ShouldReturnNoError_OnPassingAppSpecificRemapsWithSameShortcutRemappedTwiceForDifferentTargetApps)
        {
            // Remap Ctrl+A to B for testApp1 and Ctrl+A to Ctrl+V for testApp2
            Shortcut ctrlA;
            ctrlA.SetKey(VK_CONTROL);
            ctrlA.SetKey('A');
            Shortcut ctrlV;
            ctrlV.SetKey(VK_CONTROL);
            ctrlV.SetKey('V');
            RemapBuffer remapBuffer{
                RemapBufferRow{ RemapBufferItem({ ctrlA, 'B' }), testApp1 },
                RemapBufferRow{ RemapBufferItem({ ctrlA, ctrlV }), testApp2 },
            };

            // Assert that remapping set is valid
            Assert::AreEqual(ShortcutErrorType::NoError, LoadingAndSavingRemappingHelper::CheckIfRemappingsAreValid(remapBuffer));
        }

        // Test if the GetOrphanedKeys method return an empty vector on passing no remaps
        TEST_METHOD (GetOrphanedKeys_ShouldReturnEmptyVector_OnPassingNoRemaps)
        {
            RemapBuffer remapBuffer{};

            // Assert that there are no orphaned keys
            Assert::AreEqual(static_cast<size_t>(0), LoadingAndSavingRemappingHelper::GetOrphanedKeys(remapBuffer).size());
        }

        // Test if the GetOrphanedKeys method return one orphaned on passing one key remap
        TEST_METHOD (GetOrphanedKeys_ShouldReturnOneOrphanedKey_OnPassingOneKeyRemap)
        {
            // Remap A to B
            RemapBuffer remapBuffer{
                RemapBufferRow{ RemapBufferItem({ 'A', 'B' }), std::wstring() },
            };

            // Assert that only A is orphaned
            const std::vector<DWORD> orphanedKeys = LoadingAndSavingRemappingHelper::GetOrphanedKeys(remapBuffer);
            Assert::AreEqual(static_cast<size_t>(1), orphanedKeys.size());
            Assert::AreEqual(static_cast<DWORD>('A'), orphanedKeys[0]);
        }

        // Test if the GetOrphanedKeys method return an empty vector on passing swapped key remaps
        TEST_METHOD (GetOrphanedKeys_ShouldReturnEmptyVector_OnPassingSwappedKeyRemap)
        {
            // Remap A to B and B to A
            RemapBuffer remapBuffer{
                RemapBufferRow{ RemapBufferItem({ 'A', 'B' }), std::wstring() },
                RemapBufferRow{ RemapBufferItem({ 'B', 'A' }), std::wstring() },
            };

            // Assert that there are no orphaned keys
            Assert::AreEqual(static_cast<size_t>(0), LoadingAndSavingRemappingHelper::GetOrphanedKeys(remapBuffer).size());
        }

        // Test if the GetOrphanedKeys method return one orphaned on passing two key remaps where one key is mapped to a remapped key
        TEST_METHOD (GetOrphanedKeys_ShouldReturnOneOrphanedKey_OnPassingTwoKeyRemapsWhereOneKeyIsMappedToARemappedKey)
        {
            // Remap A to Ctrl+B and C to A
            Shortcut ctrlB;
            ctrlB.SetKey(VK_CONTROL);
            ctrlB.SetKey('B');
            RemapBuffer remapBuffer{
                RemapBufferRow{ RemapBufferItem({ 'A', ctrlB }), std::wstring() },
                RemapBufferRow{ RemapBufferItem({ 'C', 'A' }), std::wstring() },
            };

            // Assert that only C is orphaned
            const std::vector<DWORD> orphanedKeys = LoadingAndSavingRemappingHelper::GetOrphanedKeys(remapBuffer);
            Assert::AreEqual(static_cast<size_t>(1), orphanedKeys.size());
            Assert::AreEqual(static_cast<DWORD>('C'), orphanedKeys[0]);
        }

        // Test if the PreProcessRemapTable method combines all the modifier pairs when the left and right modifiers are remapped to the same target
        TEST_METHOD (PreProcessRemapTable_ShouldCombineAllPairs_OnPassingLeftAndRightModifiersRemappedToTheSameTarget)
        {
            // Remap LCtrl and RCtrl to A, LAlt and RAlt to B, LShift and RShift to C, LWin and RWin to D
            SingleKeyRemapTable remapTable{
                { VK_LCONTROL, 'A' },
                { VK_RCONTROL, 'A' },
                { VK_LMENU, 'B' },
                { VK_RMENU, 'B' },
                { VK_LSHIFT, 'C' },
                { VK_RSHIFT, 'C' },
                { VK_LWIN, 'D' },
                { VK_RWIN, 'D' },
            };

            // Pre process table
            LoadingAndSavingRemappingHelper::PreProcessRemapTable(remapTable);

            // Expected Ctrl remapped to A, Alt to B, Shift to C, Win to D
            SingleKeyRemapTable expectedTable{
                { VK_CONTROL, 'A' },
                { VK_MENU, 'B' },
                { VK_SHIFT, 'C' },
                { CommonSharedConstants::VK_WIN_BOTH, 'D' },
            };

            Assert::AreEqual(expectedTable, remapTable);
        }

        // Test if the PreProcessRemapTable method does not combines any of the modifier pairs when the left and right modifiers are remapped to different targets
        TEST_METHOD (PreProcessRemapTable_ShouldNotCombineAnyPairs_OnPassingLeftAndRightModifiersRemappedToTheDifferentTargets)
        {
            // Remap left modifiers to A and right modifiers to B
            SingleKeyRemapTable remapTable{
                { VK_LCONTROL, 'A' },
                { VK_RCONTROL, 'B' },
                { VK_LMENU, 'A' },
                { VK_RMENU, 'B' },
                { VK_LSHIFT, 'A' },
                { VK_RSHIFT, 'B' },
                { VK_LWIN, 'A' },
                { VK_RWIN, 'B' },
            };

            // Pre process table
            LoadingAndSavingRemappingHelper::PreProcessRemapTable(remapTable);

            // Expected unchanged table
            SingleKeyRemapTable expectedTable{
                { VK_LCONTROL, 'A' },
                { VK_RCONTROL, 'B' },
                { VK_LMENU, 'A' },
                { VK_RMENU, 'B' },
                { VK_LSHIFT, 'A' },
                { VK_RSHIFT, 'B' },
                { VK_LWIN, 'A' },
                { VK_RWIN, 'B' },
            };

            Assert::AreEqual(expectedTable, remapTable);
        }

        // Test if the ApplySingleKeyRemappings method resets the keyboard manager state's single key remappings on passing an empty buffer
        TEST_METHOD (ApplySingleKeyRemappings_ShouldResetSingleKeyRemappings_OnPassingEmptyBuffer)
        {
            MappingConfiguration testShortcuts;
            RemapBuffer remapBuffer{};

            // Remap A to B
            testShortcuts.AddSingleKeyRemap('A', 'B');

            // Apply the single key remaps from the buffer to the keyboard manager state variable
            LoadingAndSavingRemappingHelper::ApplySingleKeyRemappings(testShortcuts, remapBuffer, false);

            // Assert that single key remapping in the kbm state variable is empty
            Assert::AreEqual(static_cast<size_t>(0), testShortcuts.singleKeyReMap.size());
        }

        // Test if the ApplySingleKeyRemappings method copies only the valid remappings to the keyboard manager state variable when some of the remappings are invalid
        TEST_METHOD (ApplySingleKeyRemappings_ShouldCopyOnlyValidRemappings_OnPassingBufferWithSomeInvalidRemappings)
        {
            MappingConfiguration testShortcuts;

            // Add A->B, B->Ctrl+V, C to incomplete shortcut and D to incomplete key remappings to the buffer
            Shortcut ctrlV;
            ctrlV.SetKey(VK_CONTROL);
            ctrlV.SetKey('V');
            Shortcut leftAlt;
            leftAlt.SetKey(VK_LMENU);
            RemapBuffer remapBuffer{
                RemapBufferRow{ RemapBufferItem({ 'A', 'B' }), std::wstring() },
                RemapBufferRow{ RemapBufferItem({ 'B', ctrlV }), std::wstring() },
                RemapBufferRow{ RemapBufferItem({ 'C', NULL }), std::wstring() },
                RemapBufferRow{ RemapBufferItem({ 'D', leftAlt }), std::wstring() },
            };

            // Apply the single key remaps from the buffer to the keyboard manager state variable
            LoadingAndSavingRemappingHelper::ApplySingleKeyRemappings(testShortcuts, remapBuffer, false);

            // Expected A remapped to B, B remapped to Ctrl+V
            SingleKeyRemapTable expectedTable{
                { 'A', 'B' },
                { 'B', ctrlV },
            };

            Assert::AreEqual(expectedTable, testShortcuts.singleKeyReMap);
        }

        // Test if the ApplySingleKeyRemappings method splits common modifiers to their left and right version when copying to the keyboard manager state variable if remappings from common modifiers are passed
        TEST_METHOD (ApplySingleKeyRemappings_ShouldSplitRemappingsFromCommonModifiers_OnPassingBufferWithSomeMappingsFromCommonModifiers)
        {
            // Add Ctrl->A, Alt->B, Shift->C and Win->D remappings to the buffer
            MappingConfiguration testShortcuts;
            RemapBuffer remapBuffer{
                RemapBufferRow{ RemapBufferItem({ VK_CONTROL, 'A' }), std::wstring() },
                RemapBufferRow{ RemapBufferItem({ VK_MENU, 'B' }), std::wstring() },
                RemapBufferRow{ RemapBufferItem({ VK_SHIFT, 'C' }), std::wstring() },
                RemapBufferRow{ RemapBufferItem({ CommonSharedConstants::VK_WIN_BOTH, 'D' }), std::wstring() },
            };

            // Apply the single key remaps from the buffer to the keyboard manager state variable
            LoadingAndSavingRemappingHelper::ApplySingleKeyRemappings(testShortcuts, remapBuffer, false);

            // Expected LCtrl/RCtrl remapped to A, LAlt/RAlt to B, LShift/RShift to C, LWin/RWin to D
            SingleKeyRemapTable expectedTable{
                { VK_LCONTROL, 'A' },
                { VK_RCONTROL, 'A' },
                { VK_LMENU, 'B' },
                { VK_RMENU, 'B' },
                { VK_LSHIFT, 'C' },
                { VK_RSHIFT, 'C' },
                { VK_LWIN, 'D' },
                { VK_RWIN, 'D' },
            };

            Assert::AreEqual(expectedTable, testShortcuts.singleKeyReMap);
        }

        // Test if the ApplyShortcutRemappings method resets the keyboard manager state's os level and app specific shortcut remappings on passing an empty buffer
        TEST_METHOD (ApplyShortcutRemappings_ShouldResetShortcutRemappings_OnPassingEmptyBuffer)
        {
            MappingConfiguration testShortcuts;
            RemapBuffer remapBuffer{};

            // Remap Ctrl+A to Ctrl+B for all apps and Ctrl+C to Alt+V for testApp1
            Shortcut ctrlA;
            ctrlA.SetKey(VK_CONTROL);
            ctrlA.SetKey('A');
            Shortcut ctrlB;
            ctrlB.SetKey(VK_CONTROL);
            ctrlB.SetKey('B');
            Shortcut ctrlC;
            ctrlC.SetKey(VK_CONTROL);
            ctrlC.SetKey('C');
            Shortcut altV;
            altV.SetKey(VK_MENU);
            altV.SetKey('V');
            testShortcuts.AddOSLevelShortcut(ctrlA, ctrlB);
            testShortcuts.AddAppSpecificShortcut(testApp1, ctrlA, ctrlB);

            // Apply the shortcut remaps from the buffer to the keyboard manager state variable
            LoadingAndSavingRemappingHelper::ApplyShortcutRemappings(testShortcuts, remapBuffer, false);

            // Assert that shortcut remappings in the kbm state variable is empty
            Assert::AreEqual(static_cast<size_t>(0), testShortcuts.osLevelShortcutReMap.size());
            Assert::AreEqual(static_cast<size_t>(0), testShortcuts.appSpecificShortcutReMap.size());
        }

        // Test if the ApplyShortcutRemappings method copies only the valid remappings to the keyboard manager state variable when some of the remappings are invalid
        TEST_METHOD (ApplyShortcutRemappings_ShouldCopyOnlyValidRemappings_OnPassingBufferWithSomeInvalidRemappings)
        {
            MappingConfiguration testShortcuts;

            // Add Ctrl+A->Ctrl+B, Ctrl+C->Alt+V, Ctrl+F->incomplete shortcut and Ctrl+G->incomplete key os level remappings to buffer
            // Add Ctrl+F->Alt+V, Ctrl+G->Ctrl+B, Ctrl+A->incomplete shortcut and  Ctrl+C->incomplete key app specific remappings to buffer
            Shortcut ctrlA;
            ctrlA.SetKey(VK_CONTROL);
            ctrlA.SetKey('A');
            Shortcut ctrlB;
            ctrlB.SetKey(VK_CONTROL);
            ctrlB.SetKey('B');
            Shortcut ctrlC;
            ctrlC.SetKey(VK_CONTROL);
            ctrlC.SetKey('C');
            Shortcut altV;
            altV.SetKey(VK_MENU);
            altV.SetKey('V');
            Shortcut ctrlF;
            ctrlF.SetKey(VK_CONTROL);
            ctrlF.SetKey('F');
            Shortcut ctrlG;
            ctrlG.SetKey(VK_CONTROL);
            ctrlG.SetKey('G');
            Shortcut ctrl;
            ctrl.SetKey(VK_CONTROL);
            RemapBuffer remapBuffer{
                RemapBufferRow{ RemapBufferItem({ ctrlA, ctrlB }), std::wstring() },
                RemapBufferRow{ RemapBufferItem({ ctrlC, altV }), std::wstring() },
                RemapBufferRow{ RemapBufferItem({ ctrlF, NULL }), std::wstring() },
                RemapBufferRow{ RemapBufferItem({ ctrlG, ctrl }), std::wstring() },
                RemapBufferRow{ RemapBufferItem({ ctrlF, altV }), testApp1 },
                RemapBufferRow{ RemapBufferItem({ ctrlG, ctrlB }), testApp1 },
                RemapBufferRow{ RemapBufferItem({ ctrlA, NULL }), testApp1 },
                RemapBufferRow{ RemapBufferItem({ ctrlC, ctrl }), testApp1 },
            };

            // Apply the shortcut remaps from the buffer to the keyboard manager state variable
            LoadingAndSavingRemappingHelper::ApplyShortcutRemappings(testShortcuts, remapBuffer, false);

            // Ctrl+A->Ctrl+B and Ctrl+C->Alt+V
            ShortcutRemapTable expectedOSLevelTable{
                { ctrlA, RemapShortcut(ctrlB) },
                { ctrlC, RemapShortcut(altV) },
            };

            // Ctrl+F->Alt+V and Ctrl+G->Ctrl+B for testApp1
            AppSpecificShortcutRemapTable expectedAppSpecificLevelTable{
                { testApp1, {
                                { ctrlF, RemapShortcut(altV) },
                                { ctrlG, RemapShortcut(ctrlB) },
                            } },
            };

            Assert::AreEqual(expectedOSLevelTable, testShortcuts.osLevelShortcutReMap);
            Assert::AreEqual(expectedAppSpecificLevelTable, testShortcuts.appSpecificShortcutReMap);
        }
    };
}
