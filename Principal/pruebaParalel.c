#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <omp.h>

// Cambiamos la función para devolver el array de primos y el conteo
int* sieve_of_eratosthenes(int limit, int* count_out) {
    bool *prime = malloc((limit + 1) * sizeof(bool));
    if (prime == NULL) {
        fprintf(stderr, "Error al asignar memoria para la criba de Eratóstenes.\n");
        exit(EXIT_FAILURE);
    }
    
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
    if (primes == NULL) {
        fprintf(stderr, "Error al asignar memoria para el array de primos.\n");
        free(prime);
        exit(EXIT_FAILURE);
    }
    
    int index = 0;
    for (int p = 2; p <= limit; p++) {
        if (prime[p]) {
            primes[index++] = p;
        }
    }
    
    printf("Total de primos hasta %d: %d\n", limit, count);
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
    printf("Tiempo para generar primos: %.2f segundos\n", time_taken);

    srand(time(NULL)); // Inicializar la semilla para rand()

    int anchor = primes[rand() % count];
    int key = anchor * primes[rand() % count]; // Utiliza índices válidos dentro del rango de primos

    bool mining = true;

    sequence_start = clock();
    for (int i = 0; i < count && mining; i++) {
        for (int j = 0; j < count && mining; j++) {
            int trying = primes[i] * primes[j];
            if (trying == key && primes[i] == anchor) {
                printf("Clave encontrada: %d\n", trying);
                time_taken = ((double) (clock() - sequence_start)) / CLOCKS_PER_SEC;
                printf("Tiempo en secuencial para encontrar la clave: %.2f segundos\n", time_taken);
                printf("Multiplicacion de primos %d * %d = %d, la llave era: %d\n", primes[i], primes[j], trying, key);
                mining = false;
            }
        }
    }
    
    printf("Inicio paralelo\n");
    bool mining2 = true;
    parallel_start = clock();
    
    #pragma omp parallel shared(mining2)
    {
        #pragma omp for schedule(dynamic)
        for (int i = 0; i < count; i++) {
            if (!mining2) continue;  // Si se ha encontrado la clave, salta el resto de iteraciones
            
            for (int j = 0; j < count; j++) {
                if (!mining2) continue;  // Si se ha encontrado la clave, salta el resto de iteraciones
                
                int trying = primes[i] * primes[j];
                if (trying == key && primes[i] == anchor) {
                    #pragma omp critical
                    {
                        if (mining2) {  // Verifica que otro hilo no haya encontrado la clave
                            printf("Clave encontrada: %d\n", trying);
                            time_taken = ((double) (clock() - parallel_start)) / CLOCKS_PER_SEC;
                            printf("Tiempo en paralelo: %.2f segundos\n", time_taken);
                            printf("Multiplicacion de primos %d * %d = %d, la llave era: %d\n", primes[i], primes[j], trying, key);
                            mining2 = false;  // Marca la clave como encontrada
                        }
                    }
                }
            }
        }
    }

    printf("Fin de la búsqueda de primos\n");

    free(primes); // Liberar la memoria del array de primos
    return 0;
}
