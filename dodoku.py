import sys
import time
import numpy as np
try:
    import serial
except:
    import pip
    pip.main(['install','pyserial'])
    import serial
from serial.tools import list_ports
        
class MDCLab(object):

    def __init__(self, port=None, baud=9600):
        if (sys.platform == 'darwin') and not port:
            port = '/dev/cu.wchusbserial1410'
        else:
            print('Silahkan cek port serial anda!')
        print('Koneksi serial . . . ')
        self.sp = serial.Serial(port=port, baudrate=baud, timeout=2)
        self.sp.flushInput()
        self.sp.flushOutput()
        time.sleep(3)
        print('Terhubung di port: ' + port)
        
    def stop(self):
        return self.read('S')
    
    def version(self):
        return self.read('VER')
    
    @property
    def KC1(self):
        self._KC1 = self.read('KC1')
        return self._KC1
    
    @property
    def KC2(self):
        self._KC2 = self.read('KC2')
        return self._KC2
        
    def LED(self,pwm):
        pwm = max(0.0,min(100.0,pwm))/2.0
        self.write('LED',pwm)
        return pwm

    def U1(self,pwm):
        pwm = max(0.0,min(100.0,pwm)) 
        self.write('U1',pwm)
        return pwm
        
    def U2(self,pwm):
        pwm = max(0.0,min(100.0,pwm)) 
        self.write('U2',pwm)
        return pwm

    def read(self,cmd):
        cmd_str = self.build_cmd_str(cmd,'')
        try:
            self.sp.write(cmd_str.encode())
            self.sp.flush()
        except Exception:
            return None
        return self.sp.readline().decode('UTF-8').replace("\r\n", "")
    
    def write(self,cmd,pwm):       
        cmd_str = self.build_cmd_str(cmd,(pwm,))
        try:
            self.sp.write(cmd_str.encode())
            self.sp.flush()
        except:
            return None
        return self.sp.readline().decode('UTF-8').replace("\r\n", "")
    
    def build_cmd_str(self,cmd, args=None):
        if args:
            args = ' '.join(map(str, args))
        else:
            args = ''
        return "{cmd} {args}\n".format(cmd=cmd, args=args)
    
    def close(self):
        try:
            self.sp.close()
            print('Koneksi terputus sempurna')
        except:
            print('Cabut dan re-konek lagi.')
        return True
