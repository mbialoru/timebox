# Assuming all went well and choco is available

# Make `refreshenv` available right away, by defining the $env:ChocolateyInstall
# variable and importing the Chocolatey profile module.
# NOTE: Using `. $PROFILE` instead *may* work, but isn't guaranteed to.
$env:ChocolateyInstall = Convert-Path "$((Get-Command choco).Path)\..\.."
Import-Module "$env:ChocolateyInstall\helpers\chocolateyProfile.psm1"

choco install -y git
choco install -y cmake.install --installargs '"ADD_CMAKE_TO_PATH=System"'

# TODO: cmake requires nmake - where the fuck do I get it ?

# refreshenv is now an alias for Update-SessionEnvironment
# (rather than invoking refreshenv.cmd, the *batch file* for use with cmd.exe)
# This should make programs accessible via the refreshed $env:PATH, so that they
# can be called by name only.
refreshenv