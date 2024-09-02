#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <omp.h>

// Función para generar números primos usando la criba de Eratóstenes
int* sieve_of_eratosthenes(int limit, int* count_out) {
    // Asignar memoria para un array de booleanos que indicará si un número es primo o no
    bool *prime = malloc((limit + 1) * sizeof(bool));
    if (prime == NULL) {
        // Si la asignación de memoria falla, muestra un mensaje de error y termina el programa
        fprintf(stderr, "Error al asignar memoria para la criba de Eratostenes.\n");
        exit(EXIT_FAILURE);
    }
    
    // Inicializar todos los valores en el array como verdaderos (asumiendo que todos son primos inicialmente)
    for (int i = 0; i <= limit; i++) {
        prime[i] = true;
    }
    
    // Marcar 0 y 1 como no primos
    prime[0] = prime[1] = false;
    
    // Implementación de la criba de Eratóstenes
    for (int p = 2; p * p <= limit; p++) {
        if (prime[p]) {
            // Si p es primo, marca todos sus múltiplos como no primos
            for (int i = p * p; i <= limit; i += p) {
                prime[i] = false;
            }
        }
    }
    
    // Contar el número de primos encontrados
    int count = 0;
    for (int p = 2; p <= limit; p++) {
        if (prime[p]) {
            count++;
        }
    }

    // Asignar memoria para almacenar los números primos encontrados
    int *primes = malloc(count * sizeof(int));
    if (primes == NULL) {
        // Si la asignación de memoria falla, muestra un mensaje de error, libera la memoria previa y termina el programa
        fprintf(stderr, "Error al asignar memoria para el array de primos.\n");
        free(prime);
        exit(EXIT_FAILURE);
    }
    
    // Llenar el array de números primos
    int index = 0;
    for (int p = 2; p <= limit; p++) {
        if (prime[p]) {
            primes[index++] = p;
        }
    }
    
    // Imprime el número total de primos encontrados hasta el límite dado
    printf("Total de primos hasta %d: %d\n", limit, count);
    free(prime); // Libera la memoria utilizada por el array de booleanos
    
    *count_out = count; // Devuelve el número de primos encontrados a través de un puntero
    return primes; // Devuelve el array de primos
}

// Función para la búsqueda secuencial por cada minero
void sequential_miner_search(int* primes, int count, int key, int anchor, int miner_id) {
    clock_t sequence_start = clock(); // Inicia el cronómetro para medir el tiempo
    bool mining = true; // Variable para controlar el estado de la minería
    
    printf("Minero %d iniciando busqueda secuencial...\n", miner_id);
    for (int i = 0; i < count && mining; i++) {
        for (int j = 0; j < count && mining; j++) {
            int trying = primes[i] * primes[j]; // Calcula el producto de dos primos
            if (trying == key && primes[i] == anchor) {
                double time_taken = ((double) (clock() - sequence_start)) / CLOCKS_PER_SEC;
                // Si se encuentra la clave, se imprime el resultado y el tiempo tomado
                printf("Minero %d encontro la clave: %d\n", miner_id, trying);
                printf("Tiempo en secuencial para minero %d: %.2f segundos\n", miner_id, time_taken);
                printf("Multiplicacion de primos %d * %d = %d, la llave era: %d\n", primes[i], primes[j], trying, key);
                mining = false; // Termina la búsqueda si se encuentra la clave
            }
        }
    }
    printf("Minero %d finalizo la busqueda secuencial.\n", miner_id);
}

// Función para la búsqueda paralela
void parallel_search(int* primes, int count, int key, int anchor) {
    clock_t parallel_start = clock(); // Inicia el cronómetro para medir el tiempo
    bool mining2 = true; // Variable para controlar el estado de la minería
    
    #pragma omp parallel shared(mining2)
    {
        int id = omp_get_thread_num(); // Obtener ID del hilo (minero)
        int num_threads = omp_get_num_threads(); // Obtener el número de hilos (mineros)
        
        printf("Minero %d iniciando busqueda en paralelo...\n", id);
        
        #pragma omp for schedule(dynamic) nowait
        for (int i = 0; i < count; i++) {
            for (int j = 0; j < count && mining2; j++) {
                int trying = primes[i] * primes[j]; // Calcula el producto de dos primos
                if (trying == key && primes[i] == anchor) {
                    #pragma omp critical
                    {
                        // Si se encuentra la clave, se imprime el resultado y el tiempo tomado
                        if (mining2) {
                            double time_taken = ((double) (clock() - parallel_start)) / CLOCKS_PER_SEC;
                            printf("Minero %d encontro la clave en paralelo: %d\n", id, trying);
                            printf("Tiempo en paralelo: %.2f segundos\n", time_taken);
                            printf("Multiplicacion de primos %d * %d = %d, la llave era: %d\n", primes[i], primes[j], trying, key);
                            mining2 = false; // Termina la búsqueda si se encuentra la clave
                        }
                    }
                }
            }
        }
        
        printf("Minero %d finalizo la busqueda en paralelo.\n", id);
    }
}

int main() {
    int limit = 99999999; // Límite superior para la criba de Eratóstenes, ajustar según sea necesario
    int count = 0; // Variable para almacenar el número de primos
    clock_t start_time = clock(); // Inicia el cronómetro para medir el tiempo
    
    // Llama a la función y obtiene el array de primos
    int* primes = sieve_of_eratosthenes(limit, &count);
    
    double time_taken = ((double) (clock() - start_time)) / CLOCKS_PER_SEC;
    // Imprime el tiempo tomado para generar los números primos
    printf("Tiempo para generar primos: %.2f segundos\n", time_taken);

    srand(time(NULL)); // Inicializar la semilla para rand() usando el tiempo actual

    // Selecciona un número primo aleatorio como "anchor"
    int anchor = primes[rand() % count];
    // Calcula la clave como el producto de "anchor" y otro primo aleatorio
    int key = anchor * primes[rand() % count]; // Utiliza índices válidos dentro del rango de primos

    int num_miners = 2; // Número de mineros secuenciales

    printf("Iniciando minado secuencial...\n");
    for (int i = 0; i < num_miners; i++) {
        sequential_miner_search(primes, count, key, anchor, i); // Ejecuta la búsqueda secuencial por cada minero
    }
    
    printf("Iniciando minado paralelo...\n");

    // Ajusta el número de hilos para la búsqueda paralela
    omp_set_num_threads(num_miners);

    // Llamada a la función de búsqueda paralela con varios mineros
    parallel_search(primes, count, key, anchor);

    printf("Fin de la busqueda de primos\n");

    free(primes); // Liberar la memoria del array de primos
    return 0; // Fin del programa
}