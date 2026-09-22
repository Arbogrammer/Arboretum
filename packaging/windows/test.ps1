$ErrorActionPreference = 'Stop'
# Run the packaged EXE outside MSYS2, with only Windows and bundled DLLs on PATH.
$package = Join-Path $env:GITHUB_WORKSPACE 'dist/Arboretum-Windows-x64'
$logs = Join-Path $env:GITHUB_WORKSPACE 'dist/test-logs'
New-Item -ItemType Directory -Force -Path $logs | Out-Null
$env:PATH = "$package;$env:SystemRoot\System32;$env:SystemRoot"
$env:XDG_DATA_DIRS = Join-Path $package 'share'
$env:GSETTINGS_SCHEMA_DIR = Join-Path $package 'share/glib-2.0/schemas'
$env:ARBORETUM_DIAGNOSTIC = '1'
$env:GSK_RENDERER = 'cairo'
Remove-Item Env:GTK_IM_MODULE -ErrorAction SilentlyContinue
$failed = $false
foreach ($test in @('KEYBOARD', 'IO')) {
    Remove-Item Env:ARBORETUM_KEYBOARD_SMOKE_TEST -ErrorAction SilentlyContinue
    Remove-Item Env:ARBORETUM_IO_SMOKE_TEST -ErrorAction SilentlyContinue
    Set-Item "Env:ARBORETUM_${test}_SMOKE_TEST" '1'
    $stdout = Join-Path $logs "$test.stdout.txt"
    $stderr = Join-Path $logs "$test.stderr.txt"
    $process = Start-Process (Join-Path $package 'arboretum.exe') -WorkingDirectory $package `
        -PassThru -RedirectStandardOutput $stdout -RedirectStandardError $stderr
    if (-not $process.WaitForExit(90000)) {
        Stop-Process -Id $process.Id -Force
        $failed = $true
        Write-Warning "$test test timed out"
    } else {
        $process.WaitForExit()
        if ($process.ExitCode -ne 0) { $failed = $true }
        Write-Host "$test exit code: $($process.ExitCode)"
    }
    Get-Content $stdout, $stderr
}
if ($failed) { throw 'Windows runtime test failed; see test-logs artifact.' }
