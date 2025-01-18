from adafruit_ble import BLERadio
from adafruit_ble.advertising.standard import ProvideServicesAdvertisement
from adafruit_ble.services.nordic import UARTService
import time

ble = BLERadio()

# List to hold multiple UART connections and a dictionary to track addresses
uart_connections = []
connection_addresses = {}  # Maps connections to addresses
max_connections = 5  # Set this to the desired number of simultaneous sensors

while True:
    # Scan for new devices if there are fewer connections than desired
    if len(uart_connections) < max_connections:
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




















