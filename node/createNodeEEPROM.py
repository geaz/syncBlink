import sys

if len(sys.argv) != 2:
    print('Usage: createNodeEEPROM.py LED_COUNT')
    sys.exit()

ledCount = int(sys.argv[1])
ledCountArr = ledCount.to_bytes(4 + 96 + 1, 'little') # 4bytes for the led count, 96bytes for the wifi infos, and one byte for the node label as a placeholder

with open('nodeEEPROM.bin', 'wb') as f:
    f.write(ledCountArr)

print('Done! Please flash with: esptool.py --port PORT write_flash 0x003FB000 nodeEEPROM.bin')