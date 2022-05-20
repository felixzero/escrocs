import requests
import tkinter.messagebox
from tkinter import *
from tkinter import ttk
import numpy as np
import platform
import subprocess

#headers = {'Content-Type': 'application/x-www-form-urlencoded'}

DEFAULT_HOST = "192.168.4.1"

def ping(host):
    param = '-n' if platform.system().lower()=='windows' else '-c'
    command = ['ping', param, '1', host]
    # Additionnal checks because windows is stupid -- TODO: translation for en-en locale.
    retval = 1
    try:
        retval = 'Impossible' in str(subprocess.check_output(command))
    except subprocess.CalledProcessError:
        retval = 2
    
    return retval == 0  


def get_Pose():
    if not ping(selectedHost.get()):
        read_pos_X.set("N/A")
        read_pos_Y.set("N/A")
        read_pos_T.set("N/A")
        return;
    r = requests.post("http://" + selectedHost.get() + "/" + 'action/get_pose', data = "{}")
    print("Get_pose")
    print(r)
    print(r.text)
    
    ansJSON = r.json()
    angle = np.degrees(ansJSON["theta"])
    read_pos_X.set(f'{ansJSON["x"]: .2f}')
    read_pos_Y.set(f'{ansJSON["y"]: .2f}')
    read_pos_T.set(f'{angle: .2f}°')
    write_pos_X.set('')
    write_pos_Y.set('')
    write_pos_T.set('')

def overwrite_Pose():
    if not ping(selectedHost.get()):
        can1.itemconfig(ping_oval, fill = "red")
        tkinter.messagebox.showerror("Error","ESCROBOT @"+ selectedHost.get() + " is offline.")
        return;
    r = requests.post("http://" + selectedHost.get() + "/" + 'action/overwrite_pose', data = '{"x": ' + write_pos_X.get() + ', "y": ' + write_pos_Y.get() + ', "theta": ' + str(np.radians(float(write_pos_T.get()))) + '}')
    print("overwrite_Pose")
    print(r)
    print(r.text)
    get_Pose()

def set_Pose():
    if not ping(selectedHost.get()):
        can1.itemconfig(ping_oval, fill = "red")
        tkinter.messagebox.showerror("Error","ESCROBOT @"+ selectedHost.get() + " is offline.")
        return;
    r = requests.post("http://" + selectedHost.get() + "/" + 'action/set_pose', data = '{"x": ' + write_pos_X.get() + ', "y": ' + write_pos_Y.get() + ', "theta": ' + str(np.radians(float(write_pos_T.get()))) + ', "perform_detection": false}')
    print("Set_pose")
    print(r)
    print(r.text)

def move_stepper():
    if not ping(selectedHost.get()):
        can1.itemconfig(ping_oval, fill = "red")
        tkinter.messagebox.showerror("Error","ESCROBOT @"+ selectedHost.get() + " is offline.")
        return;
    r = requests.post("http://" + selectedHost.get() + "/" + 'action/move_stepper', data = '{"channel": ' + channel_selected.get() + ', "target": ' + target_selected.get() + ', "speed": ' + speed_selected.get() +'}')
    print("move_stepper")
    print(r)
    print(r.text)

def check_liveliness():
    if not ping(selectedHost.get()):
        can1.itemconfig(ping_oval, fill = "red")
        tkinter.messagebox.showerror("Error","ESCROBOT @"+ selectedHost.get() + " is offline.")
    else:
        can1.itemconfig(ping_oval, fill = "green")





connected = ping(DEFAULT_HOST)
if connected:
    print("Escrobot is online.")
    #r2 = requests.post('http://192.168.4.1/action/move_stepper', data = '{"channel" : 0, "target": 0, "speed": 0.1}')
    #print(r2)
else:
    print("Escrobot is offline.")


win=tkinter.Tk(className="2022 - ESCROCS Control Panel 1")
win.title="2022 - ESCROCS Control Panel"
win.geometry('800x600')

win.columnconfigure(0, pad=8, minsize = 300)
win.columnconfigure(1, pad=8)
win.columnconfigure(2, pad=8)
win.columnconfigure(3, pad=8,weight=1)

win.rowconfigure(0, pad=10)
win.rowconfigure(1, pad=10)
win.rowconfigure(2, pad=10)
win.rowconfigure(3, pad=10, weight=1)
win.rowconfigure(4, pad=10)

#------ Connection status--------------------------------
connectPane = ttk.Frame(win, relief=GROOVE)
connectPane['padding'] = (10,5,10,5)
connectPane.columnconfigure(0, pad=8)
connectPane.columnconfigure(1, pad=8)
connectPane.columnconfigure(2, pad=8)
can1 = Canvas(connectPane, width=24, height=24)
fill = 'red'
if(connected):
    fill = 'green'
ping_oval = can1.create_oval(4, 4, 20, 20,fill=fill)
can1.grid(row=0,column=0)

selectedHost = tkinter.StringVar()
selectedHost.set(DEFAULT_HOST)
hostnameEntry = Entry(connectPane, exportselection=0, textvariable=selectedHost)
#hostnameEntry.insert(0, selectedHost)
hostnameEntry.grid(row=0,column=1);

btn=Button(connectPane,text="Ping", command=lambda: check_liveliness())
btn.grid(row=0,column=2);

connectPane.grid(row=0,column=0, sticky=W+E)
#--------------------------------------------------------

#------ Reported position -------------------------------
posPane = ttk.Frame(win, relief=GROOVE)
posPane['padding'] = (10,5,10,5)

read_pos_X = tkinter.StringVar();
read_pos_X.set("---")
read_pos_Y = tkinter.StringVar();
read_pos_Y.set("---")
read_pos_T = tkinter.StringVar();
read_pos_T.set("---")
write_pos_X = tkinter.StringVar();
write_pos_Y = tkinter.StringVar();
write_pos_T = tkinter.StringVar();

posPane.columnconfigure(0, pad=8)
posPane.columnconfigure(1, weight = 1)
posPane.columnconfigure(2, pad=8, weight=1)
posPane.rowconfigure(0, pad=10)
posPane.rowconfigure(1, pad=10)
posPane.rowconfigure(2, pad=10)
posPane.rowconfigure(3, pad=10)
posPane.rowconfigure(4, pad=10)
posPane.rowconfigure(5, pad=10)

btn1=Button(posPane,text="Locate", command=lambda: get_Pose())
btn1.grid(row=0, column=0, columnspan=3, sticky=W+E);

lbl1 = Label(posPane, text="X: ", width=6)
lbl1.grid(row=1,column=0)
lbl2 = Label(posPane, text="Y: ", width=6)
lbl2.grid(row=2,column=0)
lbl3 = Label(posPane, text="Th: ", width=6)
lbl3.grid(row=3,column=0)

lblX = Label(posPane, textvariable=read_pos_X)
lblX.grid(row=1,column=1)
lblY = Label(posPane, textvariable=read_pos_Y)
lblY.grid(row=2,column=1)
lblT = Label(posPane, textvariable=read_pos_T)
lblT.grid(row=3,column=1)

entry1 = Entry(posPane, textvariable=write_pos_X)
entry1.grid(row=1,column=2)
entry1 = Entry(posPane, textvariable=write_pos_Y)
entry1.grid(row=2,column=2)
entry1 = Entry(posPane, textvariable=write_pos_T)
entry1.grid(row=3,column=2)

btnO=Button(posPane,text="Overwrite", command=lambda: overwrite_Pose())
btnO.grid(row=4, column=0, columnspan=3, sticky=W+E);

btnS=Button(posPane,text="Send", command=lambda: set_Pose())
btnS.grid(row=5, column=0, columnspan=3, sticky=W+E);

posPane.grid(row=1,column=0, sticky=W+E)
#--------------------------------------------------------

#------ Stepper controls --------------------------------
stepPane = ttk.Frame(win, relief=GROOVE)
stepPane['padding'] = (10,5,10,5)
stepPane.rowconfigure(0, pad=10)
stepPane.rowconfigure(1, pad=10)
stepPane.rowconfigure(2, pad=10)
stepPane.rowconfigure(3, pad=10)

lblCh = Label(stepPane, text="Channel: ")
lblCh.grid(row=0,column=0)
lblTa = Label(stepPane, text="Target: ")
lblTa.grid(row=1,column=0)
lblSp = Label(stepPane, text="Speed: ")
lblSp.grid(row=2,column=0)

channel_selected = tkinter.StringVar()
chcombobox = ttk.Combobox(stepPane, textvariable=channel_selected)
chcombobox['values'] = ('0', '1', '2', '3')
chcombobox.grid(row=0, column=1)

target_selected = tkinter.StringVar()
target_selected.set('0')
entryTarget = Entry(stepPane, textvariable=target_selected)
entryTarget.grid(row=1, column=1)

speed_selected = tkinter.StringVar()
speed_selected.set('0.15')
entrySpeed = Entry(stepPane, textvariable=speed_selected)
entrySpeed.grid(row=2, column=1)

btnM=Button(stepPane,text="Move stepper", command=lambda: move_stepper())
btnM.grid(row=3, column=0, columnspan=2, sticky=W+E);

stepPane.grid(row=1, column=1, sticky=W+E)
#--------------------------------------------------------

terrain=Canvas(win, bg='white')
terrain.grid(row=3,column=3);




win.mainloop()
