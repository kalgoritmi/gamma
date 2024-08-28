# Gamma
A C++ program to apply gamma correction curve.

### Building & Running through Docker

- In order to build the project you need to have 
the following installed on your OS (supporting docker engine):

   - [docker](https://docs.docker.com/engine/install/)
   - make, optional

- If `make` is installed, then from project root run:

  ```shell
  make docker-run -f docker.Makefile 
  ```

  else run the following commands:

  ```shell
  docker --debug build -t gamma -f ./Dockerfile .
  docker run -v ${pwd}/data:/usr/data -it --rm gamma
  ```
  
- This will build the Dockerfile and run a container with default cmd arguments
- The run container command bind mounts a `./data` folder,
  where you can put any input image, it already contains one called `aerial-view.jpg`
- If you want to use the make target with a forced break on docker builder cache, then:

  ```shell
  CACHE=--no-cache make docker-run -f docker.Makefile 
  ```

  or without make:

  ```shell
  docker --debug build -t gamma --no-cache -f ./Dockerfile .
  ```
  
- Default entrypoint of the dockerfile `run` target is `gamma` executable,
  with arguments:

  ```shell
  aerial-view.jpg 20 0.8 aerial-view-OUT.png
  ```

- To run the program on any other image,
  just put the image in the `./data` folder, and run:

  ```shell
  make docker-run-bash -f docker.Makefile
  ```

  or without make:

  ```shell
  docker --debug build -t gamma -f ./Dockerfile .
  docker run -v ${pwd}/data:/usr/data -it --entrypoint /bin/bash --rm gamma
  ```

  this will land you on an interactive bash shell, where you can run:

  ```shell
  gamma some-other-image-basename.png 15 0.7 some-other-image-basename-OUT.png
  ```

  you can then view the output image in the `./data` folder, as the volume is persisted even though the container gets auto-removed due to `--rm`.

- To run open coverage report, run:

  ```shell
  make open-coverage -f docker.Makefile
  ```
  
  Note: This requires that image gamma has been already built once, to do this run any of the targets docker-build docker-run docker-run-bash

### Attribution
Image sample attribution:

- [aerial-view-of-a-mountain-range-with-a-lake-in-the-foreground](./data/aerial-view.jpg) from [unsplash/@danielsessler](https://unsplash.com/photos/an-aerial-view-of-a-mountain-and-a-body-of-water-aHQtYTNih_Q?utm_content=creditShareLink&utm_medium=referral&utm_source=unsplash)
