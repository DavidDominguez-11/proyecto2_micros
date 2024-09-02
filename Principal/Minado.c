#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <omp.h>

// Función para generar números primos usando la criba de Eratóstenes
int* sieve_of_eratosthenes(int limit, int* count_out) {
    bool *prime = malloc((limit + 1) * sizeof(bool));
    if (prime == NULL) {
        fprintf(stderr, "Error al asignar memoria para la criba de Eratstenes.\n");
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

// Función para la búsqueda secuencial por cada minero
void sequential_miner_search(int* primes, int count, int key, int anchor, int miner_id) {
    clock_t sequence_start = clock();
    bool mining = true;
    
    printf("Minero %d iniciando busqueda secuencial...\n", miner_id);
    for (int i = 0; i < count && mining; i++) {
        for (int j = 0; j < count && mining; j++) {
            int trying = primes[i] * primes[j];
            if (trying == key && primes[i] == anchor) {
                double time_taken = ((double) (clock() - sequence_start)) / CLOCKS_PER_SEC;
                printf("Minero %d encontro la clave: %d\n", miner_id, trying);
                printf("Tiempo en secuencial para minero %d: %.2f segundos\n", miner_id, time_taken);
                printf("Multiplicacion de primos %d * %d = %d, la llave era: %d\n", primes[i], primes[j], trying, key);
                mining = false;
            }
        }
    }
    printf("Minero %d finalizo la busqueda secuencial.\n", miner_id);
}

// Función para la búsqueda paralela
void parallel_search(int* primes, int count, int key, int anchor) {
    clock_t parallel_start = clock();
    bool mining2 = true;
    
    #pragma omp parallel shared(mining2)
    {
        int id = omp_get_thread_num(); // Obtener ID del hilo (minero)
        int num_threads = omp_get_num_threads(); // Obtener el número de hilos (mineros)
        
        printf("Minero %d iniciando busqueda en paralelo...\n", id);
        
        #pragma omp for schedule(dynamic) nowait
        for (int i = 0; i < count; i++) {
            for (int j = 0; j < count && mining2; j++) {
                int trying = primes[i] * primes[j];
                if (trying == key && primes[i] == anchor) {
                    #pragma omp critical
                    {
                        if (mining2) {
                            double time_taken = ((double) (clock() - parallel_start)) / CLOCKS_PER_SEC;
                            printf("Minero %d encontro la clave en paralelo: %d\n", id, trying);
                            printf("Tiempo en paralelo: %.2f segundos\n", time_taken);
                            printf("Multiplicacion de primos %d * %d = %d, la llave era: %d\n", primes[i], primes[j], trying, key);
                            mining2 = false;
                        }
                    }
                }
            }
        }
        
        printf("Minero %d finalizo la busqueda en paralelo.\n", id);
    }
}

int main() {
    int limit = 99999999; // Ajusta este valor según sea necesario
    int count = 0; // Variable para almacenar el número de primos
    clock_t start_time = clock();
    
    // Llama a la función y obtiene el array de primos
    int* primes = sieve_of_eratosthenes(limit, &count);
    
    double time_taken = ((double) (clock() - start_time)) / CLOCKS_PER_SEC;
    printf("Tiempo para generar primos: %.2f segundos\n", time_taken);

    srand(time(NULL)); // Inicializar la semilla para rand()

    int anchor = primes[rand() % count];
    int key = anchor * primes[rand() % count]; // Utiliza índices válidos dentro del rango de primos

    int num_miners = 4; // Número de mineros secuenciales

    printf("Iniciando minado secuencial...\n");
    for (int i = 0; i < num_miners; i++) {
        sequential_miner_search(primes, count, key, anchor, i);
    }
    
    printf("Iniciando minado paralelo...\n");

    // Ajusta el número de hilos para la búsqueda paralela
    omp_set_num_threads(4);

    // Llamada a la función de búsqueda paralela con varios mineros
    parallel_search(primes, count, key, anchor);

    printf("Fin de la busqueda de primos\n");

    free(primes); // Liberar la memoria del array de primos
    return 0;
}
