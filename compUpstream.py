import serial
import serial.tools.list_ports
import time
import platform
from time import sleep
import sys
import threading
import psutil
import tkinter
import tkinter.ttk
import re
from tkinter import messagebox

ser=None
getStatFreq=2
newProceStartEvent=threading.Event()
lastByteRecv=0
lastByteSent=0
lastRectime=time.time()
firstRun=True
ostype=platform.system()

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

def connVerify():
    #verification
    try:
        hostname=platform.node()
        verifyMsg="a114514"
        ser.write(verifyMsg.encode('utf-8'))
        returnMsg=ser.readline().decode('utf-8').rstrip()
        if(returnMsg=="good"):
            ser.write(hostname.encode('utf-8'))
            print('Board Ready')
            sleep(1)
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
            return
        else:
            firstRun=False
            newProceStartEvent.clear()
    sysStat=getSystemStatus()
    print(sysStat)
    #print(time.time()-lastRectime)
    #lastRectime=time.time()
    
    statStr = ",".join(map(str, sysStat))
    statBytes = statStr.encode()
    ser.write(statBytes)

    infoCollectTimer.start()

        

def applyButtonOnclick():
    newProceStartEvent.set()
    sleep(1)
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
        connVerify()
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
    window.geometry('220x140')
else:
    window.geometry('210x130')
window.resizable(False,False)

def onWindowClose():
    window.destroy()
    newProceStartEvent.set()
    stopCode="upstreamStop"
    ser.write(stopCode.encode())

portSelector=tkinter.ttk.Combobox(window,values=availPorts)

infoRetrieveFreq=tkinter.ttk.Entry(window)

infoRetrieveFreq.insert(0,'input freqency')
infoRetrieveFreq.config(foreground='grey')
infoRetrieveFreq.place(x=0,y=30)
def onFocusEntry(event):
    infoRetrieveFreq.delete(0,tkinter.END)
    infoRetrieveFreq.config(foreground='black')
def onFocusOut(event):
    infoRetrieveFreq.delete(0,tkinter.END)
    infoRetrieveFreq.insert(0,'input freqency')
    infoRetrieveFreq.config(foreground='grey')

infoRetrieveFreq.bind('<FocusIn>',onFocusEntry)
infoRetrieveFreq.bind('<FocusOut>',onFocusOut)

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

textSelectPort.grid(row=0,column=0,sticky='w',columnspan=2)
portSelector.grid(row=1,column=0,sticky='w',columnspan=2)
textSelectFreq.grid(row=2,column=0,sticky='w',columnspan=2)
infoRetrieveFreq.grid(row=3,column=0,sticky='w',columnspan=2)
buttonApply.grid(row=4,column=0,sticky='w')
buttonSaveCfg.grid(row=4,column=1,sticky='w')

#textSelectPort.pack()
#portSelector.pack()
#textSelectFreq.pack()
#infoRetrieveFreq.pack()
#buttonApply.pack()
#buttonSaveCfg.pack()

window.mainloop()