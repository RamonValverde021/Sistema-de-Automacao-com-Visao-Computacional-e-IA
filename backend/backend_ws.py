import asyncio
import websockets
import json
import random

# Memória do sistema
dados_esteira = {
    "counts": {"coca": 0, "fanta": 0, "fanta_uva": 0, "sprite": 0, "erro": 0},
    "status": "Aguardando",
    "rate": 0,
    "last_item": None,
    "accuracy": 0.0
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
                itens = ['Coca-Cola', 'Fanta', 'Fanta Uva', 'Sprite', 'Erro de Fabricação']
                item = random.choice(itens)
                
                if item == 'Coca-Cola': dados_esteira["counts"]["coca"] += 1
                elif item == 'Fanta': dados_esteira["counts"]["fanta"] += 1
                elif item == 'Fanta Uva': dados_esteira["counts"]["fanta_uva"] += 1
                elif item == 'Sprite': dados_esteira["counts"]["sprite"] += 1
                else: dados_esteira["counts"]["erro"] += 1
                
                dados_esteira["last_item"] = item
                dados_esteira["accuracy"] = round(random.uniform(0.85, 0.99), 2)
            else:
                dados_esteira["last_item"] = None
            
            dados_esteira["status"] = "Processando"
            dados_esteira["rate"] = random.randint(35, 45)
        else:
            dados_esteira["status"] = "Parada"
            dados_esteira["rate"] = 0

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
            print(f"Mensagem recebida do painel: {mensagem}")
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