import asyncio
import websockets
import serial

class Serial_Wrapper:
    def __init__(self, device='/dev/ttyUSB0', baud=921600):  
        self.ser = serial.Serial(device, baud)
        self.ser.flush()  # Flush the serial buffer

    def send_data(self, data, expect_confirmation=False, print_confirmation=True):
        self.ser.write(data)
        print('\t|||"{}" over serial.'.format(str(data)))
        if expect_confirmation:
            rec = self.ser.readline()
            if print_confirmation:
                print('\t||| Received "{}" over serial. Which "decode().rstrip()"s to \'{}\''.format(
                    str(rec), rec.decode('ASCII').rstrip()))

    def flush_buffer(self):
        self.ser.flushOutput()

def format_joystick_data(input):
    byte_value = int(input * 10)  # Multiply by 10 and convert to int
    return byte_value.to_bytes(1, 'big', signed=True)  # Convert to signed byte

async def receive_data():
    uri = 'ws://172.20.10.9:5678'  # Replace with the server's IP address
    serial_wrapper = Serial_Wrapper(device='/dev/ttyUSB0', baud=921600)
    
    try:
        async with websockets.connect(uri, timeout=20) as websocket:
            while True:
                try:
                    data = await websocket.recv()
                    print(f"Received data: {data}")
                    
                    parsed_data = eval(data)
                    print(parsed_data)

                    # Create the list with 'A' and the formatted inputs
                    formatted_data_list = [b'A'] + [format_joystick_data(input) for input in parsed_data]

                    # Convert the list to bytes
                    formatted_data_bytes = b''.join(formatted_data_list) + b'\n'
                    print(formatted_data_bytes)

                    # Send the data over serial
                    serial_wrapper.send_data(formatted_data_bytes)                   

                except websockets.exceptions.ConnectionClosed as e:
                    print(f"WebSocket connection closed: {e}")
                    break
                except Exception as e:
                    print(f"Error processing data: {e}")

                # Wait 50 milliseconds before the next iteration
                await asyncio.sleep(0.05)
                
    except Exception as e:
        print(f"Error connecting to WebSocket: {e}")

asyncio.get_event_loop().run_until_complete(receive_data())
