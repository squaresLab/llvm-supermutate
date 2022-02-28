#!/bin/bash
set -eu

DIR_EXAMPLE="/example"

export LLVM11_DIR="/opt/llvm11"
export LLVMSUPERMUTATE_DIR="/opt/llvmsupermutate"
export PATH="${LLVMSUPERMUTATE_DIR}/bin:${LLVM11_DIR}/bin:${PATH}"

export LD_LIBRARY_PATH="${LLVMSUPERMUTATE_DIR}/lib:${LLVM11_DIR}/lib:${LD_LIBRARY_PATH:-}"

pushd "${DIR_EXAMPLE}"

# generate .bc and .ll
clang-11 -emit-llvm -o foo.bc -c foo.c
llvm-dis -o foo.ll foo.bc

exec $@
