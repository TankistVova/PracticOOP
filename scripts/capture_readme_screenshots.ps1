param(
    [string]$ExePath = "C:\Users\morgu\Desktop\AIP\PracticHub\x64\Debug\PracticHub.exe",
    [string]$OutputDir = "C:\Users\morgu\Desktop\AIP\PracticHub\docs\screenshots"
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

Add-Type -AssemblyName System.Drawing
Add-Type -AssemblyName System.Windows.Forms
Add-Type -AssemblyName Microsoft.VisualBasic

Add-Type @"
using System;
using System.Runtime.InteropServices;

public static class WinApi
{
    [StructLayout(LayoutKind.Sequential)]
    public struct RECT
    {
        public int Left;
        public int Top;
        public int Right;
        public int Bottom;
    }

    [DllImport("user32.dll")]
    public static extern bool SetForegroundWindow(IntPtr hWnd);

    [DllImport("user32.dll")]
    public static extern bool ShowWindowAsync(IntPtr hWnd, int nCmdShow);

    [DllImport("user32.dll")]
    public static extern bool GetWindowRect(IntPtr hWnd, out RECT rect);
}
"@

function Wait-ForMainWindow {
    param(
        [Parameter(Mandatory = $true)]
        [System.Diagnostics.Process]$Process,
        [int]$TimeoutSeconds = 15
    )

    $deadline = (Get-Date).AddSeconds($TimeoutSeconds)
    while ((Get-Date) -lt $deadline) {
        $Process.Refresh()
        if ($Process.MainWindowHandle -ne 0) {
            return $Process.MainWindowHandle
        }

        Start-Sleep -Milliseconds 400
    }

    throw "Main window was not created in time."
}

function Activate-Window {
    param(
        [Parameter(Mandatory = $true)]
        [System.Diagnostics.Process]$Process,
        [Parameter(Mandatory = $true)]
        [IntPtr]$Handle
    )

    [WinApi]::ShowWindowAsync($Handle, 5) | Out-Null
    [Microsoft.VisualBasic.Interaction]::AppActivate($Process.Id) | Out-Null
    [WinApi]::SetForegroundWindow($Handle) | Out-Null
    Start-Sleep -Milliseconds 800
}

function Save-WindowCapture {
    param(
        [Parameter(Mandatory = $true)]
        [IntPtr]$Handle,
        [Parameter(Mandatory = $true)]
        [string]$Path
    )

    $rect = New-Object WinApi+RECT
    [WinApi]::GetWindowRect($Handle, [ref]$rect) | Out-Null

    $width = $rect.Right - $rect.Left
    $height = $rect.Bottom - $rect.Top
    if ($width -le 0 -or $height -le 0) {
        throw "Window rectangle is invalid."
    }

    $bitmap = New-Object System.Drawing.Bitmap $width, $height
    $graphics = [System.Drawing.Graphics]::FromImage($bitmap)
    $graphics.CopyFromScreen($rect.Left, $rect.Top, 0, 0, $bitmap.Size)
    $bitmap.Save($Path, [System.Drawing.Imaging.ImageFormat]::Png)
    $graphics.Dispose()
    $bitmap.Dispose()
}

New-Item -ItemType Directory -Force -Path $OutputDir | Out-Null

$process = Start-Process -FilePath $ExePath -PassThru

try {
    Start-Sleep -Seconds 3
    try {
        $null = $process.WaitForInputIdle(10000)
    }
    catch {
    }

    $handle = Wait-ForMainWindow -Process $process
    Activate-Window -Process $process -Handle $handle

    $captures = @(
        @{ Name = "practice-01.png"; Tabs = 0 },
        @{ Name = "practice-02.png"; Tabs = 1 },
        @{ Name = "practice-03.png"; Tabs = 1 },
        @{ Name = "practice-07.png"; Tabs = 4 },
        @{ Name = "practice-08.png"; Tabs = 1 },
        @{ Name = "practice-09.png"; Tabs = 1 }
    )

    foreach ($capture in $captures) {
        for ($i = 0; $i -lt $capture.Tabs; $i++) {
            [System.Windows.Forms.SendKeys]::SendWait("^{TAB}")
            Start-Sleep -Milliseconds 700
        }

        Activate-Window -Process $process -Handle $handle
        Save-WindowCapture -Handle $handle -Path (Join-Path $OutputDir $capture.Name)
    }
}
finally {
    if ($null -ne $process -and -not $process.HasExited) {
        $null = $process.CloseMainWindow()
        Start-Sleep -Seconds 2

        if (-not $process.HasExited) {
            Stop-Process -Id $process.Id -Force
        }
    }
}
