# Use an official base image
FROM ubuntu:latest

# Install required packages
RUN apt-get update && \
    apt-get upgrade -y && \
    apt-get install -y \
    sudo \
    && rm -rf /var/lib/apt/lists/*

# Set the working directory
WORKDIR /app

# Copy the source code into the container
COPY . /app

# Remove the cmake-build-debug folder
RUN rm -rf /app/cmake-build-debug

# Make all scripts in /app/build-utils executable
RUN chmod +x /app/build-utils/*.sh

# Set the entry point
ENTRYPOINT ["/bin/bash"]
