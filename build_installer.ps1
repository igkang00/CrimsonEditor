#requires -Version 5.1
<#
.SYNOPSIS
    Build the Crimson Editor installer end-to-end.

.DESCRIPTION
    Runs every step needed to produce dist\cedt-<ver>-setup.exe:

      1. Build cedt Release-KR + Release-US (x64) via MSBuild.
      2. Build tools\launch (x64 Release) and tools\shellext (x64 Release).
      3. Make sure dist\redist\vc_redist.x64.exe is present (download if not).
      4. Invoke ISCC on installer.iss.

    The script tries hard to fail loudly and early — if any prerequisite
    is missing (MSBuild, Inno Setup) it exits with a clear message rather
    than letting ISCC complain about a missing input file later.

.PARAMETER SkipBuild
    Skip the MSBuild step. Useful when iterating on installer.iss
    against an existing build/ tree.

.PARAMETER SkipRedist
    Skip the vc_redist download check. Useful in offline / sandboxed
    runs where the file is staged some other way.

.EXAMPLE
    .\build_installer.ps1
    .\build_installer.ps1 -SkipBuild
#>

param(
    [switch]$SkipBuild,
    [switch]$SkipRedist
)

$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $MyInvocation.MyCommand.Path
Set-Location $root

function Find-MSBuild {
    $candidates = @(
        "C:\Program Files\Microsoft Visual Studio\18\Professional\MSBuild\Current\Bin\MSBuild.exe",
        "C:\Program Files\Microsoft Visual Studio\17\Professional\MSBuild\Current\Bin\MSBuild.exe",
        "C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\MSBuild.exe",
        "C:\Program Files\Microsoft Visual Studio\17\Community\MSBuild\Current\Bin\MSBuild.exe",
        "C:\Program Files (x86)\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe",
        "C:\Program Files (x86)\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"
    )
    foreach ($p in $candidates) { if (Test-Path $p) { return $p } }
    $where = (Get-Command msbuild.exe -ErrorAction SilentlyContinue)
    if ($where) { return $where.Source }
    throw "MSBuild.exe not found. Install Visual Studio 2026 (v145 toolset) or run from a VS Developer prompt."
}

function Find-ISCC {
    $candidates = @(
        "C:\Program Files (x86)\Inno Setup 6\ISCC.exe",
        "C:\Program Files\Inno Setup 6\ISCC.exe"
    )
    foreach ($p in $candidates) { if (Test-Path $p) { return $p } }
    throw "ISCC.exe (Inno Setup 6) not found. Install from https://jrsoftware.org/isdl.php."
}

function Invoke-MSBuild {
    param([string]$msbuild, [string]$project, [string]$config, [string]$platform)
    Write-Host "  -> $project [$config|$platform]" -ForegroundColor DarkGray
    $out = & $msbuild $project /p:Configuration=$config /p:Platform=$platform /m /nologo /v:m 2>&1
    $errs = $out | Select-String -Pattern 'error C\d|error LNK|error MSB|error RC' -CaseSensitive:$false
    if ($errs.Count -gt 0) {
        Write-Host ($out -join "`n") -ForegroundColor Red
        throw "MSBuild failed for $project [$config|$platform]"
    }
}

function Ensure-Redist {
    $dir  = Join-Path $root 'dist\redist'
    $file = Join-Path $dir  'vc_redist.x64.exe'
    if (Test-Path $file) {
        Write-Host "[3/4] vc_redist.x64.exe already present ($(((Get-Item $file).Length / 1MB).ToString('F1')) MB)" -ForegroundColor Cyan
        return
    }
    Write-Host "[3/4] Downloading vc_redist.x64.exe..." -ForegroundColor Cyan
    New-Item -ItemType Directory -Force -Path $dir | Out-Null
    # Microsoft's permanent redirect URL for the current x64 redist.
    $url = 'https://aka.ms/vs/17/release/vc_redist.x64.exe'
    Invoke-WebRequest -Uri $url -OutFile $file -UseBasicParsing
    Write-Host "      Saved to $file ($(((Get-Item $file).Length / 1MB).ToString('F1')) MB)" -ForegroundColor DarkGray
}

# ---------------------------------------------------------------------------

$msbuild = Find-MSBuild
$iscc    = Find-ISCC

Write-Host "MSBuild : $msbuild" -ForegroundColor DarkGray
Write-Host "ISCC    : $iscc"    -ForegroundColor DarkGray
Write-Host ""

if (-not $SkipBuild) {
    Write-Host "[1/4] Building cedt (Release-KR, Release-US — x64)..." -ForegroundColor Cyan
    Invoke-MSBuild $msbuild 'cedt.sln' 'Release-KR' 'x64'
    Invoke-MSBuild $msbuild 'cedt.sln' 'Release-US' 'x64'

    Write-Host "[2/4] Building helper tools (launch.exe, ShellExt.dll — x64)..." -ForegroundColor Cyan
    Invoke-MSBuild $msbuild 'tools\launch\launch.vcxproj'     'Release' 'x64'
    Invoke-MSBuild $msbuild 'tools\shellext\shellext.vcxproj' 'Release' 'x64'
} else {
    Write-Host "[1-2/4] -SkipBuild: assuming build\ and tools\*\build\ are up to date." -ForegroundColor Yellow
}

if (-not $SkipRedist) {
    Ensure-Redist
} else {
    Write-Host "[3/4] -SkipRedist: not checking dist\redist\vc_redist.x64.exe" -ForegroundColor Yellow
    if (-not (Test-Path (Join-Path $root 'dist\redist\vc_redist.x64.exe'))) {
        throw "dist\redist\vc_redist.x64.exe is missing — installer.iss [Files] section will fail. Re-run without -SkipRedist."
    }
}

Write-Host "[4/4] Compiling installer.iss with ISCC..." -ForegroundColor Cyan
& $iscc installer.iss
if ($LASTEXITCODE -ne 0) { throw "ISCC failed (exit $LASTEXITCODE)" }

$setup = Get-ChildItem -Path (Join-Path $root 'dist') -Filter 'cedt-*-setup.exe' -File |
         Sort-Object LastWriteTime -Descending | Select-Object -First 1
if ($setup) {
    $sizeMB = ($setup.Length / 1MB).ToString('F1')
    Write-Host ""
    Write-Host "OK -> $($setup.FullName) ($sizeMB MB)" -ForegroundColor Green
} else {
    throw "ISCC reported success but no setup.exe found under dist\"
}
