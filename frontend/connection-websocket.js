// =====================================================================
// COMUNICAÇÃO COM O PYTHON (WebSocket)
// =====================================================================

// Conecta ao servidor WebSocket do Python
const socket = new WebSocket('ws://localhost:8765');

// --- ELEMENTOS DO DOM ---
const consoleLog = document.getElementById('console-log');
const consoleInput = document.getElementById('console-input');
const consoleSend = document.getElementById('console-send');
const controlPanelButtons = document.querySelectorAll('#control-panel button');

// --- FUNÇÕES AUXILIARES ---

/**
 * Adiciona uma mensagem ao console de comunicação.
 * @param {string} message - A mensagem a ser exibida.
 * @param {'user' | 'system' | 'info'} sender - Quem enviou a mensagem.
 */
function addMessageToConsole(message, sender) {
    const time = new Date().toLocaleTimeString([], { hour: '2-digit', minute: '2-digit', second: '2-digit' });
    const messageElement = document.createElement('div');

    let senderPrefix = '';
    let messageColor = '';
    let icon = '';

    if (sender === 'user') {
        senderPrefix = `<span class="text-green-400 font-bold">Você:</span>`;
        messageColor = 'text-gray-200';
        icon = `<i class="fa-solid fa-chevron-right text-green-400 mr-2"></i>`;
    } else if (sender === 'system') {
        senderPrefix = `<span class="text-cyan-400 font-bold">Sistema:</span>`;
        messageColor = 'text-gray-300';
        icon = `<i class="fa-solid fa-cogs text-cyan-400 mr-2"></i>`;
    } else { // info
        messageColor = 'text-yellow-400 italic';
        icon = `<i class="fa-solid fa-info-circle text-yellow-400 mr-2"></i>`;
    }

    messageElement.innerHTML = `
                <div class="flex justify-between items-start">
                    <pre class="${messageColor} break-all">${icon} ${senderPrefix} ${message}</pre>
                    <span class="text-xs text-gray-500 flex-shrink-0 ml-4">${time}</span>
                </div>
            `;

    consoleLog.appendChild(messageElement);
    consoleLog.scrollTop = consoleLog.scrollHeight; // Auto-scroll
}

/**
 * Envia um comando para o backend via WebSocket.
 * @param {string} command - O nome do comando.
 * @param {any} [value=null] - Um valor opcional para o comando.
 */
// Função para enviar comandos JSON para o Python
function sendCommand(command, designation) {
    if (socket.readyState === WebSocket.OPEN) {
        if (designation == "controle") {
            const message = {
                id: "dashboard",
                designacao: "controle",
                comando: command
            };
            socket.send(JSON.stringify(message));
        } else if (designation == "console") {
            const message = {
                id: "dashboard",
                designacao: "console_de_comunicacao",
                requisicao: command
            };
            socket.send(JSON.stringify(message));
        }
    } else {
        addMessageToConsole("Falha ao enviar: WebSocket não está conectado.", 'info');
        console.error("WebSocket não está conectado. Não foi possível enviar o comando.");
    }
}

function handleSendCommand() {
    const message = consoleInput.value;
    if (message.trim() !== '') {
        sendCommand(message, 'console');
        addMessageToConsole(message, 'user');
        consoleInput.value = '';
    }
}

// --- LÓGICA DE CONEXÃO WEBSOCKET ---

// Quando a conexão for estabelecida com sucesso
socket.onopen = function (event) {
    document.getElementById('connection-dot').classList.remove('bg-red-500');
    document.getElementById('connection-dot').classList.add('bg-green-500', 'status-pulse');
    document.getElementById('connection-text').innerText = 'CONECTADO AO PYTHON (WS)';
    document.getElementById('connection-text').className = 'text-sm font-bold text-glow-green tracking-wider uppercase';
    console.log("WebSocket Conectado!");

    // Habilita os controles
    controlPanelButtons.forEach(btn => btn.disabled = false);
    consoleInput.disabled = false;
    consoleSend.disabled = false;
    consoleInput.placeholder = "Digite um comando e pressione Enter...";

    addMessageToConsole("Conexão com o servidor estabelecida.", 'info');
};

// Quando um novo pacote de dados chegar do Python
socket.onmessage = function (event) {
    const data = JSON.parse(event.data);

    // Verifica se é uma resposta para o console
    if (data.type === 'console_response' && data.message) {
        addMessageToConsole(data.message, 'system');
    }
    // Verifica se são dados do dashboard (mantém compatibilidade)
    else if (data.id == "servidor") {
        // A função updateDashboard() deve existir no escopo global (ex: em script.js)
        if (typeof updateDashboard === 'function') {
            updateDashboard(data); // Atualiza a interface instantaneamente
        }
    }
    // Fallback para outras mensagens do sistema
    else {
        const prettyData = JSON.stringify(data, null, 2);
        addMessageToConsole(`Pacote de dados não reconhecido recebido: <pre class="mt-2 bg-black/20 p-2 rounded">${prettyData}</pre>`, 'system');
        console.log("WebSocket: Pacote de dados não reconhecido:", data);
    }
};

// Se a conexão cair ou der erro
socket.onclose = function (event) {
    document.getElementById('connection-dot').classList.remove('bg-green-500', 'status-pulse');
    document.getElementById('connection-dot').classList.add('bg-red-500');
    document.getElementById('connection-text').innerText = 'SISTEMA OFFLINE';
    document.getElementById('connection-text').className = 'text-sm font-bold text-glow-red tracking-wider uppercase';
    console.log("WebSocket Desconectado.");

    // Desabilita os controles
    controlPanelButtons.forEach(btn => btn.disabled = true);
    consoleInput.disabled = true;
    consoleSend.disabled = true;
    consoleInput.placeholder = "Desconectado. Aguardando reconexão...";

    addMessageToConsole("Conexão com o servidor perdida.", 'info');
};

// --- EVENT LISTENERS ---
document.getElementById('btn-ligar').addEventListener('click', () => {
    sendCommand('ligar_esteira', 'controle');
    addMessageToConsole("Comando 'Ligar Esteira' enviado.", 'user');
});
document.getElementById('btn-desligar').addEventListener('click', () => {
    sendCommand('desligar_esteira', 'controle');
    addMessageToConsole("Comando 'Desligar Esteira' enviado.", 'user');
});
document.getElementById('btn-pausar').addEventListener('click', () => {
    sendCommand('pausar_processamento', 'controle');
    addMessageToConsole("Comando 'Pausar Processamento' enviado.", 'user');
});
document.getElementById('btn-reiniciar').addEventListener('click', () => {
    sendCommand('reiniciar_sistema', 'controle');
    addMessageToConsole("Comando 'Reiniciar Sistema' enviado.", 'user');
});

consoleSend.addEventListener('click', handleSendCommand);
consoleInput.addEventListener('keypress', (e) => {
    if (e.key === 'Enter') {
        handleSendCommand();
    }
});