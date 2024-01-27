from setuptools import setup

APP=['compUpstream.py']
DATA_FILES = []
OPTIONS = {
    'argv_emulation': True,
    'packages': ['serial', 'serial.tools.list_ports', 'psutil', 'tkinter','scipy'],
    #'includes': ['scipy.linalg','PyQt5.QtXml','gi.repository.AppIndicator3','gi.repository.GModule','PySide2.QtScriptTools','PyQt6.QtBluetooth'],
}

setup(
    app=APP,
    data_files=DATA_FILES,
    options={'py2app': OPTIONS},
    setup_requires=['py2app'],
)