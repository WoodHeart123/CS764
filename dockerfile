FROM ubuntu:20.04

# Set the maintainer label
LABEL maintainer="your-email@example.com"

# Install GCC, make, GDB, and Valgrind
RUN apt-get update && apt-get install -y \
    g++ \
    make \
    gdb \
    valgrind 

# Set the working directory in the container
WORKDIR /app

# Copy the local Makefile and source code to the working directory
COPY . .

# Build the application with make
RUN make

ENTRYPOINT ["tail", "-f", "/dev/null"]