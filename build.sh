#!/bin/bash

PROJECT_ROOT=$(pwd)
SRC_DIR="${PROJECT_ROOT}/src"
BUILD_DIR="${PROJECT_ROOT}/build"
BIN_DIR="${PROJECT_ROOT}/bin"

mkdir -p "${BUILD_DIR}/server_side"
mkdir -p "${BUILD_DIR}/client_side"
mkdir -p "${BIN_DIR}"

echo "Building server_side..."
cd "${BUILD_DIR}/server_side"
cmake "${SRC_DIR}/server_side" -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
if [ -f configuration_manager_service ]; then
    cp configuration_manager_service "${BIN_DIR}/"
    echo "Server built: ${BIN_DIR}/configuration_manager_service"
else
    echo "Error: Server executable not found!"
    exit 1
fi

echo -e "\nBuilding client_side..."
cd "${BUILD_DIR}/client_side"
cmake "${SRC_DIR}/client_side" -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
if [ -f conf_manager_application ]; then
    cp conf_manager_application "${BIN_DIR}/"
    echo "Client built: ${BIN_DIR}/conf_manager_application"
else
    echo "Error: Client executable not found!"
    exit 1
fi

echo -e "\nBuild complete! Binaries are in: ${BIN_DIR}"