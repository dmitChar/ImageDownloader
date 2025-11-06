FROM ubuntu:24.04 AS build

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
    && apt-get install -y \
        build-essential \
        qt6-base-dev \
        qt6-base-dev-tools \
        qt6-tools-dev \
        qt6-tools-dev-tools \
        libopencv-dev \
        pkg-config \
        ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY . .

RUN qmake6 ImageDownloader.pro \
    && make -j"$(nproc)"


FROM ubuntu:24.04 AS runtime

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
    && apt-get install -y \
        qt6-base-dev \
        libopencv-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

RUN mkdir -p /Output

COPY --from=build /app/ImageDownloader ./ImageDownloader

CMD ["./ImageDownloader"]
