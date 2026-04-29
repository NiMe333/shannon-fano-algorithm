FROM alpine:latest AS build

WORKDIR /app

RUN apk add --no-cache g++

COPY vaja5.cpp .
COPY test.txt .

RUN g++ -Wall -Wextra -std=c++17 vaja5.cpp -static -o vaja5


FROM alpine:latest

WORKDIR /app

COPY --from=build /app/vaja5 .
COPY --from=build /app/test.txt .

CMD ["./vaja5"]