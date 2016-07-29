<#
    .SYNOPSIS
        This is a PowerShell script that will let you obtain a kernel driver's 
        inflight recorder (IFR) log and/or if the driver is a KMDF driver you can also
        obtain the driver's KMDF IFR log.

    .PARAMETER Service
        Required parameter. Indicates the kernel driver service whose IFR is
        being retrieved. Note this is the service name and not image name.

    .PARAMETER IncludeKmdfIfr
        Optional switch. Indicates that the kernel driver is a KMDF driver and
        the WDF IFR associated with the driver is also to be obtained.

    .PARAMETER Guid
        Optional parameter. By default the WPP trace GUID for the service is 
        determined automatically by reading the registry. You can use this parameter
        to provide your own WPP trace GUID instead.

    .DESCRIPTION
        Usage:
        GetIfr.ps1 -Service <Svc>  [-IncludeKmdfIfr] [-Guid '{...}']

        Usage Examples:

        (1) Obtain custom IFR for WinUSB. Obtain trace GUID automatically.
            GetIfr.ps1 -Service WinUsb

        (2) Obtain driver's IFR for and WDF IFR for WinUSB. Obtain trace GUID automatically.
            GetIfr.ps1 -Service WinUsb -IncludeKmdfIfr

        (3) Obtain custom IFR for and WDF IFR for WinUSB. Use provided trace GUID.
            GetIfr.ps1 -Service MyKernelService -IncludeKmdfIfr -Guid '{485E7dee-0a80-11d8-ad15-505054500707}'

    .EXAMPLE
        GetIfr.ps1 -Service <Svc> [-IncludeKmdfIfr] [-Guid '{...}']

    .EXAMPLE
        GetIfr.ps1 -Service WinUsb

    .EXAMPLE
        GetIfr.ps1 -Service WinUsb -IncludeKmdfIfr

    .EXAMPLE
        GetIfr.ps1 -Service MyKernelService -IncludeKmdfIfr -Guid '{...}'
#>
param(
    [string]$Service = "Unknown",
    [ValidatePattern('{[A-Z0-9]{8}-([A-Z0-9]{4}-){3}[A-Z0-9]{12}}')]
    [string]$Guid = "Unknown",
    [switch]$IncludeKmdfIfr
)

#
# Parameter validation
#
if ($Service -eq "Unknown") {
    Write-Warning "Must provide a service name whose logs are to be obtained"
    Write-Warning "GetIfr.ps1 -Service <ServiceName>"
    return
}

#
# Function FuncCheckService checks if the service is installed and running
#
function FuncCheckService{
    param($ServiceName)
    if ($svcObj = (Get-Service -Name $ServiceName -ErrorAction Ignore)) {
        if ($svcObj.Status -eq "Running"){
            Write-Verbose -message "$ServiceName found running."
            return 1
        }
    }
    Write-Warning "Service $ServiceName is not running. "
    Write-Warning "Note: Specify service name and not image name."
    return 0
}

#
# Service validation
#
$serviceRunning = FuncCheckService -ServiceName $Service
if ($serviceRunning -eq 0) {
    Write-Warning -message "Service $Service not found running. Exiting."
    return
}

#
# If a GUID is not supplied by the user look up registry for WPP recorder trace GUID
#
$WppRecorderGuidName = "WppRecorder_TraceGuid"
$RegPath   = "HKLM:\system\CurrentControlSet\Services\"
$RegPath = $RegPath+$Service+"\Parameters\"
if ($Guid -eq "Unknown") {
    Write-Verbose -message "Looking for $WppRecorderGuidName under key $RegPath"

    $Guid = (Get-ItemProperty -path $RegPath -Name $WppRecorderGuidName -ErrorAction Ignore).$WppRecorderGuidName
    if (!$Guid) {
        Write-Warning -Message "$WppRecorderGuidName not found under $RegPath Cannot capture WPP recorder trace."
        Write-Warning -Message "You can supply a GUID yourself using the -Guid switch"
        return
    }
}

#
# Given the GUID, call logman to get the ETL files for WPP recorder traces
#
$SessionName = "WppRecorderTraceSession"
$WppRecorderGuid = "{772013fb-617e-4269-a691-66a6847d2856}"
$LogmanCreateArgs = "create trace $SessionName -o  $Service.etl -ets"
$LogmanUpdateArgument1 = "update $SessionName -p $WppRecorderGuid 0xff 0xff -ets" 
$LogmanUpdateArgument2 = "update $SessionName -p $Guid 0x40000000 0xff -ets" 
$LogmanStopArgument =  "stop $SessionName -ets"

Write-Verbose -message "Launching logman.exe $LogmanCreateArgs"
start-process logman.exe -WindowStyle Hidden -ArgumentList $LogmanCreateArgs -wait

Write-Verbose -message "Launching logman.exe $LogmanUpdateArgument1"
start-process logman.exe -WindowStyle Hidden -ArgumentList $LogmanUpdateArgument1 -wait

Write-Verbose -message "Launching logman.exe $LogmanUpdateArgument2"
start-process logman.exe -WindowStyle Hidden -ArgumentList $LogmanUpdateArgument2 -wait

Write-Verbose -message "Launching logman.exe $LogmanStopArgument"
start-process logman.exe -WindowStyle Hidden -ArgumentList $LogmanStopArgument -wait

Write-Host "WppRecorder Log written to $Service.etl"


if ($IncludeKmdfIfr) {
    #
    # Try to get to the KMDF IFR
    #
    Write-Verbose -message "Attempting to get to KMDF IFR"

    $KmdfSessionName = "KmdfWppRecorderTraceSession"
    $KmdfIfrRegPath =  "HKLM:\System\CurrentControlSet\Control\Wdf"
    $KmdfWppTraceGuid = "{544D4C9D-942C-46D5-BF50-DF5CD9524A50}"
    $KmdfLogmanCreateArgs = "create trace $KmdfSessionName -o  KmdfIfrFor$Service.etl -ets"
    $KmdfLogmanUpdateArgument1 = "update $KmdfSessionName -p $KmdfWppTraceGuid 0x200000 0xff -ets" 
    $KmdfLogmanStopArgument =  "stop $KmdfSessionName -ets"

    #
    # Set the multi sz reg. key and then start a trace session
    # 
    Clear-ItemProperty -Path $KmdfIfrRegPath -Name WdfIfrCaptureServiceList -ErrorAction Ignore -InformationAction SilentlyContinue
    $temp = New-ItemProperty -Path $KmdfIfrRegPath -Name WdfIfrCaptureServiceList -PropertyType MultiString -Value $Service -Force -InformationAction SilentlyContinue

    Write-Verbose -message "Launching logman.exe $KmdfLogmanCreateArgs"
    start-process logman.exe -WindowStyle Hidden -ArgumentList $KmdfLogmanCreateArgs -wait

    Write-Verbose -message "Launching logman.exe $KmdfLogmanUpdateArgument1"
    start-process logman.exe -WindowStyle Hidden -ArgumentList $KmdfLogmanUpdateArgument1 -wait

    Write-Verbose -message "Launching logman.exe $KmdfLogmanStopArgument"
    start-process logman.exe -WindowStyle Hidden -ArgumentList $KmdfLogmanStopArgument -wait

    Write-Host "KMDF IFR log written to KmdfIfrFor$Service.etl"
}


