##########################---------------------------------####################################################################
import asyncio
import platform

from bleak import BleakClient


async def print_services(mac_addr: str, loop: asyncio.AbstractEventLoop):
    try:
        async with BleakClient(mac_addr, loop=loop, timeout=10.0) as client:
            svcs = client.services
            for service in svcs:
                print("Services : " + service.uuid + " Desc : " + service.description)
    except:
        print("not connected T.T")


# mac_addr = "C0:35:57:0F:05:E6"

loop = asyncio.get_event_loop()
loop.run_until_complete(print_services("C0:35:57:0F:05:E6", loop))
#####################################################################################################getService

##########################---------------------------------####################################################################
# import asyncio
# from bleak import BleakScanner
# ble_devices = list()
# async def run():
#     global ble_devices
#     devices = await BleakScanner.discover()
#     for d in devices:
#         ble_devices.append(d.address + " : "+ d.name)
#
# loop = asyncio.get_event_loop()
# loop.run_until_complete(run())
# print(ble_devices)
##########################---------------------------------##########################################find BLE Devices
