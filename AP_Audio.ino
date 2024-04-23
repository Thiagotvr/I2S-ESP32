#include <driver/i2s.h>  // Biblioteca para interface I2S.
#include <WiFi.h>        // Biblioteca para operações de WiFi.
#include <WiFiUdp.h>     // Biblioteca para UDP sobre WiFi.

// Credenciais da rede WiFi.
const char* ssid = "Teste"; 
const char* password = "12345678";

// Endereço IP e porta do servidor UDP.
const char *udpAddress = "192.168.137.51";
const int udpPort = 8192;

// Definições dos pinos I2S.
#define I2S_WS 25  // Word Select (ou LR Clock).
#define I2S_SD 32  // Serial Data.
#define I2S_SCK 33 // Serial Clock.
#define I2S_PORT I2S_NUM_0 // Porta I2S utilizada.

// Configurações de amostragem de áudio.
#define SAMPLE_RATE 16000  // Taxa de amostragem em Hz.
#define BUFFER_SIZE 1024   // Tamanho do buffer para leitura de dados.

WiFiUDP udp; // Instância de WiFiUDP para enviar dados por UDP.

void setup() {
  Serial.begin(115200); // Inicializa comunicação serial.
  WiFi.begin(ssid, password); // Inicia conexão WiFi.
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); // Aguarda conexão ser estabelecida.
    Serial.print(".");
  }
  Serial.println("WiFi connected");

  // Configuração da interface I2S para recepção de áudio.
  i2s_config_t i2s_config = {
    .mode = I2S_MODE_MASTER | I2S_MODE_RX, // Define o modo mestre e de recepção.
    .sample_rate = SAMPLE_RATE, // Define a taxa de amostragem.
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, // Bits por amostra.
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT, // Formato de canal único.
    .communication_format = I2S_COMM_FORMAT_I2S_MSB, // Formato de comunicação I2S.
    .intr_alloc_flags = 0, // Flags de alocação de interrupção.
    .dma_buf_count = 4, // Número de buffers DMA.
    .dma_buf_len = 256, // Comprimento dos buffers DMA.
    .use_apll = false, // Não usa o PLL de áudio.
    .tx_desc_auto_clear = false, // Desabilita limpeza automática do descritor de transmissão.
    .fixed_mclk = 0 // Clock M mestre fixo desabilitado.
  };
  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK, // Pino de clock serial.
    .ws_io_num = I2S_WS, // Pino de seleção de palavra.
    .data_out_num = -1, // Sem pino de saída de dados.
    .data_in_num = I2S_SD // Pino de entrada de dados.
  };
  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL); // Instala driver I2S.
  i2s_set_pin(I2S_PORT, &pin_config); // Configura os pinos I2S.

  udp.begin(WiFi.localIP(), udpPort); // Inicia UDP no endereço IP local e porta especificada.
}

void loop() {
  uint8_t buffer[BUFFER_SIZE]; // Buffer para armazenar dados de áudio.
  size_t bytesRead;

  // Lê dados do dispositivo I2S.
  i2s_read(I2S_PORT, &buffer, sizeof(buffer), &bytesRead, portMAX_DELAY);
  if (bytesRead > 0) {
    udp.beginPacket(udpAddress, udpPort); // Inicia um pacote UDP.
    udp.write(buffer, bytesRead); // Escreve o buffer no pacote UDP.
    udp.endPacket(); // Envia o pacote.
  }
}
