#include <stdio.h>
#include <string.h>

#define DIFFICULTY_PREFIX "AC"  // Ejemplo de dificultad (prefijo deseado)

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
    int desplazamiento = 0;
    char resultado[256];

    while (1) {
        cifrado_cesar(texto, desplazamiento, resultado);
        if (check_difficulty(resultado)) {
            printf("Desplazamiento encontrado: %d\n", desplazamiento);
            printf("Texto cifrado: %s\n", resultado);
            break;
        }
        desplazamiento++;
    }
}

int main() {
    const char *texto = "AB";
    mine_block(texto);
    return 0;
}
