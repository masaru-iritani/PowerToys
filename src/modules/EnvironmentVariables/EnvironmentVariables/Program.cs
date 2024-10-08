﻿// Copyright (c) Microsoft Corporation
// The Microsoft Corporation licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.

using System;
using System.Threading;

using ManagedCommon;
using Microsoft.UI.Dispatching;
using Microsoft.Windows.AppLifecycle;

namespace EnvironmentVariables
{
    public static class Program
    {
        [STAThread]
        public static void Main(string[] args)
        {
            Logger.InitializeLogger("\\EnvironmentVariables\\Logs");

            WinRT.ComWrappersSupport.InitializeComWrappers();

            if (PowerToys.GPOWrapper.GPOWrapper.GetConfiguredEnvironmentVariablesEnabledValue() == PowerToys.GPOWrapper.GpoRuleConfigured.Disabled)
            {
                Logger.LogWarning("Tried to start with a GPO policy setting the utility to always be disabled. Please contact your systems administrator.");
                return;
            }

            var instanceKey = AppInstance.FindOrRegisterForKey("PowerToys_EnvironmentVariables_Instance");

            if (instanceKey.IsCurrent)
            {
                Microsoft.UI.Xaml.Application.Start((p) =>
                {
                    var context = new DispatcherQueueSynchronizationContext(DispatcherQueue.GetForCurrentThread());
                    SynchronizationContext.SetSynchronizationContext(context);
                    _ = new App();
                });
            }
            else
            {
                Logger.LogWarning("Another instance of Environment Variables is running. Exiting.");
            }
        }
    }
}
