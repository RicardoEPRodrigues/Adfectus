#!/bin/bash

git fetch origin release

git reset --hard

git pull origin release

git checkout release

git lfs fetch