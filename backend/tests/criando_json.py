import json
from dataclasses import asdict
# Importa a classe do novo arquivo
from config_model import ConfiguracaoEsteira
# Importa biblioteca de acesso a caminhos e pastas
import os

# 1. Instanciando o objeto normalmente
config = ConfiguracaoEsteira(velocidade_motor=150, cor_alvo="vermelha", tolerancia_opencv=0.85)

# ==========================================
# SALVANDO O OBJETO EM UM ARQUIVO .JSON
# ==========================================

# Pega o caminho da pasta onde este o script está rodando
base_path = os.path.dirname(__file__)
model_path = os.path.join(base_path, "tests")

with open(model_path + "configuracao.json", "w", encoding="utf-8") as arquivo:
    # asdict(config) converte a classe em um Dicionário Python padrão
    # json.dump pega esse dicionário e escreve no arquivo em formato JSON
    json.dump(asdict(config), arquivo, indent=4) 
    # indent=4 formata o arquivo com quebras de linha para ficar fácil de ler

print("Arquivo configuracao.json salvo com sucesso!")

# ==========================================
# LENDO O ARQUIVO .JSON DE VOLTA PARA OBJETO
# ==========================================

with open(model_path + "configuracao.json", "r", encoding="utf-8") as arquivo:
    # json.load lê o texto do arquivo e converte para um Dicionário Python
    dados_lidos = json.load(arquivo) 
    
    # Recriando o objeto da classe
    # Os dois asteriscos (**) "desempacotam" o dicionário, passando as chaves 
    # como argumentos para a classe (ex: velocidade_motor=150, etc)
    config_recuperada = ConfiguracaoEsteira(**dados_lidos)

print(f"Objeto recuperado: Buscando a cor {config_recuperada.cor_alvo} com velocidade {config_recuperada.velocidade_motor}.")