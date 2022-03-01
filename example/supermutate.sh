#!/bin/bash
set -eu

/opt/llvmsupermutate/bin/llvmsupermutate \
  --mutate-function main \
  --mutated-filename supermutant.bc \
  --supermutate \
  foo.bc
