import asyncio
import websockets
import serial
import time

class Serial_Wrapper:
    def __init__(self, device='/dev/serial0', baud=115200):  # Use the GPIO UART interface with a lower baud rate
        self.ser = serial.Serial(device, baud)
        self.ser.flush()  # Flush the serial buffer

    def send_data(self, data, expect_confirmation=False, print_confirmation=True):
        self.ser.write(data)
        if print_confirmation:
            print(f"\n!!! Sent {str(data)} over serial.")
        if expect_confirmation:
            rec = self.ser.readline()
            if print_confirmation:
                print(f"\n!!! Received {str(rec)} over serial.")
            return rec

    def flush_buffer(self):
        self.ser.flushOutput()

def format_joystick_data(data_list):
    # Only take the first element (baseX) for simplicity
    value = data_list[0]
    formatted_data = f"{value:.2f}"
    return f"[{formatted_data}]"

async def receive_data():
    uri = 'ws://172.20.10.9:5678'  # Replace with the server's IP address
    serial_wrapper = Serial_Wrapper(device='/dev/serial0', baud=115200)

    try:
        async with websockets.connect(uri, timeout=20) as websocket:
            while True:
                try:
                    data = await websocket.recv()
                    print(f"Received data: {data}")
                    
                    parsed_data = eval(data)
                    print(parsed_data)
                    
                    formatted_data = format_joystick_data(parsed_data).encode() + b'\n'
                    print(f"Formatted values: {formatted_data}")
                    serial_wrapper.send_data(formatted_data)
                except websockets.exceptions.ConnectionClosed as e:
                    print(f"WebSocket connection closed: {e}")
                    break
                except Exception as e:
                    print(f"Error processing data: {e}")
    except Exception as e:
        print(f"Error connecting to WebSocket: {e}")

asyncio.get_event_loop().run_until_complete(receive_data())