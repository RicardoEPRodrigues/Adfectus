#!/bin/bash

set -e
set -x

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
BUILD_DIR="${DIR}/Build/"
PROJECT="${DIR}/Adfectus.uproject"

UE_DIR="/Users/Shared/Epic Games/UE_5.1"
UE_UAT="${UE_DIR}/Engine/Build/BatchFiles/RunUAT.command"
PLATFORM="Mac"

cd "${UE_DIR}"
"${UE_UAT}" BuildCookRun \
		-project="${PROJECT}" -target=CPPThirdPerson -clientconfig=Shipping \
		-package -platform=${PLATFORM} \
		-archive -archivedirectory="${BUILD_DIR}" \
		-compile -build -cook -stage \
		-nop4 -iostore -pak -prereqs -nodebuginfo -installed -compressed
