#!/bin/bash

set -e
set +x

echo "Building Adfectus for Linux..."

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
BUILD_DIR="${DIR}/Build/"
PROJECT="${DIR}/Adfectus.uproject"

UE_DIR="/home/ricardo/Documents/Unreal Engine/5.1.0"
UE_UAT="${UE_DIR}/Engine/Build/BatchFiles/RunUAT.sh"
PLATFORM="Linux"

# Get unreal engine version from uproject file.
UE_VERSION=$(grep "EngineAssociation" "${PROJECT}" | cut -d '"' -f 4)

echo "Adfectus will use Unreal Engine ${UE_VERSION}..."

# load unreal engine installs from ~/.config/Epic/UnrealEngine/Install.ini

if [ ! -f ~/.config/Epic/UnrealEngine/Install.ini ]; then
	echo "No Unreal Engine installations found. Please install Unreal Engine ${UE_VERSION}."
	exit 1
fi

# Get the path to the Unreal Engine version.
UE_DIR=$(grep "${UE_VERSION}=" ~/.config/Epic/UnrealEngine/Install.ini | cut -d '=' -f 2)

if [ -z "${UE_DIR}" ]; then
	echo "Unreal Engine ${UE_VERSION} not found. Please install Unreal Engine ${UE_VERSION}."
	exit 1
fi

# Get the path to the Unreal Engine UAT.
UE_UAT="${UE_DIR}/Engine/Build/BatchFiles/RunUAT.sh"

if [ -z "${UE_UAT}" ]; then
	echo "Unreal Engine ${UE_VERSION} UAT not found. Please install Unreal Engine ${UE_VERSION}."
	exit 1
fi

echo "Found Unreal Engine ${UE_VERSION}. Building..."

cd "${UE_DIR}"
set -x
"${UE_UAT}" BuildCookRun \
		-project="${PROJECT}" -target=CPPThirdPerson -clientconfig=Shipping \
		-package -platform=${PLATFORM} \
		-archive -archivedirectory="${BUILD_DIR}" \
		-compile -build -cook -stage \
		-nop4 -iostore -pak -prereqs -nodebuginfo -installed -compressed