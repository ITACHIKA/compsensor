var esp32StatFreq = 0;


var ctrlButton = document.getElementById('powerCtrl');
var espSettingPage = document.getElementById('espSettings');
var autoPwrSec = document.getElementById('autoPowerCtrl');
var divl2 = document.getElementById('divLine2');
ctrlButton.style.display = 'none';
espSettingPage.style.display = 'none';
autoPwrSec.style.display='none';
divl2.style.display='none';

var host = window.location.hostname;
// 如果你的服务器端口不是默认的 HTTP 端口（80），你也可以获取端口号
var port = window.location.port;
// 创建一个完整的 URL，包括协议、主机和端口
var serverURL = window.location.protocol + "//" + host;
if (port) {
    serverURL += ":" + port;
}

console.log(serverURL);

// 假设这是您的 CPU 和内存使用率数据
document.getElementById('toggleManagement').addEventListener('click', function (event) {
    event.preventDefault(); // 阻止默认的超链接行为

    // 获取需要切换显示状态的元素
    var sysinfoPanel = document.getElementById('contain1');
    var ctrlButton = document.getElementById('powerCtrl');
    var espSettingPage = document.getElementById('espSettings');
    var getInfoView = document.getElementById('genInfo');
    var autoPwrSec = document.getElementById('autoPowerCtrl');
    var divl2 = document.getElementById('divLine2');

    // 切换元素的显示状态
    sysinfoPanel.style.display = 'none';
    ctrlButton.style.display = 'block';
    espSettingPage.style.display = 'none';
    autoPwrSec.style.display='none';
    divl2.style.display='none';

    //getInfoView.style.display='none';

});

document.getElementById('toggleOverview').addEventListener('click', function (event) {
    event.preventDefault(); // 阻止默认的超链接行为

    var sysinfoPanel = document.getElementById('contain1');
    var ctrlButton = document.getElementById('powerCtrl');
    var getInfoView = document.getElementById('genInfo');
    var espSettingPage = document.getElementById('espSettings');
    var autoPwrSec = document.getElementById('autoPowerCtrl');
    var divl2 = document.getElementById('divLine2');

    // 切换元素的显示状态
    sysinfoPanel.style.display = 'grid';
    ctrlButton.style.display = 'none';
    espSettingPage.style.display = 'none';
    autoPwrSec.style.display='none';
    divl2.style.display='block';

    //getInfoView.style.display='block';

});

document.getElementById('toggleSetting').addEventListener('click', function (event) {
    event.preventDefault(); // 阻止默认的超链接行为

    var sysinfoPanel = document.getElementById('contain1');
    var ctrlButton = document.getElementById('powerCtrl');
    var getInfoView = document.getElementById('genInfo');
    var espSettingPage = document.getElementById('espSettings');
    var autoPwrSec = document.getElementById('autoPowerCtrl');
    var divl2 = document.getElementById('divLine2');

    // 切换元素的显示状态
    sysinfoPanel.style.display = 'none';
    ctrlButton.style.display = 'none';
    espSettingPage.style.display = 'block';
    autoPwrSec.style.display='block';
    divl2.style.display='block';

    //timeSetStatusChange(true);

    //getInfoView.style.display='block';

});

function timeSetStatusChange(status){

    var enterBox=document.getElementsByClassName("userEnter");
    for(var i of enterBox){
        i.disabled=status;
    }
}

function setNetworkInfo(apssid,appwd,ssid,pwd)
{
    var apssidText=document.getElementById('apSSID');
    var appwdText=document.getElementById('apPWD');
    var ssidText=document.getElementById('staSSID');
    var pwdText=document.getElementById('staPWD');
    apssidText.textContent=apssid;
    appwdText.textContent=appwd;
    ssidText.textContent=ssid;
    pwdText.textContent=pwd;
}

var cpuData = {
    labels: [, , , , , , , , ,],
    datasets: [{
        label: 'CPU Usage',
        backgroundColor: '	#F08080',
        borderColor: '#FF0000',
        borderWidth: 1,
        data: [, , , , , , , , ,]
    }]
};

var memoryData = {
    labels: [, , , , , , , , ,],
    datasets: [{
        label: 'Memory Usage',
        backgroundColor: '#9370DB',
        borderColor: '#4B0082',
        borderWidth: 1,
        data: [, , , , , , , , ,]
    }]
};

var netInData = {
    labels: [, , , , , , , , ,],
    datasets: [{
        label: 'Network In',
        backgroundColor: "#90EE90",
        borderColor: "#32CD32",
        borderWidth: 1,
        data: [, , , , , , , , ,]
    }]
};

var netOutData = {
    labels: [, , , , , , , , ,],
    datasets: [{
        label: 'Network Out',
        backgroundColor: '#00BFFF',
        borderColor: '#0000CD',
        borderWidth: 1,
        data: [, , , , , , , , ,]
    }]
};

function createChartcm(ctx, data) {
    return new Chart(ctx, {
        type: 'line',
        data: data,
        options: {
            scales: {
                y: {
                    min: 0,
                    max: 100
                },
                x: {
                    grid: {
                        display: false
                    }
                }
            }
        },
    });
}

function createChartnet(ctx, data) {
    return new Chart(ctx, {
        type: 'line',
        data: data,
        options: {
            scales: {
                y: {
                    min: 0
                },
                x: {
                    grid: {
                        display: false
                    }
                }
            }
        },
    });
}

// 创建 CPU 使用率图表
var cpuCtx = document.getElementById('cpuChart').getContext('2d');
var netInCtx = document.getElementById('netInChart').getContext('2d');
var netOutCtx = document.getElementById('netOutChart').getContext('2d');
// 创建内存使用率图表
var memoryCtx = document.getElementById('memoryChart').getContext('2d');

var cpuChart = createChartcm(cpuCtx, cpuData);
var memoryChart = createChartcm(memoryCtx, memoryData);
var netInChart = createChartnet(netInCtx, netInData);
var netOutChart = createChartnet(netOutCtx, netOutData);

var espIpText = document.getElementById("espIp");
espIpText.textContent = serverURL;

function updateGraph(nCpuDat, nMemDat, nNiDat, nNoDat, init) {
    if (init) {
        cpuChart.data.labels = [, , , , , , , , ,];
        memoryChart.data.labels = [, , , , , , , , ,];
        cpuChart.data.datasets[0].data = [, , , , , , , , ,];
        memoryChart.data.datasets[0].data = [, , , , , , , , ,];
        netInChart.data.labels = [, , , , , , , , ,];
        netOutChart.data.labels = [, , , , , , , , ,];
        netInChart.data.datasets[0].data = [, , , , , , , , ,];
        netOutChart.data.datasets[0].data = [, , , , , , , , ,];
        return;
    }

    cpuChart.options.animation = false;
    memoryChart.options.animation = false;
    netInChart.options.animation = 0;
    netOutChart.options.animation = 0;

    var currentTime = new Date();
    var hours = currentTime.getHours().toString().padStart(2, '0');
    var minutes = currentTime.getMinutes().toString().padStart(2, '0');
    var seconds = currentTime.getSeconds().toString().padStart(2, '0');

    // 将时、分、秒拼接成时：分：秒的格式
    var timeString = hours + ':' + minutes + ':' + seconds;
    // 生成新数据点
    var newCpuData = nCpuDat
    var newMemoryData = nMemDat
    var newNiData = nNiDat
    var newNoData = nNoDat

    // 将新数据点添加到图表的末尾
    cpuChart.data.labels.push(timeString);
    memoryChart.data.labels.push(timeString);
    cpuChart.data.datasets[0].data.push(newCpuData);
    memoryChart.data.datasets[0].data.push(newMemoryData);

    netInChart.data.labels.push(timeString);
    netOutChart.data.labels.push(timeString);
    netInChart.data.datasets[0].data.push(newNiData);
    netOutChart.data.datasets[0].data.push(newNoData);

    // 限制图表显示的数据点数量，避免无限增长
    if (cpuChart.data.labels.length > 10) {
        cpuChart.data.labels.shift(); // 删除最旧的数据点
        memoryChart.data.labels.shift(); // 删除最旧的数据点
        cpuChart.data.datasets[0].data.shift(); // 删除最旧的数据点
        memoryChart.data.datasets[0].data.shift(); // 删除最旧的数据点

        netInChart.data.labels.shift(); // 删除最旧的数据点
        netOutChart.data.labels.shift(); // 删除最旧的数据点
        netInChart.data.datasets[0].data.shift(); // 删除最旧的数据点
        netOutChart.data.datasets[0].data.shift(); // 删除最旧的数据点
    }

    // 更新图表
    cpuChart.update();
    memoryChart.update();
    netInChart.update();
    netOutChart.update();
};

var autoInitConn;
var getDisDataIntv;

function getDispData() {
    var xhr = new XMLHttpRequest();
    xhr.open("GET", serverURL + "/data", true);
    xhr.onload = function () {
        if (xhr.status == 200) {
            console.log(xhr.responseText);
            var statList = this.responseText.split(",");
            if (parseInt(statList[0]) != esp32StatFreq) {
                if (parseInt(statList[0]) == -1) {
                    clearInterval(getDisDataIntv);
                    document.getElementById("statusText").textContent = "Disconnected / Powered off";
                    timeSetStatusChange(true);
                    document.getElementById("sysName").textContent = "NaN";
                    autoInitConn = setInterval(initConn, 1000);
                }
                else {
                    clearInterval(getDisDataIntv);
                    esp32StatFreq = parseInt(statList[0]);
                    getDisDataIntv = setInterval(getDispData, esp32StatFreq * 1000);
                }
            }
            else {
                var mfName = document.getElementById("sysName");
                mfName.textContent = statList[1];
                updateGraph(statList[2], statList[3], statList[4], statList[5], 0);
            }
        } else {
            console.error('Request failed: ' + xhr.status);
        }
    };
    xhr.send();
}

function initConn() {
    var xhr = new XMLHttpRequest();
    xhr.open("GET", serverURL + "/init", true);
    xhr.onload = function () {
        if (xhr.status == 200) {
            // 请求成功，处理返回的数据
            console.log(xhr.responseText);
            // 在这里解析数据并进行相应的操作
            var initList = this.responseText.split(",");
            if (initList[0] != "-1") {
                esp32StatFreq = parseInt(initList[0]);
                setNetworkInfo(initList[1],initList[2],initList[3],initList[4]);
                clearInterval(autoInitConn);
                document.getElementById("statusText").textContent = "Connected";
                updateGraph(-1, -1, -1, -1, 1);
                getDisDataIntv = setInterval(getDispData, esp32StatFreq * 1000);
                timeSetStatusChange(false);
            }
        } else {
            // 请求失败，输出错误信息
            console.error('Request failed: ' + xhr.status);
        }
    };
    xhr.send();
}

autoInitConn = setInterval(initConn, 1000);

document.getElementById('poweronButton').addEventListener('click', function () {
    var xhr = new XMLHttpRequest();
    xhr.open('GET', serverURL + '/poweron', true);
    xhr.onreadystatechange = function () {
        if (xhr.readyState === XMLHttpRequest.DONE) {
            if (xhr.status === 200) {
                console.log('Request succeeded:', xhr.responseText);
            } else {
                console.error('Request failed:', xhr.status);
            }
        }
    };
    xhr.send();
});

document.getElementById('poweroffButton').addEventListener('click', function () {
    var xhr = new XMLHttpRequest();
    xhr.open('GET', serverURL + '/poweroff', true);
    xhr.onreadystatechange = function () {
        if (xhr.readyState === XMLHttpRequest.DONE) {
            if (xhr.status === 200) {
                console.log('Request succeeded:', xhr.responseText);
            } else {
                console.error('Request failed:', xhr.status);
            }
        }
    };
    xhr.send();
});

document.getElementById('bootTimeSet').addEventListener('click', function () {
    var xhr = new XMLHttpRequest();
    var header="onTime";
    var bootHr=document.getElementById('bootHr').value;
    var bootMin=document.getElementById('bootMin').value;
    var bootSec=document.getElementById('bootSec').value;
    var data=header+","+bootHr+","+bootMin+","+bootSec;
    if(!(parseFloat(bootHr)%1==0)||!(parseFloat(bootMin)%1==0)||!(parseFloat(bootSec)%1==0)||parseInt(bootHr)<0 || parseInt(bootHr)>25 || parseInt(bootMin)<0 || parseInt(bootMin)>61 || parseInt(bootSec)<0 || parseInt(bootSec)>61)
    {
        alert("Invalid Time!");
    }
    else
    {
    	xhr.open('POST', serverURL, true);
    	xhr.setRequestHeader("Content-Type", "text/plain"); // 设置请求头，指定发送的数据类型为纯文本
    	xhr.onreadystatechange = function () { // 监听状态改变
    	    if (xhr.readyState === 4 && xhr.status === 200) { // 如果请求完成且成功
    	        console.log(xhr.responseText); // 输出服务器响应
    	    }
    	};
    	xhr.send("data="+data); // 发送数据
    }
});

document.getElementById('offTimeSet').addEventListener('click', function () {
    var xhr = new XMLHttpRequest();
    var header="offTime";
    var offHr=document.getElementById('offHr').value;
    var offMin=document.getElementById('offMin').value;
    var offSec=document.getElementById('offSec').value;
    var data=header+","+offHr+","+offMin+","+offSec;
    if(!(parseFloat(offHr)%1==0)||!(parseFloat(offMin)%1==0)||!(parseFloat(offSec)%1==0)||parseInt(offHr)<0 || parseInt(offHr)>25 || parseInt(offMin)<0 || parseInt(offMin)>61 || parseInt(offSec)<0 || parseInt(offSec)>61)
    {
        alert("Invalid Time!");
    }
    else
    {
    	xhr.open('POST', serverURL, true);
    	xhr.setRequestHeader("Content-Type", "text/plain"); // 设置请求头，指定发送的数据类型为纯文本
    	xhr.onreadystatechange = function () { // 监听状态改变
    	    if (xhr.readyState === 4 && xhr.status === 200) { // 如果请求完成且成功
    	        console.log(xhr.responseText); // 输出服务器响应
    	    }
    	};
    	xhr.send("data="+data); // 发送数据
    }
});