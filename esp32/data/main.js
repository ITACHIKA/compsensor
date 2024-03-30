var ctrlButton=document.getElementById('powerCtrl');
ctrlButton.style.display='none';

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
document.getElementById('toggleManagement').addEventListener('click', function(event) {
    event.preventDefault(); // 阻止默认的超链接行为

    // 获取需要切换显示状态的元素
    var sysinfoPanel = document.getElementById('contain1');
    var ctrlButton=document.getElementById('powerCtrl');
    var getInfoView=document.getElementById('genInfo');

    // 切换元素的显示状态
    sysinfoPanel.style.display='none';
    ctrlButton.style.display='block';
    //getInfoView.style.display='none';

});

document.getElementById('toggleOverview').addEventListener('click', function(event) {
    event.preventDefault(); // 阻止默认的超链接行为

    var sysinfoPanel = document.getElementById('contain1');
    var ctrlButton=document.getElementById('powerCtrl');
    var getInfoView=document.getElementById('genInfo');

    // 切换元素的显示状态
    sysinfoPanel.style.display='grid';
    ctrlButton.style.display='none';
    //getInfoView.style.display='block';

});

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

function updateGraph (nCpuDat,nMemDat,nNiDat,nNoDat) {
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

var autoInitConn=setInterval(function initConn(){
    var xhr=new XMLHttpRequest();
    xhr.open("GET", serverURL+"init", true);
    xhr.onload = function() {
        if (xhr.status == 200) {
          // 请求成功，处理返回的数据
          console.log(xhr.responseText);
          
          // 在这里解析数据并进行相应的操作
        } else {
          // 请求失败，输出错误信息
          console.error('Request failed: ' + xhr.status);
        }
      };
    // xhr.send();      
},1000)