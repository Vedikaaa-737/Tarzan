import serial
port = "COM3"  
baudrate = 38400  
try:    
    with serial.Serial(port, baudrate, timeout=1) as ser:
        print(f"Connected to {port} at {baudrate} baud.")
        
        
        while True:
            if ser.in_waiting > 0:  # 
                data = ser.readline().decode('utf-8').strip()  
                print(f"Received: {data}")
except serial.SerialException as e:
    print(f"Error: {e}")
except KeyboardInterrupt:
    print("Exiting...")
