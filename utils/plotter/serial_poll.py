import serial
import struct
import threading

class SerialPoll(threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self)
        self._port = '/dev/ttyACM0'
        self._baudrate = 9600
        self._datapoints = {}
        self._continue = True

    def get_channel_data(self, channel):
        return self._datapoints.get(channel)
    
    def stop(self):
        self._continue = False

    def run(self):
        with serial.Serial(port=self._port, baudrate=self._baudrate) as sp:
            if not sp.isOpen():
                print('Could not open serial port.')
                return

            while self._continue:
                # We skip until we get a beginning of transmission
                if sp.read() != b'\xFF':
                    continue
                if sp.read() != b'\xFF':
                    continue
                if sp.read() != b'\xFF':
                    continue
                if sp.read() != b'\xFF':
                    continue
                
                # Arduino is little endian
                channel, timestamp, value = struct.unpack('<bif', sp.read(9))

                # Append data
                if channel not in self._datapoints:
                    self._datapoints[channel] = []
                self._datapoints[channel].append((timestamp, value))

