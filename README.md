# configuration-manager

## Dependencies

1. nlohmann_json
2. sdbus-c++

## Build

To build service and application use build.sh script

## Usage

Usage example:

Run service.



gdbus call -e -d com.system.configurationManager   -o /com/system/configurationManager/Application/confManagerApplication1   -m com.system.configurationManager.Application.Configuration.ChangeConfiguration   "Timeout" "<uint32 50000>"

gdbus call -e -d com.system.configurationManager   -o /com/system/configurationManager/Application/confManagerApplication1   -m com.system.configurationManager.Application.Configuration.ChangeConfiguration   "TimeoutPhrase" "<'Please stop me'>"