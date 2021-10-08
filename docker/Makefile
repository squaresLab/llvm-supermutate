DOCKER_ORG=christimperley

all: install

builder:
	docker build -f Dockerfile.builder -t ${DOCKER_ORG}/llvmsupermutate:builder .

llvm: builder
	docker build -f Dockerfile.llvm -t ${DOCKER_ORG}/llvmsupermutate:llvm .

llvmsupermutate: builder llvm
	docker build -f Dockerfile.llvmsupermutate -t ${DOCKER_ORG}/llvmsupermutate ..

install: llvmsupermutate
	./install

.PHONY: builder install llvm llvmsupermutate

