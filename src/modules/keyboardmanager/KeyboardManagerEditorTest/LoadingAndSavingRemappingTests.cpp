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
            RemapBuffer remapBuffer;

            // Assert that remapping set is valid
            bool isSuccess = (LoadingAndSavingRemappingHelper::CheckIfRemappingsAreValid(remapBuffer) == ShortcutErrorType::NoError);
            Assert::IsTrue(isSuccess);
        }

        // Test if the CheckIfRemappingsAreValid method is successful when valid key to key remaps are passed
        TEST_METHOD (CheckIfRemappingsAreValid_ShouldReturnNoError_OnPassingValidKeyToKeyRemaps)
        {
            RemapBuffer remapBuffer;

            // Remap A to B and B to C
            remapBuffer.push_back({ RemapBufferItem({ 'A', 'B' }), L"" });
            remapBuffer.push_back({ RemapBufferItem({ 'B', 'C' }), L"" });

            // Assert that remapping set is valid
            bool isSuccess = (LoadingAndSavingRemappingHelper::CheckIfRemappingsAreValid(remapBuffer) == ShortcutErrorType::NoError);
            Assert::IsTrue(isSuccess);
        }

        // Test if the CheckIfRemappingsAreValid method is successful when valid key to shortcut remaps are passed
        TEST_METHOD (CheckIfRemappingsAreValid_ShouldReturnNoError_OnPassingValidKeyToShortcutRemaps)
        {
            RemapBuffer remapBuffer;

            // Remap A to Ctrl+V and B to Alt+Tab
            Shortcut s1;
            s1.SetKey(VK_CONTROL);
            s1.SetKey('V');
            Shortcut s2;
            s2.SetKey(VK_MENU);
            s2.SetKey(VK_TAB);
            remapBuffer.push_back({ RemapBufferItem({ 'A', s1 }), L"" });
            remapBuffer.push_back({ RemapBufferItem({ 'B', s2 }), L"" });

            // Assert that remapping set is valid
            bool isSuccess = (LoadingAndSavingRemappingHelper::CheckIfRemappingsAreValid(remapBuffer) == ShortcutErrorType::NoError);
            Assert::IsTrue(isSuccess);
        }

        // Test if the CheckIfRemappingsAreValid method is successful when valid shortcut to key remaps are passed
        TEST_METHOD (CheckIfRemappingsAreValid_ShouldReturnNoError_OnPassingValidShortcutToKeyRemaps)
        {
            RemapBuffer remapBuffer;

            // Remap Ctrl+V to A and Alt+Tab to B
            Shortcut s1;
            s1.SetKey(VK_CONTROL);
            s1.SetKey('V');
            Shortcut s2;
            s2.SetKey(VK_MENU);
            s2.SetKey(VK_TAB);
            remapBuffer.push_back({ RemapBufferItem({ s1, 'A' }), L"" });
            remapBuffer.push_back({ RemapBufferItem({ s2, 'B' }), L"" });

            // Assert that remapping set is valid
            bool isSuccess = (LoadingAndSavingRemappingHelper::CheckIfRemappingsAreValid(remapBuffer) == ShortcutErrorType::NoError);
            Assert::IsTrue(isSuccess);
        }

        // Test if the CheckIfRemappingsAreValid method is successful when valid shortcut to shortcut remaps are passed
        TEST_METHOD (CheckIfRemappingsAreValid_ShouldReturnNoError_OnPassingValidShortcutToShortcutRemaps)
        {
            RemapBuffer remapBuffer;

            // Remap Ctrl+V to Ctrl+D and Alt+Tab to Win+A
            Shortcut src1;
            src1.SetKey(VK_CONTROL);
            src1.SetKey('V');
            Shortcut dest1;
            dest1.SetKey(VK_CONTROL);
            dest1.SetKey('D');
            Shortcut src2;
            src2.SetKey(VK_MENU);
            src2.SetKey(VK_TAB);
            Shortcut dest2;
            dest2.SetKey(CommonSharedConstants::VK_WIN_BOTH);
            dest2.SetKey('A');
            remapBuffer.push_back({ RemapBufferItem({ src1, dest1 }), L"" });
            remapBuffer.push_back({ RemapBufferItem({ src2, dest2 }), L"" });

            // Assert that remapping set is valid
            bool isSuccess = (LoadingAndSavingRemappingHelper::CheckIfRemappingsAreValid(remapBuffer) == ShortcutErrorType::NoError);
            Assert::IsTrue(isSuccess);
        }

        // Test if the CheckIfRemappingsAreValid method is successful when valid remaps are passed
        TEST_METHOD (CheckIfRemappingsAreValid_ShouldReturnNoError_OnPassingValidRemapsOfAllTypes)
        {
            RemapBuffer remapBuffer;

            // Remap Ctrl+V to Ctrl+D, Alt+Tab to A, A to B and B to Win+A
            Shortcut src1;
            src1.SetKey(VK_CONTROL);
            src1.SetKey('V');
            Shortcut dest1;
            dest1.SetKey(VK_CONTROL);
            dest1.SetKey('D');
            Shortcut src2;
            src2.SetKey(VK_MENU);
            src2.SetKey(VK_TAB);
            Shortcut dest2;
            dest2.SetKey(CommonSharedConstants::VK_WIN_BOTH);
            dest2.SetKey('A');
            remapBuffer.push_back({ RemapBufferItem({ src1, dest1 }), L"" });
            remapBuffer.push_back({ RemapBufferItem({ src2, 'A' }), L"" });
            remapBuffer.push_back({ RemapBufferItem({ 'A', 'B' }), L"" });
            remapBuffer.push_back({ RemapBufferItem({ 'B', dest2 }), L"" });

            // Assert that remapping set is valid
            bool isSuccess = (LoadingAndSavingRemappingHelper::CheckIfRemappingsAreValid(remapBuffer) == ShortcutErrorType::NoError);
            Assert::IsTrue(isSuccess);
        }

        // Test if the CheckIfRemappingsAreValid method is unsuccessful when remaps with null keys are passed
        TEST_METHOD (CheckIfRemappingsAreValid_ShouldReturnRemapUnsuccessful_OnPassingRemapsWithNullKeys)
        {
            RemapBuffer remapBuffer;

            // Remap A to NULL
            remapBuffer.push_back({ RemapBufferItem({ 'A', VK_NULL }), L"" });

            // Assert that remapping set is invalid
            bool isSuccess = (LoadingAndSavingRemappingHelper::CheckIfRemappingsAreValid(remapBuffer) == ShortcutErrorType::RemapUnsuccessful);
            Assert::IsTrue(isSuccess);
        }

        // Test if the CheckIfRemappingsAreValid method is unsuccessful when remaps with invalid shortcuts are passed
        TEST_METHOD (CheckIfRemappingsAreValid_ShouldReturnRemapUnsuccessful_OnPassingRemapsWithInvalidShortcut)
        {
            RemapBuffer remapBuffer;

            // Remap A to incomplete shortcut (Ctrl)
            Shortcut src1;
            src1.SetKey(VK_CONTROL);
            remapBuffer.push_back({ RemapBufferItem({ 'A', src1 }), L"" });

            // Assert that remapping set is invalid
            bool isSuccess = (LoadingAndSavingRemappingHelper::CheckIfRemappingsAreValid(remapBuffer) == ShortcutErrorType::RemapUnsuccessful);
            Assert::IsTrue(isSuccess);
        }

        // Test if the CheckIfRemappingsAreValid method is unsuccessful when remaps with the same key remapped twice are passed
        TEST_METHOD (CheckIfRemappingsAreValid_ShouldReturnRemapUnsuccessful_OnPassingRemapsWithSameKeyRemappedTwice)
        {
            RemapBuffer remapBuffer;

            // Remap A to B and A to Ctrl+C
            Shortcut src1;
            src1.SetKey(VK_CONTROL);
            src1.SetKey('C');
            remapBuffer.push_back({ RemapBufferItem({ 'A', 'B' }), L"" });
            remapBuffer.push_back({ RemapBufferItem({ 'A', src1 }), L"" });

            // Assert that remapping set is invalid
            bool isSuccess = (LoadingAndSavingRemappingHelper::CheckIfRemappingsAreValid(remapBuffer) == ShortcutErrorType::RemapUnsuccessful);
            Assert::IsTrue(isSuccess);
        }

        // Test if the CheckIfRemappingsAreValid method is unsuccessful when remaps with the same shortcut remapped twice are passed
        TEST_METHOD (CheckIfRemappingsAreValid_ShouldReturnRemapUnsuccessful_OnPassingRemapsWithSameShortcutRemappedTwice)
        {
            RemapBuffer remapBuffer;

            // Remap Ctrl+A to B and Ctrl+A to Ctrl+V
            Shortcut src1;
            src1.SetKey(VK_CONTROL);
            src1.SetKey('A');
            Shortcut dest1;
            dest1.SetKey(VK_CONTROL);
            dest1.SetKey('V');
            remapBuffer.push_back({ RemapBufferItem({ src1, 'B' }), L"" });
            remapBuffer.push_back({ RemapBufferItem({ src1, dest1 }), L"" });

            // Assert that remapping set is invalid
            bool isSuccess = (LoadingAndSavingRemappingHelper::CheckIfRemappingsAreValid(remapBuffer) == ShortcutErrorType::RemapUnsuccessful);
            Assert::IsTrue(isSuccess);
        }

        // Test if the CheckIfRemappingsAreValid method is unsuccessful when app specific remaps with the same shortcut remapped twice for the same target app are passed
        TEST_METHOD (CheckIfRemappingsAreValid_ShouldReturnRemapUnsuccessful_OnPassingAppSpecificRemapsWithSameShortcutRemappedTwiceForTheSameTargetApp)
        {
            RemapBuffer remapBuffer;

            // Remap Ctrl+A to B and Ctrl+A to Ctrl+V for testApp1
            Shortcut src1;
            src1.SetKey(VK_CONTROL);
            src1.SetKey('A');
            Shortcut dest1;
            dest1.SetKey(VK_CONTROL);
            dest1.SetKey('V');
            remapBuffer.push_back({ RemapBufferItem({ src1, 'B' }), testApp1 });
            remapBuffer.push_back({ RemapBufferItem({ src1, dest1 }), testApp1 });

            // Assert that remapping set is invalid
            bool isSuccess = (LoadingAndSavingRemappingHelper::CheckIfRemappingsAreValid(remapBuffer) == ShortcutErrorType::RemapUnsuccessful);
            Assert::IsTrue(isSuccess);
        }

        // Test if the CheckIfRemappingsAreValid method is successful when app specific remaps with the same shortcut remapped twice for different target apps are passed
        TEST_METHOD (CheckIfRemappingsAreValid_ShouldReturnNoError_OnPassingAppSpecificRemapsWithSameShortcutRemappedTwiceForDifferentTargetApps)
        {
            RemapBuffer remapBuffer;

            // Remap Ctrl+A to B for testApp1 and Ctrl+A to Ctrl+V for testApp2
            Shortcut src1;
            src1.SetKey(VK_CONTROL);
            src1.SetKey('A');
            Shortcut dest1;
            dest1.SetKey(VK_CONTROL);
            dest1.SetKey('V');
            remapBuffer.push_back({ RemapBufferItem({ src1, 'B' }), testApp1 });
            remapBuffer.push_back({ RemapBufferItem({ src1, dest1 }), testApp2 });

            // Assert that remapping set is valid
            bool isSuccess = (LoadingAndSavingRemappingHelper::CheckIfRemappingsAreValid(remapBuffer) == ShortcutErrorType::NoError);
            Assert::IsTrue(isSuccess);
        }

        // Test if the GetOrphanedKeys method return an empty vector on passing no remaps
        TEST_METHOD (GetOrphanedKeys_ShouldReturnEmptyVector_OnPassingNoRemaps)
        {
            RemapBuffer remapBuffer;

            // Assert that there are no orphaned keys
            Assert::IsTrue(LoadingAndSavingRemappingHelper::GetOrphanedKeys(remapBuffer).empty());
        }

        // Test if the GetOrphanedKeys method return one orphaned on passing one key remap
        TEST_METHOD (GetOrphanedKeys_ShouldReturnOneOrphanedKey_OnPassingOneKeyRemap)
        {
            RemapBuffer remapBuffer;

            // Remap A to B
            remapBuffer.push_back({ RemapBufferItem({ 'A', 'B' }), L"" });

            // Assert that only A is orphaned
            Assert::AreEqual((size_t)1, LoadingAndSavingRemappingHelper::GetOrphanedKeys(remapBuffer).size());
            Assert::AreEqual(static_cast<DWORD>('A'), LoadingAndSavingRemappingHelper::GetOrphanedKeys(remapBuffer)[0]);
        }

        // Test if the GetOrphanedKeys method return an empty vector on passing swapped key remaps
        TEST_METHOD (GetOrphanedKeys_ShouldReturnEmptyVector_OnPassingSwappedKeyRemap)
        {
            RemapBuffer remapBuffer;

            // Remap A to B and B to A
            remapBuffer.push_back({ RemapBufferItem({ 'A', 'B' }), L"" });
            remapBuffer.push_back({ RemapBufferItem({ 'B', 'A' }), L"" });

            // Assert that there are no orphaned keys
            Assert::IsTrue(LoadingAndSavingRemappingHelper::GetOrphanedKeys(remapBuffer).empty());
        }

        // Test if the GetOrphanedKeys method return one orphaned on passing two key remaps where one key is mapped to a remapped key
        TEST_METHOD (GetOrphanedKeys_ShouldReturnOneOrphanedKey_OnPassingTwoKeyRemapsWhereOneKeyIsMappedToARemappedKey)
        {
            RemapBuffer remapBuffer;

            // Remap A to Ctrl+B and C to A
            Shortcut dest1;
            dest1.SetKey(VK_CONTROL);
            dest1.SetKey('B');
            remapBuffer.push_back({ RemapBufferItem({ 'A', dest1 }), L"" });
            remapBuffer.push_back({ RemapBufferItem({ 'C', 'A' }), L"" });

            // Assert that only C is orphaned
            Assert::AreEqual((size_t)1, LoadingAndSavingRemappingHelper::GetOrphanedKeys(remapBuffer).size());
            Assert::AreEqual(static_cast<DWORD>('C'), LoadingAndSavingRemappingHelper::GetOrphanedKeys(remapBuffer)[0]);
        }

        // Test if the PreProcessRemapTable method combines all the modifier pairs when the left and right modifiers are remapped to the same target
        TEST_METHOD (PreProcessRemapTable_ShouldCombineAllPairs_OnPassingLeftAndRightModifiersRemappedToTheSameTarget)
        {
            SingleKeyRemapTable remapTable;

            // Remap LCtrl and RCtrl to A, LAlt and RAlt to B, LShift and RShift to C, LWin and RWin to D
            remapTable[VK_LCONTROL] = 'A';
            remapTable[VK_RCONTROL] = 'A';
            remapTable[VK_LMENU] = 'B';
            remapTable[VK_RMENU] = 'B';
            remapTable[VK_LSHIFT] = 'C';
            remapTable[VK_RSHIFT] = 'C';
            remapTable[VK_LWIN] = 'D';
            remapTable[VK_RWIN] = 'D';

            // Pre process table
            LoadingAndSavingRemappingHelper::PreProcessRemapTable(remapTable);

            // Expected Ctrl remapped to A, Alt to B, Shift to C, Win to D
            SingleKeyRemapTable expectedTable;
            expectedTable[VK_CONTROL] = 'A';
            expectedTable[VK_MENU] = 'B';
            expectedTable[VK_SHIFT] = 'C';
            expectedTable[CommonSharedConstants::VK_WIN_BOTH] = 'D';

            bool areTablesEqual = (expectedTable == remapTable);
            Assert::IsTrue(areTablesEqual);
        }

        // Test if the PreProcessRemapTable method does not combines any of the modifier pairs when the left and right modifiers are remapped to different targets
        TEST_METHOD (PreProcessRemapTable_ShouldNotCombineAnyPairs_OnPassingLeftAndRightModifiersRemappedToTheDifferentTargets)
        {
            SingleKeyRemapTable remapTable;

            // Remap left modifiers to A and right modifiers to B
            remapTable[VK_LCONTROL] = 'A';
            remapTable[VK_RCONTROL] = 'B';
            remapTable[VK_LMENU] = 'A';
            remapTable[VK_RMENU] = 'B';
            remapTable[VK_LSHIFT] = 'A';
            remapTable[VK_RSHIFT] = 'B';
            remapTable[VK_LWIN] = 'A';
            remapTable[VK_RWIN] = 'B';

            // Pre process table
            LoadingAndSavingRemappingHelper::PreProcessRemapTable(remapTable);

            // Expected unchanged table
            SingleKeyRemapTable expectedTable;
            expectedTable[VK_LCONTROL] = 'A';
            expectedTable[VK_RCONTROL] = 'B';
            expectedTable[VK_LMENU] = 'A';
            expectedTable[VK_RMENU] = 'B';
            expectedTable[VK_LSHIFT] = 'A';
            expectedTable[VK_RSHIFT] = 'B';
            expectedTable[VK_LWIN] = 'A';
            expectedTable[VK_RWIN] = 'B';

            bool areTablesEqual = (expectedTable == remapTable);
            Assert::IsTrue(areTablesEqual);
        }

        // Test if the ApplySingleKeyRemappings method resets the keyboard manager state's single key remappings on passing an empty buffer
        TEST_METHOD (ApplySingleKeyRemappings_ShouldResetSingleKeyRemappings_OnPassingEmptyBuffer)
        {
            MappingConfiguration testShortcuts;
            RemapBuffer remapBuffer;

            // Remap A to B
            testShortcuts.AddSingleKeyRemap('A', 'B');

            // Apply the single key remaps from the buffer to the keyboard manager state variable
            LoadingAndSavingRemappingHelper::ApplySingleKeyRemappings(testShortcuts, remapBuffer, false);

            // Assert that single key remapping in the kbm state variable is empty
            Assert::AreEqual((size_t)0, testShortcuts.singleKeyReMap.size());
        }

        // Test if the ApplySingleKeyRemappings method copies only the valid remappings to the keyboard manager state variable when some of the remappings are invalid
        TEST_METHOD (ApplySingleKeyRemappings_ShouldCopyOnlyValidRemappings_OnPassingBufferWithSomeInvalidRemappings)
        {
            MappingConfiguration testShortcuts;
            RemapBuffer remapBuffer;

            // Add A->B, B->Ctrl+V, C to incomplete shortcut and D to incomplete key remappings to the buffer
            Shortcut s1;
            s1.SetKey(VK_CONTROL);
            s1.SetKey('V');
            Shortcut s2;
            s2.SetKey(VK_LMENU);
            remapBuffer.push_back({ RemapBufferItem({ 'A', 'B' }), L"" });
            remapBuffer.push_back({ RemapBufferItem({ 'B', s1 }), L"" });
            remapBuffer.push_back({ RemapBufferItem({ 'C', VK_NULL }), L"" });
            remapBuffer.push_back({ RemapBufferItem({ 'D', s2 }), L"" });

            // Apply the single key remaps from the buffer to the keyboard manager state variable
            LoadingAndSavingRemappingHelper::ApplySingleKeyRemappings(testShortcuts, remapBuffer, false);

            // Expected A remapped to B, B remapped to Ctrl+V
            SingleKeyRemapTable expectedTable;
            expectedTable['A'] = 'B';
            expectedTable['B'] = s1;

            bool areTablesEqual = (expectedTable == testShortcuts.singleKeyReMap);
            Assert::IsTrue(areTablesEqual);
        }

        // Test if the ApplySingleKeyRemappings method splits common modifiers to their left and right version when copying to the keyboard manager state variable if remappings from common modifiers are passed
        TEST_METHOD (ApplySingleKeyRemappings_ShouldSplitRemappingsFromCommonModifiers_OnPassingBufferWithSomeMappingsFromCommonModifiers)
        {
            MappingConfiguration testShortcuts;
            RemapBuffer remapBuffer;

            // Add Ctrl->A, Alt->B, Shift->C and Win->D remappings to the buffer
            remapBuffer.push_back({ RemapBufferItem({ VK_CONTROL, 'A' }), L"" });
            remapBuffer.push_back({ RemapBufferItem({ VK_MENU, 'B' }), L"" });
            remapBuffer.push_back({ RemapBufferItem({ VK_SHIFT, 'C' }), L"" });
            remapBuffer.push_back({ RemapBufferItem({ (DWORD)CommonSharedConstants::VK_WIN_BOTH, 'D' }), L"" });

            // Apply the single key remaps from the buffer to the keyboard manager state variable
            LoadingAndSavingRemappingHelper::ApplySingleKeyRemappings(testShortcuts, remapBuffer, false);

            // Expected LCtrl/RCtrl remapped to A, LAlt/RAlt to B, LShift/RShift to C, LWin/RWin to D
            SingleKeyRemapTable expectedTable;
            expectedTable[VK_LCONTROL] = 'A';
            expectedTable[VK_RCONTROL] = 'A';
            expectedTable[VK_LMENU] = 'B';
            expectedTable[VK_RMENU] = 'B';
            expectedTable[VK_LSHIFT] = 'C';
            expectedTable[VK_RSHIFT] = 'C';
            expectedTable[VK_LWIN] = 'D';
            expectedTable[VK_RWIN] = 'D';

            bool areTablesEqual = (expectedTable == testShortcuts.singleKeyReMap);
            Assert::IsTrue(areTablesEqual);
        }

        // Test if the ApplyShortcutRemappings method resets the keyboard manager state's os level and app specific shortcut remappings on passing an empty buffer
        TEST_METHOD (ApplyShortcutRemappings_ShouldResetShortcutRemappings_OnPassingEmptyBuffer)
        {
            MappingConfiguration testShortcuts;
            RemapBuffer remapBuffer;

            // Remap Ctrl+A to Ctrl+B for all apps and Ctrl+C to Alt+V for testApp1
            Shortcut src1;
            src1.SetKey(VK_CONTROL);
            src1.SetKey('A');
            Shortcut dest1;
            dest1.SetKey(VK_CONTROL);
            dest1.SetKey('B');
            Shortcut src2;
            src2.SetKey(VK_CONTROL);
            src2.SetKey('C');
            Shortcut dest2;
            dest2.SetKey(VK_MENU);
            dest2.SetKey('V');
            testShortcuts.AddOSLevelShortcut(src1, dest1);
            testShortcuts.AddAppSpecificShortcut(testApp1, src1, dest1);

            // Apply the shortcut remaps from the buffer to the keyboard manager state variable
            LoadingAndSavingRemappingHelper::ApplyShortcutRemappings(testShortcuts, remapBuffer, false);

            // Assert that shortcut remappings in the kbm state variable is empty
            Assert::AreEqual((size_t)0, testShortcuts.osLevelShortcutReMap.size());
            Assert::AreEqual((size_t)0, testShortcuts.appSpecificShortcutReMap.size());
        }

        // Test if the ApplyShortcutRemappings method copies only the valid remappings to the keyboard manager state variable when some of the remappings are invalid
        TEST_METHOD (ApplyShortcutRemappings_ShouldCopyOnlyValidRemappings_OnPassingBufferWithSomeInvalidRemappings)
        {
            MappingConfiguration testShortcuts;
            RemapBuffer remapBuffer;

            // Add Ctrl+A->Ctrl+B, Ctrl+C->Alt+V, Ctrl+F->incomplete shortcut and Ctrl+G->incomplete key os level remappings to buffer
            // Add Ctrl+F->Alt+V, Ctrl+G->Ctrl+B, Ctrl+A->incomplete shortcut and  Ctrl+C->incomplete key app specific remappings to buffer
            Shortcut src1;
            src1.SetKey(VK_CONTROL);
            src1.SetKey('A');
            Shortcut dest1;
            dest1.SetKey(VK_CONTROL);
            dest1.SetKey('B');
            Shortcut src2;
            src2.SetKey(VK_CONTROL);
            src2.SetKey('C');
            Shortcut dest2;
            dest2.SetKey(VK_MENU);
            dest2.SetKey('V');
            Shortcut src3;
            src3.SetKey(VK_CONTROL);
            src3.SetKey('F');
            Shortcut src4;
            src4.SetKey(VK_CONTROL);
            src4.SetKey('G');
            Shortcut dest4;
            dest4.SetKey(VK_CONTROL);
            remapBuffer.push_back({ RemapBufferItem({ src1, dest1 }), L"" });
            remapBuffer.push_back({ RemapBufferItem({ src2, dest2 }), L"" });
            remapBuffer.push_back({ RemapBufferItem({ src3, VK_NULL }), L"" });
            remapBuffer.push_back({ RemapBufferItem({ src4, dest4 }), L"" });
            remapBuffer.push_back({ RemapBufferItem({ src3, dest2 }), testApp1 });
            remapBuffer.push_back({ RemapBufferItem({ src4, dest1 }), testApp1 });
            remapBuffer.push_back({ RemapBufferItem({ src1, VK_NULL }), testApp1 });
            remapBuffer.push_back({ RemapBufferItem({ src2, dest4 }), testApp1 });

            // Apply the shortcut remaps from the buffer to the keyboard manager state variable
            LoadingAndSavingRemappingHelper::ApplyShortcutRemappings(testShortcuts, remapBuffer, false);

            // Ctrl+A->Ctrl+B and Ctrl+C->Alt+V
            ShortcutRemapTable expectedOSLevelTable;
            expectedOSLevelTable[src1] = RemapShortcut(dest1);
            expectedOSLevelTable[src2] = RemapShortcut(dest2);

            // Ctrl+F->Alt+V and Ctrl+G->Ctrl+B for testApp1
            AppSpecificShortcutRemapTable expectedAppSpecificLevelTable;
            expectedAppSpecificLevelTable[testApp1][src3] = RemapShortcut(dest2);
            expectedAppSpecificLevelTable[testApp1][src4] = RemapShortcut(dest1);

            bool areOSLevelTablesEqual = (expectedOSLevelTable == testShortcuts.osLevelShortcutReMap);
            bool areAppSpecificTablesEqual = (expectedAppSpecificLevelTable == testShortcuts.appSpecificShortcutReMap);
            Assert::IsTrue(areOSLevelTablesEqual);
            Assert::IsTrue(areAppSpecificTablesEqual);
        }
    };
}
