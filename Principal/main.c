#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <time.h>

#define MAX_NONCE 1000000

typedef struct {
    int version;
    char prevHash[65];
    char merkleRoot[65]; //árbol hash es una estructura de datos en árbol
    int timestamp;
    int bits;
    int nonce;
} Block;

typedef struct {
    long long hashesCalculated;
    long long noncesChecked;
    double elapsedTime;
} Statistics;

// Función para aplicar el Cifrado César modificado
char applyModifiedCaesarCipher(char c, int offset) {
    printf("aplicando cesar mod\n");
    if (c >= 'A' && c <= 'Z') {
        return 'A' + (c - 'A' + offset) % 26;
    } else if (c >= 'a' && c <= 'z') {
        return 'a' + (c - 'a' + offset) % 26;
    } else {
        return c;
    }
}

// Función para calcular el hash utilizando Cifrado César modificado
void calculateModifiedCaesarHash(Block* block, int nonce, char* hash) {
    printf("calculanso hash con cesar md\n");
    sprintf(hash, "%d%s%s%d%d%d", block->version, block->prevHash, block->merkleRoot, block->timestamp, block->bits, nonce);

    for (int i = 0; i < strlen(hash); i++) {
        hash[i] = applyModifiedCaesarCipher(hash[i], nonce);
    }

    //Operaciones adicionales para alcanzar ~15 segundos
    for (int i = 0; i < 1000000; i++) {
        hash[i % strlen(hash)] = applyModifiedCaesarCipher(hash[i % strlen(hash)], i % 26);
    }
}

// Función para verificar si el hash cumple con la dificultad
int isHashValid(char* hash, int difficulty) {
    // Comprobar si el hash comienza con el patrón de dificultad (por ejemplo, cierto número de ceros)
    printf("comprobando si el hash cumple difi\n");
    char pattern[difficulty + 1];
    memset(pattern, '0', difficulty);
    pattern[difficulty] = '\0';
    return strncmp(hash, pattern, difficulty) == 0;
}

// Función para minar de manera secuencial
int mineSequential(Block* block, int difficulty) {
    printf("Inicio de minado Secuencial\n");
    Statistics stats = {0};
    stats.elapsedTime = omp_get_wtime();

    for (int nonce = 0; nonce < MAX_NONCE; nonce++) {
        char hash[128];
        calculateModifiedCaesarHash(block, nonce, hash);
        stats.hashesCalculated++;
        stats.noncesChecked++;

        if (isHashValid(hash, difficulty)) {
            stats.elapsedTime = omp_get_wtime() - stats.elapsedTime;
            printf("Secuential mining:\n");
            printf("  Nonce found: %d\n", nonce);
            printf("  Hashes calculated: %lld\n", stats.hashesCalculated);
            printf("  Nonces checked: %lld\n", stats.noncesChecked);
            printf("  Time elapsed: %.2f seconds\n", stats.elapsedTime);
            return nonce;
        }
    }

    stats.elapsedTime = omp_get_wtime() - stats.elapsedTime;
    printf("Secuential mining:\n");
    printf("  No valid nonce found\n");
    printf("  Hashes calculated: %lld\n", stats.hashesCalculated);
    printf("  Nonces checked: %lld\n", stats.noncesChecked);
    printf("  Time elapsed: %.2f seconds\n", stats.elapsedTime);
    return -1;
}

// Función para actualizar el rango de búsqueda
void updateSearchRange(int* rangeStart, int* rangeEnd) {
    printf("actualizar el rango de busq\n");
    *rangeStart = *rangeEnd + 1;
    *rangeEnd = *rangeEnd + 1000000;
}

// Función para combinar las estadísticas locales y globales
void combineStatistics(Statistics* global, Statistics* local) {
    printf("estadisticas globalses\n");
    global->hashesCalculated += local->hashesCalculated;
    global->noncesChecked += local->noncesChecked;
}

// Función para minar de manera paralela
int mineParallel(Block* block, int difficulty, int numThreads) {
    printf("Inicio de minado Paralelo\n");
    Statistics statsGlobal = {0};
    statsGlobal.elapsedTime = omp_get_wtime();

    int bestNonce = -1;
    int found = 0;
    int rangeStart = 0;
    int rangeEnd = MAX_NONCE - 1;

    double localElapsedTime = 0;

    #pragma omp parallel num_threads(numThreads) shared(bestNonce, found, statsGlobal, rangeStart, rangeEnd)
    {
        printf("entra en paralel 1\n");
        Statistics statsLocal = {0};

        double threadStartTime = omp_get_wtime();

        while (!found) {
            printf("entra en pragma 2\n");
            #pragma omp for schedule(dynamic)
            for (int nonce = rangeStart; nonce <= rangeEnd; nonce++) {
                if (!found) {
                    char hash[128];
                    calculateModifiedCaesarHash(block, nonce, hash);
                    statsLocal.hashesCalculated++;
                    statsLocal.noncesChecked++;

                    if (isHashValid(hash, difficulty)) {
                        #pragma omp critical
                        {
                            if (!found) {
                                bestNonce = nonce;
                                found = 1;
                            }
                        }
                    }
                }
            }

            #pragma omp barrier

            #pragma omp single
            {
                updateSearchRange(&rangeStart, &rangeEnd);
            }
        }

        #pragma omp critical
        {
            printf("pragma 3 critical\n");
            combineStatistics(&statsGlobal, &statsLocal);
            localElapsedTime = omp_get_wtime() - threadStartTime;
            if (localElapsedTime > statsGlobal.elapsedTime) {
                statsGlobal.elapsedTime = localElapsedTime;
            }
        }
    }

    printf("Parallel mining:\n");
    printf("  Nonce found: %d\n", bestNonce);
    printf("  Hashes calculated: %lld\n", statsGlobal.hashesCalculated);
    printf("  Nonces checked: %lld\n", statsGlobal.noncesChecked);
    printf("  Time elapsed: %.2f seconds\n", statsGlobal.elapsedTime);
    printf("  Speedup: %.2f\n", statsGlobal.elapsedTime / localElapsedTime);
    return bestNonce;
}

int main() {
    printf("Inicio de programa\n");
    Block block;
    block.version = rand();
    sprintf(block.prevHash, "%064x", rand());
    sprintf(block.merkleRoot, "%064x", rand());
    block.timestamp = time(NULL);
    block.bits = rand();

    int difficulty = 0;
    printf("Introduzca la dificultad deseada (número de ceros iniciales en el hash): ");
    scanf("%d", &difficulty);

    int numThreads;
    printf("Numero de hilos a utilizar: ");
    scanf("%d", &numThreads);

    int nonceSequential = mineSequential(&block, difficulty);
    int nonceParallel = mineParallel(&block, difficulty, numThreads);

    return 0;
}