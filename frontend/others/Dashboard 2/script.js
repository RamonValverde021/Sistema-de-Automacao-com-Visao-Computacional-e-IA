const state = { coca: 0, fanta: 0, sprite: 0, erro: 0, status: 'processing', taxa: 0, confianca: 0, ciclo: 0, running: true };
const sparkData = Array(20).fill(0);

const $ = id => document.getElementById(id);
const rand = (a, b) => Math.random() * (b - a) + a;

function formatTime(d) { return d.toLocaleTimeString('pt-BR'); }
function formatDate(d) { return d.toLocaleDateString('pt-BR', { weekday: 'short', day: '2-digit', month: 'short', year: 'numeric' }); }

function tick() {
    const d = new Date();
    $('clock').textContent = formatTime(d);
    $('dateLbl').textContent = formatDate(d);
}
tick(); setInterval(tick, 1000);

let prodChart, donutChart, sparkChart;
Chart.defaults.color = '#64748b';

function initCharts() {
    prodChart = new Chart($('prodChart'), {
        type: 'bar',
        data: {
            labels: ['Coca-Cola', 'Fanta', 'Sprite', 'Erro'],
            datasets: [{
                label: 'Unidades',
                data: [0, 0, 0, 0],
                backgroundColor: ['rgba(204,0,0,.7)', 'rgba(255,116,0,.7)', 'rgba(29,185,84,.7)', 'rgba(255,51,102,.7)'],
                borderColor: ['#cc0000', '#ff7400', '#1db954', '#ff3366'],
                borderWidth: 1, borderRadius: 4
            }]
        },
        options: {
            responsive: true, maintainAspectRatio: false, animation: { duration: 400 },
            plugins: { legend: { display: false } },
            scales: {
                x: { grid: { color: 'rgba(255,255,255,.04)' }, ticks: { font: { size: 10 } } },
                y: { grid: { color: 'rgba(255,255,255,.04)' }, ticks: { font: { size: 10 } }, beginAtZero: true }
            }
        }
    });

    donutChart = new Chart($('donutChart'), {
        type: 'doughnut',
        data: {
            labels: ['Coca-Cola', 'Fanta', 'Sprite', 'Erro'],
            datasets: [{ data: [1, 1, 1, 1], backgroundColor: ['#cc0000', '#ff7400', '#1db954', '#ff3366'], borderWidth: 0, hoverOffset: 4 }]
        },
        options: { responsive: false, plugins: { legend: { display: false } }, cutout: '70%', animation: { duration: 400 } }
    });

    sparkChart = new Chart($('sparkChart'), {
        type: 'line',
        data: {
            labels: sparkData.map((_, i) => i),
            datasets: [{
                data: sparkData, borderColor: '#00d4ff', borderWidth: 1.5, fill: true,
                backgroundColor: 'rgba(0,212,255,.07)', pointRadius: 0, tension: .4
            }]
        },
        options: {
            responsive: true, maintainAspectRatio: false, animation: { duration: 200 },
            plugins: { legend: { display: false } },
            scales: { x: { display: false }, y: { display: false, beginAtZero: true } }
        }
    });
}

function updateCharts() {
    const { coca, fanta, sprite, erro } = state;
    const total = coca + fanta + sprite + erro || 1;
    prodChart.data.datasets[0].data = [coca, fanta, sprite, erro];
    prodChart.update('none');
    donutChart.data.datasets[0].data = [coca || .01, fanta || .01, sprite || .01, erro || .01];
    donutChart.update('none');
    sparkData.push(state.taxa); sparkData.shift();
    sparkChart.data.datasets[0].data = [...sparkData];
    sparkChart.update('none');

    $('cnt-coca').textContent = coca;
    $('cnt-fanta').textContent = fanta;
    $('cnt-sprite').textContent = sprite;
    $('cnt-error').textContent = erro;
    $('totalCount').textContent = coca + fanta + sprite + erro;

    const pct = v => ((v / total) * 100).toFixed(0) + '%';
    $('pct-coca').textContent = pct(coca);
    $('pct-fanta').textContent = pct(fanta);
    $('pct-sprite').textContent = pct(sprite);
    $('pct-error').textContent = pct(erro);

    const maxV = Math.max(coca, fanta, sprite, erro, 1);
    $('bar-coca').style.width = (coca / maxV * 100) + '%';
    $('bar-fanta').style.width = (fanta / maxV * 100) + '%';
    $('bar-sprite').style.width = (sprite / maxV * 100) + '%';
    $('bar-error').style.width = (erro / maxV * 100) + '%';

    $('detRate').textContent = state.taxa.toFixed(1);
    $('bar-rate').style.width = Math.min(state.taxa / 10 * 100, 100) + '%';
    $('aiConf').textContent = state.confianca.toFixed(1);
    $('bar-conf').style.width = state.confianca + '%';
    $('cycleTime').textContent = state.ciclo.toFixed(0);
}

const feedItems = [];
const classes = ['Coca-Cola', 'Fanta', 'Sprite', 'Erro'];
const badges = ['feed-badge-coca', 'feed-badge-fanta', 'feed-badge-sprite', 'feed-badge-error'];
const icons = ['🥤', '🍊', '💚', '⚠️'];

function addFeed(cls, time) {
    const idx = classes.indexOf(cls);
    if (idx < 0) return;
    feedItems.unshift({ cls, badge: badges[idx], icon: icons[idx], time: formatTime(time || new Date()) });
    if (feedItems.length > 8) feedItems.pop();
    const el = $('feedList');
    el.innerHTML = feedItems.map(f =>`
    <div class="feed-item">
      <span>${f.icon}</span>
      <span class="feed-badge ${f.badge}">${f.cls}</span>
      <i class="ti ti-robot" style="font-size:13px;color:var(--c-muted)" aria-hidden="true"></i>
      <span style="font-size:11px;color:var(--c-muted)">Detectado via TM</span>
      <span class="feed-time">${f.time}</span>
    </div>`).join('');
}

const statusConfig = {
    running: { cls: 'status-running', text: 'PROCESSANDO', conv: 'running' },
    stopped: { cls: 'status-stopped', text: 'PARADA', conv: 'stopped' },
    error: { cls: 'status-error', text: 'ERRO', conv: 'error' },
    processing: { cls: 'status-processing', text: 'INICIALIZANDO', conv: 'processing' }
};

function setStatus(s) {
    state.status = s; state.running = (s === 'running' || s === 'processing');
    const cfg = statusConfig[s] || statusConfig.running;
    const pill = $('statusPill');
    pill.className = 'status-pill ' + cfg.cls;
    $('statusText').textContent = cfg.text;
    const track = $('convTrack');
    track.style.animationPlayState = state.running ? 'running' : 'paused';
}

function resetAll() {
    state.coca = state.fanta = state.sprite = state.erro = 0;
    sparkData.fill(0);
    feedItems.length = 0;
    $('feedList').innerHTML = '';
    updateCharts();
}

function simulateTick() {
    if (!state.running) return;
    const roll = Math.random();
    if (roll < .38) state.coca++;
    else if (roll < .68) state.fanta++;
    else if (roll < .92) state.sprite++;
    else state.erro++;
    state.taxa = parseFloat((rand(2, 8)).toFixed(1));
    state.confianca = parseFloat((rand(82, 99)).toFixed(1));
    state.ciclo = parseFloat((rand(120, 340)).toFixed(0));

    const picked = roll < .38 ? 'Coca-Cola' : roll < .68 ? 'Fanta' : roll < .92 ? 'Sprite' : 'Erro';
    addFeed(picked, new Date());
    updateCharts();
}

window.setStatus = setStatus;
window.resetAll = resetAll;

window.receiveJSON = function (json) {
    try {
        const d = typeof json === 'string' ? JSON.parse(json) : json;
        if (d.coca !== undefined) state.coca = d.coca;
        if (d.fanta !== undefined) state.fanta = d.fanta;
        if (d.sprite !== undefined) state.sprite = d.sprite;
        if (d.erro !== undefined) state.erro = d.erro;
        if (d.status) setStatus(d.status);
        if (d.taxa !== undefined) state.taxa = d.taxa;
        if (d.confianca !== undefined) state.confianca = d.confianca;
        if (d.ciclo !== undefined) state.ciclo = d.ciclo;
        if (d.ultimo) addFeed(d.ultimo);
        updateCharts();
    } catch (e) { console.error('JSON inválido', e); }
};

initCharts();
setInterval(simulateTick, 1100);