# Importa a ferramenta que permite carregar o "cérebro" da Inteligência Artificial que já foi treinado
from keras.models import load_model
# Importa a ferramenta que permite ligar a câmera e ver imagens na tela
import cv2
# Importa a ferramenta usada para fazer contas matemáticas complexas com a imagem
import numpy as np
# Importa biblioteca de acesso a caminhos e pastas
import os

# Configura o programa para mostrar números comuns, evitando formatos matemáticos difíceis de ler
np.set_printoptions(suppress=True)

# Pega o caminho da pasta onde este script .py está salvo
base_path = os.path.dirname(__file__)

# Carrega o arquivo "keras_model.h5", que é a Inteligência Artificial (IA) pronta para reconhecer objetos
model_path = os.path.join(base_path, "keras_model.h5")
# Carrega usando o caminho completo dinâmico
model = load_model(model_path, compile=False)

# Abre e lê o arquivo "labels.txt", que contém os nomes das coisas que a IA aprendeu a reconhecer
class_names_path = os.path.join(base_path, "labels.txt")
# Carrega usando o caminho completo dinâmico
class_names = open(class_names_path, "r").readlines()

# Liga a câmera do computador (O número '1' geralmente é uma webcam externa, '0' seria a câmera embutida)
camera = cv2.VideoCapture(0)

# Define qual vai ser a largura da imagem gravada pela câmera (em pontinhos de luz, chamados pixels)
cam_width = 320
# Define qual vai ser a altura da imagem gravada pela câmera
cam_height = 240
# Aplica a largura configurada na câmera
camera.set(cv2.CAP_PROP_FRAME_WIDTH, cam_width)
# Aplica a altura configurada na câmera
camera.set(cv2.CAP_PROP_FRAME_HEIGHT, cam_height)

# Inicia um ciclo infinito, como um filme rodando sem parar, até que a gente mande fechar
while True:
    # Tira uma "foto" rápida (um quadro do vídeo) e guarda essa imagem na memória
    ret, image = camera.read()

    # Pega as medidas originais dessa foto que acabamos de tirar (altura 'h' e largura 'w')
    h, w, _ = image.shape

    # Calcula onde começa o corte de cima para baixo para focar bem no meio da tela
    start_y = (h - 224) // 2
    # Calcula onde começa o corte da esquerda para a direita para focar no meio da tela
    start_x = (w - 224) // 2
    # Calcula onde termina o corte na parte de baixo
    end_y = start_y + 224
    # Calcula onde termina o corte na parte da direita
    end_x = start_x + 224

    # Recorta a imagem original, criando um "quadradinho" bem no centro da tela que será enviado para a IA
    roi = image[start_y:end_y, start_x:end_x]

    # Desenha uma caixa azul na tela para o usuário ver exatamente onde a IA está olhando
    cv2.rectangle(image, (start_x, start_y), (end_x, end_y), (255, 0, 0), 2)

    # Abre uma janela no computador e mostra a imagem da câmera com a caixa azul desenhada
    cv2.imshow("Webcam Image", image)

    # Converte aquele recorte do centro em uma lista de números que o "cérebro" da IA consegue entender
    roi_array = np.asarray(roi, dtype=np.float32).reshape(1, 224, 224, 3)

    # Ajusta as cores da imagem recortada para facilitar o trabalho de reconhecimento da IA
    roi_array = (roi_array / 127.5) - 1

    # Entrega a imagem para a Inteligência Artificial e pede para ela "adivinhar" o que é
    prediction = model.predict(roi_array)
    # Descobre qual foi o palpite que a IA teve a maior certeza de estar certo
    index = np.argmax(prediction)
    # Pega o nome do objeto reconhecido na nossa listinha de nomes
    class_name = class_names[index]
    # Pega o nível de confiança (a "certeza") que a IA tem de que aquele palpite está correto
    confidence_score = prediction[0][index]

    # Escreve no terminal (tela de texto) o nome do que a IA achou que é
    print("Class:", class_name[2:], end="")
    # Escreve no terminal a porcentagem de certeza que a IA tem do objeto encontrado
    print("Confidence Score:", str(np.round(confidence_score * 100))[:-2], "%")

    # Presta atenção por um milissegundo para ver se alguém apertou alguma tecla
    keyboard_input = cv2.waitKey(1)

    # Verifica se a tecla apertada foi a tecla "Esc" do teclado (que tem o código número 27)
    if keyboard_input == 27:
        # Se foi o botão "Esc", quebra o ciclo infinito, encerrando o programa
        break

# Desliga a câmera e libera ela para outros programas usarem
camera.release()
# Fecha a janelinha de vídeo que tinha sido aberta na tela do computador
cv2.destroyAllWindows()
