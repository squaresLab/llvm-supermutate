#!/bin/bash
HERE_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

IMAGE_NAME="llvm-supermutate/examples:foo"
VOLUME_LLVM="llvm11_opt"
VOLUME_SUPERMUTATE="llvmsupermutate_opt"

pushd "${HERE_DIR}"
docker build -t "${IMAGE_NAME}" .
docker run \
  --rm \
  -v "${VOLUME_LLVM}:/opt/llvm11" \
  -v "${VOLUME_SUPERMUTATE}:/opt/llvmsupermutate" \
  -it \
  "${IMAGE_NAME}"
