#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

void sieve_of_eratosthenes(int limit) {
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
    
    printf("Total primes up to %d: %d\n", limit, count);
    free(prime);
}

int main() {
    clock_t start_time, end_time;
    double time_taken;
    
    int limit = 999999999; // Ajusta este valor según sea necesario
    start_time = clock();
    
    sieve_of_eratosthenes(limit);
    
    end_time = clock();
    time_taken = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;
    
    printf("Time taken: %.2f seconds\n", time_taken);
    
    return 0;
}
