# V prvi fazi namesti prevajalnik in prevede C++ program
FROM alpine:latest AS build

WORKDIR /app

RUN apk add --no-cache g++

COPY vaja5.cpp .
COPY test.txt .

RUN g++ -Wall -Wextra -std=c++17 vaja5.cpp -static -o vaja5

# V drugi fazi pa v končni image skopira samo prevedeno datoteko in testno datoteko
FROM alpine:latest

WORKDIR /app

COPY --from=build /app/vaja5 .
COPY --from=build /app/test.txt .

CMD ["./vaja5"]