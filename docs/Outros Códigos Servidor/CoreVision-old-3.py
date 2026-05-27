import asyncio
from time import time
import websockets
import json
import os
import cv2
import numpy as np
import random  # Mantido para simulação de taxas secundárias se necessário
import serial  # Integração de comunicação física via protocolo RS232/USB
from dotenv import load_dotenv
from keras.models import load_model
from google import genai

# =====================================================================
# 1. CONFIGURAÇÕES INICIAIS, MODELOS E PORTA SERIAL
# =====================================================================

# Configura o numpy para mostrar números comuns no console
np.set_printoptions(suppress=True)

# Carrega as variáveis de ambiente do arquivo .env
load_dotenv()

# Pega a chave da API do Gemini e valida
api_key = os.getenv("GEMINI_API_KEY")
if not api_key:
    raise ValueError("A chave da API do Gemini (GEMINI_API_KEY) não foi encontrada. Verifique seu arquivo .env")

# Inicializa o cliente do Gemini
client = genai.Client(api_key=api_key)

# Pega o caminho da pasta onde este script está salvo para evitar erros de path relativo
base_path = os.path.dirname(__file__)

# Carrega o modelo da Inteligência Artificial (Keras)
model_path = os.path.join(base_path, "keras_model/keras_model.h5")
model = load_model(model_path, compile=False)

# Abre e lê o arquivo "labels.txt" para mapear as classes da esteira
class_names_path = os.path.join(base_path, "keras_model/labels.txt")
# Lista para armazenar os nomes das classes em ordem de índice (0, 1, 2, ...) conforme o modelo Keras espera.
class_names = [] 
# Lê cada linha do arquivo, limpa espaços e quebras de linha, e extrai apenas o nome da classe (ignorando o número do índice se presente).
with open(class_names_path, "r", encoding="utf-8") as f: 
    for line in f.readlines():
        clean_name = line.strip().split(" ", 1)[1].strip() if " " in line else line.strip()
        class_names.append(clean_name)

# Configurações da Porta Serial vindas do arquivo Comunicacao_Esteira.py
PORTA_SERIAL_ESTEIRA = 'COM5' 
PORTA_SERIAL_ROBO = 'COM9' # COM11 para o modulo Bluetooth HC-05, COM9 para conexão direta com Arduino via USB
BAUD_RATE = 38400
esteira = None  # Objeto global que guardará a conexão ativa da serial da esteira
robo = None  # Objeto global que guardará a conexão ativa da serial do robo

# =====================================================================
# 2. MEMÓRIA GLOBAL DO SISTEMA (ESTADO DA ESTEIRA E TELEMETRIA)
# =====================================================================
dados_esteira = {
    "id": "servidor",
    "designacao": "dados_esteira",
    "dados": {
        "contagem_garrafas": {
            "coca_cola": 0, 
            "sprite": 0, 
            "fanta_laranja": 0, 
            "fanta_uva": 0, 
            
            "coca_cola_vazia": 0,
            "sprite_vazia": 0,
            "fanta_laranja_vazia": 0,
            "fanta_uva_vazia": 0,
            
            "coca_cola_com_sprite": 0,
            "coca_cola_com_fanta_laranja": 0,
            "coca_cola_com_fanta_uva": 0,
            
            "sprite_com_coca_cola": 0,
            "sprite_com_fanta_laranja": 0,
            "sprite_com_fanta_uva": 0,

            "fanta_com_coca_cola": 0,
            "fanta_com_sprite": 0,

            "erro": 0
        },
        "status": "Aguardando",
        "taxa_deteccao": 0,
        "ultimo_item": None,
        "precisao": 0.0
    }
}

# Lista de controle para os clientes WebSocket conectados (Frontends)
clientes_conectados = set()

# =====================================================================
# 3. CONEXÃO DA CÂMERA E CONFIGURAÇÕES DO OPENCV
# =====================================================================
camera = cv2.VideoCapture(0)  # 0 para WebCam Integrada, 1 para Externa
# Reduzido para 320x240 para melhorar a performance, mantendo a proporção 4:3 e a qualidade suficiente para o modelo Keras
cam_width = 320 
cam_height = 240 
camera.set(cv2.CAP_PROP_FRAME_WIDTH, cam_width)
camera.set(cv2.CAP_PROP_FRAME_HEIGHT, cam_height)

# Subtrator de fundo para detecção física de presença na Região de Interesse (ROI)
bg_subtractor = cv2.createBackgroundSubtractorMOG2(history=100, varThreshold=40, detectShadows=False)

# Constantes de calibração do OpenCV
CONFIDENCE_THRESHOLD = 0.95  # 95% de certeza mínima
MIN_PIXELS_MOTION = 2000     # Sensibilidade de movimento dentro da ROI

# =====================================================================
# 4. ROTINAS DE COMUNICAÇÃO HARDWARE (SERIAL / ESTEIRA / ROBO)
# =====================================================================
def conectar_esteira():
    """
    Realiza o handshake síncrono inicial com a esteira antes de abrir
    as tarefas assíncronas do sistema central, garantindo estabilidade de portas.
    """
    try:
        esteira_obj = serial.Serial(PORTA_SERIAL_ESTEIRA, BAUD_RATE, timeout=0.1)
        print(f"[Serial] Conectando à porta {PORTA_SERIAL_ESTEIRA}...")
        time.sleep(2)  # Aguarda o reset automático padrão do bootloader da esteira
        
        print("[Serial] Aguardando sinal PING de inicialização da esteira...")
        timeout_handshake = time.time() + 10  # Janela limite de 10s para estabelecer conexão
        
        while time.time() < timeout_handshake: # Loop de espera para o handshake inicial da esteira
            if esteira_obj.in_waiting > 0: # Verifica se há dados chegando da esteira
                resposta = esteira_obj.readline().decode('utf-8').strip() # Lê a linha completa enviada pela esteira e decodifica para string
                if resposta.startswith("{"): # Tenta interpretar a resposta como JSON, esperando um formato específico de handshake
                    try:
                        dados = json.loads(resposta) # Converte a string JSON em um dicionário Python para análise
                        if dados.get("id") == "esteira" and dados.get("conexao") == "aguardando":
                            print("[Serial] Esteira localizada! Enviando confirmação de escuta...")
                            mensagem_resposta = {"id": "servidor", "conexao": "ouvindo"}
                            esteira_obj.write((json.dumps(mensagem_resposta) + "\n").encode('utf-8'))
                            time.sleep(0.5) # Pequena pausa para garantir que a Esteira processe a resposta antes de esperar o próximo sinal
                            
                        elif dados.get("id") == "esteira" and dados.get("conexao") == "estabelecida":
                            print("[Serial] Canal de comunicação serial bidirecional JSON ATIVO!\n")
                            return esteira_obj # Retorna o objeto de conexão serial para uso nas tarefas assíncronas
                    except json.JSONDecodeError: # Se a resposta não for um JSON válido, ignora e continua esperando o handshake correto    
                        pass # Ignora mensagens que não sejam JSON ou que não sigam o formato esperado do handshake
        print("[Aviso Serial] Handshake falhou por timeout. Prosseguindo em modo simulado/offline.")
        return None
    except Exception as e:
        print(f"[Erro Serial] Não foi possível acessar a porta {PORTA_SERIAL_ESTEIRA}: {e}")
        return None

def conectar_robo():
    """
    Realiza o handshake síncrono inicial com o robo antes de abrir
    as tarefas assíncronas do sistema central, garantindo estabilidade de portas.
    """
    try:
        robo_obj = serial.Serial(PORTA_SERIAL_ROBO, BAUD_RATE, timeout=0.1)
        print(f"[Serial] Conectando à porta {PORTA_SERIAL_ROBO}...")
        time.sleep(2)  # Aguarda o reset automático padrão do bootloader do Robo
        
        print("[Serial] Aguardando sinal PING de inicialização do Robô...")
        timeout_handshake = time.time() + 10  # Janela limite de 10s para estabelecer conexão
        
        while time.time() < timeout_handshake: # Loop de espera para o handshake inicial do Robo
            if robo_obj.in_waiting > 0: # Verifica se há dados chegando do Robo
                resposta = robo_obj.readline().decode('utf-8').strip() # Lê a linha completa enviada pelo Robo e decodifica para string
                if resposta.startswith("{"): # Tenta interpretar a resposta como JSON, esperando um formato específico de handshake
                    try:
                        dados = json.loads(resposta) # Converte a string JSON em um dicionário Python para análise
                        if dados.get("id") == "robo" and dados.get("conexao") == "aguardando":
                            print("[Serial] Robo localizado! Enviando confirmação de escuta...")
                            mensagem_resposta = {"id": "servidor", "conexao": "ouvindo"}
                            robo_obj.write((json.dumps(mensagem_resposta) + "\n").encode('utf-8'))
                            time.sleep(0.5) # Pequena pausa para garantir que o Robo processe a resposta antes de esperar o próximo sinal
                            
                        elif dados.get("id") == "robo" and dados.get("conexao") == "estabelecida":
                            print("[Serial] Canal de comunicação serial bidirecional JSON ATIVO!\n")
                            return robo_obj # Retorna o objeto de conexão serial para uso nas tarefas assíncronas
                    except json.JSONDecodeError: # Se a resposta não for um JSON válido, ignora e continua esperando o handshake correto    
                        pass # Ignora mensagens que não sejam JSON ou que não sigam o formato esperado do handshake
        print("[Aviso Serial] Handshake falhou por timeout. Prosseguindo em modo simulado/offline.")
        return None
    except Exception as e:
        print(f"[Erro Serial] Não foi possível acessar a porta {PORTA_SERIAL_ROBO}: {e}")
        return None


async def escutar_retorno_serial():
    """
    Tarefa assíncrona executada em background para monitorar respostas e logs do Arduino
    sem bloquear o processamento de imagens do OpenCV ou conexões de rede.
    """
    global esteira, robo
    if not robo: # Se a conexão serial não foi estabelecida, esta tarefa não tem função e pode ser encerrada imediatamente
        print("[Serial] Modo simulado: Nenhum hardware conectado. A tarefa de escuta serial permanecerá inativa.")
        return 
        
    print("[Serial] Task assíncrona de escuta serial iniciada.")
    while True:
        await asyncio.sleep(0.05)  # Evita sobrecarga de processamento no processador
        
        try:
            # Verifica se há dados chegando do Esteira
            if esteira.in_waiting > 0: 
                resposta_bruta = esteira.readline().decode('utf-8').strip() # Lê a linha completa enviada pela Esteira e decodifica para string
                try:
                    dados_resposta = json.loads(resposta_bruta)
                    print(f"[Esteira -> Servidor] {dados_resposta}")
                except json.JSONDecodeError:
                    # Se não for JSON válido, printa como mensagem de texto crua para debug/log
                    if resposta_bruta:
                        print(f"[Esteira Log] {resposta_bruta}")
        except Exception as e:
            print(f"[Erro Serial] Falha na leitura física: {e}")
            break
        
        try:
            # Verifica se há dados chegando do Robo
            if robo.in_waiting > 0: 
                resposta_bruta = robo.readline().decode('utf-8').strip() # Lê a linha completa enviada pelo Robo e decodifica para string
                try:
                    dados_resposta = json.loads(resposta_bruta)
                    print(f"[Robo -> Servidor] {dados_resposta}")
                except json.JSONDecodeError:
                    # Se não for JSON válido, printa como mensagem de texto crua para debug/log
                    if resposta_bruta:
                        print(f"[Robo Log] {resposta_bruta}")
        except Exception as e:
            print(f"[Erro Serial] Falha na leitura física: {e}")
            break

def enviar_comando_maquina(label_item):
    """
    Avalia a String identificada pela visão computacional e despacha o payload
    específico para o barramento serial mapeando atuadores ou rotinas do robô.
    """
    global esteira, robo
    if not robo: #if not esteira and not robo:
        print(f"[Simulação Serial] Comando retido (Sem hardware): {label_item}")
        return

    label_minusculo = label_item.lower()

    # Cenário 1: Erros de fabricação -> Dispara STRING direta para o Robô
    if "erro" in label_minusculo or label_item == "Erro de Fabricacao":
        payload = "ROTINA_01\n"
        robo.write(payload.encode('utf-8'))
        print(f"[Serial] Comando enviado ao Robô: {payload.strip()}")

    # Cenário 2: Garrafas qualificadas como Vazias -> Dispara STRING direta para o Robô
    elif "coca_cola_vazia" in label_minusculo or "sprite_vazia" in label_minusculo or "fanta_laranja_vazia" in label_minusculo or "fanta_uva_vazia" in label_minusculo:
        payload = "ROTINA_02\n"
        robo.write(payload.encode('utf-8'))
        print(f"[Serial] Comando enviado ao Robô: {payload.strip()}")
        
    # Cenário 3: Garrafas corretas/normais -> Dispara JSON estruturado para a Esteira
    else:
        payload_dict = {
            "id": "servidor",
            "deteccao_de_item": label_item
        }
        payload_json = json.dumps(payload_dict) + "\n"
        esteira.write(payload_json.encode('utf-8'))
        print(f"[Serial] JSON enviado à Esteira: {payload_dict}")

# =====================================================================
# 5. LOOP PRINCIPAL DE VISÃO COMPUTACIONAL (ASSÍNCRONO)
# =====================================================================
async def loop_visao_computacional(): 
    """
    Captura frames da câmera, processa a detecção de movimento e classificação por IA,
    atualiza o estado global e despacha os dados via WebSocket e Serial simultaneamente.
    """
    global dados_esteira
    print("[Visão] Câmera e processamento OpenCV iniciados...")
    
    item_counted = False
    current_label = ""
    roi_color = (255, 0, 0)  # Blue inicial

    while True:
        # Permite que outras tarefas assíncronas (como o servidor WS) rodem sem travar
        await asyncio.sleep(0.01) # Pequena pausa para evitar bloqueio total do loop e permitir resposta a conexões WebSocket
        
        ret, image = camera.read()
        if not ret:
            print("[Erro] Falha ao ler a câmera.")
            await asyncio.sleep(1)
            continue

        h, w, _ = image.shape
        start_y = (h - 224) // 2
        start_x = (w - 224) // 2
        end_y = start_y + 224
        end_x = start_x + 224

        # Recorta a região central de análise (ROI)
        roi = image[start_y:end_y, start_x:end_x]

        # Detector de presença físico por movimento
        fg_mask = bg_subtractor.apply(roi)
        motion_pixels = cv2.countNonZero(fg_mask)
        object_in_roi = motion_pixels > MIN_PIXELS_MOTION

        mudou_estado = False 

        if object_in_roi: # Se há um objeto presente na ROI, processa a classificação por IA
            if not item_counted: # Evita múltiplas contagens para o mesmo item enquanto ele estiver na ROI
                roi_color = (0, 255, 255)  # Amarelo: Processando
                dados_esteira["dados"]["status"] = "Processando"
                mudou_estado = True
                
                # Prepara imagem para o Keras
                roi_array = np.asarray(roi, dtype=np.float32).reshape(1, 224, 224, 3)
                roi_array = (roi_array / 127.5) - 1

                # Executa a predição da IA local
                prediction = model.predict(roi_array, verbose=0) 
                index = np.argmax(prediction)
                class_name = class_names[index]
                confidence_score = prediction[0][index]

                if confidence_score >= CONFIDENCE_THRESHOLD and "Fundo" not in class_name:
                    # Mapeia a string vinda do label para a chave correta do dicionário JSON (minúsculo)
                    chave_contador = class_name.lower()
                    
                    if chave_contador in dados_esteira["dados"]["contagem_garrafas"]:
                        dados_esteira["dados"]["contagem_garrafas"][chave_contador] += 1
                    else:
                        dados_esteira["dados"]["contagem_garrafas"]["erro"] += 1

                    # Atualiza os dados de telemetria da esteira
                    dados_esteira["dados"]["ultimo_item"] = class_name
                    dados_esteira["dados"]["precisao"] = round(float(confidence_score), 2)
                    dados_esteira["dados"]["taxa_deteccao"] = random.randint(35, 45) 
                    
                    print(f"[{class_name}] identificado com {int(confidence_score * 100)}% de precisão.")
                    
                    item_counted = True
                    current_label = class_name
                    mudou_estado = True

                    # --- ACIONAMENTO DA AUTOMAÇÃO VIA SERIAL ---
                    # Despacha o sinal imediatamente para o hardware assim que confirmado pela IA
                    enviar_comando_maquina(class_name)

                    # Ajuste dinâmico de cores da caixa baseado no item real detectado BGR
                    if current_label == "Coca_Cola": roi_color = (5, 13, 192) 
                    elif current_label == "Sprite": roi_color = (50, 205, 50) 
                    elif current_label == "Fanta_Laranja": roi_color = (0, 165, 255) 
                    elif current_label == "Fanta_Uva": roi_color = (130, 0, 75) 
                    else: roi_color = (0, 0, 255) 
        else:
            # Se a esteira ficou limpa e o estado anterior era diferente, limpa os campos
            if item_counted or dados_esteira["dados"]["status"] != "Aguardando":
                item_counted = False
                current_label = ""
                roi_color = (255, 0, 0)
                dados_esteira["dados"]["status"] = "Aguardando"
                dados_esteira["dados"]["taxa_deteccao"] = 0
                mudou_estado = True

        # --- PROCESSAMENTO GRÁFICO (RESTAURADO DO COREVISION) ---
        cv2.rectangle(image, (start_x, start_y), (end_x, end_y), roi_color, 2)
        if item_counted and current_label:
            cv2.rectangle(image, (start_x, start_y), (start_x + 224, start_y + 25), roi_color, cv2.FILLED)
            cv2.putText(image, f"{current_label}", (start_x + 5, start_y + 18),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 0, 0), 2)

        # Define o título da janela para "Scanner de Esteira" para melhor identificação
        cv2.imshow("Scanner de Esteira", image) 
        
        # Se houver mudança de estado ou nova contagem, envia imediatamente via WebSocket
        if mudou_estado and clientes_conectados: # Evita enviar dados se não houver clientes conectados para receber
            mensagem = json.dumps(dados_esteira) # Converte o estado atualizado da esteira para JSON
            await asyncio.gather(*[cliente.send(mensagem) for cliente in clientes_conectados]) # Envia a mensagem para todos os clientes conectados de forma assíncrona e eficiente

        # Monitora a tecla ESC para fechar a aplicação com segurança
        if cv2.waitKey(1) == 27:
            print("[Sistema] Solicitação de encerramento pelo usuário.")
            raise KeyboardInterrupt

# =====================================================================
# 6. GERENCIADOR DO SERVIDOR WEBSOCKET E CHATBOT GEMINI
# =====================================================================
async def manipulador_conexao(websocket):
    """
    Gerencia conexões de rede ativas de painéis web e processa as
    perguntas enviadas ao console integrado de inteligência artificial.
    """
    clientes_conectados.add(websocket)
    print(f"[Rede] Painel conectado. Total de telas ativas: {len(clientes_conectados)}")
    
    # Converter o estado atual da esteira para JSON para o formato de mensagem e enviar ao cliente WebSocket recém-conectado, garantindo que ele tenha os dados mais recentes assim que se conectar ao painel web.
    await websocket.send(json.dumps(dados_esteira))  
    try: 
        async for mensagem in websocket: # Loop para receber mensagens do cliente WebSocket (painel web)
            dados_resposta = json.loads(mensagem) # Converte a mensagem JSON recebida em um dicionário Python para processamento
            
            # Filtra requisições vindas do Console de IA do Dashboard
            if dados_resposta.get("id") == "dashboard" and dados_resposta.get("designacao") == "console_de_comunicacao":
                requisicao = dados_resposta.get("requisicao")
                print(f"-> Pergunta recebida para o Gemini: {requisicao}")
                
                try:
                    contexto_prompt = f"""Você é um assistente de IA especialista, integrado a um sistema de automação industrial. O sistema utiliza Visão Computacional (OpenCV, Keras/TensorFlow) para identificar objetos em uma esteira, comunicação serial (Python/PySerial) com um microcontrolador Arduino que controla atuadores, e um backend (Python/WebSockets) que serve um frontend (HTML/JS) para monitoramento em tempo real. Seu objetivo é fornecer respostas e análises considerando a totalidade deste ecossistema.

REGRAS E FORMATO DA RESPOSTA (OBRIGATÓRIO):
1. Estilo de Console: Sua resposta será exibida em um console de log no frontend (dentne de uma tag <pre>). Formate sua saída como se fosse uma mensagem de sistema inteligente.
2. Analise os dados em tempo real da produção para responder: {json.dumps(dados_esteira)}
3. Sem Formatação Complexa: NÃO use Markdown (como `###`, `*`, `-` para listas). Use estritamente tags HTML simples (<b>, i, <pre>, <ul>, <li>, <hr>) para gerar a formatação quando necessário.
4. O comprimento de cada linha da resposta deve ser curto para caber na tela do console, sem quebras de linhas duplas ou blocos densos de texto vago.
5. Linguagem Técnica: Utilize termos apropriados de engenharia e software (ex: 'payload JSON', 'WebSocket', 'Keras', 'ROI OpenCV', 'Serial UART').

Responda à pergunta do operador de forma clara e direta:
"""
                    pergunta = f"{contexto_prompt}\n\n{requisicao}"
                    
                    # Chamada oficial da API do Gemini utilizando o modelo flash recomendado
                    response = client.models.generate_content(
                        model='gemini-2.5-flash',
                        contents=pergunta,
                    )
                    texto_resposta = response.text
                    
                except Exception as e:
                    print(f"[Erro IA] Falha ao gerar resposta com Gemini: {e}")
                    texto_resposta = "<b>[ERRO SISTEMA]</b> Não foi possível processar a requisição com a IA."
                
                # Devolve a resposta estruturada para o frontend emitir no log
                resposta_para_cliente = {
                    "id": "servidor",
                    "designacao": "console_de_comunicacao",
                    "resposta": texto_resposta
                }
                await websocket.send(json.dumps(resposta_para_cliente))
                
    except websockets.exceptions.ConnectionClosed: # Captura a desconexão do cliente WebSocket para limpeza de recursos e atualização de status
        pass # A desconexão é tratada no bloco 
    # Remove o cliente da lista de conectados e atualiza o status no console
    finally: 
        clientes_conectados.remove(websocket)
        print(f"[Rede] Painel desconectado. Total de telas ativas: {len(clientes_conectados)}")

# =====================================================================
# 7. INICIALIZAÇÃO ASSÍNCRONA COMPLETA
# =====================================================================
async def main():
    # Inicializa o servidor WebSocket escutando na porta 8765
    servidor_ws = await websockets.serve(manipulador_conexao, "localhost", 8765)
    print("[Servidor] Servidor WebSocket rodando em ws://localhost:8765")
    
    # Executa em background a task paralela que monitora os retornos/logs enviados pelo Arduino
    asyncio.create_task(escutar_retorno_serial())
    
    # Dispara e gerencia o loop de processamento da câmera real de forma simultânea
    await loop_visao_computacional()

if __name__ == "__main__":
    # Executa o handshake de conexão serial ANTES de entrar no loop assíncrono de eventos
    esteira = conectar_esteira()
    robo = conectar_robo()
    
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        print("\n[Sistema] Desligando o scanner de esteira e serviços de rede...")
    finally:
        # Liberação e destruição segura de todos os recursos de hardware externos ativados
        camera.release()
        cv2.destroyAllWindows()
        if esteira and esteira.is_open:
            esteira.close()
            print("[Serial - Esteira] Porta de comunicação física fechada com segurança.")
        if robo and robo.is_open:
            robo.close()
            print("[Serial - Robo] Porta de comunicação física fechada com segurança.")
        print("[Sistema] Recursos de hardware liberados com sucesso.")