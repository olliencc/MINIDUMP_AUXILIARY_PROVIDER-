# MINIDUMP_AUXILIARY_PROVIDER Research

## Background
Within modern versions of Microsoft Windows there is an undocumented feature of MiniDumpWriteDump found whilst reading the .NET CoreCLR code.

## Example - Unsigned 

Under `Demo` in this project there are two binaries
* ExampleDLL.dll - this gets loaded when a MiniDump is done against MiniDump.exe
* MiniDump.exe - does a `MiniDumpWriteDump` of itself

If you then populate the following registry keys on Windows 10 as such

Under `HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node\Microsoft\Windows NT\CurrentVersion\MiniDumpAuxiliaryDlls` put
```
"C:\\Data\\!Code\\git.public\\MINIDUMP_AUXILIARY_PROVIDER-\\Demo\\Debug\\MiniDump.exe"="C:\\Data\\!Code\\git.public\\MINIDUMP_AUXILIARY_PROVIDER-\\Demo\\Debug\\ExampleDLL.dll
```

Under `HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node\Microsoft\Windows NT\CurrentVersion\KnownManagedDebuggingDlls` put
```
"C:\\Data\\!Code\\git.public\\MINIDUMP_AUXILIARY_PROVIDER-\\Demo\\Debug\\ExampleDLL.dll"=dword:00000000
```

* `MiniDump.exe` simply causes a mini dump of itself to be written to `C:\Users\%USERNAME%\AppData\Local\Temp\MiniDump` with a `.dmp` extension
* `ExampleDLL.dll` simply causes a file to be written to `C:\Users\%USERNAME%\AppData\Local\Temp\MiniDump` with a `.run` extension in its `DllMain` function to prove execution

Then run `MiniDump.exe` you will see that `ExampleDLL.dll` is loaded and the `.run` file appears as expected.

## Example - Signed

 Under `Demo` in this project there are two binaries
* ExampleDLL.dll - this gets loaded when a MiniDump is done against MiniDump.exe
* MiniDump.exe - does a `MiniDumpWriteDump` of itself

`MiniDump.exe` has a resource section as described below

## Original Problem
It was described in the original issue (see further reading section) as:

_Today if you try to collect a mini or triage dump on Windows none of the information relevant for debugging a stack trace through jitted code is collected. This renders Windows Error Reporting largely useless, as well as causing problems for other diagnostic scenarios that would like to use minidumps._

_The root cause of the problem is .Net Core doesn't have access to the same extensibility mechanism in `MiniDumpWriteDump` that the desktop and .Net Native CLR implementations use. The current extensibility mechanism provided by the OS requires registering a dll in `HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\MiniDumpAuxiliaryDlls`. That registry location requires admin privileges and can't be set when .Net Core installation is limited to xcopy only._

## Proposed Solution

From the original issue (see further reading section):

_We have coordinated with the Windows OS team to design an additional extensibility mechanism to `MiniDumpWriteDump`. In lieu of admin privileges and registry edits, this one uses authenticode signature verification to ensure security and reads the PE resources in loaded modules for discovery. Specifically MiniDumpWriteDump will scan each module in the dump process looking for any resources with `name=DUMP_HELPER, type=RT_RCDATA, lang=neutral`. The resource contains the UTF8 encoded, null terminated name of a minidump auxilliary provider dll that is located in the same directory as the binary with the resource in it. After discovery and signature verification, MiniDumpWriteDump uses the minidump auxilliary provider no differently than if it had been loaded using the pre-existing registry technique._

## Implementation
It is implemented as such (see further reading section):

_When an application crashes, Windows MinidumpWriteDump is planning to scan modules looking for this resource. The content of the resource is expected to be the name of a dll in the same folder, encoded in UTF8, null terminated, that implements the CLRDataCreateInterface function. For OS security purposes `MinidumpWriteDump` will do an authenticode signing check before loading the indicated binary, however if your build isn't signed you can get around this limitation by registering it at `HKLM\Software\Microsoft\WindowsNT\CurrentVersion\MiniDumpAuxilliaryDll`_

## The Actual Implementation
In reality it is implemented by a resource section called:

`MINIDUMP_AUXILIARY_PROVIDER`

The scanner under `Scanner` is a small Python script which will enumerate all the files with this resource name. We see on a Windows 10 host with a couple of versions of Visual Studio installed the following files have it.

```
C:/Program Files/dotnet\shared\Microsoft.NETCore.App\2.0.9\coreclr.dll - MINIDUMP_AUXILIARY_PROVIDER
C:/Program Files/dotnet\shared\Microsoft.NETCore.App\2.1.9\coreclr.dll - MINIDUMP_AUXILIARY_PROVIDER
C:/Program Files (X86)/Microsoft SDKs\UWPNuGetPackages\microsoft.net.native.compiler\2.2.3\tools\csc\mrt150.dll - MINIDUMP_AUXILIARY_PROVIDER
C:/Program Files (X86)/Microsoft SDKs\UWPNuGetPackages\runtime.win10-arm.microsoft.net.native.compiler\2.2.3\tools\arm\ilc\mrt150.dll - MINIDUMP_AUXILIARY_PROVIDER
C:/Program Files (X86)/Microsoft SDKs\UWPNuGetPackages\runtime.win10-arm.microsoft.net.native.compiler\2.2.3\tools\arm\ilc\Lib\Runtime\mrt100_app.dll - MINIDUMP_AUXILIARY_PROVIDER
C:/Program Files (X86)/Microsoft SDKs\UWPNuGetPackages\runtime.win10-arm.microsoft.net.native.compiler\2.2.3\tools\arm\ilc\Lib\Runtime\mrt150.dll - MINIDUMP_AUXILIARY_PROVIDER
C:/Program Files (X86)/Microsoft SDKs\UWPNuGetPackages\runtime.win10-arm64.microsoft.net.native.compiler\2.2.3\tools\arm64\ilc\mrt150.dll - MINIDUMP_AUXILIARY_PROVIDER
C:/Program Files (X86)/Microsoft SDKs\UWPNuGetPackages\runtime.win10-arm64.microsoft.net.native.compiler\2.2.3\tools\arm64\ilc\Lib\Runtime\mrt100_app.dll - MINIDUMP_AUXILIARY_PROVIDER
C:/Program Files (X86)/Microsoft SDKs\UWPNuGetPackages\runtime.win10-arm64.microsoft.net.native.compiler\2.2.3\tools\arm64\ilc\Lib\Runtime\mrt150.dll - MINIDUMP_AUXILIARY_PROVIDER
C:/Program Files (X86)/Microsoft SDKs\UWPNuGetPackages\runtime.win10-x64.microsoft.net.native.compiler\2.2.3\tools\x64\ilc\mrt150.dll - MINIDUMP_AUXILIARY_PROVIDER
C:/Program Files (X86)/Microsoft SDKs\UWPNuGetPackages\runtime.win10-x64.microsoft.net.native.compiler\2.2.3\tools\x64\ilc\Lib\Runtime\mrt100_app.dll - MINIDUMP_AUXILIARY_PROVIDER
C:/Program Files (X86)/Microsoft SDKs\UWPNuGetPackages\runtime.win10-x64.microsoft.net.native.compiler\2.2.3\tools\x64\ilc\Lib\Runtime\mrt150.dll - MINIDUMP_AUXILIARY_PROVIDER
C:/Program Files (X86)/Microsoft SDKs\UWPNuGetPackages\runtime.win10-x86.microsoft.net.native.compiler\2.2.3\tools\x86\ilc\mrt150.dll - MINIDUMP_AUXILIARY_PROVIDER
C:/Program Files (X86)/Microsoft SDKs\UWPNuGetPackages\runtime.win10-x86.microsoft.net.native.compiler\2.2.3\tools\x86\ilc\Lib\Runtime\mrt100_app.dll - MINIDUMP_AUXILIARY_PROVIDER
C:/Program Files (X86)/Microsoft SDKs\UWPNuGetPackages\runtime.win10-x86.microsoft.net.native.compiler\2.2.3\tools\x86\ilc\Lib\Runtime\mrt150.dll - MINIDUMP_AUXILIARY_PROVIDER
C:/Program Files (X86)/Microsoft Visual Studio\2017\Community\CoreCon\Binaries\Phone Tools\Debugger\CoreClr\arm64\coreclr.dll - MINIDUMP_AUXILIARY_PROVIDER
```

## Anomly Observations

### You don't need to have registry entries

You can have a `version.dll` next to the binary and it will be loaded when a mini dump is run. Thus a rather poor persistance technique.

## Further Reading
* README: 
  * https://github.com/dotnet/coreclr/blob/master/src/dlls/mscoree/coreclr/README.md
* Issue: Support Windows Minidump
  * https://github.com/dotnet/coreclr/issues/10334
* Pull Request: Support Windows Minidump
  * https://github.com/dotnet/coreclr/pull/10336
  
## TODO