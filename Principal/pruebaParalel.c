#include <stdio.h>
#include <string.h>
#include <omp.h>

#define DIFFICULTY_PREFIX "AAAA"  // Prefijo deseado
#define MAX_DESPLAZAMIENTO 1000000 // Ajusta el rango según tu necesidad

void cifrado_cesar(const char *texto, int desplazamiento, char *resultado) {
    for (int i = 0; texto[i] != '\0'; ++i) {
        char letra = texto[i];
        if (letra >= 'A' && letra <= 'Z') {
            resultado[i] = (letra - 'A' + desplazamiento) % 26 + 'A';
        } else if (letra >= 'a' && letra <= 'z') {
            resultado[i] = (letra - 'a' + desplazamiento) % 26 + 'a';
        } else {
            resultado[i] = letra;
        }
    }
    resultado[strlen(texto)] = '\0';
}

int check_difficulty(const char *texto_cifrado) {
    return strncmp(texto_cifrado, DIFFICULTY_PREFIX, strlen(DIFFICULTY_PREFIX)) == 0;
}

void mine_block(const char *texto) {
    char resultado[256];
    int desplazamiento;
    int encontrado = 0;

    #pragma omp parallel private(desplazamiento, resultado) shared(encontrado)
    {
        #pragma omp for
        for (desplazamiento = 0; desplazamiento < MAX_DESPLAZAMIENTO; ++desplazamiento) {
            if (encontrado) continue;

            cifrado_cesar(texto, desplazamiento, resultado);
            if (check_difficulty(resultado)) {
                #pragma omp critical
                {
                    if (!encontrado) {
                        printf("Desplazamiento encontrado: %d\n", desplazamiento);
                        printf("Texto cifrado: %s\n", resultado);
                        encontrado = 1;
                    }
                }
            }
        }
    }
}

int main() {
    const char *texto = "Texto de prueba";
    mine_block(texto);
    return 0;
}
