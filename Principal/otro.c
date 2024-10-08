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
    clock_t start_time, end_time, sequence_start, parallel_start;
    double time_taken;
    
    int limit = 99999999; // Ajusta este valor según sea necesario
    int count = 0; // Variable para almacenar el número de primos
    start_time = clock();
    
    // Llama a la función y obtiene el array de primos
    int* primes = sieve_of_eratosthenes(limit, &count);
    
    end_time = clock();
    time_taken = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;
    printf("Time taken: %.2f seconds\n", time_taken);

    srand(time(NULL)); // Inicializar la semilla para rand()

    int anchor = primes[rand() % count];
    int key = anchor * primes[rand() % count]; // Utiliza índices válidos dentro del rango de primos

    bool mining =  true;

    sequence_start = clock();
    while(mining == true){
          for (int i = 0; i < count; i++) {  // Utiliza count en lugar de primes.size()
            for (int j = 0; j < count; j++) {  // Corrige el bucle interno con j++
                int trying = primes[i] * primes[j];
                if (trying == key && primes[i] == anchor) {
                    printf("Clave encontrada: %d\n", trying);
                    time_taken = ((double) (clock() - sequence_start)) / CLOCKS_PER_SEC;
                    printf("Tiempo en secuencial para encontrar key: %.2f seconds\n", time_taken);
                    printf("Multiplicacion de primos %d * %d = %d, la llave era: %d\n", primes[i], primes[j], trying, key);
                    mining = false;
                    break;
                }
            }
            if(mining == false) break;
        }
    }
    printf("Inicio paraelo\n");
    bool mining2 = true;
    parallel_start = clock();
    while(mining2 == true){
        #pragma omp parallel for 
            for (int i = 0; i < count; i++) {  // Utiliza count en lugar de primes.size()
            for (int j = 0; j < count; j++) {  // Corrige el bucle interno con j++
                int trying = primes[i] * primes[j];
                if (trying == key && primes[i] == anchor) {
                    printf("Clave encontrada: %d\n", trying);
                    time_taken = ((double) (clock() - parallel_start)) / CLOCKS_PER_SEC;
                    printf("Tempo en paralelo: %.2f seconds\n", time_taken);
                    printf("Multiplicacion de primos %d * %d = %d, la llave era: %d\n", primes[i], primes[j], trying, key);
                    mining2 = false;
                }
                if(mining2 == false) break;
            }
            if(mining2 == false) break;
        }
    }

    printf("Se acabaron los primos");

    free(primes); // Liberar la memoria del array de primos
    return 0;
}
