from adafruit_ble import BLERadio
from adafruit_ble.advertising.standard import ProvideServicesAdvertisement
from adafruit_ble.services.nordic import UARTService
from tb_gateway_mqtt import TBDeviceMqttClient
import logging.handlers
import time

   
ACCESS_TOKEN = "dotqWC7IdD1wMDy9SN1D"
THINGSBOARD_SERVER = 'iot.makowski.edu.pl'
client = None
logging.basicConfig(level=logging.DEBUG)

ble = BLERadio()

# List to hold multiple UART connections and a dictionary to track addresses
uart_connections = []
connection_addresses = {}  # Maps connections to addresses
max_connections = 5  # Set this to the desired number of simultaneous sensors
lineRead = False;

# callback function that will call when we will change value of our Shared Attribute
def attribute_callback(result, _):
     print(result)
     # make sure that you paste YOUR shared attribute name
     period = result.get('blinkingPeriod', 1.0)

# callback function that will call when we will send RPC
def rpc_callback(id, request_body):
    # request body contains method and other parameters
    print(request_body)
    method = request_body.get('method')
    if method == 'getTelemetry':
        attributes, telemetry = prepare_data()
        client.send_attributes(attributes)
        client.send_telemetry(telemetry)
    else:
        print('Unknown method: ' + method)
 

def getData():
    line = ''
    # Scan for new devices if there are fewer connections than desired
    if len(uart_connections) < max_connections:
        print(uart_connections)
        print("Scanning for new devices...")
        for adv in ble.start_scan(ProvideServicesAdvertisement, timeout=2):
            # Only connect to devices that haven't been connected before
            if UARTService in adv.services and adv.address not in connection_addresses.values():
                try:
                    # Attempt to connect to the advertisement
                    connection = ble.connect(adv)
                    print(f"Connected to device with address {adv.address}")

                    # Add connection to list and store its address
                    uart_connections.append(connection)
                    connection_addresses[connection] = adv.address

                except Exception as e:
                    print(f"Failed to connect to {adv.address}: {e}")
                # No need to break; continue scanning to find other devices

        # Stop scanning to process existing connections
        ble.stop_scan()

    # Check for data from each active connection
    for connection in uart_connections[:]:  # Copy list to allow safe removal
        if connection and connection.connected:
            uart_service = connection[UARTService]
            try:
                # Read line from the UART service if available
                if uart_service.in_waiting:
                    lineRead = True;
                    line = uart_service.readline().decode("utf-8").strip()
                    print(f"Received from {connection}: {line}")

            except Exception as e:
                print(f"Error reading from {connection}: {e}")

        # Handle disconnected devices by removing them from the list and address dictionary
        if not connection.connected:
            address = connection_addresses.pop(connection, "Unknown")
            print(f"Disconnected from device with address {address}")
            uart_connections.remove(connection)

    # Add a small delay to prevent excessive CPU usage
    time.sleep(0.1)
    return line
 
   
def prepare_data():
   
    temperature = float(sensorData[1:5])
    pressure = float(sensorData[7:13])
    humidity = float(sensorData[15:19])

    attributes = {
        'ip_address': ip_address,
        'macaddress': mac_address
    }
    telemetry = {
        'temperature': temperature,
        'pressure': pressure,
        'humidity': humidity
    }
    print(attributes, telemetry)
    return attributes, telemetry
   
# request attribute callback
def sync_state(result, exception=None):
     global period
     if exception is not None:
         print("Exception: " + str(exception))
     else:
         period = result.get('shared', {'blinkingPeriod': 1.0})['blinkingPeriod']

def main():
     
     global client
     client = TBDeviceMqttClient(THINGSBOARD_SERVER, username=ACCESS_TOKEN)
     client.connect()
     client.request_attributes(shared_keys=['blinkingPeriod'], callback=sync_state)
        
     # now attribute_callback will process shared attribute request from server
     sub_id_1 = client.subscribe_to_attribute("blinkingPeriod", attribute_callback)
     sub_id_2 = client.subscribe_to_all_attributes(attribute_callback)

     # now rpc_callback will process rpc requests from server
     client.set_server_side_rpc_request_handler(rpc_callback)
    
     while not lineRead:
          line = getData()
          
     while not client.stopped:
         attributes, telemetry = prepare_data()
         client.send_attributes(attributes)
         client.send_telemetry(telemetry)
         time.sleep(60)
   
if __name__=='__main__':
    if ACCESS_TOKEN != "TEST_TOKEN":
        main()
    else:
        print("Please change the ACCESS_TOKEN variable to match your device access token and run script again.")
