#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// Cambiamos la función para devolver el array de primos y el conteo
int* sieve_of_eratosthenes(int limit, int* count_out) {
    bool *prime = malloc((limit + 1) * sizeof(bool));
    for (int i = 0; i <= limit; i++) {
        prime[i] = true;
    }
    
    prime[0] = prime[1] = false;
    
    for (int p = 2; p * p <= limit; p++) {
        if (prime[p]) {
            for (int i = p * p; i <= limit; i += p) {
                prime[i] = false;
            }
        }
    }
    
    int count = 0;
    for (int p = 2; p <= limit; p++) {
        if (prime[p]) {
            count++;
        }
    }

    int *primes = malloc(count * sizeof(int));
    int index = 0;
    for (int p = 2; p <= limit; p++) {
        if (prime[p]) {
            primes[index++] = p;
        }
    }
    
    printf("Total primes up to %d: %d\n", limit, count);
    free(prime);
    
    *count_out = count; // Devuelve el número de primos encontrados
    return primes; // Devuelve el array de primos
}

int main() {
    clock_t start_time, end_time;
    double time_taken;
    
    int limit = 999999999; // Ajusta este valor según sea necesario
    int count = 0; // Variable para almacenar el número de primos
    start_time = clock();
    
    // Llama a la función y obtiene el array de primos
    int* primes = sieve_of_eratosthenes(limit, &count);
    
    end_time = clock();
    time_taken = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;
    printf("Time taken: %.2f seconds\n", time_taken);

    srand(time(NULL)); // Inicializar la semilla para rand()

    int key = primes[rand() % count] * primes[rand() % count]; // Utiliza índices válidos dentro del rango de primos

    for (int i = 0; i < count; i++) {  // Utiliza count en lugar de primes.size()
        for (int j = 0; j < count; j++) {  // Corrige el bucle interno con j++
            int trying = primes[i] * primes[j];
            if (trying == key) {
                printf("Clave encontrada: %d\n", trying);
                time_taken = ((double) (clock() - end_time)) / CLOCKS_PER_SEC;
                printf("Time taken to find key: %.2f seconds\n", time_taken);
                break;
            }
        }
    }

    free(primes); // Liberar la memoria del array de primos
    return 0;
}
