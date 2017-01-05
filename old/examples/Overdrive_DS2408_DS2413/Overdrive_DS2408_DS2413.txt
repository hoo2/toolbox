/*
  This example performs an overdrive search on setup, and then checks the status of DS2408 / DS2413 devices
*/

#include <OneWire.h>
#define READ_PIO 0xf5
#define OVERDRIVE_SKIP 0x3c


typedef struct device {
  device *next;
  byte addr[8];
} device;

device *_devices = NULL;
OneWire net(54);  //pin A0 of arduino due

void PrintBytes(uint8_t* addr, uint8_t count, bool newline = false) {
  for (uint8_t i = 0; i < count; i++) {
    Serial.print(addr[i] >> 4, HEX);
    Serial.print(addr[i] & 0x0f, HEX);
  }
  if (newline)
    Serial.println();
}

void printBinary(byte in, bool newline = false) {
  for (byte mask = 1 << 7; mask; mask >>= 1) {
    Serial.print((mask & in ? 1 : 0));
  }
  if (newline)
    Serial.println();
}


bool test2408(OneWire* net, uint8_t* addr, bool overdrive) {
  bool result = true;
  uint8_t buf[13];  // Put everything in the buffer so we can compute CRC easily.
  buf[0] = 0xF0;    // Read PIO Registers
  buf[1] = 0x88;    // LSB address
  buf[2] = 0x00;    // MSB address
  net->reset(overdrive);
  net->select(addr, overdrive);
  net->write_bytes(buf, 3, overdrive);
  net->read_bytes(buf + 3, 10, overdrive);   // 3 cmd bytes, 6 data bytes, 2 0xFF, 2 CRC16
  if (!OneWire::check_crc16(buf, 11, &buf[11])) {
    Serial.print("CRC failure in answer of DS2408 at ");
    PrintBytes(addr, 8, true);
    result = false;
  }
  return result;
}

void setup(void) {
  Serial.begin(9600);
  byte addr[8];
  net.reset(false); //perform a normal speed reset
  net.write(OVERDRIVE_SKIP, false); //write 0x3c (overdrive skip rom) in _normal_ speed to the bus. This puts all overdrive capeable devices into overdrive mode
  net.reset(true); //this is only understood by overdrive devices
  
  while (net.search(addr, true)) {
    Serial.print("Found ");
    PrintBytes(addr, 8);
    if (OneWire::crc8(addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      continue;
    }
    device *dev = (device *)malloc(sizeof(device));
    if (!dev) {
      Serial.println("Out of memory");
      return;
    }
    dev->next = NULL;
    memcpy(dev->addr, addr, 8);
    if (!_devices) {
      _devices = dev;
    }
    else {
      device *next = _devices;
      while (next->next) {
        next = next->next;
      };
      next->next = dev;
    }
  }
}

bool test2413(OneWire *net, byte *addr, bool overdrive=false) {
  bool result = true;
  net->reset(overdrive);
  net->select(addr, overdrive);
  net->write(READ_PIO, overdrive);
  byte answer = net->read(overdrive);
  byte orig = answer & 0xf;
  byte comp = (~(answer >> 4)) & 0xf;
  if (orig != comp) {
    Serial.print("Error while reading status of DS2413 ");
    PrintBytes(addr, 8);
    Serial.print(", answer was: ");
    printBinary(answer, true);
    result = false;
  }
  return result;
}

void loop(void) {
  device *next = _devices;
  while (next) {
    if (next->addr[0] == 0x3a) {
      test2413(&net, next->addr, true);
    }
    else if(next->addr[0] == 0x29) {
      test2408(&net, next->addr, true);
    }
    next = next->next;
  }
  delay(2000);
}
