<script>
    import { onMount } from 'svelte';
    import { Chart, registerables } from 'chart.js';

    // 注册 Chart.js 的所有必要组件
    Chart.register(...registerables);

    let lineChart;
    let scatterChart;

    onMount(() => {
        // WebSocket 连接
        const ws = new WebSocket('ws://localhost:9000');

        // 初始化线图 (line chart)
        const lineCtx = document.getElementById('lineChart').getContext('2d');
        lineChart = new Chart(lineCtx, {
            type: 'line',
            data: {
                labels: [],  // 时间/序列
                datasets: [{
                    label: 'Line Data',
                    data: [],
                    borderColor: 'rgba(75, 192, 192, 1)',
                    borderWidth: 1
                }]
            },
            options: {
                scales: {
                    y: { beginAtZero: true }
                }
            }
        });

        // 初始化星座图 (scatter chart)
        const scatterCtx = document.getElementById('scatterChart').getContext('2d');
        scatterChart = new Chart(scatterCtx, {
            type: 'scatter',
            data: {
                datasets: [{
                    label: 'Constellation',
                    data: [],  // 复数数据点
                    backgroundColor: 'rgba(255, 99, 132, 1)'
                }]
            },
            options: {
                scales: {
                    x: {
                        type: 'linear',  // 注册 linear scale
                        position: 'bottom'
                    },
                    y: {
                        type: 'linear'  // 注册 linear scale
                    }
                }
            }
        });

        // WebSocket 接收数据并更新图表
        ws.onmessage = (event) => {
            const data = JSON.parse(event.data);

            // 假设 `data.line` 是线图数据，`data.constellation` 是星座图数据
            if (data.line) {
                const lineData = data.line.map(point => point.y);
                lineChart.data.labels = data.line.map(point => point.x);
                lineChart.data.datasets[0].data = lineData;
                lineChart.update();
            }

            if (data.constellation) {
                const scatterData = data.constellation.map(point => ({
                    x: point.real,   // 复数点的实部
                    y: point.imag    // 复数点的虚部
                }));
                scatterChart.data.datasets[0].data = scatterData;
                scatterChart.update();
            }
        };
    });
</script>

<div style="display: flex;">
    <canvas id="lineChart" width="400" height="400"></canvas>
    <canvas id="scatterChart" width="400" height="400"></canvas>
</div>
