DOCKER_ORG=christimperley

builder:
	docker build -f Dockerfile.builder -t ${DOCKER_ORG}/llvmsupermutate:builder .

llvm: builder
	docker build -f Dockerfile.llvm -t ${DOCKER_ORG}/llvmsupermutate:llvm .

.PHONY: builder llvm

