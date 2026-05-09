const canvas = document.getElementById('radar');
const ctx = canvas.getContext('2d');
const status = document.getElementById('status');
const socket = new WebSocket('ws://' + window.location.hostname + '/ws');

socket.onopen = () => status.innerText = "ONLINE";
socket.onclose = () => status.innerText = "OFFLINE";

function drawUI() {
    ctx.strokeStyle = 'rgba(0, 255, 65, 0.15)';
    ctx.lineWidth = 1;
    // Entfernungskreise
    for(let i=1; i<=4; i++) {
        ctx.beginPath();
        ctx.arc(300, 430, i * 100, Math.PI, 2 * Math.PI);
        ctx.stroke();
        ctx.fillStyle = 'rgba(0, 255, 65, 0.3)';
        ctx.fillText(`${i}m`, 305, 430 - (i * 100));
    }
    // Achsen
    ctx.beginPath();
    ctx.moveTo(300, 430); ctx.lineTo(300, 20);
    ctx.stroke();
}

socket.onmessage = (event) => {
    const targets = JSON.parse(event.data);
    
    // Motion Blur Effekt
    ctx.fillStyle = 'rgba(0, 0, 0, 0.15)';
    ctx.fillRect(0, 0, canvas.width, canvas.height);
    
    drawUI();

    targets.forEach(t => {
        const x = 300 + (t.x / 10);
        const y = 430 - (t.y / 10);
        const color = (t.s > 0) ? '255, 50, 50' : '50, 255, 50';

        ctx.shadowBlur = 10;
        ctx.shadowColor = `rgb(${color})`;
        ctx.fillStyle = `rgb(${color})`;
        
        ctx.beginPath();
        ctx.arc(x, y, 7, 0, Math.PI * 2);
        ctx.fill();
        
        ctx.shadowBlur = 0;
        ctx.fillStyle = "#fff";
        ctx.fillText(`ID:${t.id} ${t.s}cm/s`, x + 12, y);
    });
};
