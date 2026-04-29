# Uporabimo uradno GCC sliko
FROM gcc:13

# Nastavimo delovno mapo v zabojniku
WORKDIR /app

# Kopiramo vse datoteke projekta v zabojnik
COPY . .

# Prevedemo C++ program
RUN g++ -Wall -Wextra -std=c++17 vaja5.cpp -o vaja5

# Ukaz, ki se izvede ob zagonu zabojnika
CMD ["./vaja5"]