# Importa a ferramenta que permite carregar o "cérebro" da Inteligência Artificial
from keras.models import load_model
# Importa a ferramenta que permite ligar a câmera e manipular imagens
import cv2
# Importa a ferramenta usada para fazer contas matemáticas complexas com a imagem
import numpy as np
# Importa biblioteca de acesso a caminhos e pastas
import os

# Configura o programa para mostrar números comuns
np.set_printoptions(suppress=True)

# Pega o caminho da pasta onde este script .py está salvo
base_path = os.path.dirname(__file__)

# Carrega o modelo da Inteligência Artificial (IA)
model_path = os.path.join(base_path, "keras_model.h5")
model = load_model(model_path, compile=False)

# Abre e lê o arquivo "labels.txt" e cria os contadores individuais
class_names_path = os.path.join(base_path, "labels.txt")
class_names = []
counters = {}

with open(class_names_path, "r", encoding="utf-8") as f:
    for line in f.readlines():
        # Limpa o texto, removendo o número inicial e quebras de linha (ex: "0 Coca-Cola\n" vira "Coca-Cola")
        clean_name = line.strip().split(" ", 1)[1].strip() if " " in line else line.strip()
        class_names.append(clean_name)
        # Inicializa o contador dessa classe com zero
        counters[clean_name] = 0

# Configurações da Câmera
camera = cv2.VideoCapture(1)
cam_width = 320 
cam_height = 240 
camera.set(cv2.CAP_PROP_FRAME_WIDTH, cam_width)
camera.set(cv2.CAP_PROP_FRAME_HEIGHT, cam_height)

# --- CONFIGURAÇÕES DE AUTOMAÇÃO E ESTADOS ---
# Subtrator de fundo para detectar movimento/presença na esteira sem usar a IA
bg_subtractor = cv2.createBackgroundSubtractorMOG2(history=100, varThreshold=40, detectShadows=False)

CONFIDENCE_THRESHOLD = 0.85  # Confiança mínima (85%) para confirmar a identificação
MIN_PIXELS_MOTION = 2000     # Quantidade de pixels alterados para considerar que há um objeto na ROI

item_counted = False         # Flag para evitar contagem duplicada do mesmo objeto
current_label = ""           # Guarda o nome do objeto atual para exibir na tela
roi_color = (255, 0, 0)      # Cor inicial da ROI (Azul = Aguardando)

print("Sistema de automação iniciado. Aguardando objetos na esteira...")

while True:
    ret, image = camera.read()
    if not ret:
        break

    h, w, _ = image.shape
    start_y = (h - 224) // 2
    start_x = (w - 224) // 2
    end_y = start_y + 224
    end_x = start_x + 224

    # Recorta a ROI no centro da tela
    roi = image[start_y:end_y, start_x:end_x]

    # 1. VERIFICAÇÃO DE PRESENÇA (Economia de Processamento)
    # Aplica o subtrator de fundo apenas na área da ROI
    fg_mask = bg_subtractor.apply(roi)
    # Conta quantos pixels mudaram em relação ao fundo padrão
    motion_pixels = cv2.countNonZero(fg_mask)
    
    # Se a quantidade de pixels alterados for maior que o mínimo, tem algo na ROI
    object_in_roi = motion_pixels > MIN_PIXELS_MOTION

    if object_in_roi:
        if not item_counted:
            # Objeto entrou na ROI e ainda não foi validado. Cor amarela = Processando.
            roi_color = (0, 255, 255) 
            
            # Prepara a imagem para a IA apenas neste momento
            roi_array = np.asarray(roi, dtype=np.float32).reshape(1, 224, 224, 3)
            roi_array = (roi_array / 127.5) - 1

            # verbose=0 evita que o TensorFlow polua o console com logs de carregamento
            prediction = model.predict(roi_array, verbose=0) 
            index = np.argmax(prediction)
            class_name = class_names[index]
            confidence_score = prediction[0][index]

            # 2. VALIDAÇÃO E CONTAGEM
            # Se atingiu a confiança e não é a classe "Fundo" (caso ela ainda seja pega)
            if confidence_score >= CONFIDENCE_THRESHOLD and "Fundo" not in class_name:
                counters[class_name] += 1
                
                # Exibe a mensagem única no console
                print(f"[{class_name}] identificado com {int(confidence_score * 100)}% de certeza. (Total: {counters[class_name]})")
                
                # Trava o estado para não ler mais o mesmo objeto
                item_counted = True
                current_label = class_name
                roi_color = (0, 255, 0) # Cor verde = Identificado com sucesso
    else:
        # 3. RESET DE ESTADO (Área Limpa)
        # Quando o objeto sai da ROI, a quantidade de pixels em movimento cai.
        # Resetamos as variáveis para aguardar a próxima garrafinha.
        item_counted = False
        current_label = ""
        roi_color = (255, 0, 0) # Cor azul = Aguardando

    # --- DESENHO DA INTERFACE ---
    # Desenha a caixa da ROI com a cor baseada no estado atual
    cv2.rectangle(image, (start_x, start_y), (end_x, end_y), roi_color, 2)

    # Adiciona a legenda no canto superior esquerdo se o item foi validado
    if item_counted and current_label:
        # Coloca um fundo preto no texto para facilitar a leitura dependendo da iluminação
        cv2.rectangle(image, (start_x, start_y), (start_x + 224, start_y + 25), roi_color, cv2.FILLED)
        cv2.putText(image, f"{current_label} ({counters[current_label]})", (start_x + 5, start_y + 18),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 0, 0), 2)

    # Mostra a imagem na tela
    cv2.imshow("Scanner de Esteira", image)
    
    # (Opcional) Descomente a linha abaixo para ver a "visão" do detector de movimento
    # cv2.imshow("Mascara de Movimento", fg_mask)

    keyboard_input = cv2.waitKey(1)
    if keyboard_input == 27: # Tecla ESC
        break

# Encerramento seguro
camera.release()
cv2.destroyAllWindows()

# Exibe o relatório final de produção ao fechar o programa
print("\n--- Relatório Final de Contagem ---")
for nome, quantidade in counters.items():
    if "Fundo" not in nome: # Ignora o background no relatório final
        print(f"{nome}: {quantidade}")
print("-----------------------------------")