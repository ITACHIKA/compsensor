import serial
import serial.tools.list_ports
import time
import platform
from time import sleep
import threading
import psutil
import tkinter
import tkinter.ttk
import re
from tkinter import messagebox
from datetime import datetime

ser=None
getStatFreq=2
newProceStartEvent=threading.Event()
lastByteRecv=0
lastByteSent=0
lastRectime=time.time()
firstRun=True
ostype=platform.system()
hostname=""

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
    global hostname
    #verification
    try:
        hostname=platform.node()
        systime=datetime.now()

        hour=systime.hour
        minute=systime.minute
        sec=systime.second

        initStr = ",".join(map(str,[hour,minute,sec]))
        initStr = "init,"+initStr+"\n"
        print(initStr)
        initBytes = initStr.encode("utf-8")
        ser.write(initBytes)

    except Exception as e:
        print(e)

def infoCollectDeliv():

    try:
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
        statStr = "data,"+hostname+","+statStr+"\n"
        print(statStr)
        statBytes = statStr.encode('utf-8')
        ser.write(statBytes)

    except Exception as e:
        firstRun=True
        print(e)


    infoCollectTimer.start()

def applyButtonOnclick():
    global getStatFreq
    newProceStartEvent.set()
    sleep(getStatFreq)
    if(portSelector.get()!=''):
        if(infoRetrieveFreq.get()!="input freqency" and re.match(r'^\d+(\.\d+)?$',infoRetrieveFreq.get()) is not None):
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
    port=portSelector.get()
    freq=re.match(r'^\d+(\.\d+)?$',infoRetrieveFreq.get()).group()
    if(freq==None):
        freq=getStatFreq
    with open("sensorconfig",'w') as cfgfile:
        cfgfile.writelines([port+"\n",str(freq)+"\n"])

fullAvailPorts=list(serial.tools.list_ports.comports())
availPorts=[]
for port in fullAvailPorts:
    availPorts.append(port.device)

window=tkinter.Tk()
window.title("Arduino Sensor")
if(ostype=="Windows"):
    window.geometry('190x140')
else:
    window.geometry('210x140')
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
        portSelector.set(cfg[0].strip())
        infoRetrieveFreq.delete(0,tkinter.END)
        infoRetrieveFreq.insert(0,float(cfg[1].strip()))

except FileNotFoundError:
    messagebox.showwarning("Warning","Cfg file not found")


textSelectPort=tkinter.Label(window,text="Select Arduino port")
textSelectFreq=tkinter.Label(window,text="refresh Freq, default 2s")
buttonApply=tkinter.Button(window,text="Apply",command=applyButtonOnclick)
buttonSaveCfg=tkinter.Button(window,text="save cfg",command=writeCfg)

window.protocol("WM_DELETE_WINDOW", onWindowClose)

textSelectPort.grid(row=1,column=0,sticky='w',columnspan=2)
portSelector.grid(row=2,column=0,sticky='w',columnspan=2)
textSelectFreq.grid(row=3,column=0,sticky='w',columnspan=2)
infoRetrieveFreq.grid(row=4,column=0,sticky='w',columnspan=2)
buttonApply.grid(row=5,column=0,sticky='w')
buttonSaveCfg.grid(row=5,column=1,sticky='w')

window.mainloop()