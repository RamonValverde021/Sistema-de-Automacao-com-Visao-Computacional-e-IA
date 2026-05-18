import requests, json

dados = {
    "coca": 42, "fanta": 31, "sprite": 28, "erro": 2,
    "status": "running",  # running | stopped | error | processing
    "taxa": 4.2,
    "confianca": 93.7,
    "ciclo": 215,
    "ultimo": "Coca-Cola"
}
requests.post("http://localhost:5050/data", json=dados)