#!/bin/bash

set -e
set -x

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

cd "${DIR}"
git fetch
git reset --hard HEAD
git clean -xfd
git merge origin/release
git lfs fetch
