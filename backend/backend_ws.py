import asyncio
import websockets
import json
import random
import os
from dotenv import load_dotenv
from google import genai

# Memória do sistema
dados_esteira = {
    "id": "servidor",
    "designacao": "dados_esteira",
    "dados": {
        "contagem_garrafas": {
            "coca_cola": 0, 
            "sprite": 0, 
            "fanta_laranja": 0, 
            "fanta_uva": 0, 
            
            "coca_cola_vazia":0,
            "sprite_vazia":0,
            "fanta_laranja_vazia":0,
            "fanta_uva_vazia":0,
            
            "coca_cola_com_sprite":0,
            "coca_cola_com_fanta_laranja":0,
            "coca_cola_com_fanta_uva":0,
            
            "sprite_com_coca_cola":0,
            "sprite_com_fanta_laranja":0,
            "sprite_com_fanta_uva":0,

            "fanta_com_coca_cola":0,
            "fanta_com_sprite":0,

            "erro": 0
            },
        "status": "Aguardando",
        "taxa_deteccao": 0,
        "ultimo_item": None,
        "precisao": 0.0
    }
}

# Carrega as variáveis de ambiente do arquivo .env
load_dotenv()

# Pega a chave da API das variáveis de ambiente
api_key = os.getenv("GEMINI_API_KEY")

# Verifica se a chave foi encontrada antes de configurar
if not api_key:
    raise ValueError("A chave da API do Gemini (GEMINI_API_KEY) não foi encontrada. Verifique seu arquivo .env")

# Substitua pela sua chave de API ou configure-a como variável de ambiente
client = genai.Client(api_key=api_key)

# Resposta simulada
resposta_simulada_IA = {
    "id": "servidor",
    "designacao": "console_de_comunicacao",
    "resposta": "Texto a ser enviado"
}

# Lista para guardar todos os painéis (navegadores) conectados
clientes_conectados = set()

# =====================================================================
# 1. LOOP DA VISÃO COMPUTACIONAL (Gerador de Dados)
# =====================================================================
async def loop_visao_computacional():
    global dados_esteira
    print("[Visão] Câmera iniciada. Aguardando garrafas...")
    
    while True:
        await asyncio.sleep(1.5)  # Simula o tempo de processamento
        
        is_processing = random.random() > 0.1
        
        if is_processing:
            if random.random() > 0.4:
                itens = ['Coca-Cola', 'Sprite', 'Fanta Laranja', 'Fanta Uva', 'Erro de Fabricação']
                item = random.choice(itens)
                
                if item == 'Coca-Cola': dados_esteira["dados"]["contagem_garrafas"]["coca_cola"] += 1
                elif item == 'Sprite': dados_esteira["dados"]["contagem_garrafas"]["sprite"] += 1
                elif item == 'Fanta Laranja': dados_esteira["dados"]["contagem_garrafas"]["fanta_laranja"] += 1
                elif item == 'Fanta Uva': dados_esteira["dados"]["contagem_garrafas"]["fanta_uva"] += 1
                
                else: dados_esteira["dados"]["contagem_garrafas"]["erro"] += 1
                
                dados_esteira["dados"]["ultimo_item"] = item
                dados_esteira["dados"]["precisao"] = round(random.uniform(0.85, 0.99), 2)
            else:
                dados_esteira["dados"]["ultimo_item"] = None
            
            dados_esteira["dados"]["status"] = "Processando"
            dados_esteira["dados"]["taxa_deteccao"] = random.randint(35, 45)
        else:
            dados_esteira["dados"]["status"] = "Parada"
            dados_esteira["dados"]["taxa_deteccao"] = 0

        # O PULO DO GATO: Assim que a visão processa, envia para todos os painéis abertos!
        if clientes_conectados:
            mensagem = json.dumps(dados_esteira)
            # Envia a mensagem para todos os clientes conectados simultaneamente
            await asyncio.gather(*[cliente.send(mensagem) for cliente in clientes_conectados])

# =====================================================================
# 2. SERVIDOR WEBSOCKET
# =====================================================================
async def manipulador_conexao(websocket):
    # Quando o painel web se conecta, adicionamos ele à lista
    clientes_conectados.add(websocket)
    print(f"[Rede] Novo painel conectado! Total de telas: {len(clientes_conectados)}")
    
    try:
        # Fica aguardando caso o frontend queira enviar algum comando (ex: "Parar Esteira")
        # Neste exemplo, estamos apenas enviando, mas a porta fica aberta.
        async for mensagem in websocket:
            #print(f"Mensagem recebida do painel: {mensagem}")
            
            # Converte a mensagem JSON recebida em um dicionário Python estruturado
            dados_resposta = json.loads(mensagem)
            if dados_resposta.get("id") == "dashboard":
                if dados_resposta.get("designacao") == "console_de_comunicacao":
                    requisicao = dados_resposta.get("requisicao")
                    print(f"-> Pergunta recebida para IA: {requisicao}")
                    
                    try:
                    # Complementação Prompt
                        contexto_prompt = """Você é um assistente de IA especialista, integrado a um sistema de automação industrial. O sistema utiliza Visão Computacional (OpenCV, Keras/TensorFlow) para identificar objetos em uma esteira, comunicação serial (Python/PySerial) com um microcontrolador Arduino que controla atuadores, e um backend (Python/WebSockets) que serve um frontend (HTML/JS) para monitoramento em tempo real. Seu objetivo é fornecer respostas e análises considerando a totalidade deste ecossistema.

REGRAS E FORMATO DA RESPOSTA (OBRIGATÓRIO):
1. Estilo de Console: Sua resposta será exibida em um console de log no frontend (dentro de uma tag <pre>). Formate sua saída como se fosse uma mensagem de sistema inteligente.
2. Leias a variável dados_esteira para se atualizar e devolver uma resposta adequada.
3. Sem Formatação Complexa: NÃO use Markdown (como `###`, `*`, `-` para listas). Use tags HTML (<b>, <i>, <pre>, <ul>, <li>, <hr>) para gerar a formatação quando necessario. A resposta deve ser compatível com HTML. 
4. O comprimento de cada linha da resposta tem que ser menor que a largura da tela de console, não deve contem qubreas de linhas duplas e muitos espações vazios. Lambrando que a estilização da resposta deve ser baseada em uma resposta de console.
5. Linguagem Técnica: Utilize termos como 'backend', 'OpenCV', 'serial', 'WebSocket', 'payload JSON', etc., quando apropriado.

Com base em todo este contexto, responda à seguinte pergunta de forma clara e objetiva para o console:

"""
                        pergunta = f"{contexto_prompt}\n\n{requisicao}"
                        
                        # Chama a API do Gemini para obter uma resposta
                        response = client.models.generate_content(
                        model='gemini-2.5-flash',
                        contents=pergunta,
                        )
                        texto_resposta = response.text
                        print(texto_resposta)
                        
                    except Exception as e:
                        print(f"[Erro IA] Falha ao gerar resposta: {e}")
                        texto_resposta = "Desculpe, ocorreu um erro ao processar sua pergunta."
                    
                    # Monta o payload da resposta
                    resposta_para_cliente = {
                        "id": "servidor",
                        "designacao": "console_de_comunicacao",
                        "resposta": texto_resposta
                    }
                    
                    # Envia a resposta APENAS para o cliente que fez a pergunta
                    await websocket.send(json.dumps(resposta_para_cliente))
            
    except websockets.exceptions.ConnectionClosed:
        pass
    finally:
        # Se o usuário fechar a aba do navegador, removemos ele da lista
        clientes_conectados.remove(websocket)
        print(f"[Rede] Painel desconectado. Total de telas: {len(clientes_conectados)}")

# =====================================================================
# INICIALIZAÇÃO
# =====================================================================
async def main():
    # Inicia o servidor WebSocket na porta 8765
    servidor_ws = await websockets.serve(manipulador_conexao, "localhost", 8765)
    print("[Servidor] Servidor WebSocket rodando em ws://localhost:8765")
    
    # Roda a simulação da câmera simultaneamente
    await loop_visao_computacional()

if __name__ == "__main__":
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        print("\n[Sistema] Desligando sistema...")