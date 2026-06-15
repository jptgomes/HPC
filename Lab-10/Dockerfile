FROM ubuntu:22.04

RUN apt update && apt install -y \
    gcc \
    make \
    libopenblas-dev \
    time \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY matriz_blas.c .

RUN gcc matriz_blas.c -O3 -o matriz_blas -lopenblas

CMD ["./matriz_blas"]
