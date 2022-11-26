# Make `refreshenv` available right away, by defining the $env:ChocolateyInstall
# variable and importing the Chocolatey profile module.
# NOTE: Using `. $PROFILE` instead *may* work, but isn't guaranteed to.
$env:ChocolateyInstall = Convert-Path "$((Get-Command choco).Path)\..\.."
Import-Module "$env:ChocolateyInstall\helpers\chocolateyProfile.psm1"

# Make powershell the default ssh shell
New-ItemProperty -Path "HKLM:\SOFTWARE\OpenSSH" -Name DefaultShell -Value "C:\Windows\System32\WindowsPowerShell\v1.0\powershell.exe" -PropertyType String -Force

# Install packages via chocolatey
choco install -y git ccache mingw cppcheck ninja python cmake cmake.install doxygen.install pkgconfiglite
# choco install -y visualstudio2022buildtools --package-parameters "--add Microsoft.VisualStudio.Workload.VCTools;includeRecommended;includeOptional --passive --locale en-US"
# choco install -y boost-msvc-14.3

# Make Developer Powershell environent setup on powershell open by default
# NOTE: Ouput redirection to null keeps rsync working
New-item -type file -force $profile
echo "& 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\Launch-VsDevShell.ps1' | out-null" > C:\Users\vagrant\Documents\WindowsPowerShell\Microsoft.PowerShell_profile.ps1

# refreshenv is now an alias for Update-SessionEnvironment
# (rather than invoking refreshenv.cmd, the *batch file* for use with cmd.exe)
# This should make programs accessible via the refreshed $env:PATH, so that they
# can be called by name only.
refreshenv

# TODO: Enable developer mode
REG ADD "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\AppModelUnlock" /t REG_DWORD /f /v "AllowDevelopmentWithoutDevLicense" /d "1"
DISM /Online /Add-Capability /CapabilityName:Tools.DeveloperMode.Core~~~~0.0.1.0