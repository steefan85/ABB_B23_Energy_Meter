const char index_page[] PROGMEM = R"=====(
<!doctype html>
<html lang="de">
    <head>
        <title>POWER METER</title>
        <meta charset="utf-8"/>
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <script src="https://cdn.jsdelivr.net/npm/chart.js@3.4.1"></script>
        <style>
            body {
              font-family: sans-serif;
              font-size: 100%;
              margin: 0 auto;
              background-color: #999;
            }

            h1 {
              border-bottom: 1px solid #ccc;
              padding-bottom: .5em;
              margin-bottom: .5em;
              text-align: center;
            }

            h2 {
                text-align: center;
            }

            meter {
              margin-left: 5%;
              width: 90%;
              height: 2em;
              display: inline-block;
            }

            label {
                display: inline-block;
                min-width: 50px;
                text-align: right;
            }

            ul {
                list-style-type:none;
            }

            .value {
              width: 100%;
              text-align: center;
            }

            .container {
              -js-display: flex;
              display: flex;
              flex-wrap: wrap;
            }

            .item {
              flex: 1 0 230px;
              background-color: #EEE;
              border:solid 1px #000;
              margin: .5em;
              padding: .25em;
            }
        </style>
    </head>

    <body>
        <h1>POWER METER</h1>
        <div class="container"> 
            <div class="item" id="U_LN">
                <h2>Netzspannung L-N</h2>
                <span id="U_LN_L1">
                    <label>L1</label>
                    <meter id="meter_U_LN_L1" value="230" min="207" max="253"></meter>
                    <div id="text_U_LN_L1" class="value">??.?? V</div>
                </span>
                <span id="U_LN_L2">
                    <label>L2</label>
                    <meter id="meter_U_LN_L2" value="230" min="207" max="253"></meter>
                    <div id="text_U_LN_L2" class="value">??.?? V</div>
                </span>
                <span id="U_LN_L3">
                    <label>L3</label>
                    <meter id="meter_U_LN_L3" value="230" min="207" max="253"></meter>
                    <div id="text_U_LN_L3" class="value">??.?? V</div>
                </span>
                <canvas id="canvas_U_LN"></canvas>
            </div> 
            <div class="item" id="U_LL">
                <h2>Netzspannung L-L</h2>
                <span id="U_LL_L1">
                    <label>L1-L2</label>
                    <meter id="meter_U_LL_L1" value="400" min="360" max="440"></meter>
                    <div id="text_U_LL_L1" class="value">??.?? V</div>
                </span>
                <span id="U_LL_L2">
                    <label>L2-L3</label>
                    <meter id="meter_U_LL_L2" value="400" min="360" max="440"></meter>
                    <div id="text_U_LL_L2" class="value">??.?? V</div>
                </span>
                <span id="U_LL_L3">
                    <label>L3-L1</label>
                    <meter id="meter_U_LL_L3" value="400" min="360" max="440"></meter>
                    <div id="text_U_LL_L3" class="value">??.?? V</div>
                </span>
                <canvas id="canvas_U_LL"></canvas>
            </div>
            <div class="item" id="I">
                <h2>Strom</h2>
                <span id="I_L1">
                    <label>L1</label>
                    <meter id="meter_I_L1" value="0" min="0" max="32"></meter>
                    <div id="text_I_L1" class="value">??.?? A</div>
                </span>
                <span id="I_L2">
                    <label>L2</label>
                    <meter id="meter_I_L2" value="0" min="0" max="32"></meter>
                    <div id="text_I_L2" class="value">??.?? A</div>
                </span>
                <span id="I_L3">
                    <label>L3</label>
                    <meter id="meter_I_L3" value="0" min="0" max="32"></meter>
                    <div id="text_I_L3" class="value">??.?? A</div>
                </span>
                <span id="I_N">
                    <label>N</label>
                    <meter id="meter_I_N" value="0" min="0" max="32"></meter>
                    <div id="text_I_N" class="value">??.?? A</div>
                </span>
                <canvas id="canvas_I"></canvas>
            </div>
            <div class="item" id="P">
                <h2>Wirkleistung</h2>
                <span id="P_L1">
                    <label>L1</label>
                    <meter id="meter_P_L1" value="0" min="0" max="7200"></meter>
                    <div id="text_P_L1" class="value">??.?? W</div>
                </span>
                <span id="P_L2">
                    <label>L2</label>
                    <meter id="meter_P_L2" value="0" min="0" max="7200"></meter>
                    <div id="text_P_L2" class="value">??.?? W</div>
                </span>
                <span id="P_L3">
                    <label>L3</label>
                    <meter id="meter_P_L3" value="0" min="0" max="7200"></meter>
                    <div id="text_P_L3" class="value">??.?? W</div>
                </span>
                <span id="P_SYSTEM">
                    <label>∑</label>
                    <meter id="meter_P_SYSTEM" value="0" min="0" max="21600"></meter>
                    <div id="text_P_SYSTEM" class="value">??.?? W</div>
                </span>
                <canvas id="canvas_P"></canvas>
            </div>
            <div class="item" id="S">
                <h2>Scheinleistung</h2>
                <span id="S_L1">
                    <label>L1</label>
                    <meter id="meter_S_L1" value="0" min="0" max="7200"></meter>
                    <div id="text_S_L1" class="value">??.?? VA</div>
                </span>
                <span id="S_L2">
                    <label>L2</label>
                    <meter id="meter_S_L2" value="0" min="0" max="7200"></meter>
                    <div id="text_S_L2" class="value">??.?? VA</div>
                </span>
                <span id="S_L3">
                    <label>L3</label>
                    <meter id="meter_S_L3" value="0" min="0" max="7200"></meter>
                    <div id="text_S_L3" class="value">??.?? VA</div>
                </span>
                <span id="S_SYSTEM">
                    <label>∑</label>
                    <meter id="meter_S_SYSTEM" value="0" min="0" max="21600"></meter>
                    <div id="text_S_SYSTEM" class="value">??.?? VA</div>
                </span>
                <canvas id="canvas_S"></canvas>
            </div>
            <div class="item" id="Q">
                <h2>Blindleistung</h2>
                <span id="Q_L1">
                    <label>L1</label>
                    <meter id="meter_Q_L1" value="0" min="0" max="7200"></meter>
                    <div id="text_Q_L1" class="value">??.?? VAr</div>
                </span>
                <span id="Q_L2">
                    <label>L2</label>
                    <meter id="meter_Q_L2" value="0" min="0" max="7200"></meter>
                    <div id="text_Q_L2" class="value">??.?? VAr</div>
                </span>
                <span id="Q_L3">
                    <label>L3</label>
                    <meter id="meter_Q_L3" value="0" min="0" max="7200"></meter>
                    <div id="text_Q_L3" class="value">??.?? VAr</div>
                </span>
                <span id="Q_SYSTEM">
                    <label>∑</label>
                    <meter id="meter_Q_SYSTEM" value="0" min="0" max="21600"></meter>
                    <div id="text_Q_SYSTEM" class="value">??.?? VAr</div>
                </span>
                <canvas id="canvas_Q"></canvas>
            </div>
            <div class="item" id="PF">
                <h2>Leistungsfaktor</h2>
                <span id="PF_L1">
                    <label>L1</label>
                    <meter id="meter_PF_L1" value="1" min="0" max="2" high="1.2" low="0.8"></meter>
                    <div id="text_PF_L1" class="value">??.??</div>
                </span>
                <span id="PF_L2">
                    <label>L2</label>
                    <meter id="meter_PF_L2" value="1" min="0" max="2" high="1.2" low="0.8"></meter>
                    <div id="text_PF_L2" class="value">??.??</div>
                </span>
                <span id="PF_L3">
                    <label>L3</label>
                    <meter id="meter_PF_L3" value="1" min="0" max="2" high="1.2" low="0.8"></meter>
                    <div id="text_PF_L3" class="value">??.??</div>
                </span>
                <span id="PF_SYSTEM">
                    <label>Ø</label>
                    <meter id="meter_PF_SYSTEM" value="1" min="0" max="2" high="1.2" low="0.8"></meter>
                    <div id="text_PF_SYSTEM" class="value">??.??</div>
                </span>
                <canvas id="canvas_PF"></canvas>
            </div>
            <div class="item" id="F">
                <h2>Netzfrequenz</h2>
                <span id="F"><span id="F_SYSTEM"></span>
                    <meter id="meter_F_SYSTEM" value="50.00" min="49.8" max="50.2" high="50.01" low="49.99"></meter>
                    <div class="value"><span id="text_F_SYSTEM">??.?? Hz</span><span id="F_CTRL"> (123)</span></div>
                </span>
                <canvas id="canvas_F"></canvas>
            </div> 
            <div class="item" id="IMPORT">
                <h2>Stromimport</h2>
                <label>Wirkleistung</label>
                <div id="text_i_P" class="value">??.?? Wh</div>
                <label>Blindleistung Induktiv</label>
                <div id="text_i_Q_la" class="value">??.?? VArh</div>
                <label>Blindleistung Kapazitiv</label>
                <div id="text_i_Q_le" class="value">??.?? VArh</div>
                <label>Scheinleistung</label>
                <div id="text_i_S" class="value">??.?? VAh</div>
            </div> 
            <div class="item" id="EXPORT">
                <h2>Stromexport</h2>
                <label>Wirkleistung</label>
                <div id="text_e_P" class="value">??.?? Wh</div>
                <label>Blindleistung Induktiv</label>
                <div id="text_e_Q_la" class="value">??.?? VArh</div>
                <label>Blindleistung Kapazitiv</label>
                <div id="text_e_Q_le" class="value">??.?? VArh</div>
                <label>Scheinleistung</label>
                <div id="text_e_S" class="value">??.?? VAh</div>
            </div> 
            <div class="item" id="CFG">
                <h2>Konfiguration</h2>
                <ul>
                    <li>
                        <input type="checkbox" id="C_U_LN" onchange="updateCfg()"> Netzspannung L-N
                        <ul>
                            <li><input type="checkbox" id="C_U_LN_L1" onchange="updateCfg()"> L1</li>
                            <li><input type="checkbox" id="C_U_LN_L2" onchange="updateCfg()"> L2</li>
                            <li><input type="checkbox" id="C_U_LN_L3" onchange="updateCfg()"> L3</li>
                            <li><input type="checkbox" id="C_U_LN_GRAPH" onchange="updateCfg()"> Graph</li>
                        </ul>
                        </li>
                    <li>
                        <input type="checkbox" id="C_U_LL" onchange="updateCfg()"> Netzspannung L-L
                        <ul>
                            <li><input type="checkbox" id="C_U_LL_L1" onchange="updateCfg()"> L1-L2</li>
                            <li><input type="checkbox" id="C_U_LL_L2" onchange="updateCfg()"> L2-L3</li>
                            <li><input type="checkbox" id="C_U_LL_L3" onchange="updateCfg()"> L3-L1</li>
                            <li><input type="checkbox" id="C_U_LL_GRAPH" onchange="updateCfg()"> Graph</li>
                        </ul>
                    </li>
                    <li>
                        <input type="checkbox" id="C_I" onchange="updateCfg()"> Strom
                        <ul>
                            <li><input type="checkbox" id="C_I_L1" onchange="updateCfg()"> L1</li>
                            <li><input type="checkbox" id="C_I_L2" onchange="updateCfg()"> L2</li>
                            <li><input type="checkbox" id="C_I_L3" onchange="updateCfg()"> L3</li>
                            <li><input type="checkbox" id="C_I_N" onchange="updateCfg()"> N</li>
                            <li><input type="checkbox" id="C_I_GRAPH" onchange="updateCfg()"> Graph</li>
                        </ul>
                    </li>
                    <li>
                        <input type="checkbox" id="C_P" onchange="updateCfg()"> Wirkleistung
                        <ul>
                            <li><input type="checkbox" id="C_P_L1" onchange="updateCfg()"> L1</li>
                            <li><input type="checkbox" id="C_P_L2" onchange="updateCfg()"> L2</li>
                            <li><input type="checkbox" id="C_P_L3" onchange="updateCfg()"> L3</li>
                            <li><input type="checkbox" id="C_P_S" onchange="updateCfg()"> ∑</li>
                            <li><input type="checkbox" id="C_P_GRAPH" onchange="updateCfg()"> Graph</li>
                        </ul>
                    </li>
                    <li>
                        <input type="checkbox" id="C_S" onchange="updateCfg()"> Scheinleistung
                        <ul>
                            <li><input type="checkbox" id="C_S_L1" onchange="updateCfg()"> L1</li>
                            <li><input type="checkbox" id="C_S_L2" onchange="updateCfg()"> L2</li>
                            <li><input type="checkbox" id="C_S_L3" onchange="updateCfg()"> L3</li>
                            <li><input type="checkbox" id="C_S_S" onchange="updateCfg()"> ∑</li>
                            <li><input type="checkbox" id="C_S_GRAPH" onchange="updateCfg()"> Graph</li>
                        </ul>
                    </li>
                    <li>
                        <input type="checkbox" id="C_Q" onchange="updateCfg()"> Blindleistung
                        <ul>
                            <li><input type="checkbox" id="C_Q_L1" onchange="updateCfg()"> L1</li>
                            <li><input type="checkbox" id="C_Q_L2" onchange="updateCfg()"> L2</li>
                            <li><input type="checkbox" id="C_Q_L3" onchange="updateCfg()"> L3</li>
                            <li><input type="checkbox" id="C_Q_S" onchange="updateCfg()"> ∑</li>
                            <li><input type="checkbox" id="C_Q_GRAPH" onchange="updateCfg()"> Graph</li>
                        </ul>
                    </li>
                    <li>
                        <input type="checkbox" id="C_PF" onchange="updateCfg()"> Leistungsfaktor
                        <ul>
                            <li><input type="checkbox" id="C_PF_L1" onchange="updateCfg()"> L1</li>
                            <li><input type="checkbox" id="C_PF_L2" onchange="updateCfg()"> L2</li>
                            <li><input type="checkbox" id="C_PF_L3" onchange="updateCfg()"> L3</li>
                            <li><input type="checkbox" id="C_PF_S" onchange="updateCfg()"> Ø</li>
                            <li><input type="checkbox" id="C_PF_GRAPH" onchange="updateCfg()"> Graph</li>
                        </ul>
                    </li>
                    <li>
                        <input type="checkbox" id="C_F" onchange="updateCfg()"> Netzfrequenz
                        <ul>
                            <li><input type="checkbox" id="C_F_GRAPH" onchange="updateCfg()"> Graph</li>
                        </ul>
                    </li>
                    <li>
                        <input type="checkbox" id="C_IMPORT" onchange="updateCfg()"> Stromimport
                        <ul>
                            <li>Preis pro kWh: <input type="text" id="C_IMPORT_E" onchange="updateCfg()"></input>€</li>
                        </ul>
                    </li>
                    <li>
                        <input type="checkbox" id="C_EXPORT" onchange="updateCfg()"> Stromexport
                        <ul>
                            <li>Preis pro kWh: <input type="text" id="C_EXPORT_E" onchange="updateCfg()"></input>€</li>
                        </ul>
                    </li>
                    <li>
                        System
                        <ul>
                            <li>Version: <input type="text" id="C_VERSION" readonly></input></li>
                            <li>Automatische Updates alle: <input type="text" id="C_SYS_UPDATE" onchange="updateCfg()"></input> Sekunden</li>
                            <li>Diagrammhistorie: <input type="text" id="C_SYS_DP" onchange="updateCfg()"></input> Datenpunkte</li>
                        </ul>
                    </li>
                </ul>
            </div> 
        </div>
        
        <small>Update: <span id="update">Unknown</span></small>

        <script>
            /* Avoid scientific notation
               VisioN @ https://stackoverflow.com/a/40157744 */
            Number.prototype.toFixedSpecial = function(n) {
                var str = this.toFixed(n);
                if (str.indexOf('e+') < 0)
                    return str;

                // if number is in scientific notation, pick (b)ase and (p)ower
                return str.replace('.', '').split('e+').reduce(function(p, b) {
                    return p + Array(b - p.length + 2).join(0);
                }) + '.' + Array(n + 1).join(0);
            };

            var xmlhttp = new XMLHttpRequest();
            var url = "/json";

            function updateStart() {
                xmlhttp.onreadystatechange = function() {
                    if (this.readyState == 4) {
                        if(this.status == 200) {
                            try {
                                var data = JSON.parse(this.responseText);
                                updateView(data);
                            } catch (e) {
                                console.log("Update failed: " + e);
                                console.log(this.responseText);
                            }
                        }

                        window.setTimeout(updateStart, cfg['SYS_UPDATE'] * 1000);
                    }
                };
                xmlhttp.open("GET", url, true);
                xmlhttp.send();
            }

            function updateBlock(main, sub, chart, chartnum, value, metric, force=false) {
                try {
                    if(force || cfg[main + '_' + sub]) {
                        document.getElementById("meter_" + main + '_' + sub).value = value;
                        document.getElementById("text_" + main + '_' + sub).innerHTML = value + metric;
                        document.getElementById(main + '_' + sub).style.display = 'block';
                        chart.data.datasets[chartnum].data.push(value);
                    } else {
                        chart.data.datasets[chartnum].data.push(null);
                        document.getElementById(main + '_' + sub).style.display = 'none';
                    }
                } catch (e) {
                    console.log("Update for Block " + main + " -> " + sub + " to " + value + metric + " failed: " + e);
                }
            }

            function updateChart(chart, ident) {
                while(chart.data.datasets[0].data.length > cfg['SYS_DP']) {
                    chart.data.labels.shift();
                    chart.data.datasets.forEach((dataset) => {
                        dataset.data.shift();
                    });
                }

                if(cfg[ident + '_GRAPH']) {
                    document.getElementById("canvas_" + ident).style.display = 'block';
                    chart.update();
                }else{
                    document.getElementById("canvas_" + ident).style.display = 'none';
                }
            }

            function updateView(data) {
                var d = new Date();
                document.getElementById("update").innerHTML = d.toLocaleString()

                if(cfg['U_LN']) {
                    chart_ln.data.labels.push(d.toLocaleTimeString());

                    updateBlock('U_LN', 'L1', chart_ln, 0, data.BZ40i_U_LN_P1, 'V');
                    updateBlock('U_LN', 'L2', chart_ln, 1, data.BZ40i_U_LN_P2, 'V');
                    updateBlock('U_LN', 'L3', chart_ln, 2, data.BZ40i_U_LN_P3, 'V');
                    updateChart(chart_ln, 'U_LN');
                }

                if(cfg['U_LL']) {
                    chart_ll.data.labels.push(d.toLocaleTimeString());

                    updateBlock('U_LL', 'L1', chart_ll, 0, data.BZ40i_U_LL_P12, 'V');
                    updateBlock('U_LL', 'L2', chart_ll, 1, data.BZ40i_U_LL_P23, 'V');
                    updateBlock('U_LL', 'L3', chart_ll, 2, data.BZ40i_U_LL_P31, 'V');
                    updateChart(chart_ll, 'U_LL');
                }

                if(cfg['I']) {
                    chart_i.data.labels.push(d.toLocaleTimeString());

                    updateBlock('I', 'L1', chart_i, 0, data.BZ40i_I_P1, 'A');
                    updateBlock('I', 'L2', chart_i, 1, data.BZ40i_I_P2, 'A');
                    updateBlock('I', 'L3', chart_i, 2, data.BZ40i_I_P3, 'A');
                    updateBlock('I', 'N',  chart_i, 3, data.BZ40i_I_N, 'A');
                    updateChart(chart_i, 'I');
                }

                if(cfg['P']) {
                    chart_p.data.labels.push(d.toLocaleTimeString());
                    updateBlock('P', 'L1', chart_p, 0, data.BZ40i_P_P1, 'W');
                    updateBlock('P', 'L2', chart_p, 1, data.BZ40i_P_P2, 'W');
                    updateBlock('P', 'L3', chart_p, 2, data.BZ40i_P_P3, 'W');
                    updateBlock('P', 'SYSTEM', chart_p, 3, data.BZ40i_P_SYSTEM, 'W');
                    updateChart(chart_p, 'P');

                    console.log(cfg['EXPORT_E'] + " - " + data.BZ40i_P_SYSTEM);

                    if(cfg['IMPORT_E'] != 0 && data.BZ40i_P_SYSTEM >= 0) {
                        document.getElementById("text_P_SYSTEM").innerHTML += " (" + (data.BZ40i_P_SYSTEM / 1000 * 24 * cfg['IMPORT_E']).toFixed(2) +"€/Tag)";
                    } else if(cfg['EXPORT_E'] != 0 && data.BZ40i_P_SYSTEM < 0) {
                        document.getElementById("text_P_SYSTEM").innerHTML += " (" + (data.BZ40i_P_SYSTEM / 1000 * 24 * cfg['EXPORT_E']).toFixed(2) +"€/Tag)";
                    }
                }

                if(cfg['S']) {
                    chart_s.data.labels.push(d.toLocaleTimeString());

                    updateBlock('S', 'L1', chart_s, 0, data.BZ40i_S_P1, 'VA');
                    updateBlock('S', 'L2', chart_s, 1, data.BZ40i_S_P2, 'VA');
                    updateBlock('S', 'L3', chart_s, 2, data.BZ40i_S_P3, 'VA');
                    updateBlock('S', 'SYSTEM', chart_s, 3, data.BZ40i_S_SYSTEM, 'VA');
                    updateChart(chart_s, 'S');
                }

                if(cfg['Q']) {
                    chart_q.data.labels.push(d.toLocaleTimeString());

                    updateBlock('Q', 'L1', chart_q, 0, data.BZ40i_Q_P1, 'VAr');
                    updateBlock('Q', 'L2', chart_q, 1, data.BZ40i_Q_P2, 'VAr');
                    updateBlock('Q', 'L3', chart_q, 2, data.BZ40i_Q_P3, 'VAr');
                    updateBlock('Q', 'SYSTEM', chart_q, 3, data.BZ40i_Q_SYSTEM, 'VAr');
                    updateChart(chart_q, 'Q');
                }

                if(cfg['PF']) {
                    chart_pf.data.labels.push(d.toLocaleTimeString());

                    updateBlock('PF', 'L1', chart_pf, 0, data.BZ40i_PF_P1, ' ');
                    updateBlock('PF', 'L2', chart_pf, 1, data.BZ40i_PF_P2, ' ');
                    updateBlock('PF', 'L3', chart_pf, 2, data.BZ40i_PF_P3, ' ');
                    updateBlock('PF', 'SYSTEM', chart_pf, 3, data.BZ40i_PF_SYSTEM, ' ');
                    updateChart(chart_pf, 'PF');
                }

                if(cfg['F']) {
                    chart_f.data.labels.push(d.toLocaleTimeString());
                    updateBlock('F', 'SYSTEM', chart_f, 0, (data.BZ40i_F / 1000), 'Hz', true);
                    updateChart(chart_f, 'F');
                    
                    if(true) { // Regelleistung
                        if(data.BZ40i_F >= 50010) {
                            deviation = data.BZ40i_F - 50010;
                            deviation = 3000 / 200 * deviation;
                            document.getElementById("F_CTRL").innerHTML = " (-" + deviation + "MW Regelleistung)";
                        }else if(data.BZ40i_F <= 49990) {
                            deviation = 49990 - data.BZ40i_F;
                            deviation = 3000 / 200 * deviation;
                            document.getElementById("F_CTRL").innerHTML = " (+" + deviation + "MW Regelleistung)";
                        }else{
                            document.getElementById("F_CTRL").innerHTML = "";
                        }
                    }
                }

                if(cfg['IMPORT']) {
                    temp = (data.BZ40i_IMPORT_P);
                    if(temp > 1000) temp = (temp/1000).toFixedSpecial(3) + "k";
                    document.getElementById("text_i_P").innerHTML = temp + "Wh";
                    if(cfg['IMPORT_E'] != 0) {
                        document.getElementById("text_i_P").innerHTML += " (" + (data.BZ40i_IMPORT_P / 1000 * cfg['IMPORT_E']).toFixed(2) +"€)";
                    }
                    temp = (data.BZ40i_IMPORT_Q_LAG);
                    if(temp > 1000) temp = (temp/1000).toFixedSpecial(3) + "k";
                    document.getElementById("text_i_Q_la").innerHTML = (temp) + "VArh";
                    temp = (data.BZ40i_IMPORT_Q_LEAD);
                    if(temp > 1000) temp = (temp/1000).toFixedSpecial(3) + "k";
                    document.getElementById("text_i_Q_le").innerHTML = (temp) + "VArh";
                    temp = (data.BZ40i_IMPORT_S);
                    if(temp > 1000) temp = (temp/1000).toFixedSpecial(3) + "k";
                    document.getElementById("text_i_S").innerHTML = (temp) + "VAh";
                }

                if(cfg['EXPORT']) {
                    temp = (data.BZ40i_EXPORT_P);
                    if(temp > 1000) temp = (temp/1000).toFixedSpecial(3) + "k";
                    document.getElementById("text_e_P").innerHTML = (temp) + "Wh";
                    if(cfg['EXPORT_E'] != 0) {
                        document.getElementById("text_e_P").innerHTML += " (" + (data.BZ40i_EXPORT_P / 1000 * cfg['EXPORT_E']).toFixed(2) +"€)";
                    }
                    temp = (data.BZ40i_EXPORT_Q_LAG);
                    if(temp > 1000) temp = (temp/1000).toFixedSpecial(3) + "k";
                    document.getElementById("text_e_Q_la").innerHTML = (temp) + "VArh";
                    temp = (data.BZ40i_EXPORT_Q_LEAD);
                    if(temp > 1000) temp = (temp/1000).toFixedSpecial(3) + "k";
                    document.getElementById("text_e_Q_le").innerHTML = (temp) + "VArh";
                    temp = (data.BZ40i_EXPORT_S);
                    if(temp > 1000) temp = (temp/1000).toFixedSpecial(3) + "k";
                    document.getElementById("text_e_S").innerHTML = (temp) + "VAh";
                }
            }

            function setCookie(name, value) {
                var d = new Date();
                d.setTime(d.getTime() + (90 * 24 * 60 * 60 * 1000));
                var expires = "expires="+d.toUTCString();
                document.cookie = name + "=" + JSON.stringify(value) + ";" + expires + ";path=/";
                console.log(document.cookie);
            }

            function getCookie(name) {
                var result = document.cookie.match(new RegExp(name + '=([^;]+)'));
                result && (result = JSON.parse(result[1]));
                return result;
            }

            function applyCfg() {
                for (const [key, value] of Object.entries(cfg)) {
                    console.log(key, value);
                    try{
                        if(document.getElementById("C_"+key).type.toLowerCase() == 'checkbox') {
                            document.getElementById("C_"+key).checked = value;
                            if(value) {
                                document.getElementById(key).style.display = 'block';
                            }else{
                                document.getElementById(key).style.display = 'none';
                            }
                        } else if(document.getElementById("C_"+key).type.toLowerCase() == 'text') {
                            document.getElementById("C_"+key).value = value;
                        }
                    } catch (e) {
                        console.log("Apply failed: " + e);
                    }
                }
            }

            function updateCfg() {
                changed = false;
                for (const [key, value] of Object.entries(cfg)) {
                    if(!document.getElementById("C_"+key)) {
                        console.log("Skipping update of " + key + ": Broken");
                    }else if(document.getElementById("C_"+key).type.toLowerCase() == "checkbox") {
                        try {
                            if(document.getElementById("C_"+key).checked != value) {
                                cfg[key] = document.getElementById("C_"+key).checked;
                                changed = true;
                                console.log(key + " changed to " + cfg[key]);
                            } else{
                                console.log("Skipping update of " + key + ": Unchanged");
                            }
                        } catch (e) {
                            console.log("UCfg checkbox for " + key + " = " + value + " failed: " + e);
                        }
                    }else if(document.getElementById("C_"+key).type.toLowerCase() == "text") {
                        try {
                            if(document.getElementById("C_"+key).value != value) {
                                cfg[key] = document.getElementById("C_"+key).value;
                                changed = true;
                                console.log(key + " changed to " + cfg[key]);
                            } else{
                                console.log("Skipping update of " + key + ": Unchanged");
                            }
                        } catch (e) {
                            console.log("UCfg text for " + key + " = " + value + " failed: " + e);
                        }
                    }else{
                        console.log("Skipping update of " + key + ": Unknown type " + document.getElementById("C_"+key).type.toLowerCase());
                    }
                }
                if(changed) {
                    setCookie('BZ40i_cfg', cfg);
                    applyCfg();
                }
            }

            function newChart(id, lbl) {
                var ctx = document.getElementById(id).getContext('2d');
                //var colors = ['rgb(255, 99, 132)', 'rgb(132, 255, 99)',  'rgb(99, 132, 255)', 'rgb(132, 99, 255)'];
                var colors = ['rgb(110, 94, 68)', 'rgb(0, 0, 0)',  'rgb(95, 95, 95)', 'rgb(99, 99, 255)']; //CENELEC color codes

                vdatasets = [];
                for(let i=0; i<lbl.length; i++) {
                    vdatasets.push(
                        {
                            label: lbl[i],
                            borderColor: colors[i],
                            data: []
                        }
                    );
                };

                var chart_out = new Chart(ctx, {
                    type: 'line',
                    data: {
                        labels: [],
                        datasets: vdatasets
                    },
                    options: {}
                });

                return chart_out;
            }
            
            var chart_ln = newChart('canvas_U_LN', ['L1', 'L2', 'L3']);
            var chart_ll = newChart('canvas_U_LL', ['L1-L2', 'L2-L3', 'L3-L1']);
            var chart_i  = newChart('canvas_I',    ['L1', 'L2', 'L3', 'N']);
            var chart_p  = newChart('canvas_P',    ['L1', 'L2', 'L3', '∑']);
            var chart_s  = newChart('canvas_S',    ['L1', 'L2', 'L3', '∑']);
            var chart_q  = newChart('canvas_Q',    ['L1', 'L2', 'L3', '∑']);
            var chart_pf = newChart('canvas_PF',   ['L1', 'L2', 'L3', 'Ø']);
            var chart_f  = newChart('canvas_F',    ['Netzfrequenz']);

            cfg = getCookie('BZ40i_cfg');
            if(cfg == null || cfg['VERSION'] < 4) {
                cfg = {};
                cfg['VERSION'] = 4;
                cfg['U_LN'] = true;
                cfg['U_LN_L1'] = true;
                cfg['U_LN_L2'] = true;
                cfg['U_LN_L3'] = true;
                cfg['U_LN_GRAPH'] = true;
                cfg['U_LL'] = true;
                cfg['U_LL_L1'] = true;
                cfg['U_LL_L2'] = true;
                cfg['U_LL_L3'] = true;
                cfg['U_LL_GRAPH'] = true;
                cfg['I'] = true;
                cfg['I_L1'] = true;
                cfg['I_L2'] = true;
                cfg['I_L3'] = true;
                cfg['I_N'] = true;
                cfg['I_GRAPH'] = true;
                cfg['P'] = true;
                cfg['P_L1'] = true;
                cfg['P_L2'] = true;
                cfg['P_L3'] = true;
                cfg['P_SYSTEM'] = true;
                cfg['P_GRAPH'] = true;
                cfg['S'] = true;
                cfg['S_L1'] = true;
                cfg['S_L2'] = true;
                cfg['S_L3'] = true;
                cfg['S_SYSTEM'] = true;
                cfg['S_GRAPH'] = true;
                cfg['Q'] = true;
                cfg['Q_L1'] = true;
                cfg['Q_L2'] = true;
                cfg['Q_L3'] = true;
                cfg['Q_SYSTEM'] = true;
                cfg['Q_GRAPH'] = true;
                cfg['PF'] = true;
                cfg['PF_L1'] = true;
                cfg['PF_L2'] = true;
                cfg['PF_L3'] = true;
                cfg['PF_SYSTEM'] = true;
                cfg['PF_GRAPH'] = true;
                cfg['F'] = true;
                cfg['F_GRAPH'] = true;
                cfg['IMPORT'] = true;
                cfg['IMPORT_E'] = 0.00;
                cfg['EXPORT'] = true;
                cfg['EXPORT_E'] = 0.00;
                cfg['SYS_UPDATE'] = 6;
                cfg['SYS_DP'] = 100;
                console.log('Created new configuration');
            }else{
                console.log('Using existing configuration');
            }
            setCookie('BZ40i_cfg', cfg); //Refresh deletion timer

            applyCfg();
            updateStart();
        </script>
    </body>
</html>
)=====";
