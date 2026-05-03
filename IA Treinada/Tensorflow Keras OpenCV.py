#from tensorflow.keras.models import load_model
from keras.models import load_model  # TensorFlow is required for Keras to work
import cv2  # Install opencv-python
import numpy as np

# Disable scientific notation for clarity
np.set_printoptions(suppress=True)

# Load the model
model = load_model("keras_Model.h5", compile=False)

# Load the labels
class_names = open("labels.txt", "r").readlines()

# CAMERA can be 0 or 1 based on default camera of your computer
camera = cv2.VideoCapture(1)

# Define a resolução de trabalho da webcam
cam_width = 320 # 1920 640 320  -  720 480
cam_height = 240 # 1080 480 240  -  480 320
camera.set(cv2.CAP_PROP_FRAME_WIDTH, cam_width)
camera.set(cv2.CAP_PROP_FRAME_HEIGHT, cam_height)

while True:
    # Grab the webcamera's image.
    ret, image = camera.read()

    # Pega as dimensões da imagem original da webcam
    h, w, _ = image.shape

    # Calcula as coordenadas para centralizar a região de interesse (224x224)
    start_y = (h - 224) // 2
    start_x = (w - 224) // 2
    end_y = start_y + 224
    end_x = start_x + 224

    # Extrai apenas a região de interesse (ROI) para o modelo
    roi = image[start_y:end_y, start_x:end_x]

    # Desenha um retângulo verde na imagem principal para mostrar onde é a ROI
    cv2.rectangle(image, (start_x, start_y), (end_x, end_y), (0, 255, 0), 2)

    # Show the full image with the ROI rectangle in a window
    cv2.imshow("Webcam Image", image)

    # Make the ROI a numpy array and reshape it to the models input shape.
    roi_array = np.asarray(roi, dtype=np.float32).reshape(1, 224, 224, 3)

    # Normalize the image array
    roi_array = (roi_array / 127.5) - 1

    # Predicts the model
    prediction = model.predict(roi_array)
    index = np.argmax(prediction)
    class_name = class_names[index]
    confidence_score = prediction[0][index]

    # Print prediction and confidence score
    print("Class:", class_name[2:], end="")
    print("Confidence Score:", str(np.round(confidence_score * 100))[:-2], "%")

    # Listen to the keyboard for presses.
    keyboard_input = cv2.waitKey(1)

    # 27 is the ASCII for the esc key on your keyboard.
    if keyboard_input == 27:
        break

camera.release()
cv2.destroyAllWindows()
