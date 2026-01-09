# -------- Build stage --------
FROM ubuntu:24.04 AS build

RUN apt-get update && apt-get install -y \
    cmake \
    g++ \
    make \
    nlohmann-json3-dev \
    && rm -rf /var/lib/apt/lists/*

 
WORKDIR /src
COPY . .

# Create build dir and build exactly like you do locally
RUN mkdir build \
 && cd build \
 && cmake -DPENTOBI_BUILD_GUI=OFF -DCMAKE_BUILD_TYPE=Release .. \
 && cmake --build . -j$(nproc)

# -------- Runtime stage --------
FROM ubuntu:24.04

WORKDIR /app

# Copy the actual built binary
COPY --from=build /src/build/pybind/pentobi_engine /app/cpp_server

RUN mkdir -p /sockets


