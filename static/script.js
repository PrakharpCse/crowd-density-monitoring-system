const HALL_CAPACITY = 100;

let crowdChart;
let gateChart;
let dataHistory = [];
let lastCrowd = null;
let alertPlaying = false;

document.addEventListener("DOMContentLoaded", () => {
    initCharts();
    initButtons();
    fetchData();
    setInterval(fetchData, 2000);
    loadHistory();
    setInterval(loadHistory, 4000);
});


function loadHistory() {
    fetch("/api/history")
        .then(res => res.json())
        .then(data => {
            const box = document.getElementById("historyBox");

            if (!data.history || data.history.length === 0) {
                box.textContent = "No logs yet.";
                return;
            }

            box.textContent = data.history.slice(-30).join("");
        });
}


function initCharts() {
    const crowdCtx = document.getElementById("crowdChart").getContext("2d");
    const gateCtx = document.getElementById("gateChart").getContext("2d");

    crowdChart = new Chart(crowdCtx, {
        type: "line",
        data: {
            labels: [],
            datasets: [{ label: "Crowd", data: [], borderWidth: 2 }]
        },
        options: { responsive: true }
    });

    gateChart = new Chart(gateCtx, {
        type: "bar",
        data: {
            labels: ["Entry", "Exit"],
            datasets: [{ label: "Gate", data: [0, 0], borderWidth: 2 }]
        },
        options: { responsive: true }
    });
}


function initButtons() {
    const themeToggle = document.getElementById("themeToggle");
    themeToggle.addEventListener("click", () => {
        document.body.classList.toggle("dark");
        themeToggle.textContent =
            document.body.classList.contains("dark")
                ? "☀ Light Mode"
                : "🌙 Dark Mode";
    });

    document.getElementById("downloadCsv").addEventListener("click", downloadCsv);
    document.getElementById("downloadHistory").addEventListener("click", () => {
        window.location.href = "/download_history";
    });

    document.getElementById("generateQr").addEventListener("click", () => {
        const qr = document.getElementById("qrcode");
        qr.innerHTML = "";
        new QRCode(qr, window.location.href);
    });
}


async function fetchData() {
    try {
        const res = await fetch("/get_data");
        const data = await res.json();

        updateUI(data);
        updateCharts(data);
        updateFlow(data.crowd);
        updateStatus(true);

    } catch {
        updateStatus(false);
    }
}


function updateUI(d) {
    document.getElementById("entryCount").textContent = d.entry;
    document.getElementById("exitCount").textContent = d.exit;
    document.getElementById("crowdCount").textContent = d.crowd;

    const percent = Math.min(100, Math.round((d.crowd / HALL_CAPACITY) * 100));
    document.getElementById("capacityPercent").textContent = percent + "%";
    document.getElementById("capacityBarFill").style.width = percent + "%";
}


function updateCharts(d) {
    const t = new Date().toLocaleTimeString();

    dataHistory.push({ time: t, entry: d.entry, exit: d.exit, crowd: d.crowd });
    if (dataHistory.length > 50) dataHistory.shift();

    crowdChart.data.labels = dataHistory.map(x => x.time);
    crowdChart.data.datasets[0].data = dataHistory.map(x => x.crowd);
    crowdChart.update();

    gateChart.data.datasets[0].data = [d.entry, d.exit];
    gateChart.update();
}


function updateFlow(crowd) {
    const icon = document.getElementById("flowIcon");
    const text = document.getElementById("flowText");

    if (lastCrowd === null) {
        icon.textContent = "⬍";
        text.textContent = "Stable";
    } else if (crowd > lastCrowd) {
        icon.textContent = "⬆";
        text.textContent = "Increasing";
    } else if (crowd < lastCrowd) {
        icon.textContent = "⬇";
        text.textContent = "Decreasing";
    } else {
        icon.textContent = "⬍";
        text.textContent = "Stable";
    }

    lastCrowd = crowd;
}


function updateStatus(online) {
    const dot = document.getElementById("statusDot");
    const txt = document.getElementById("statusText");

    if (online) {
        dot.style.background = "#22c55e";
        txt.textContent = "Device Online";
    } else {
        dot.style.background = "red";
        txt.textContent = "Device Offline";
    }
}


function downloadCsv() {
    if (dataHistory.length === 0) return alert("No data.");

    let csv = "Time,Entry,Exit,Crowd\n";
    dataHistory.forEach(d => {
        csv += `${d.time},${d.entry},${d.exit},${d.crowd}\n`;
    });

    const a = document.createElement("a");
    a.href = "data:text/csv;charset=utf-8," + encodeURIComponent(csv);
    a.download = "crowd_report.csv";
    a.click();
}
