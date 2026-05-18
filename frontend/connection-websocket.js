// =====================================================================
        // COMUNICAÇÃO COM O PYTHON (WebSocket)
        // =====================================================================
        
        // Conecta ao servidor WebSocket do Python
        const socket = new WebSocket('ws://localhost:8765');

        // Quando a conexão for estabelecida com sucesso
        socket.onopen = function(event) {
            document.getElementById('connection-dot').classList.remove('bg-red-500');
            document.getElementById('connection-dot').classList.add('bg-green-500', 'status-pulse');
            document.getElementById('connection-text').innerText = 'CONECTADO AO PYTHON (WS)';
            document.getElementById('connection-text').className = 'text-sm font-bold text-glow-green tracking-wider uppercase';
            console.log("WebSocket Conectado!");
        };

        // Quando um novo pacote de dados chegar do Python
        socket.onmessage = function(event) {
            const dadosJson = JSON.parse(event.data);
            updateDashboard(dadosJson); // Atualiza a interface instantaneamente
        };

        // Se a conexão cair ou der erro
        socket.onclose = function(event) {
            document.getElementById('connection-dot').classList.remove('bg-green-500', 'status-pulse');
            document.getElementById('connection-dot').classList.add('bg-red-500');
            document.getElementById('connection-text').innerText = 'SISTEMA OFFLINE';
            document.getElementById('connection-text').className = 'text-sm font-bold text-glow-red tracking-wider uppercase';
            console.log("WebSocket Desconectado.");
        };