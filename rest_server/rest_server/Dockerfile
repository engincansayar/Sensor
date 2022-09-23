FROM alpine:3.14

# Copy the current folder which contains C++ source code to the Docker image under /usr/src
COPY . /usr/src/dockertest1

# Specify the working directory
WORKDIR /usr/src/dockertest1

# Use GCC to compile the http_denemesi.c source file
#RUN gcc -o Test http_denemesi.c

# Run the program output from the previous step
CMD ["./Test"]