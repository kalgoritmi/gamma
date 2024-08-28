FROM ubuntu:22.04 AS build

ENV DEBIAN_FRONTEND=noninteractive

RUN apt update && \
    apt install -y --no-install-recommends \
    # build-essential auto configures alternatives but we need v12 rather than v11
    gcc-12 g++-12 make lcov ca-certificates gpg wget libopencv-dev && \
    # add alternatives as high priority, no other installed so it defaults to these
    update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-12 20 && \
    update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-12 20 && \
    update-alternatives --install /usr/bin/gcov gcov /usr/bin/gcov-12 20 && \
    wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2> /dev/null | \
    gpg --dearmor - | tee /etc/apt/trusted.gpg.d/kitware-archive-keyring.gpg > /dev/null && \
    echo 'deb [signed-by=/etc/apt/trusted.gpg.d/kitware-archive-keyring.gpg] https://apt.kitware.com/ubuntu/ jammy main' | \
    tee /etc/apt/sources.list.d/kitware.list && \
    apt update && \
    apt install -y --no-install-recommends cmake && \
    apt clean && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /usr/src/gamma
COPY . .

# build, then run tests
RUN mkdir build && cd build && cmake .. && make && ./src/tests

# generate coverage report
RUN lcov --capture --directory . --output-file coverage && \
    lcov --extract coverage \
      '/usr/src/gamma/src/*' \
      '/usr/src/gamma/include/lib/*' \
      --output-file coverage.report && \
    genhtml coverage.report --output-directory /tmp/coverage_report

SHELL ["/bin/bash", "-c"]
CMD bash

FROM ubuntu:22.04 AS run

COPY --from=build /usr/lib/ /usr/lib/
COPY --from=build /etc /etc

WORKDIR /usr/data

COPY --from=build /tmp/coverage_report /tmp/coverage_report

# sample image credit link:
# https://unsplash.com/photos/a-view-of-a-mountain-range-with-a-lake-in-the-foreground-rSsQ4qXg41I?utm_content=creditShareLink&utm_medium=referral&utm_source=unsplash
# ADD https://unsplash.com/photos/aHQtYTNih_Q/download?ixid=M3wxMjA3fDB8MXxhbGx8NDV8fHx8fHwyfHwxNzI0NjE4MTMzfA&force=true&w=1920 /usr/data/aerial-view.jpg

COPY --from=build /usr/src/gamma/build/src/gamma /usr/local/bin/gamma

ENTRYPOINT ["gamma"]
CMD ["aerial-view.jpg", "20", "0.8", "aerial-view-OUT.png"]
