#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_LEIT 50000
#define MAX_SENS 50

typedef enum {
    TIPO_INT, TIPO_BOOL, TIPO_FLOAT, TIPO_STR
} TipoDado;

typedef union {
    int inteiro;
    bool booleano; 
    double racional;
    char string[20];
} Valor;

typedef struct {
    long timestamp;
    char sensor[20];
    TipoDado tipo;
    Valor valor;
} Leitura;

typedef struct {
    char nome[20];
    TipoDado tipo;
    int count;
} InfoSensor;

Leitura dados[MAX_LEIT];
InfoSensor sensores[MAX_SENS];
int numLeit = 0, numSens = 0;

TipoDado detectarTipo(char* str) {
    if (!strcmp(str, "true") || !strcmp(str, "false")) 
        return TIPO_BOOL;
    
    char *end;
    strtol(str, &end, 10);
    if (*end == '\0') return TIPO_INT;
    
    strtod(str, &end);
    if (*end == '\0') return TIPO_FLOAT;
    
    return TIPO_STR;
}

Valor converterValor(char* str, TipoDado t) {
    Valor v;
    switch(t) {
        case TIPO_INT: v.inteiro = atoi(str); break;
        case TIPO_BOOL: v.booleano = !strcmp(str, "true"); break;
        case TIPO_FLOAT: v.racional = atof(str); break;
        case TIPO_STR: strcpy(v.string, str); break;
    }
    return v;
}

// Ordenação

void ordenarDados(Leitura *arr, int n) {
    for(int i = 0; i < n-1; i++) {
        bool trocou = false;
        for(int j = 0; j < n-i-1; j++) {
            if(arr[j].timestamp > arr[j+1].timestamp) {
                Leitura tmp = arr[j];
                arr[j] = arr[j+1]; 
                arr[j+1] = tmp;
                trocou = true;
            }
        }
        if(!trocou) break;
    }
}

void escreverValor(FILE *f, Valor v, TipoDado t) {
    switch(t) {
        case TIPO_INT: fprintf(f, "%d", v.inteiro); break;
        case TIPO_BOOL: fprintf(f, "%s", v.booleano ? "true" : "false"); break;
        case TIPO_FLOAT: fprintf(f, "%.6f", v.racional); break;
        case TIPO_STR: fprintf(f, "%s", v.string); break;
    }
}

int main(int argc, char **argv) {
    char nomeArq[256];
    
    if(argc == 2) {
        strcpy(nomeArq, argv[1]);
    } else {
        printf("Digite o nome do arquivo: ");
        scanf("%s", nomeArq);
    }
    
    FILE *f = fopen(nomeArq, "r");
    if(!f) {
        printf("Erro ao abrir %s\n", nomeArq);
        return 1;
    }

    char linha[256], timestampStr[32], sensorTemp[20], valorStr[20];
    
    while(fgets(linha, sizeof(linha), f)) {
        if(sscanf(linha, "%s %s %s", timestampStr, sensorTemp, valorStr) != 3) 
            continue;
            
        if(numLeit >= MAX_LEIT) {
            printf("Limite de leituras atingido: %d\n", MAX_LEIT);
            break;
        }
        
        dados[numLeit].timestamp = atol(timestampStr);
        strcpy(dados[numLeit].sensor, sensorTemp);
        
        TipoDado tipoDetect = detectarTipo(valorStr);
        
        int indSensor = -1;
        for(int i = 0; i < numSens; i++) {
            if(!strcmp(sensores[i].nome, sensorTemp)) {
                indSensor = i;
                if(sensores[i].tipo != tipoDetect) {
                    tipoDetect = sensores[i].tipo;
                }
                break;
            }
        }
        
        // Novo sensor
        
        if(indSensor == -1) {
            if(numSens >= MAX_SENS) break;
            strcpy(sensores[numSens].nome, sensorTemp);
            sensores[numSens].tipo = tipoDetect;
            sensores[numSens].count = 0;
            indSensor = numSens++;
        }
        
        dados[numLeit].tipo = tipoDetect;
        dados[numLeit].valor = converterValor(valorStr, tipoDetect);
        sensores[indSensor].count++;
        numLeit++;
    }
    fclose(f);
    
    printf("Total de leituras: %d\n", numLeit);
    printf("Sensores encontrados: %d\n", numSens);
    
    for(int i = 0; i < numSens; i++) {
        char *nomesTipo[] = {"inteiro", "booleano", "racional", "string"};
        printf("%s: %d leituras (%s)\n", sensores[i].nome, sensores[i].count, 
               nomesTipo[sensores[i].tipo]);
    }
    
    // Arquivos por sensor

    for(int s = 0; s < numSens; s++) {
        char nomeArqSens[30];
        sprintf(nomeArqSens, "%s.txt", sensores[s].nome);
        
        Leitura *dadosSensor = malloc(sensores[s].count * sizeof(Leitura));
        int countSensor = 0;
        
        for(int i = 0; i < numLeit; i++) {
            if(!strcmp(dados[i].sensor, sensores[s].nome)) {
                dadosSensor[countSensor++] = dados[i];
            }
        }
        
        ordenarDados(dadosSensor, countSensor);
        
        FILE *arq = fopen(nomeArqSens, "w");
        if(arq) {
            for(int i = 0; i < countSensor; i++) {
                fprintf(arq, "%ld %s ", dadosSensor[i].timestamp, dadosSensor[i].sensor);
                escreverValor(arq, dadosSensor[i].valor, dadosSensor[i].tipo);
                fprintf(arq, "\n");
            }
            fclose(arq);
            printf("Arquivo %s criado com %d leituras.\n", nomeArqSens, countSensor);
        }
        free(dadosSensor);
    }
    
}