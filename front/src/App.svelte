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
      if (data.evm_vs_symbol) {
        evmVsSymbolData = data.evm_vs_symbol;
        updateEvmVsSymbolChart(evmVsSymbolData);
      }

      // 更新 EVM vs RB 数据
      if (data.evm_vs_rb) {
        evmVsRbData = data.evm_vs_rb;
        updateEvmVsRbChart(evmVsRbData);
      }

      // 更新星座图
      if (data.constellation_data) {
        constellations = data.constellation_data;
        updateConstellationChart(constellations);
      }

      // 更新 EVM 表格
      if (data.evm_res) {
        evmTable = data.evm_res;
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
            label: 'EVM vs RB',
            data: evmVsRbData,
            borderColor: 'rgba(54, 162, 235, 1)',
            fill: false
          }]
        },
        options: {
          responsive: true,
          scales: {
            x: { display: true },
            y: { display: true }
          }
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
      <td>{evm.evm_rms}</td>
      <td>{evm.evm_max}</td>
      <td>{evm.evm_min}</td>
    </tr>
    {/each}
  </tbody>
</table>
