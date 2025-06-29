# configuration-manager

## Dependencies

1. nlohmann_json
2. sdbus-c++

## Build

To build service and application use build.sh script

## Usage

Usage example:

Build project(it will create .json file)

Run service.

Run application.

You can change application's behavior via:

```gdbus call -e -d com.system.configurationManager   -o /com/system/configurationManager/Application/confManagerApplication1   -m com.system.configurationManager.Application.Configuration.ChangeConfiguration   "Timeout" "<uint32 50000>"```

or

```gdbus call -e -d com.system.configurationManager   -o /com/system/configurationManager/Application/confManagerApplication1   -m com.system.configurationManager.Application.Configuration.ChangeConfiguration   "TimeoutPhrase" "<'Please stop me'>"```