#include <stdio.h>
#include <string.h>
#include "esp_spiffs.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"

#define TAG         "SPIFFS"

#define lineLength  64

// Função para converter ascii char[] para hex-string (char[])
void string2hexString(char* input, char* output)
{
    int loop;
    int i; 
    
    i=0;
    loop=0;
    
    while(input[loop] != '\0')
    {
        sprintf((char*)(output+i),"%02X", input[loop]);
        loop+=1;
        i+=2;
    }

    // Insere NULL no final da string de saida
    output[i++] = '\0';
}

// Converte hex-string para dado em hex
unsigned char hexString2hexData(unsigned char dado) {

    if (dado >= '0' && dado <= '9') {
        return dado - '0';
    }
    if (dado >= 'A' && dado <= 'F') {
        return dado - 'A' + 0x0A;
    }
    if (dado >= 'a' && dado <= 'f') {
        return dado - 'a' + 0x0A;
    }
    return 0;
}

// Função principal
void app_main()
{
    // Início
    ESP_LOGI(TAG, "Starting SPIFFS");
    esp_vfs_spiffs_conf_t spiffs_config = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true,
    };

    // Use as configurações definidas acima para inicializar e montar o sistema de arquivos SPIFFS.
    esp_err_t ret = esp_vfs_spiffs_register(&spiffs_config);
    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return;
    }

    // Verifica as informações da partição spiffs
    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }

    // Abrir arquivo
    FILE *arquivo = fopen("/spiffs/pasta/arquivo1.hex", "r");

    // Verifica se o arquivo foi aberto
    if (arquivo == NULL) {
        ESP_LOGE(TAG, "Not possible to open file");
    } else {
        char line[lineLength];
        char hex_str[(lineLength*2)+1];
        int valueHex;
        while(fgets(line, sizeof(line), arquivo) != NULL)
        {
            memset(hex_str, 0x00, sizeof(hex_str));
            string2hexString(line, hex_str);
            for (int i = 0; i < sizeof(hex_str); i+=2) {    
                if(hex_str[i] != '\0') {                            
                    printf("HexChar = %c%c ", hex_str[i],hex_str[i+1]);                
                    valueHex = hexString2hexData(hex_str[i]) << 4 | hexString2hexData(hex_str[i+1]);
                    printf("HexData: 0x%02X\n", valueHex);
                    valueHex = 0;
                }  else {
                    break;
                }
            }
            fflush(stdout);
        }
    }
}
