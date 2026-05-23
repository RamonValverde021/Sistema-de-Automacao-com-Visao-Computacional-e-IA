// --- Configuração do Gráfico (Chart.js) ---
const ctx = document.getElementById('rateChart').getContext('2d');
const rateChart = new Chart(ctx, {
    type: 'line',
    data: {
        labels: Array(15).fill(''),
        datasets: [{
            label: 'Garrafas por Minuto',
            data: Array(15).fill(0),
            borderColor: '#00f0ff',
            backgroundColor: 'rgba(0, 240, 255, 0.1)',
            borderWidth: 2,
            pointBackgroundColor: '#00f0ff',
            pointRadius: 3,
            pointHoverRadius: 6,
            fill: true,
            tension: 0.4
        }]
    },
    options: {
        responsive: true,
        maintainAspectRatio: false,
        plugins: { legend: { display: false } },
        scales: {
            y: {
                beginAtZero: true, max: 60,
                grid: { color: 'rgba(255, 255, 255, 0.05)' },
                ticks: { color: '#64748b', font: { family: 'Rajdhani' } }
            },
            x: {
                grid: { display: false },
                ticks: { display: false }
            }
        },
        animation: { duration: 400, easing: 'linear' }
    }
});

// --- Variáveis de Estado ---
let totals = { coca: 0, fanta: 0, fanta_uva: 0, sprite: 0, erro: 0 };
const logContainer = document.getElementById('log-container');

// --- Função para Atualizar a Interface com o JSON do Python ---
function updateDashboard(data) {
    //  Verifica se a mensagem recebida é vinda do servidor e se são dados da esteira
    if (data.id == "servidor") {
        if (data.designacao == "dados_esteira") {
            if (typeof data.dados.status !== 'undefined') {

                // Atualiza Contadores
                document.getElementById('count-coca').innerText = data.dados.contagem_garrafas.coca_cola;
                document.getElementById('count-sprite').innerText = data.dados.contagem_garrafas.sprite;
                document.getElementById('count-fanta-laranja').innerText = data.dados.contagem_garrafas.fanta_laranja;
                if (document.getElementById('count-fanta-uva') && data.dados.contagem_garrafas.fanta_uva !== undefined) {
                    document.getElementById('count-fanta-uva').innerText = data.dados.contagem_garrafas.fanta_uva;
                }
                document.getElementById('count-error').innerText = data.dados.contagem_garrafas.erro;

                // Atualiza Taxa
                document.getElementById('detection-rate').innerText = data.dados.taxa_deteccao;

                // Atualiza Status da Esteira
                const statusEl = document.getElementById('belt-status');
                const gearIcon = document.getElementById('gear-icon');

                statusEl.innerText = data.dados.status.toUpperCase();

                // Lógica visual baseada no status
                statusEl.className = 'text-2xl font-orbitron font-bold ';
                gearIcon.className = 'fa-solid fa-cogs text-3xl ';

                if (data.dados.status === 'Processando' || data.dados.status === 'Ligada') {
                    statusEl.classList.add('text-glow-green');
                    gearIcon.classList.add('text-glow-cyan', 'fa-spin'); // Faz a engrenagem girar
                } else if (data.dados.status === 'Erro') {
                    statusEl.classList.add('text-glow-red');
                    gearIcon.classList.add('text-red-500');
                } else {
                    statusEl.classList.add('text-glow-orange');
                    gearIcon.classList.add('text-gray-500');
                }

                // Atualiza Gráfico
                const chartData = rateChart.data.datasets[0].data;
                chartData.push(data.dados.taxa_deteccao);
                chartData.shift(); // Remove o mais antigo
                rateChart.update();

                // Atualiza Log (se houver um novo item detectado)
                if (data.dados.ultimo_item) {
                    addLogEntry(data.dados.ultimo_item, data.dados.precisao);
                }
            } else {
                console.log("WebSocket: Pacote de dados não reconhecido:", data);
            }
        } else if (data.designacao == "console_de_comunicacao") {
            if (data.comando == "resposta") {
                addMessageToConsole(data.dados, 'system');
            }
        }
    }
    return;
}

// --- Função para adicionar itens no Log ---
function addLogEntry(item, accuracy) {
    const time = new Date().toLocaleTimeString([], { hour: '2-digit', minute: '2-digit', second: '2-digit' });
    let colorClass = 'text-cyan-400';
    let icon = 'fa-check-circle';

    if (item.toLowerCase() === 'erro' || item.toLowerCase() === 'descarte') {
        colorClass = 'text-red-400';
        icon = 'fa-times-circle';
    }

    const logHTML = `
                <div class="flex items-center justify-between p-2 bg-gray-800/40 rounded border border-gray-700/50 animate-fade-in">
                    <div class="flex items-center gap-3">
                        <i class="fa-solid ${icon} ${colorClass}"></i>
                        <span class="text-sm text-gray-200 font-bold">${item}</span>
                    </div>
                    <div class="text-right">
                        <div class="text-xs text-gray-500">${time}</div>
                        <div class="text-xs text-cyan-500">${(accuracy * 100).toFixed(1)}%</div>
                    </div>
                </div>
            `;

    logContainer.insertAdjacentHTML('afterbegin', logHTML);

    // Mantém apenas os últimos 15 logs para não pesar a memória
    if (logContainer.children.length > 15) {
        logContainer.removeChild(logContainer.lastElementChild);
    }
}
