# Use an official base image
FROM ubuntu:latest

# Install required packages
RUN apt-get update && \
    apt-get upgrade -y && \
    apt-get install -y \
    cmake \
    g++ \
    libxi-dev \
    libxcursor-dev \
    libxinerama-dev \
    libsdl2-dev \
    x11-apps \
    && rm -rf /var/lib/apt/lists/*

# Set the working directory
WORKDIR /app

# Copy the source code into the container
COPY . /app

# Fetch the libraries and make them available
# RUN mkdir -p /app/libs && \
#     cd /app/libs && \
#     wget https://github.com/SanderMertens/flecs/archive/refs/tags/v3.2.11.tar.gz && \
#     mv v3.2.11.tar.gz flecs-3.2.11.tar.gz && \
#     tar -xzf flecs-3.2.11.tar.gz && \
#     wget https://github.com/raysan5/raylib/archive/refs/tags/4.5.0.tar.gz && \
#     mv 4.5.0.tar.gz raylib-4.5.0.tar.gz && \
#     tar -xzf raylib-4.5.0.tar.gz && \
#     wget https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz && \
#     mv v1.14.0.tar.gz googletest-1.14.0.tar.gz && \
#     tar -xzf googletest-1.14.0.tar.gz && \
#     wget https://github.com/libsdl-org/SDL/archive/refs/tags/release-2.28.3.tar.gz && \
#     mv release-2.28.3.tar.gz SDL2-2.28.3.tar.gz && \
#     tar -xzf SDL2-2.28.3.tar.gz

# Build and run the project
#RUN cmake . -B cmake-build-debug && \
#    cd cmake-build-debug && \
#    make

# Set the entry point
ENTRYPOINT ["/bin/bash"]
