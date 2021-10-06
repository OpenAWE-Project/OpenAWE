<#
    Powershell installation script based on https://github.com/actions/virtual-environments/issues/18
#>

Invoke-WebRequest -Uri https://sdk.lunarg.com/sdk/download/latest/windows/vulkan-sdk.exe
$installer = Start-Process -FilePath vulkan-sdk.exe -Wait -PassThru -ArgumentList @("/S");
$installer.WaitForExit();
