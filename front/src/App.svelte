<script>
  import { onMount } from 'svelte';
  import { Chart, registerables } from 'chart.js';
  Chart.register(...registerables);

  let spectrumData = []; // 频谱数据
  let evmVsSymbolData = []; // EVM vs Symbol 数据
  let evmVsRbData = []; // EVM vs RB 数据
  let constellations = []; // 星座图数据
  let evmTable = []; // EVM 表格数据

  let spectrumChart, evmVsSymbolChart, evmVsRbChart, constellationChart;

  // 建立 WebSocket 连接
  onMount(() => {
    const ws = new WebSocket('ws://localhost:9000/frontend');

    ws.onmessage = (event) => {
      const data = JSON.parse(event.data);
      // 更新频谱数据
      if (data.spectrum) {
        spectrumData = data.spectrum;
        updateSpectrumChart(spectrumData);
      }

      // 更新 EVM vs Symbol 数据
      if (data.evmVsSymbol) {
        evmVsSymbolData = data.evmVsSymbol;
        updateEvmVsSymbolChart(evmVsSymbolData);
      }
      // 更新 EVM vs RB 数据
      if (data.evmVsRb) {
        evmVsRbData = data.evmVsRb;
        updateEvmVsRbChart(evmVsRbData);
      }

      // 更新星座图
      if (data.constellationData) {
        constellations = data.constellationData;
        updateConstellationChart(constellations);
      }

      // 更新 EVM 表格
      if (data.evmRes) {
        evmTable = data.evmRes;
      }
    };
  });

  // 更新频谱图
  function updateSpectrumChart(spectrumData) {
    const ctx = document.getElementById('spectrum-chart').getContext('2d');
    if (!spectrumChart) {
      spectrumChart = new Chart(ctx, {
        type: 'line',
        data: {
          labels: spectrumData.map((_, index) => index),
          datasets: [{
            label: 'Spectrum',
            data: spectrumData,
            borderColor: 'rgba(75, 192, 192, 1)',
            fill: false
          }]
        },
        options: {
          responsive: true,
          animation: false,  // 关闭所有动画
          scales: {
            x: { display: true },
            y: { display: true }
          }
        }
      });
    } else {
      spectrumChart.data.datasets[0].data = spectrumData;
      spectrumChart.update();
    }
  }

  // 更新 EVM vs Symbol 图
  function updateEvmVsSymbolChart(evmVsSymbolData) {
    const ctx = document.getElementById('evm-vs-symbol-chart').getContext('2d');
    if (!evmVsSymbolChart) {
      evmVsSymbolChart = new Chart(ctx, {
        type: 'scatter', // 或者 'line' 带有 stem 样式的
        data: {
          labels: evmVsSymbolData.map((_, index) => index),
          datasets: [{
            label: 'EVM vs Symbol',
            data: evmVsSymbolData,
            borderColor: 'rgba(255, 99, 132, 1)',
            fill: false
          }]
        },
        options: {
          responsive: true,
          animation: false,  // 关闭所有动画
          scales: {
            x: { display: true },
            y: { display: true }
          }
        }
      });
    } else {
      evmVsSymbolChart.data.datasets[0].data = evmVsSymbolData;
      evmVsSymbolChart.update();
    }
  }

  // 更新 EVM vs RB 图
  function updateEvmVsRbChart(evmVsRbData) {
    const ctx = document.getElementById('evm-vs-rb-chart').getContext('2d');
    if (!evmVsRbChart) {
      evmVsRbChart = new Chart(ctx, {
        type: 'scatter',
        data: {
          labels: evmVsRbData.map((_, index) => index),
          datasets: [{
                    label: 'Stem Plot',
                    data: evmVsRbData,
                    borderColor: 'blue', // 线的颜色
                    showLine: true,      // 显示线
                    pointRadius: 5,      // 点的大小
                    pointBackgroundColor: 'red', // 点的颜色
                    borderWidth: 1,      // 线的宽度
                    fill: false,         // 不填充
          }]
        },
        options: {
            scales: {
                x: {
                    type: 'linear',
                    position: 'bottom'
                },
                y: {
                    beginAtZero: true  // y 轴从 0 开始
                }
            },
            elements: {
                line: {
                    tension: 0 // 线段不平滑
                }
            },
            animation: false // 关闭动画加速更新
        }
      });
    } else {
      evmVsRbChart.data.datasets[0].data = evmVsRbData;
      evmVsRbChart.update();
    }
  }

  // 更新星座图
  function updateConstellationChart(constellations) {
    const ctx = document.getElementById('constellation-chart').getContext('2d');
    const real = constellations.flatMap(c => c.real);
    const imag = constellations.flatMap(c => c.imag);
    const refReal = constellations.flatMap(c => c.ref_real);
    const refImag = constellations.flatMap(c => c.ref_imag);

    if (!constellationChart) {
      constellationChart = new Chart(ctx, {
        type: 'scatter',
        data: {
          datasets: [{
            label: 'Received Constellation',
            data: real.map((r, i) => ({ x: r, y: imag[i] })),
            backgroundColor: 'rgba(255, 99, 132, 1)'
          }, {
            label: 'Reference Constellation',
            data: refReal.map((r, i) => ({ x: r, y: refImag[i] })),
            backgroundColor: 'rgba(54, 162, 235, 1)'
          }]
        },
        options: {
          responsive: true,
          animation: false,  // 关闭所有动画
          plugins: {
            decimation: {
                enabled: true,
                algorithm: 'min-max',  // 使用 min-max 抽样
                samples: 1000          // 渲染时限制为1000个数据点
            }
          },
          scales: {
            x: { display: true },
            y: { display: true }
          }
        }
      });
    } else {
      constellationChart.data.datasets[0].data = real.map((r, i) => ({ x: r, y: imag[i] }));
      constellationChart.data.datasets[1].data = refReal.map((r, i) => ({ x: r, y: refImag[i] }));
      constellationChart.update();
    }
  }
</script>
<!-- 布局容器 -->
<div class="grid-container">
    <!-- 频谱图 -->
    <canvas id="spectrum-chart"></canvas>

    <!-- EVM vs Symbol 图 -->
    <canvas id="evm-vs-symbol-chart"></canvas>

    <!-- EVM vs RB 图 -->
    <canvas id="evm-vs-rb-chart"></canvas>

    <!-- 星座图 -->
    <canvas id="constellation-chart"></canvas>

    <!-- EVM 表格 -->
    <table>
      <thead>
        <tr>
          <th>Field</th>
          <th>EVM RMS</th>
          <th>EVM Max</th>
          <th>EVM Min</th>
        </tr>
      </thead>
      <tbody>
        {#each evmTable as evm}
        <tr>
          <td>{evm.field}</td>
          <td>{evm.evmRms}</td>
          <td>{evm.evmMax}</td>
          <td>{evm.evmMin}</td>
        </tr>
        {/each}
      </tbody>
    </table>
</div>

<!-- CSS 部分 -->
<style>
  /* 设置网格布局 */
  .grid-container {
    display: grid;
    grid-template-columns: repeat(2, 1fr);  /* 两列 */
    grid-template-rows: repeat(2, auto);    /* 两行 */
    gap: 20px;                              /* 图表之间的间距 */
    margin: 20px;
  }

  /* 图表根据页面大小自动调整 */
  canvas {
    width: 100%;        /* 宽度自适应 */
    height: 100%;       /* 高度自适应 */
    max-width: 600px;   /* 设置图表的最大宽度 */
    max-height: 400px;  /* 设置图表的最大高度 */
    border: 1px solid #ccc; /* 添加边框 */
  }

  /* 表格占满全宽，放置在最下方 */
  .evm-table {
    grid-column: 1 / span 2; /* 表格跨两列 */
    width: 100%;
    border-collapse: collapse;
    margin-top: 20px;
  }

  /* 表格样式 */
  .evm-table th, .evm-table td {
    border: 1px solid #ccc;
    padding: 10px;
    text-align: left;
  }

  .evm-table th {
    background-color: #f2f2f2;
  }
</style>
