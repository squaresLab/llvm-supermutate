#!/bin/bash
IMAGE_NAME="llvm-supermutate/examples:foo"
VOLUME_LLVM="llvm11_opt"
VOLUME_SUPERMUTATE="llvmsupermutate_opt"
docker build -t "${IMAGE_NAME}" .
docker run \
  --rm \
  -v "${VOLUME_LLVM}:/opt/llvm11" \
  -v "${VOLUME_SUPERMUTATE}:/opt/llvmsupermutate" \
  -it \
  "${IMAGE_NAME}"
