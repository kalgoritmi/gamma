CACHE ?=
BUILD_TARGET ?= "run"

IMAGE = gamma
CONTAINER = gamma-runner

OPEN_COMMAND =

ifeq ($(shell uname -s),Linux)
    OPEN_COMMAND = xdg-open
else ifeq ($(shell uname -s),Darwin)
    OPEN_COMMAND = open
else ifneq ($(shell uname -s | grep -E "MINGW|MSYS|CYGWIN"),)
    OPEN_COMMAND = start
endif

docker-build:
	docker --debug build --target ${BUILD_TARGET} -t ${IMAGE} ${CACHE} -f ./Dockerfile .
	@echo "\n  *** Built docker container ${IMAGE} ***\n"

get-coverage:
	@[ -z $( docker image ls --format '{{.Repository}}' --filter=reference=${IMAGE}) ] || \
 		(echo "No image \`${IMAGE}\` present, run targets docker-build or docker-run first"; exit 1)
	docker ps -a --format '{{.Names}}' --filter "name=${CONTAINER}" | xargs docker rm
	docker create --name ${CONTAINER} ${IMAGE}
	rm -rf ${CURDIR}/coverage_report 2>&1
	docker cp ${CONTAINER}:/tmp/coverage_report/ ${CURDIR}/coverage_report
	docker rm ${CONTAINER}
	@echo "\nCopied coverage report back to host directory ${CURDIR}/coverage_report\n"

open-coverage: get-coverage
	$(OPEN_COMMAND) ${CURDIR}/coverage_report/index.html

docker-run: docker-build get-coverage
	docker run -v ${CURDIR}/data:/usr/data -it --rm ${IMAGE}

docker-run-bash: docker-build get-coverage
	docker run -v ${CURDIR}/data:/usr/data -it --entrypoint /bin/bash --rm ${IMAGE}

.PHONY: docker-build docker-run docker-run-bash get-coverage open-coverage
