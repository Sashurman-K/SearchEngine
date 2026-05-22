#docker build -t search-engine:latest .
#docker run -it --name my_search_engine search-engine:latest

#Сборка приложения и запуск тестов
FROM ubuntu:22.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    git \
    libstemmer-dev \
    nlohmann-json3-dev \
    libgtest-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY . .

RUN mkdir build && cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release .. && \
    make -j$(nproc) && \
    ./unit_tests


#Финальный легковесный образ для работы
FROM ubuntu:22.04 AS runtime

# Устанавливаем только runtime-зависимости (библиотека стемминга)
RUN apt-get update && apt-get install -y --no-install-recommends \
    libstemmer0d \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /search_engine

# Копируем из первой стадии (builder) только то, что нужно для работы
COPY --from=builder /app/build/search_engine .
COPY --from=builder /app/stopwords_config.json .

# Указываем команду по умолчанию для запуска CLI-интерфейса поисковика
ENTRYPOINT ["./search_engine"]