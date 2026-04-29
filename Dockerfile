# 1. faza: build
FROM gcc:13 AS build

WORKDIR /app
COPY . .

RUN g++ -Wall -Wextra -std=c++17 vaja5.cpp -o vaja5


# 2. faza: končna aplikacija
FROM debian:bookworm-slim

WORKDIR /app

COPY --from=build /app/vaja5 .

CMD ["./vaja5"]