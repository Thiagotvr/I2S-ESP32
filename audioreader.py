import socket
import wave
import pyaudio

# Configurações de Áudio
FORMAT = pyaudio.paInt16
CHANNELS = 1
RATE = 16000  # Ajustado  para a taxa de amostragem correta de acordo como o programado no ESP32
CHUNK = 4096
RECORD_SECONDS = 5
BUFFER_SIZE = RATE * RECORD_SECONDS * 2  # 2 bytes per sample

# Configuração de Socket UDP
udp = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
udp.bind(("0.0.0.0", 8192))

def save_wav_file(data, index):
    """Salva os dados recebidos em um arquivo WAV."""
    wave_output_filename = f"output_{index}.wav"
    wf = wave.open(wave_output_filename, 'wb')
    wf.setnchannels(CHANNELS)
    wf.setsampwidth(pyaudio.PyAudio().get_sample_size(FORMAT))
    wf.setframerate(RATE)
    wf.writeframes(data)
    wf.close()
    print(f"Arquivo WAV salvo: {wave_output_filename}")

try:
    audio_data = bytearray()
    file_index = 0
    while True:
        # Recebe dados do socket UDP
        data, addr = udp.recvfrom(CHUNK)
        audio_data += data
        
        # Checa se já recebemos dados suficientes para 5 segundos
        while len(audio_data) >= BUFFER_SIZE:
            save_wav_file(audio_data[:BUFFER_SIZE], file_index)
            audio_data = audio_data[BUFFER_SIZE:]  # Resto dos dados para o próximo arquivo
            file_index += 1  # Incrementa o índice para o próximo arquivo
except KeyboardInterrupt:
    pass
finally:
    print("Shutting Down")
    udp.close()
    if len(audio_data) > 0:  # Salva qualquer dado remanescente antes de fechar
        save_wav_file(audio_data, file_index)
