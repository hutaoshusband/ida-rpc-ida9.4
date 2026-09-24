# ida-rpc for IDA 9.4

## About
ida-rpc was a quick test plugin to see what changed in the 7.0 sdk, ~~subsequently it only supports IDA 7.x as of now~~ ~~updated to IDA 9.0,~~ ~~updated to IDA 9.1~~, updated to IDA 9.4
it allows for [discord rich presence](https://discordapp.com/rich-presence) to display information about the current IDA session

## Installation
To install ida-rpc simply copy **ida-rpc64.dll** from the [latest release](https://github.com/shikataganaii/ida-rpc-ida9/releases) to ```ida_install_location/plugins/``` ,
to change options within the plugin open the plugins menu and select IDA RPC ```Edit -> Plugins -> IDA RPC``` or use the default hotkey ```Ctrl-Alt-R```

## Building
Clone the [IDA 9.4 SDK](https://github.com/HexRaysSA/ida-sdk/releases/tag/v9.4.0-sdk.1) into ```idasdk94/``` next to the solution, compile in 64bit Release64 (Visual Studio 18, toolset v145) and your done!

