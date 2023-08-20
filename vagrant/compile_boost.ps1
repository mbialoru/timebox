# Warning: Run as administrator
param(
    [Parameter(Mandatory = $False, ValueFromPipeline = $True, ValueFromPipelineByPropertyName = $True)]
    [String]$boost_version = "1.80.0",
    [String]$toolset = "gcc"
)

$boost_version_underscore = $boost_version -replace "\.", "_"
$start_directory = (pwd).path

# performance counter
$timer = [Diagnostics.Stopwatch]::StartNew()

New-Item -p ('C:\install', 'C:\boost-build', 'C:\boost') -ItemType "directory"
Invoke-WebRequest ("https://boostorg.jfrog.io/artifactory/main/release/"+$boost_version+"/source/boost_"+$boost_version_underscore+".zip") -outfile ("C:\boost_"+$boost_version_underscore+".zip")

# performance fix for otherwise slow as molasses Expand-Archive
Add-MpPreference -ExclusionPath "C:\install"
Add-Type -Assembly "System.IO.Compression.Filesystem"
[System.IO.Compression.ZipFile]::ExtractToDirectory(("C:\boost_"+$boost_version_underscore+".zip"), "C:\install")

Set-Location ("C:\install\boost_"+$boost_version_underscore+"\tools\build")
Start-Process -Wait -NoNewWindow (".\bootstrap.bat") -ArgumentList $toolset
Start-Process -Wait -NoNewWindow (".\b2") -ArgumentList ('--prefix="C:\boost-build" toolset='+$toolset+' install')
$env:PATH += ";C:\boost-build"
Set-Location ("C:\install\boost_"+$boost_version_underscore)
Start-Process -Wait -NoNewWindow ("b2") -ArgumentList ('--build-dir="C:\install\boost_'+$boost_version_underscore+'\build"'+' --build-type=complete --prefix="C:\boost" toolset='+$toolset+' install')

# cleanup
Set-Location $start_directory
Remove-Item -r -force ('C:\install', 'C:\boost-build')
$timer.Stop()
$timer.Elapsed