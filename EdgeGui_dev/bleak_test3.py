##########################---------------------------------####################################################################
import asyncio
import platform

from bleak import BleakClient

CHARACTERISTIC_UUID = "f64f0100-7fdf-4b2c-ad31-e65ca15bef6b"
async def print_services(mac_addr: str, loop: asyncio.AbstractEventLoop):
    async with BleakClient(mac_addr, loop=loop, timeout=10.0) as client:
        read = await client.read_gatt_char(CHARACTERISTIC_UUID)
        print(read)
        await client.write_gatt_char(CHARACTERISTIC_UUID, bytearray([0x37]))
        read = await client.read_gatt_char(CHARACTERISTIC_UUID)
        print(read)
        await client.write_gatt_char(CHARACTERISTIC_UUID, bytearray([0x38]))
        read = await client.read_gatt_char(CHARACTERISTIC_UUID)
        print(read)
        await client.write_gatt_char(CHARACTERISTIC_UUID, bytearray([0x39]))
        read = await client.read_gatt_char(CHARACTERISTIC_UUID)
        print(read)
        await client.write_gatt_char(CHARACTERISTIC_UUID, bytearray([0x40]))
        read = await client.read_gatt_char(CHARACTERISTIC_UUID)
        print(read)
        await client.write_gatt_char(CHARACTERISTIC_UUID, bytearray([0x41]))
        read = await client.read_gatt_char(CHARACTERISTIC_UUID)
        print(read)


# mac_addr = "C0:35:57:0F:05:E6"

loop = asyncio.get_event_loop()
loop.run_until_complete(print_services("C0:35:57:0F:05:E6", loop))
