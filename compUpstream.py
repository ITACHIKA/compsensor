import serial
import serial.tools.list_ports
import time
import platform
from time import sleep
import json
import threading
import psutil
import tkinter
import tkinter.ttk
import re
from tkinter import messagebox
from datetime import datetime
import bluetooth

ser=None
getStatFreq=2
newProceStartEvent=threading.Event()
lastByteRecv=0
lastByteSent=0
lastRectime=time.time()
firstRun=True
ostype=platform.system()
btAddr=''
btGood=True

print(ostype)

def getSystemStatus():
    global lastByteRecv,lastByteSent
    if(ostype=="Windows"):
        cpuUsage=psutil.cpu_percent(0.1)
    else:
        cpuUsage=psutil.cpu_percent()
    memUsage=psutil.virtual_memory().percent
    netTotIn=psutil.net_io_counters().bytes_recv
    netTotOut=psutil.net_io_counters().bytes_sent
    netInMbps=round(((netTotIn-lastByteRecv)/1024/1024/getStatFreq)*8,2)
    netOutMbps=round(((netTotOut-lastByteSent)/1024/1024/getStatFreq)*8,2)
    lastByteRecv=netTotIn
    lastByteSent=netTotOut
    return [cpuUsage,memUsage,netInMbps,netOutMbps]

def initArduino():
    #verification
    try:
        hostname=platform.node()
        systime=datetime.now()

        hour=systime.hour
        minute=systime.minute
        sec=systime.second
        year=systime.year
        month=systime.month
        date=systime.day
        weekday=systime.weekday()

        initStr = ",".join(map(str,[hostname,hour,minute,sec,year,month,date,weekday]))
        initStr = "init,"+initStr+"\n"
        print(initStr)
        initBytes = initStr.encode("utf-8")
        ser.write(initBytes)

    except Exception as e:
        print(e)

def infoCollectDeliv():

    global ser,firstRun,lastRectime

    infoCollectTimer=threading.Timer(getStatFreq,infoCollectDeliv)
    if(newProceStartEvent.is_set()):
        if(not firstRun):
            print("reset!")
            infoCollectTimer.cancel()
            newProceStartEvent.clear()
            ser.close()
            return
        else:
            firstRun=False
            newProceStartEvent.clear()
    sysStat=getSystemStatus()
    #print(time.time()-lastRectime)
    #lastRectime=time.time()

    statStr = ",".join(map(str, sysStat))
    statStr = "data,"+statStr+"\n"
    print(statStr)
    statBytes = statStr.encode('utf-8')
    ser.write(statBytes)

    infoCollectTimer.start()

def applyButtonOnclick():
    if(not btGood):
        messagebox.showerror("Error","BT is not ready")
        return
    newProceStartEvent.set()
    sleep(0.5)
    if(portSelector.get()!=''):
        if(infoRetrieveFreq.get()!="input freqency" and re.match(r'^\d+(\.\d+)?$',infoRetrieveFreq.get()) is not None):
            global getStatFreq
            getStatFreq=float(infoRetrieveFreq.get())
            print("freq:"+str(getStatFreq))
        #connect to serial
        global ser
        try: 
            ser=serial.Serial(port=portSelector.get(),baudrate=9600,timeout=2)
            print(portSelector.get())
        except Exception as e:
            messagebox.showerror("error",e)

        sleep(2)
        initArduino()
        sleep(0.5) #won't work without this waiting

        if(ostype=="Windows"):
            getStatFreq-=0.15
        #call stat collector
        infoCollectDeliv()

def writeCfg():
    btMac=getBtAddr.get()
    port=portSelector.get()
    freq=re.match(r'^\d+(\.\d+)?$',infoRetrieveFreq.get()).group()
    if(freq==None):
        freq=getStatFreq
    with open("sensorconfig",'w') as cfgfile:
        cfgfile.writelines([btMac+"\n"+port+"\n",str(freq)+"\n"])

def getBtOnClick():
    global btAddr
    btAdapAddr=bluetooth.read_local_bdaddr()
    btAddr=getBtAddr.get()
    btPattern = re.compile(r'^([0-9A-Fa-f]{2}[:-]){5}([0-9A-Fa-f]{2})$')
    addrGood=btPattern.match(btAddr)
    devices = bluetooth.discover_devices(lookup_names=True, lookup_class=True, device_id=-1, duration=4)
    for addr, name, _ in devices:
        print(f"Device Address: {addr}")
        print(f"Device Name: {name}")
    if(addrGood):
        try:
            btsock=bluetooth.BluetoothSocket(bluetooth.RFCOMM)
            channel=3
            btsock.settimeout(5)
            btsock.connect((btAddr,channel))
            print("Pair found. Remove.")
        except Exception as e:
            print(e)
    else:
        messagebox.showerror("Error","Invalid Mac format")

fullAvailPorts=list(serial.tools.list_ports.comports())
availPorts=[]
for port in fullAvailPorts:
    availPorts.append(port.device)

window=tkinter.Tk()
window.title("Arduino Sensor")
if(ostype=="Windows"):
    window.geometry('220x200')
else:
    window.geometry('210x190')
window.resizable(False,False)

def onWindowClose():
    window.destroy()
    newProceStartEvent.set()
    shutData="halt"
    ser.write(shutData.encode('utf-8'))

portSelector=tkinter.ttk.Combobox(window,values=availPorts)

infoRetrieveFreq=tkinter.ttk.Entry(window)

infoRetrieveFreq.insert(0,'input freqency')
infoRetrieveFreq.config(foreground='grey')

def onFocusEntry(event):
    infoRetrieveFreq.delete(0,tkinter.END)
    infoRetrieveFreq.config(foreground='black')
def onFocusOut(event):
    infoRetrieveFreq.delete(0,tkinter.END)
    infoRetrieveFreq.insert(0,'input freqency')
    infoRetrieveFreq.config(foreground='grey')

getBtAddr=tkinter.ttk.Entry(window)
getBtAddr.config(foreground='grey')

def onFocusEntryBt(event):
    getBtAddr.config(foreground='black')
def onFocusOutBt(event):
    infoRetrieveFreq.config(foreground='grey')

infoRetrieveFreq.bind('<FocusIn>',onFocusEntry)
infoRetrieveFreq.bind('<FocusOut>',onFocusOut)
 
getBtAddr.bind('<FocusIn>',onFocusEntryBt)
getBtAddr.bind('<FocusOut>',onFocusOutBt)

try:
    with open("sensorconfig",'r') as cfgfile:
        cfg=cfgfile.readlines()
        getBtAddr.insert(0,cfg[0].strip())
        portSelector.set(cfg[1].strip())
        infoRetrieveFreq.delete(0,tkinter.END)
        infoRetrieveFreq.insert(0,float(cfg[2].strip()))

except FileNotFoundError:
    messagebox.showwarning("Warning","Cfg file not found")


textSelectPort=tkinter.Label(window,text="Select Arduino port")
textSelectFreq=tkinter.Label(window,text="refresh Freq, default 2s")
buttonApply=tkinter.Button(window,text="Apply",command=applyButtonOnclick)
buttonSaveCfg=tkinter.Button(window,text="save cfg",command=writeCfg)
textBtAddr=tkinter.Label(window,text="BT module MAC")
buttonGetBtMac=tkinter.Button(window,text="BT",command=getBtOnClick)

window.protocol("WM_DELETE_WINDOW", onWindowClose)

textBtAddr.grid(row=0,column=0,sticky='w',columnspan=3)
getBtAddr.grid(row=1,column=0,sticky='w',columnspan=3)
textSelectPort.grid(row=2,column=0,sticky='w',columnspan=3)
portSelector.grid(row=3,column=0,sticky='w',columnspan=3)
textSelectFreq.grid(row=4,column=0,sticky='w',columnspan=3)
infoRetrieveFreq.grid(row=5,column=0,sticky='w',columnspan=3)
buttonGetBtMac.grid(row=6,column=0,sticky='w')
buttonApply.grid(row=6,column=1,sticky='w')
buttonSaveCfg.grid(row=6,column=2,sticky='w')

window.mainloop()