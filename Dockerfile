FROM tasp/libcommon:1.0.2

COPY . /db-pg
WORKDIR /db-pg

RUN export DEBIAN_FRONTEND=noninteractive && \
    apt-get update && apt-get install -y --no-install-recommends --reinstall \
        libpq-dev

RUN mkdir build && cd build && cmake .. && ninja install
