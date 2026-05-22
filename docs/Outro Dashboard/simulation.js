
// =====================================================================
// SIMULAÇÃO DO BACKEND PYTHON (Comentada para receber os dados reais do WS)
// =====================================================================
const items = ['Coca-Cola', 'Fanta', 'Sprite', 'Coca-Cola', 'Sprite', 'Erro de Fabricação'];

setInterval(() => {
    // Simulando a leitura de um novo item
    const isProcessing = Math.random() > 0.1; // 90% do tempo está processando
    let newItem = null;
    let accuracy = 0;

    if (isProcessing && Math.random() > 0.4) { // Gera itens aleatoriamente
        newItem = items[Math.floor(Math.random() * items.length)];
        accuracy = 0.85 + (Math.random() * 0.14); // Precisão entre 85% e 99%

        // Incrementa os totais simulados
        if (newItem === 'Coca-Cola') totals.coca++;
        else if (newItem === 'Fanta') totals.fanta++;
        else if (newItem === 'Sprite') totals.sprite++;
        else totals.erro++;
    }

    const mockJsonPayload = {
        counts: totals,
        status: isProcessing ? "Processando" : "Parada",
        rate: isProcessing ? Math.floor(Math.random() * (45 - 35 + 1) + 35) : 0, // Taxa entre 35 e 45
        last_item: newItem,
        accuracy: accuracy
    };

    updateDashboard(mockJsonPayload);
}, 1500); // Atualiza a cada 1.5 segundos simulando o tempo da esteira