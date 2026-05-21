# ============================================================
# SISTEMA DE DETECÇÃO NA ESTEIRA - Teachable Machine + OpenCV
# ============================================================

from keras.models import load_model
import cv2
import numpy as np
import os

# ── Configurações de limiar ──────────────────────────────────
CONFIANCA_MINIMA      = 0.90   # 90% — confiança para confirmar detecção
LIMIAR_OBJETO_NA_ROI  = 0.10   # 10% — diferença mínima de brilho para "ver" objeto
FRAMES_COOLDOWN       = 30     # quadros de espera após cada detecção confirmada

# ── Cores (BGR) ──────────────────────────────────────────────
COR_ROI_VAZIA      = (255,   0,   0)   # azul  — sem objeto
COR_ROI_DETECTANDO = (  0, 165, 255)   # laranja — processando
COR_ROI_CONFIRMADA = (  0, 200,   0)   # verde — detecção confirmada

# ── Caminhos dinâmicos ───────────────────────────────────────
base_path        = os.path.dirname(os.path.abspath(__file__))
model_path       = os.path.join(base_path, "keras_model.h5")
class_names_path = os.path.join(base_path, "labels.txt")

# ── Carrega modelo e rótulos ─────────────────────────────────
np.set_printoptions(suppress=True)
model = load_model(model_path, compile=False)

with open(class_names_path, "r", encoding="utf-8") as f:
    class_names = [line.strip() for line in f.readlines()]

# Extrai apenas os nomes (sem o índice numérico do início)
# Formato esperado no labels.txt: "0 Coca-Cola", "1 Fanta Laranja", …
class_labels = []
for name in class_names:
    parts = name.split(" ", 1)
    class_labels.append(parts[1].strip() if len(parts) > 1 else name.strip())

# ── Contadores individuais por classe ────────────────────────
contadores = {label: 0 for label in class_labels}

# ── Câmera ───────────────────────────────────────────────────
camera = cv2.VideoCapture(1)          # 1 = webcam externa / 0 = embutida

CAM_WIDTH  = 229
CAM_HEIGHT = 224
camera.set(cv2.CAP_PROP_FRAME_WIDTH,  CAM_WIDTH)
camera.set(cv2.CAP_PROP_FRAME_HEIGHT, CAM_HEIGHT)

# ── Tamanho da ROI e posição central ─────────────────────────
ROI_SIZE = 224   # modelo espera 224×224

# ── Variáveis de estado ──────────────────────────────────────
cor_roi          = COR_ROI_VAZIA
legenda_roi      = ""
cooldown_frames  = 0    # contador de quadros em espera pós-detecção
objeto_ativo     = False  # True enquanto há objeto na ROI sendo processado

# ── Referência de fundo (primeiro frame sem objeto) ──────────
referencia_fundo = None

def tem_objeto_na_roi(roi_atual: np.ndarray, fundo: np.ndarray) -> bool:
    """Compara o frame atual com o fundo de referência via diferença de brilho."""
    gray_atual  = cv2.cvtColor(roi_atual, cv2.COLOR_BGR2GRAY).astype(np.float32)
    gray_fundo  = cv2.cvtColor(fundo,    cv2.COLOR_BGR2GRAY).astype(np.float32)
    diff        = np.abs(gray_atual - gray_fundo) / 255.0
    return float(np.mean(diff)) > LIMIAR_OBJETO_NA_ROI

def preprocessar_roi(roi: np.ndarray) -> np.ndarray:
    """Prepara o recorte da ROI para inferência no Keras."""
    arr = np.asarray(roi, dtype=np.float32).reshape(1, ROI_SIZE, ROI_SIZE, 3)
    return (arr / 127.5) - 1

def imprimir_contadores():
    """Exibe no terminal os contadores de todas as classes."""
    print("\n── Contadores ────────────────────────────")
    for label, count in contadores.items():
        print(f"  {label}: {count}")
    print("──────────────────────────────────────────\n")

# ════════════════════════════════════════════════════════════
# LOOP PRINCIPAL
# ════════════════════════════════════════════════════════════
print("Sistema iniciado. Pressione [ESC] para encerrar.\n")

while True:
    ret, frame = camera.read()
    if not ret:
        print("Erro ao capturar frame da câmera.")
        break

    h, w, _ = frame.shape

    # Centraliza a ROI no frame
    start_y = (h - ROI_SIZE) // 2
    start_x = (w - ROI_SIZE) // 2
    end_y   = start_y + ROI_SIZE
    end_x   = start_x + ROI_SIZE

    roi = frame[start_y:end_y, start_x:end_x]

    # ── Inicializa fundo de referência no primeiro frame ─────
    if referencia_fundo is None:
        referencia_fundo = roi.copy()
        print("Fundo de referência capturado.")

    # ── Gerencia cooldown pós-detecção ───────────────────────
    if cooldown_frames > 0:
        cooldown_frames -= 1
        if cooldown_frames == 0:
            # Cooldown encerrado: redefine estado
            objeto_ativo    = False
            cor_roi         = COR_ROI_VAZIA
            legenda_roi     = ""
            referencia_fundo = roi.copy()   # atualiza fundo sem objeto

    # ── Verifica presença de objeto na ROI ───────────────────
    elif tem_objeto_na_roi(roi, referencia_fundo):
        objeto_ativo = True
        cor_roi = COR_ROI_DETECTANDO

        # ── Inferência ──────────────────────────────────────
        entrada    = preprocessar_roi(roi)
        prediction = model.predict(entrada, verbose=0)
        index      = int(np.argmax(prediction))
        confianca  = float(prediction[0][index])
        label      = class_labels[index]

        # ── Confirmação com confiança suficiente ─────────────
        if confianca >= CONFIANCA_MINIMA:
            contadores[label] += 1

            print(f"✔ Detectado: {label}  |  Confiança: {confianca * 100:.1f}%")
            imprimir_contadores()

            cor_roi        = COR_ROI_CONFIRMADA
            legenda_roi    = f"{label} ({confianca * 100:.0f}%)"
            cooldown_frames = FRAMES_COOLDOWN

    else:
        # Sem objeto — mantém ROI azul e reseta estado se necessário
        if objeto_ativo and cooldown_frames == 0:
            objeto_ativo     = False
            cor_roi          = COR_ROI_VAZIA
            legenda_roi      = ""
            referencia_fundo = roi.copy()

    # ── Desenha ROI no frame ─────────────────────────────────
    cv2.rectangle(frame, (start_x, start_y), (end_x, end_y), cor_roi, 2)

    # Legenda no canto superior esquerdo da ROI
    if legenda_roi:
        font       = cv2.FONT_HERSHEY_SIMPLEX
        font_scale = 0.6
        espessura  = 2
        padding    = 4

        (tw, th), baseline = cv2.getTextSize(legenda_roi, font, font_scale, espessura)
        tx = start_x + padding
        ty = start_y + th + padding

        # Fundo semitransparente para a legenda
        overlay = frame.copy()
        cv2.rectangle(overlay,
                      (tx - padding, start_y),
                      (tx + tw + padding, start_y + th + 2 * padding),
                      cor_roi, -1)
        cv2.addWeighted(overlay, 0.6, frame, 0.4, 0, frame)

        cv2.putText(frame, legenda_roi, (tx, ty),
                    font, font_scale, (255, 255, 255), espessura, cv2.LINE_AA)

    # ── Exibe contadores no canto superior direito do frame ──
    y_offset = 20
    for label, count in contadores.items():
        texto = f"{label}: {count}"
        cv2.putText(frame, texto, (w - 220, y_offset),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 1, cv2.LINE_AA)
        y_offset += 20

    cv2.imshow("Esteira - Deteccao de Objetos", frame)

    if cv2.waitKey(1) == 27:   # ESC
        break

# ── Encerramento ─────────────────────────────────────────────
camera.release()
cv2.destroyAllWindows()

print("\n=== Resumo Final ===")
imprimir_contadores()