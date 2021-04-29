import sys

if len(sys.argv) != 2:
    print('Usage: createNodeEEPROM.py LED_COUNT')
    sys.exit()

ledCount = int(sys.argv[1])
ledCountArr = ledCount.to_bytes(5, 'little') # Add a zero (5bytes instead of the needed 4bytes for the int) for the node label

with open('nodeEEPROM.bin', 'wb') as f:
    f.write(ledCountArr)

print('Done! Please flash with: esptool.py --port PORT write_flash 0x003FB000 nodeEEPROM.bin')