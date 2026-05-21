from keras.models import load_model
import numpy as np
import cv2
import os


# Obtém o caminho do diretório onde este script atual está salvo
base_path = os.path.dirname(__file__)
# Junta o caminho do diretório com o nome do arquivo do modelo treinado
model_path = os.path.join(base_path, "keras_model.h5")
# Carrega o modelo de Inteligência Artificial usando o Keras (compile=False evita avisos, já que não vamos treinar, apenas usar)
model = load_model(model_path, compile=False)

# Cria um array (matriz) vazio com o formato exato que a IA espera: 1 imagem, tamanho 224x224 pixels, 3 canais de cor (RGB), tipo float32
data = np.ndarray(shape=(1, 224, 224, 3), dtype=np.float32)

# Inicia a captura de vídeo da câmera. '1' geralmente é a webcam secundária/USB (use '0' para a câmera principal do notebook)
cap = cv2.VideoCapture(1)

# Lista de nomes dos objetos que a IA foi ensinada a reconhecer (a ordem precisa ser idêntica à do treinamento no Teachable Machine)
classes = ['CALCULADORA','CONTROLE','LANPADA','FUNDO']

# Inicia um laço de repetição infinito para processar o vídeo continuamente (quadro a quadro)
while True:
    # Lê um quadro (frame) da câmera. 'success' é um booleano dizendo se funcionou e 'img' é a imagem capturada
    success,img = cap.read()
    
    # Redimensiona a imagem da câmera para 224x224 pixels, que é o formato exigido pelo modelo treinado
    imgS = cv2.resize(img, (224, 224))
    
    # Converte a imagem redimensionada em um formato de lista matemática (matriz NumPy)
    image_array = np.asarray(imgS)
    
    # Normaliza as cores. Um pixel normal vai de 0 a 255. A fórmula transforma isso para o intervalo de -1 a 1, padrão da IA
    normalized_image_array = (image_array.astype(np.float32) / 127.0) - 1
    
    # Coloca a imagem tratada e normalizada dentro daquele array "data" que criamos no início
    data[0] = normalized_image_array
    
    # Pede para a Inteligência Artificial "prever/adivinhar" o que tem na imagem e salva as probabilidades
    prediction = model.predict(data)
    
    # 'prediction' tem a probabilidade para todas as classes. 'np.argmax' pega a posição do maior valor (a resposta mais provável)
    indexVal = np.argmax(prediction)

    # Desenha na imagem 'img' o nome da classe ganhadora, na posição (50, 50), fonte COMPLEX, tamanho 2, cor verde (0,255,0), espessura 2
    cv2.putText(img, str(classes[indexVal]),(50,50), cv2.FONT_HERSHEY_COMPLEX, 2, (0,255,0), 2)
    
    # Imprime também no terminal/console do seu editor de código o nome do objeto reconhecido
    print(classes[indexVal])

    # Abre uma janela com o nome 'img' para exibir na tela a imagem capturada com o texto sobreposto
    cv2.imshow('img',img)
    
    # Espera 1 milissegundo para atualizar a janela do OpenCV e ler a próxima imagem
    cv2.waitKey(1)
