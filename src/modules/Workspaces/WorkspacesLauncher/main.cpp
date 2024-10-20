#include "pch.h"

#include <WorkspacesLib/JsonUtils.h>
#include <WorkspacesLib/utils.h>

#include <Launcher.h>

#include <Generated Files/resource.h>

#include <common/utils/elevation.h>
#include <common/utils/gpo.h>
#include <common/utils/logger_helper.h>
#include <common/utils/process_path.h>
#include <common/utils/UnhandledExceptionHandler.h>
#include <common/utils/resources.h>

const std::wstring moduleName = L"Workspaces\\WorkspacesLauncher";
const std::wstring internalPath = L"";

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, LPSTR cmdline, int cmdShow)
{
    LoggerHelpers::init_logger(moduleName, internalPath, LogSettings::workspacesLauncherLoggerName);
    InitUnhandledExceptionHandler();  

    if (powertoys_gpo::getConfiguredWorkspacesEnabledValue() == powertoys_gpo::gpo_rule_configured_disabled)
    {
        Logger::warn(L"Tried to start with a GPO policy setting the utility to always be disabled. Please contact your systems administrator.");
        return 0;
    }

    if (is_process_elevated())
    {
        Logger::warn("Workspaces Launcher is elevated, restart");

        constexpr DWORD exe_path_size = 0xFFFF;
        auto exe_path = std::make_unique<wchar_t[]>(exe_path_size);
        GetModuleFileNameW(nullptr, exe_path.get(), exe_path_size);

        const auto modulePath = get_module_folderpath();
        
        std::string cmdLineStr(cmdline);
        std::wstring cmdLineWStr(cmdLineStr.begin(), cmdLineStr.end());

        run_non_elevated(exe_path.get(), cmdLineWStr, nullptr, modulePath.c_str());
        return 1;
    }

    // COM should be initialized before ShellExecuteEx is called.
    if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED)))
    {
        Logger::error("CoInitializeEx failed");
        return 1;
    }

    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    
    std::wstring cmdLineStr{ GetCommandLineW()  };
    auto cmdArgs = split(cmdLineStr, L" ");
    if (cmdArgs.size() < 2)
    {
        Logger::warn("Incorrect command line arguments");
        MessageBox(NULL, GET_RESOURCE_STRING(IDS_INCORRECT_ARGS).c_str(), GET_RESOURCE_STRING(IDS_WORKSPACES).c_str(), MB_ICONERROR | MB_OK);
        return 1;
    }
    
    std::wstring id(cmdArgs[1].begin(), cmdArgs[1].end());
    if (id.empty())
    {
        Logger::warn("Incorrect command line arguments: no workspace id");
        MessageBox(NULL, GET_RESOURCE_STRING(IDS_INCORRECT_ARGS).c_str(), GET_RESOURCE_STRING(IDS_WORKSPACES).c_str(), MB_ICONERROR | MB_OK);
        return 1;
    }

    InvokePoint invokePoint = InvokePoint::EditorButton;
    if (cmdArgs.size() > 2)
    {
        try
        {
            invokePoint = static_cast<InvokePoint>(std::stoi(cmdArgs[2]));
        }
        catch (std::exception)
        {
        }
    }

    Logger::trace(L"Invoke point: {}", invokePoint);

    // read workspaces
    std::vector<WorkspacesData::WorkspacesProject> workspaces;
    WorkspacesData::WorkspacesProject projectToLaunch{};
    if (invokePoint == InvokePoint::LaunchAndEdit)
    {
        // check the temp file in case the project is just created and not saved to the workspaces.json yet
        auto file = WorkspacesData::TempWorkspacesFile();
        auto res = JsonUtils::ReadSingleWorkspace(file);
        if (res.isOk() && projectToLaunch.id == id)
        {
            projectToLaunch = res.getValue();
        }
        else
        {
            std::wstring formattedMessage{};
            switch (res.error())
            {
            case JsonUtils::WorkspacesFileError::FileReadingError:
                formattedMessage = fmt::format(GET_RESOURCE_STRING(IDS_FILE_READING_ERROR), file);
                break;
            case JsonUtils::WorkspacesFileError::IncorrectFileError:
                formattedMessage = fmt::format(GET_RESOURCE_STRING(IDS_INCORRECT_FILE_ERROR), file);
                break;
            }
             
            MessageBox(NULL, formattedMessage.c_str(), GET_RESOURCE_STRING(IDS_WORKSPACES).c_str(), MB_ICONERROR | MB_OK);
            return 1;
        }
    }
    
    if (projectToLaunch.id.empty())
    {
        auto file = WorkspacesData::WorkspacesFile();
        auto res = JsonUtils::ReadWorkspaces(file);
        if (res.isOk())
        {
            workspaces = res.getValue();
        }
        else
        {
            std::wstring formattedMessage{};
            switch (res.error())
            {
            case JsonUtils::WorkspacesFileError::FileReadingError:
                formattedMessage = fmt::format(GET_RESOURCE_STRING(IDS_FILE_READING_ERROR), file);
                break;
            case JsonUtils::WorkspacesFileError::IncorrectFileError:
                formattedMessage = fmt::format(GET_RESOURCE_STRING(IDS_INCORRECT_FILE_ERROR), file);
                break;
            }

            MessageBox(NULL, formattedMessage.c_str(), GET_RESOURCE_STRING(IDS_WORKSPACES).c_str(), MB_ICONERROR | MB_OK);
            return 1;
        }

        if (workspaces.empty())
        {
            Logger::warn("Workspaces file is empty");
            std::wstring formattedMessage = fmt::format(GET_RESOURCE_STRING(IDS_EMPTY_FILE), file);
            MessageBox(NULL, formattedMessage.c_str(), GET_RESOURCE_STRING(IDS_WORKSPACES).c_str(), MB_ICONERROR | MB_OK);
            return 1;
        }

        for (const auto& proj : workspaces)
        {
            if (proj.id == id)
            {
                projectToLaunch = proj;
                break;
            }
        }
    }

    if (projectToLaunch.id.empty())
    {
        Logger::critical(L"Workspace {} not found", id);
        std::wstring formattedMessage = fmt::format(GET_RESOURCE_STRING(IDS_PROJECT_NOT_FOUND), id);
        MessageBox(NULL, formattedMessage.c_str(), GET_RESOURCE_STRING(IDS_WORKSPACES).c_str(), MB_ICONERROR | MB_OK);
        return 1;
    }

    Launcher launcher(projectToLaunch, workspaces, invokePoint);

    Logger::trace("Finished");
    CoUninitialize();
    return 0;
}
