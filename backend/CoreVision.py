import asyncio
import websockets
import json
import os
import cv2
import numpy as np
import random  # Mantido apenas se precisar gerar alguma taxa flutuante secundária
from dotenv import load_dotenv
from keras.models import load_model
from google import genai

# =====================================================================
# 1. CONFIGURAÇÕES INICIAIS E CARREGAMENTO DE MODELOS
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
class_names = []

with open(class_names_path, "r", encoding="utf-8") as f:
    for line in f.readlines():
        clean_name = line.strip().split(" ", 1)[1].strip() if " " in line else line.strip()
        class_names.append(clean_name)

# =====================================================================
# 2. MEMÓRIA GLOBAL DO SISTEMA (ESTADO DA ESTEIRA REASTRADO EM TEMPO REAL)
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
camera = cv2.VideoCapture(1)  # 0 para WebCam Integrada, 1 para Externa
cam_width = 320 
cam_height = 240 
camera.set(cv2.CAP_PROP_FRAME_WIDTH, cam_width)
camera.set(cv2.CAP_PROP_FRAME_HEIGHT, cam_height)

# Subtrator de fundo para detecção física de presença na Região de Interesse (ROI)
bg_subtractor = cv2.createBackgroundSubtractorMOG2(history=100, varThreshold=40, detectShadows=False)

# Constantes de calibração do OpenCV
CONFIDENCE_THRESHOLD = 0.85  # 85% de certeza mínima
MIN_PIXELS_MOTION = 2000     # Sensibilidade de movimento dentro da ROI

# =====================================================================
# 4. LOOP PRINCIPAL DE VISÃO COMPUTACIONAL (ASSÍNCRONO)
# =====================================================================
async def loop_visao_computacional():
    """
    Captura frames da câmera, processa a detecção de movimento e classificação por IA,
    atualiza o estado global e despacha os dados via WebSocket para o frontend.
    """
    global dados_esteira
    print("[Visão] Câmera e processamento OpenCV iniciados...")
    
    item_counted = False
    current_label = ""
    roi_color = (255, 0, 0)  # Blue inicial

    while True:
        # Permite que outras tarefas assíncronas (como o servidor WS) rodem sem travar
        await asyncio.sleep(0.01) 
        
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

        if object_in_roi:
            if not item_counted:
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
                    dados_esteira["dados"]["taxa_deteccao"] = random.randint(35, 45) # Simulação de cadência de leitura
                    
                    print(f"[{class_name}] identificado com {int(confidence_score * 100)}% de precisão.")
                    
                    item_counted = True
                    current_label = class_name
                    mudou_estado = True

                    # Ajuste dinâmico de cores da caixa baseado no item real detectado BGR
                    if current_label == "Coca_Cola": roi_color = (5, 13, 192) # R192 G13 B5
                    elif current_label == "Sprite": roi_color = (50, 205, 50) # LimeGreen  R50 G205 B50
                    elif current_label == "Fanta_Laranja": roi_color = (0, 165, 255) # Orange R255 G165 B0
                    elif current_label == "Fanta_Uva": roi_color = (130, 0, 75) # Indigo R75 G0 B130
                    elif current_label == "Fundo": roi_color = (255, 255, 255) # White R255 G255 B255
                    else: roi_color = (0, 0, 255) # Red R255 G0 B0
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

        cv2.imshow("Scanner de Esteira", image)
        
        # Se houver mudança de estado ou nova contagem, envia imediatamente via WebSocket
        if mudou_estado and clientes_conectados:
            mensagem = json.dumps(dados_esteira)
            await asyncio.gather(*[cliente.send(mensagem) for cliente in clientes_conectados])

        # Monitora a tecla ESC para fechar a aplicação com segurança
        if cv2.waitKey(1) == 27:
            print("[Sistema] Solicitação de encerramento pelo usuário.")
            raise KeyboardInterrupt

# =====================================================================
# 5. GERENCIADOR DO SERVIDOR WEBSOCKET E CHATBOT GEMINI
# =====================================================================
async def manipulador_conexao(websocket):
    """
    Gerencia conexões de rede ativas de painéis web e processa as
    perguntas enviadas ao console integrado de inteligência artificial.
    """
    clientes_conectados.add(websocket)
    print(f"[Rede] Painel conectado. Total de telas ativas: {len(clientes_conectados)}")
    
    # Envia o estado atual da produção assim que a tela abre
    await websocket.send(json.dumps(dados_esteira))
    
    try:
        async for mensagem in websocket:
            dados_resposta = json.loads(mensagem)
            
            # Filtra requisições vindas do Console de IA do Dashboard
            if dados_resposta.get("id") == "dashboard" and dados_resposta.get("designacao") == "console_de_comunicacao":
                requisicao = dados_resposta.get("requisicao")
                print(f"-> Pergunta recebida para o Gemini: {requisicao}")
                
                try:
                    # Contexto injetado no Gemini para que ele compreenda a planta industrial e os dados atuais
                    contexto_prompt = f"""Você é um assistente de IA especialista, integrado a um sistema de automação industrial. O sistema utiliza Visão Computacional (OpenCV, Keras/TensorFlow) para identificar objetos em uma esteira, comunicação serial (Python/PySerial) com um microcontrolador Arduino que controla atuadores, e um backend (Python/WebSockets) que serve um frontend (HTML/JS) para monitoramento em tempo real. Seu objetivo é fornecer respostas e análises considerando a totalidade deste ecossistema.

REGRAS E FORMATO DA RESPOSTA (OBRIGATÓRIO):
1. Estilo de Console: Sua resposta será exibida em um console de log no frontend (dentro de uma tag <pre>). Formate sua saída como se fosse uma mensagem de sistema inteligente.
2. Analise os dados em tempo real da produção para responder: {json.dumps(dados_esteira)}
3. Sem Formatação Complexa: NÃO use Markdown (como `###`, `*`, `-` para listas). Use estritamente tags HTML simples (<b>, i, <pre>, <ul>, <li>, <hr>) para gerar a formatação quando necessário.
4. O comprimento de cada linha da resposta deve ser curto para caber na tela do console, sem quebras de linhas duplas ou blocos densos de texto vago.
5. Linguagem Técnica: Utilize termos apropriados de engenharia e software (ex: 'payload JSON', 'WebSocket', 'Keras', 'ROI OpenCV').

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
                
    except websockets.exceptions.ConnectionClosed:
        pass
    finally:
        clientes_conectados.remove(websocket)
        print(f"[Rede] Painel desconectado. Total de telas ativas: {len(clientes_conectados)}")

# =====================================================================
# 6. INICIALIZAÇÃO ASSÍNCRONA COMPLETA
# =====================================================================
async def main():
    # Inicializa o servidor WebSocket escutando na porta padrão do seu antigo Backend.py
    servidor_ws = await websockets.serve(manipulador_conexao, "localhost", 8765)
    print("[Servidor] Servidor WebSocket rodando em ws://localhost:8765")
    
    # Dispara e gerencia o loop de processamento da câmera real de forma simultânea
    await loop_visao_computacional()

if __name__ == "__main__":
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        print("\n[Sistema] Desligando o scanner de esteira e serviços de rede...")
    finally:
        # Liberação e destruição segura de recursos de hardware externos
        camera.release()
        cv2.destroyAllWindows()
        print("[Sistema] Recursos de hardware liberados com sucesso.")