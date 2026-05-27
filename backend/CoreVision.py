import asyncio
import time  # Corrigido o import para aceitar time.sleep() nos handshakes síncronos
import websockets
import json
import os
import cv2
import numpy as np
import random
import serial
from dotenv import load_dotenv
from keras.models import load_model
from google import genai

# =====================================================================
# 1. CONFIGURAÇÕES INICIAIS, MODELOS E PORTA SERIAL
# =====================================================================

np.set_printoptions(suppress=True)
load_dotenv()

api_key = os.getenv("GEMINI_API_KEY")
if not api_key:
    raise ValueError(
        "A chave da API do Gemini (GEMINI_API_KEY) não foi encontrada. Verifique seu arquivo .env"
    )

client = genai.Client(api_key=api_key)
base_path = os.path.dirname(__file__)

# Carrega o modelo Keras
model_path = os.path.join(base_path, "keras_model/keras_model.h5")
model = load_model(model_path, compile=False)

# Mapeamento de Labels
class_names_path = os.path.join(base_path, "keras_model/labels.txt")
class_names = []
with open(class_names_path, "r", encoding="utf-8") as f:
    for line in f.readlines():
        clean_name = (
            line.strip().split(" ", 1)[1].strip() if " " in line else line.strip()
        )
        class_names.append(clean_name)

# Portas Seriais Dedicadas
PORTA_SERIAL_ESTEIRA = "COM5"
PORTA_SERIAL_ROBO = "COM9"
BAUD_RATE = 38400

esteira = None
robo = None

# =====================================================================
# 2. MEMÓRIA GLOBAL DO SISTEMA
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
            "erro": 0,
        },
        "status": "Aguardando",
        "taxa_deteccao": 0,
        "ultimo_item": None,
        "precisao": 0.0,
    },
}

clientes_conectados = set()

# AJUSTE 2.1: Flag para controlar o estado de pausa do sistema por falha de hardware
sistema_pausado_por_falha_hardware = False

# =====================================================================
# 3. CONEXÃO DA CÂMERA E CONFIGURAÇÕES DO OPENCV
# =====================================================================
camera = cv2.VideoCapture(1)
cam_width = 320
cam_height = 240
camera.set(cv2.CAP_PROP_FRAME_WIDTH, cam_width)
camera.set(cv2.CAP_PROP_FRAME_HEIGHT, cam_height)

bg_subtractor = cv2.createBackgroundSubtractorMOG2(
    history=100, varThreshold=40, detectShadows=False
)

CONFIDENCE_THRESHOLD = 0.95
MIN_PIXELS_MOTION = 2000


# =====================================================================
# 4. ROTINAS DE COMUNICAÇÃO HARDWARE INDEPENDENTES
# =====================================================================
def conectar_esteira():
    try:
        esteira_obj = serial.Serial(PORTA_SERIAL_ESTEIRA, BAUD_RATE, timeout=0.1)
        print(f"[Serial Esteira] Conectando à porta {PORTA_SERIAL_ESTEIRA}...")
        time.sleep(2)  # Corrigido uso do módulo time

        print("[Serial Esteira] Aguardando sinal PING...")
        timeout_handshake = time.time() + 6

        while time.time() < timeout_handshake:
            if esteira_obj.in_waiting > 0:
                resposta = esteira_obj.readline().decode("utf-8").strip()
                if resposta.startswith("{"):
                    try:
                        dados = json.loads(resposta)
                        if (
                            dados.get("id") == "esteira"
                            and dados.get("conexao") == "aguardando"
                        ):
                            print("[Serial Esteira] Localizada! Enviando handshake...")
                            mensagem_resposta = {"id": "servidor", "conexao": "ouvindo"}
                            esteira_obj.write(
                                (json.dumps(mensagem_resposta) + "\n").encode("utf-8")
                            )
                            time.sleep(0.5)
                        elif (
                            dados.get("id") == "esteira"
                            and dados.get("conexao") == "estabelecida"
                        ):
                            print("[Serial Esteira] Comunicação ativa!\n")
                            return esteira_obj
                    except json.JSONDecodeError:
                        pass
        print(
            "[Aviso Esteira] Handshake falhou. Executando em modo offline para Esteira."
        )
        return None
    except Exception as e:
        print(f"[Erro Esteira] Porta {PORTA_SERIAL_ESTEIRA} inacessível: {e}")
        return None


def conectar_robo():
    try:
        robo_obj = serial.Serial(PORTA_SERIAL_ROBO, BAUD_RATE, timeout=0.1)
        print(f"[Serial Robô] Conectando à porta {PORTA_SERIAL_ROBO}...")
        time.sleep(2)

        print("[Serial Robô] Aguardando sinal PING...")
        timeout_handshake = time.time() + 6

        while time.time() < timeout_handshake:
            if robo_obj.in_waiting > 0:
                resposta = robo_obj.readline().decode("utf-8").strip()
                if resposta.startswith("{"):
                    try:
                        dados = json.loads(resposta)
                        if (
                            dados.get("id") == "robo"
                            and dados.get("conexao") == "aguardando"
                        ):
                            print("[Serial Robô] Localizado! Enviando handshake...")
                            mensagem_resposta = {"id": "servidor", "conexao": "ouvindo"}
                            robo_obj.write(
                                (json.dumps(mensagem_resposta) + "\n").encode("utf-8")
                            )
                            time.sleep(0.5)
                        elif (
                            dados.get("id") == "robo"
                            and dados.get("conexao") == "estabelecida"
                        ):
                            print("[Serial Robô] Comunicação ativa!\n")
                            return robo_obj
                    except json.JSONDecodeError:
                        pass
        print("[Aviso Robô] Handshake falhou. Executando em modo offline para Robô.")
        return None
    except Exception as e:
        print(f"[Erro Robô] Porta {PORTA_SERIAL_ROBO} inacessível: {e}")
        return None


async def reconectar_hardware():
    """
    AJUSTE 2.3: Tenta reconectar com o hardware em background se a conexão for perdida.
    Executa as funções de conexão síncronas em uma thread separada para não bloquear o loop de eventos.
    """
    global esteira, robo
    while True:
        # Só tenta reconectar se o sistema estiver em estado de falha
        if sistema_pausado_por_falha_hardware:
            if not esteira:
                print("[Sistema] Tentando reconectar com a Esteira...")
                nova_conexao_esteira = await asyncio.to_thread(conectar_esteira)
                if nova_conexao_esteira:
                    esteira = nova_conexao_esteira

            if not robo:
                print("[Sistema] Tentando reconectar com o Robô...")
                nova_conexao_robo = await asyncio.to_thread(conectar_robo)
                if nova_conexao_robo:
                    robo = nova_conexao_robo
        await asyncio.sleep(5)  # Intervalo entre as tentativas de reconexão


async def escutar_retorno_serial():
    """
    Monitora de forma assíncrona ambas as portas seriais. Protegido contra
    objetos nulos (None) caso um dos arduinos não esteja conectado.
    """
    global esteira, robo
    if not robo and not esteira:
        print("[Serial] Sem hardwares detectados. Escuta serial desativada.")
        return

    print("[Serial] Task de monitoramento de hardware em execução.")
    while True:
        await asyncio.sleep(0.03)

        # Monitoramento Seguro da Esteira
        if esteira and esteira.is_open:
            try:
                if esteira.in_waiting > 0:
                    resposta_bruta = esteira.readline().decode("utf-8").strip()
                    try:
                        dados_resposta = json.loads(resposta_bruta)
                        print(f"[Esteira -> Servidor] {dados_resposta}")
                    except json.JSONDecodeError:
                        if resposta_bruta:
                            print(f"[Esteira Log] {resposta_bruta}")
            except Exception as e:
                print(
                    f"[Erro Serial Esteira] Conexão perdida: {e}. O sistema irá pausar e tentar reconectar."
                )
                esteira = None  # Desativa para evitar loops de erro

        # Monitoramento Seguro do Robô
        if robo and robo.is_open:
            try:
                if robo.in_waiting > 0:
                    resposta_bruta = robo.readline().decode("utf-8").strip()
                    try:
                        dados_resposta = json.loads(resposta_bruta)
                        print(f"[Robô -> Servidor] {dados_resposta}")
                    except json.JSONDecodeError:
                        if resposta_bruta:
                            print(f"[Robô Log] {resposta_bruta}")
            except Exception as e:
                print(
                    f"[Erro Serial Robô] Conexão perdida: {e}. O sistema irá pausar e tentar reconectar."
                )
                robo = None  # Desativa para evitar loops de erro


def enviar_comando_maquina(label_item):
    """
    Direciona comandos de forma blindada. Se o hardware alvo não existir,
    o sistema apenas simula no console sem quebrar a execução geral.
    """
    global esteira, robo
    label_minusculo = label_item.lower()

    # Cenário 1 & 2: Ações destinadas ao ROBÔ (Erros ou Vazias)
    if "erro" in label_minusculo or label_item == "Erro de Fabricacao":
        if robo and robo.is_open:
            robo.write(b"ROTINA_01\n")
            print("[Serial] 'ROTINA_01' enviada ao Robô.")
        else:
            print(f"[Simulação Robô] Ignorado 'ROTINA_01' para: {label_item}")

    elif "vazia" in label_minusculo:
        if robo and robo.is_open:
            robo.write(b"ROTINA_02\n")
            print("[Serial] 'ROTINA_02' enviada ao Robô.")
        else:
            print(f"[Simulação Robô] Ignorado 'ROTINA_02' para: {label_item}")
            
    
    elif "coca_cola" in label_minusculo:
        if robo and robo.is_open:
            robo.write(b"ROTINA_02\n")
            print("[Serial] 'ROTINA_02' enviada ao Robô.")
        else:
            print(f"[Simulação Robô] Ignorado 'ROTINA_02' para: {label_item}")

    # Cenário 3: Garrafas conformes/normais -> Enviadas à ESTEIRA via JSON
    else:
        if esteira and esteira.is_open:
            payload_dict = {"id": "servidor", "deteccao_de_item": label_item}
            payload_json = json.dumps(payload_dict) + "\n"
            esteira.write(payload_json.encode("utf-8"))
            print(f"[Serial] JSON despachado para Esteira: {payload_dict}")
        else:
            print(f"[Simulação Esteira] JSON retido para: {label_item}")


# =====================================================================
# 5. LOOP DE VISÃO COMPUTACIONAL, WEBSOCKET E INICIALIZAÇÃO
# =====================================================================
async def loop_visao_computacional():
    global dados_esteira, sistema_pausado_por_falha_hardware
    print("[Visão] Câmera e processamento OpenCV iniciados...")

    item_counted = False
    current_label = ""
    roi_color = (255, 0, 0)

    while True:
        await asyncio.sleep(0.01)  # Permite que outras tarefas asyncio rodem

        # AJUSTE 2.2: VERIFICA A CONEXÃO DO HARDWARE EM TEMPO DE EXECUÇÃO
        # Se um dos hardwares se desconectar, o loop de visão é pausado.
        if not esteira or not robo:
            if not sistema_pausado_por_falha_hardware:
                print("\n[ALERTA DE HARDWARE] Um ou mais dispositivos foram desconectados!")
                print("O processamento de visão foi pausado por segurança.")
                sistema_pausado_por_falha_hardware = True
                dados_esteira["dados"]["status"] = "FALHA DE HARDWARE"

                # Tenta enviar um comando para parar a esteira, se ela ainda estiver conectada
                if esteira and esteira.is_open:
                    try:
                        parada_payload = {"id": "servidor", "controle": "pausar_processamento"}
                        parada_json = json.dumps(parada_payload) + "\n"
                        esteira.write(parada_json.encode("utf-8"))
                        print("[Sistema] Comando de 'pausar_processamento' enviado para a esteira.")
                    except Exception as e:
                        print(f"[Erro Serial] Falha ao enviar comando de parada para a esteira: {e}")

                # Notifica o dashboard sobre a falha
                if clientes_conectados:
                    mensagem = json.dumps(dados_esteira)
                    await asyncio.gather(*[cliente.send(mensagem) for cliente in clientes_conectados])

            # Mostra uma tela de erro na janela do OpenCV
            ret, image = camera.read()
            if ret:
                h, w, _ = image.shape
                start_y, start_x = (h - 224) // 2, (w - 224) // 2
                end_y, end_x = start_y + 224, start_x + 224
                cv2.rectangle(image, (start_x, start_y), (end_x, end_y), (255, 255, 255), 2)
                cv2.rectangle(image, (start_x, start_y), (start_x + 224, start_y + 25), (255, 255, 255), cv2.FILLED)
                cv2.putText(image, "FALHA DE HARDWARE", (start_x + 15, start_y + 18), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 0, 255), 2)
                cv2.imshow("Scanner de Esteira", image)

            if cv2.waitKey(1) == 27:
                raise KeyboardInterrupt
            continue  # Pula o resto do loop de visão, esperando a reconexão

        # Se o sistema estava pausado e o hardware voltou, reseta o estado
        if sistema_pausado_por_falha_hardware:
            print("[SISTEMA] Hardware reconectado! Retomando o processamento de visão.")
            sistema_pausado_por_falha_hardware = False

        ret, image = camera.read()
        if not ret:
            await asyncio.sleep(1)
            continue

        h, w, _ = image.shape
        start_y = (h - 224) // 2
        start_x = (w - 224) // 2
        end_y = start_y + 224
        end_x = start_x + 224

        roi = image[start_y:end_y, start_x:end_x]
        fg_mask = bg_subtractor.apply(roi)
        motion_pixels = cv2.countNonZero(fg_mask)
        object_in_roi = motion_pixels > MIN_PIXELS_MOTION

        mudou_estado = False

        if object_in_roi:
            if not item_counted:
                roi_color = (0, 255, 255)
                dados_esteira["dados"]["status"] = "Processando"
                mudou_estado = True

                roi_array = np.asarray(roi, dtype=np.float32).reshape(1, 224, 224, 3)
                roi_array = (roi_array / 127.5) - 1

                prediction = model.predict(roi_array, verbose=0)
                index = np.argmax(prediction)
                class_name = class_names[index]
                confidence_score = prediction[0][index]

                if (
                    confidence_score >= CONFIDENCE_THRESHOLD
                    and "Fundo" not in class_name
                ):
                    chave_contador = class_name.lower()

                    if chave_contador in dados_esteira["dados"]["contagem_garrafas"]:
                        dados_esteira["dados"]["contagem_garrafas"][chave_contador] += 1
                    else:
                        dados_esteira["dados"]["contagem_garrafas"]["erro"] += 1

                    dados_esteira["dados"]["ultimo_item"] = class_name
                    dados_esteira["dados"]["precisao"] = round(
                        float(confidence_score), 2
                    )
                    dados_esteira["dados"]["taxa_deteccao"] = random.randint(35, 45)

                    print(
                        f"[{class_name}] identificado com {int(confidence_score * 100)}% de precisão."
                    )

                    item_counted = True
                    current_label = class_name
                    mudou_estado = True

                    # Executa disparo de comandos direcionados por regras de hardware
                    enviar_comando_maquina(class_name)

                    if current_label == "Coca_Cola":
                        roi_color = (5, 13, 192)
                    elif current_label == "Sprite":
                        roi_color = (50, 205, 50)
                    elif current_label == "Fanta_Laranja":
                        roi_color = (0, 165, 255)
                    elif current_label == "Fanta_Uva":
                        roi_color = (130, 0, 75)
                    else:
                        roi_color = (0, 0, 255)
        else:
            if item_counted or dados_esteira["dados"]["status"] != "Aguardando":
                item_counted = False
                current_label = ""
                roi_color = (255, 0, 0)
                dados_esteira["dados"]["status"] = "Aguardando"
                dados_esteira["dados"]["taxa_deteccao"] = 0
                mudou_estado = True

        cv2.rectangle(image, (start_x, start_y), (end_x, end_y), roi_color, 2)
        if item_counted and current_label:
            cv2.rectangle(
                image,
                (start_x, start_y),
                (start_x + 224, start_y + 25),
                roi_color,
                cv2.FILLED,
            )
            cv2.putText(
                image,
                f"{current_label}",
                (start_x + 5, start_y + 18),
                cv2.FONT_HERSHEY_SIMPLEX,
                0.6,
                (0, 0, 0),
                2,
            )

        cv2.imshow("Scanner de Esteira", image)

        if mudou_estado and clientes_conectados:
            mensagem = json.dumps(dados_esteira)
            await asyncio.gather(
                *[cliente.send(mensagem) for cliente in clientes_conectados]
            )

        if cv2.waitKey(1) == 27:
            raise KeyboardInterrupt


async def manipulador_conexao(websocket):
    clientes_conectados.add(websocket)
    print(f"[Rede] Painel conectado. Total de telas ativas: {len(clientes_conectados)}")
    await websocket.send(json.dumps(dados_esteira))
    try:
        async for mensagem in websocket:
            dados_resposta = json.loads(mensagem)
            # Seção 1: Requisições para o console de IA (Gemini)
            if (
                dados_resposta.get("id") == "dashboard"
                and dados_resposta.get("designacao") == "console_de_comunicacao"
            ):
                requisicao = dados_resposta.get("requisicao")

                try:
                    contexto_prompt = f"""Você é um assistente de IA especialista integrado a um sistema industrial.
                    Dados atuais: {json.dumps(dados_esteira)}
                    Responda usando estritamente tags HTML básicas (<b>, <i>, <pre>). Não use Markdown."""

                    response = client.models.generate_content(
                        model="gemini-2.5-flash",
                        contents=f"{contexto_prompt}\n\n{requisicao}",
                    )
                    texto_resposta = response.text
                except Exception as e:
                    texto_resposta = "<b>[ERRO IA]</b> Falha ao processar requisição."

                await websocket.send(
                    json.dumps(
                        {
                            "id": "servidor",
                            "designacao": "console_de_comunicacao",
                            "resposta": texto_resposta,
                        }
                    )
                )
            # AJUSTE 3: PROCESSA COMANDOS DE CONTROLE VINDOS DO DASHBOARD
            elif (
                dados_resposta.get("id") == "dashboard"
                and dados_resposta.get("designacao") == "controle"
            ):
                comando_recebido = dados_resposta.get("comando")
                print(f"[Rede] Comando de controle recebido: '{comando_recebido}'")

                # Mapeia comandos do dashboard para comandos da esteira
                comando_para_esteira = None
                if comando_recebido == "ligar_esteira":
                    comando_para_esteira = "iniciar_esteira"
                elif comando_recebido == "desligar_esteira":
                    comando_para_esteira = "pausar_processamento"
                # Comandos que já têm o nome correto
                elif comando_recebido in ["pausar_processamento", "reiniciar_sistema", "iniciar_esteira"]:
                    comando_para_esteira = comando_recebido

                if comando_para_esteira:
                    # Envia o comando para a esteira se ela estiver conectada
                    if esteira and esteira.is_open:
                        payload_dict = {"id": "servidor", "controle": comando_para_esteira}
                        payload_json = json.dumps(payload_dict) + "\n"
                        esteira.write(payload_json.encode("utf-8"))
                        print(f"[Serial] Comando '{comando_para_esteira}' enviado para a Esteira.")
                    else:
                        print(f"[Aviso Serial] Comando '{comando_recebido}' ignorado, a Esteira não está conectada.")
                else:
                    print(f"[Aviso] Comando de controle '{comando_recebido}' não reconhecido.")

    except websockets.exceptions.ConnectionClosed:
        pass
    finally:
        clientes_conectados.remove(websocket)
        print(f"[Rede] Painel desconectado. Total de telas ativas: {len(clientes_conectados)}")


async def main():
    await websockets.serve(manipulador_conexao, "localhost", 8765)
    asyncio.create_task(escutar_retorno_serial())
    asyncio.create_task(reconectar_hardware())  # Inicia a tarefa de reconexão
    await loop_visao_computacional()


if __name__ == "__main__":
    # AJUSTE 1: VERIFICAÇÃO INICIAL DE HARDWARE
    # O sistema só inicia se ambos os hardwares estiverem conectados.
    print("[Sistema] Iniciando handshakes de hardware...")
    esteira = conectar_esteira()
    robo = conectar_robo()

    if not esteira or not robo:
        print("\n[ERRO CRÍTICO] Falha na conexão com Esteira e/ou Robô.")
        print("Verifique as conexões físicas, portas COM e reinicie o programa.")
        # Garante que qualquer porta parcialmente aberta seja fechada
        if esteira and esteira.is_open:
            esteira.close()
        if robo and robo.is_open:
            robo.close()
    else:
        print("[Sistema] Handshakes concluídos. Iniciando serviços principais.")
        try:
            asyncio.run(main())
        except KeyboardInterrupt:
            print("\n[Sistema] Desligando serviços...")
        finally:
            camera.release()
            cv2.destroyAllWindows()
            if esteira and esteira.is_open:
                esteira.close()
            if robo and robo.is_open:
                robo.close()
            print("[Sistema] Conexões físicas e de vídeo encerradas com sucesso.")
