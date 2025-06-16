// Programa 03: Geração de Dados
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define MAX_SENS 20
#define LEIT_PSENSOR 2000
#define MAX_NAMES 20
#define MAX_STRING_VALUE 16

typedef enum {
    TIPO_INTEIRO,
    TIPO_BOOLEANO, 
    TIPO_RACIONAL,
    TIPO_STRING
} TipoDado;

// Sensor

typedef struct {
    char nome[MAX_NAMES];
    TipoDado tipo;
} Sens;

// Leitura

typedef struct {
    time_t timestamp;
    char sensor[MAX_NAMES];
    char valor[MAX_STRING_VALUE + 1];
} Leitura;

// Validar data

int validar_data(int dia, int mes, int ano) {
    if (dia < 1 || dia > 31 || mes < 1 || mes > 12 || ano < 1970) {
        return 0;
    }
    
    if ((mes == 4 || mes == 6 || mes == 9 || mes == 11) && dia > 30) {
        return 0;
    }
    
    if (mes == 2) {
        int bissexto = (ano % 4 == 0 && (ano % 100 != 0 || ano % 400 == 0));
        if ((bissexto && dia > 29) || (!bissexto && dia > 28)) {
            return 0;
        }
    }
    
    return 1;
}

int validar_hora(int hora, int min, int seg) {
    return (hora >= 0 && hora <= 23 && min >= 0 && min <= 59 && seg >= 0 && seg <= 59);
}

time_t converter_para_timestamp(int dia, int mes, int ano, int hora, int min, int seg) {
    struct tm t;

    t.tm_year = ano - 1900;
    t.tm_mon = mes - 1;
    t.tm_mday = dia;
    t.tm_hour = hora;
    t.tm_min = min;
    t.tm_sec = seg;
    t.tm_isdst = -1;

    time_t timestamp = mktime(&t);
    if (timestamp == -1) {
        printf("Data inválida. Tente novamente.\n");
        return -1;
    } else {
        return timestamp;
    }
}

time_t gerar_timestamp_aleatorio(struct tm *inicial, struct tm *final) {
    
    time_t timestamp_inicial, timestamp_final;
        
    timestamp_inicial = mktime(inicial);
    if (timestamp_inicial == -1) {
        printf("Data inválida.\n");
        return -1;
    }

    timestamp_final = mktime(final);
    if (timestamp_final == -1) {
        printf("Data inválida.\n");
        return -1;
    }

    time_t timestamp_aleatorio = timestamp_inicial + rand() % (timestamp_final - timestamp_inicial + 1);
    
    return timestamp_aleatorio;
}

// Valor Aleatorio

void gerarValor(TipoDado tipo, char *valor) {
    switch (tipo) {
        case TIPO_INTEIRO:
            sprintf(valor, "%d", rand() % 1000);
            break;
        case TIPO_BOOLEANO:
            strcpy(valor, (rand() % 2) ? "true" : "false");
            break;
        case TIPO_RACIONAL:
            sprintf(valor, "%.3f", (rand() / (double)RAND_MAX) * 100.0);
            break;
        case TIPO_STRING: {
            const char chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
            int len = 3 + rand() % 6;
            for (int i = 0; i < len; i++) {
                valor[i] = chars[rand() % 26];
            }
            valor[len] = '\0';
            break;
        }
    }
}

// Converte tipo pra string legível

const char* tipo_para_str(TipoDado tipo) {
    switch (tipo) {
        case TIPO_INTEIRO: return "INTEIRO";
        case TIPO_BOOLEANO: return "BOOLEANO";
        case TIPO_RACIONAL: return "RACIONAL";
        case TIPO_STRING: return "STRING";
        default: return "?";
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso correto: %s <data_inicio> <hora_inicio> <data_fim> <hora_fim> <sens> [seed]\n", argv[0]);
        printf("Ex: %s \"01/01/2024\" \"00:00:00\" \"01/01/2024\" \"23:59:59\" \"TEMP:0,PRES:2,VIBR:0\" 12345\n", argv[0]);
        printf("Tipos: 0=INT, 1=BOOL, 2=FLOAT, 3=STRING\n");
        return 1;
    }
    
    if (argc < 6) {
        printf("Argumentos insuificientes.\n");
        printf("Precisa de: data_inicio hora_inicio data_fim hora_fim sens [seed]\n");
        return 1;
    }
    
    int dia_ini, mes_ini, ano_ini, hora_ini, min_ini, seg_ini;
    int dia_fim, mes_fim, ano_fim, hora_fim, min_fim, seg_fim;
    unsigned seed = time(NULL);
    
    if (sscanf(argv[1], "%d/%d/%d", &dia_ini, &mes_ini, &ano_ini) != 3) {
        printf("Erro na data inicial. Uso correto: DD/MM/AAAA\n");
        return 1;
    }
    
    if (sscanf(argv[2], "%d:%d:%d", &hora_ini, &min_ini, &seg_ini) != 3) {
        printf("Erro na hora inicial. Uso correto: HH:MM:SS\n");
        return 1;
    }
    
    if (sscanf(argv[3], "%d/%d/%d", &dia_fim, &mes_fim, &ano_fim) != 3) {
        printf("erro na data final. Uso correto: DD/MM/AAAA\n");
        return 1;
    }
    
    if (sscanf(argv[4], "%d:%d:%d", &hora_fim, &min_fim, &seg_fim) != 3) {
        printf("Erro na hora final. Uso correto: HH:MM:SS\n");
        return 1;
    }
    
    // Valida Data e Hora

    if (!validar_data(dia_ini, mes_ini, ano_ini) || !validar_hora(hora_ini, min_ini, seg_ini)) {
        printf("Erro na data/hora inicial.\n");
        return 1;
    }
    
    if (!validar_data(dia_fim, mes_fim, ano_fim) || !validar_hora(hora_fim, min_fim, seg_fim)) {
        printf("Erro na data/hora final.\n");
        return 1;
    }
    
    // Verifica se a data final é depois da inicial

    time_t ts_ini = converter_para_timestamp(dia_ini, mes_ini, ano_ini, hora_ini, min_ini, seg_ini);
    time_t ts_fim = converter_para_timestamp(dia_fim, mes_fim, ano_fim, hora_fim, min_fim, seg_fim);
    
    if (ts_ini >= ts_fim) {
        printf("Datas fora de ordem.\n");
        return 1;
    }
    
    char sens_str[1000];
    strncpy(sens_str, argv[5], sizeof(sens_str) - 1);
    sens_str[sizeof(sens_str) - 1] = '\0';
    
    Sens sens[MAX_SENS];
    int num_sens = 0;
    
    char *pedaco = strtok(sens_str, ",");
    while (pedaco != NULL && num_sens < MAX_SENS) {
        char *dois_pts = strchr(pedaco, ':');
        if (dois_pts == NULL) {
            printf("Sensor mal formatado: %s. Use NOME:TIPO\n", pedaco);
            return 1;
        }
        
        *dois_pts = '\0';
        char *nome = pedaco;
        int tipo = atoi(dois_pts + 1);
        
        if (strlen(nome) >= MAX_NAMES) {
            printf("Nome muito grande: %s\n", nome);
            return 1;
        }
        
        if (tipo < 0 || tipo > 3) {
            printf("Tipo inválido para o sensor %s: %d\n", nome, tipo);
            return 1;
        }
        
        strcpy(sens[num_sens].nome, nome);
        sens[num_sens].tipo = (TipoDado)tipo;
        num_sens++;
        
        pedaco = strtok(NULL, ",");
    }
    
    if (num_sens == 0) {
        printf("Sem sensor válido.\n");
        return 1;
    }
    
    if (argc >= 7) {
        seed = (unsigned)atoi(argv[6]);
    }
    srand(seed);
    
    printf("# GERANDO DADOS DOS SENSORES\n");
    printf("De: %02d/%02d/%04d %02d:%02d:%02d até %02d/%02d/%04d %02d:%02d:%02d\n",
           dia_ini, mes_ini, ano_ini, hora_ini, min_ini, seg_ini,
           dia_fim, mes_fim, ano_fim, hora_fim, min_fim, seg_fim);
    printf("Sensores: %d | Seed: %u\n\n", num_sens, seed);
    
    struct tm tm_inicial = {0}, tm_final = {0};
    
    tm_inicial.tm_year = ano_ini - 1900;
    tm_inicial.tm_mon = mes_ini - 1;
    tm_inicial.tm_mday = dia_ini;
    tm_inicial.tm_hour = hora_ini;
    tm_inicial.tm_min = min_ini;
    tm_inicial.tm_sec = seg_ini;
    tm_inicial.tm_isdst = -1;
    
    tm_final.tm_year = ano_fim - 1900;
    tm_final.tm_mon = mes_fim - 1;
    tm_final.tm_mday = dia_fim;
    tm_final.tm_hour = hora_fim;
    tm_final.tm_min = min_fim;
    tm_final.tm_sec = seg_fim;
    tm_final.tm_isdst = -1;
    
    size_t total_leit = num_sens * LEIT_PSENSOR;
    static Leitura leituras[MAX_SENS * LEIT_PSENSOR];
    
    printf("%d leituras pra cada sensor.\n", LEIT_PSENSOR);
    
    size_t idx = 0;
    for (int i = 0; i < num_sens; i++) {
        printf("Processando sensor %s [%s]...\n", 
               sens[i].nome, tipo_para_str(sens[i].tipo));
        
        for (int j = 0; j < LEIT_PSENSOR; j++) {

            time_t ts = gerar_timestamp_aleatorio(&tm_inicial, &tm_final);
            if (ts == -1) {
                printf("Erro no timestamp.\n");
                return 1;

            }
            
            leituras[idx].timestamp = ts;
            strncpy(leituras[idx].sensor, sens[i].nome, MAX_NAMES - 1);
            leituras[idx].sensor[MAX_NAMES - 1] = '\0';
            
            gerarValor(sens[i].tipo, leituras[idx].valor);
            
            idx++;
        }
    }
    
    // Salva no arquivo
    FILE *arquivo = fopen("dados.txt", "w");
    if (arquivo == NULL) {
        printf("Erro ao criar dados.txt.\n");
        return 1;
    }
    
    printf("\nSalvando no dados.txt...\n");
    
    for (size_t i = 0; i < total_leit; i++) {
        fprintf(arquivo, "%ld %s %s\n",
                (long)leituras[i].timestamp,
                leituras[i].sensor,
                leituras[i].valor);
    }
    
    fclose(arquivo);
    
    // Relatório final

    printf("\n# SUCESSO\n");
    printf("Arquivo: dados.txt criado com sucesso!\n");
    printf("Total: %zu leituras\n", total_leit);
    printf("Sensores: %d\n", num_sens);
    
    printf("\nResumo dos sensores:\n");
    for (int i = 0; i < num_sens; i++) {
        printf("- %s (%s): %d leituras\n", 
               sens[i].nome, tipo_para_str(sens[i].tipo), LEIT_PSENSOR);
    }

}