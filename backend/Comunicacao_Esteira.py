import serial
import json
import time

# CONFIGURAÇÃO DA PORTA
# USB: Geralmente 'COM3', 'COM4' no Windows ou '/dev/ttyUSB0' no Linux
# Bluetooth: Procure nas configurações do PC qual porta COM foi atribuída ao HC-05 após o pareamento
# COM9 USB arduino UNO
# COM11 para Bluetooth HC-06
# !IMPORTANTE! Não abra o monitor serial quando for rodar o código, No Windows, dois programas não podem controlar a mesma porta serial simultaneamente.
PORTA_SERIAL = 'COM11' 
BAUD_RATE = 38400

def conectar_arduino():
    try:
        # Abre a porta serial. O timeout é vital para não travar o script se o Arduino sumir
        arduino = serial.Serial(PORTA_SERIAL, BAUD_RATE, timeout=2)
        print(f"Conectando à porta {PORTA_SERIAL}...")
        time.sleep(2) # Aguarda o reset automático do Arduino ao abrir a porta
        
        # --- PASSO 4: PROCESSO DE HANDSHAKE ---
        print("Aguardando sinal PING do Arduino...")
        while True:
            if arduino.in_waiting > 0:
                linha = arduino.readline().decode('utf-8').strip()
                
                if linha == "PING":
                    print("Arduino detectado! Enviando PONG...")
                    arduino.write("PONG\n".encode('utf-8'))
                    
                # Aguarda a confirmação em JSON do Arduino
                elif linha.startswith("{"):
                    try:
                        dados = json.loads(linha)
                        if dados.get("conexao") == "estabelecida":
                            print("Comunicação bidirecional JSON ativa!\n")
                            return arduino
                    except json.JSONDecodeError:
                        pass
    except Exception as e:
        print(f"Erro ao conectar: {e}")
        return None

def loop_comunicacao(arduino):
    try:
        contador = 0
        while True:
            contador += 1
            # Alterna o LED do Arduino baseado no contador (par = liga, ímpar = desliga)
            estado_led = 1 if contador % 2 == 0 else 0
            
            # --- PASSO 1: PYTHON ENVIA MSG (JSON) ---
            payload = {
                "comando": f"leitura_{contador}",
                "led": estado_led
            }
            
            # Transforma dicionário Python em String JSON e adiciona \n no final
            mensagem_json = json.dumps(payload) + "\n"
            arduino.write(mensagem_json.encode('utf-8'))
            print(f"Enviado para Arduino: {payload}")
            
            # Pequena pausa para o Arduino processar
            time.sleep(0.5)
            
            # --- PASSO 2 e 3: PYTHON LÊ E PROCESSA RESPOSTA ---
            if arduino.in_waiting > 0:
                resposta_bruta = arduino.readline().decode('utf-8').strip()
                
                try:
                    # Converte a String JSON recebida em um dicionário Python estruturado
                    dados_resposta = json.loads(resposta_bruta)
                    print(f"Recebido do Arduino: {dados_resposta}")
                    print(f"-> Valor do Sensor A0: {dados_resposta.get('sensor_A0')}")
                    print(f"-> Status da Operação: {dados_resposta.get('status')}\n")
                except json.JSONDecodeError:
                    print(f"Erro ao decodificar resposta: {resposta_bruta}")
                    
            time.sleep(1.5) # Intervalo entre os envios do loop
            
    except KeyboardInterrupt:
        print("\nFinalizando comunicação...")
    finally:
        arduino.close()
        print("Porta serial fechada.")

if __name__ == "__main__":
    conexao = conectar_arduino()
    if conexao:
        loop_comunicacao(conexao)