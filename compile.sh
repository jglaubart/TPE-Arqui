#!/bin/bash
set -euo pipefail

CONTAINER_NAME="tpe"
IMAGE_NAME="agodio/itba-so:2.0"
PROJECT_ROOT="$(pwd)"
CONTAINER_WORKDIR="/root"
TOOLCHAIN_DIR="${CONTAINER_WORKDIR}/Toolchain"

BUILD_COMMANDS="make clean -C ${TOOLCHAIN_DIR} && make clean -C ${CONTAINER_WORKDIR} && make -C ${TOOLCHAIN_DIR} && make -C ${CONTAINER_WORKDIR}"

EXISTING_CONTAINER=$(docker ps -aq -f "name=^${CONTAINER_NAME}$")
if [ -z "$EXISTING_CONTAINER" ]; then
    echo "Creando contenedor ${CONTAINER_NAME} con la imagen ${IMAGE_NAME}."
    docker run -dit \
        --name "${CONTAINER_NAME}" \
        -v "${PROJECT_ROOT}:${CONTAINER_WORKDIR}" \
        -w "${CONTAINER_WORKDIR}" \
        "${IMAGE_NAME}" bash >/dev/null
fi

RUNNING_CONTAINER=$(docker ps -q -f "name=^${CONTAINER_NAME}$")
if [ -z "$RUNNING_CONTAINER" ]; then
    docker start "$CONTAINER_NAME" >/dev/null
fi

docker exec -it "$CONTAINER_NAME" bash -lc "$BUILD_COMMANDS"

if [ -f Image/x64BareBonesImage.qcow2 ]; then
    sudo chown "$(whoami)" Image/x64BareBonesImage.qcow2
fi
