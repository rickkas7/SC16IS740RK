

#include "SC16IS740RK.h"

Logger log("app.ser");

// Converts addr (0-3) corresponding to the value set by A0 and A1 to an address
// Note these values are half that of Table 32 in the data sheet because the data sheet
// values include the I2C R/W bit in bit 0, the LSB.
static const uint8_t subAddrs[4] = { 0x4d, 0x4c, 0x49, 0x48};

SC16IS740::SC16IS740(TwoWire &wire, int addr, int intPin) : wire(wire), intPin(intPin) {

	if (addr < (int) sizeof(subAddrs)) {
		// Use lookup table
		this->addr = subAddrs[addr];
	}
	else {
		// Use actual address
		this->addr = addr;
	}
}

SC16IS740::~SC16IS740() {

}

void SC16IS740::begin(int baudRate, uint8_t options) {
	wire.begin();

	// My test board uses this oscillator
	// KC3225K1.84320C1GE00
	// OSC XO 1.8432MHz CMOS SMD $1.36
	// https://www.digikey.com/product-detail/en/avx-corp-kyocera-corp/KC3225K1.84320C1GE00/1253-1488-1-ND/5322590
	// Another suggested frequency from the data sheet is 3.072 MHz

	// The divider devices the clock frequency to 16x the baud rate
	int div = oscillatorHz / (baudRate * 16);

	writeRegister(LCR_REG, LCR_SPECIAL_START); // 0x80
	writeRegister(DLL_REG, div & 0xff);
	writeRegister(DLH_REG, div >> 8);
	writeRegister(LCR_REG, LCR_SPECIAL_END); // 0xbf

	writeRegister(LCR_REG, options & 0x3f);

	// Enable FIFOs
	writeRegister(FCR_IIR_REG, 0x07); // Enable FIFO, Clear RX and TX FIFOs

	// Also MCR?

}

int SC16IS740::available() {
	return readRegister(RXLVL_REG);
}

int SC16IS740::availableForWrite() {
	return readRegister(TXLVL_REG);
}


int SC16IS740::read() {
	if (hasPeek) {
		hasPeek = false;
		return peekByte;
	}
	else {
		if (available()) {
			return readRegister(RHR_THR_REG);
		}
		else {
			return -1;
		}
	}
}

int SC16IS740::peek() {
	if (!hasPeek) {
		hasPeek = true;
		peekByte = read();
	}
	return peekByte;
}

void SC16IS740::flush() {
	while(availableForWrite() < 64) {
		delay(1);
	}
}

size_t SC16IS740::write(uint8_t c) {

	if (writeBlocksWhenFull) {
		// Block until there is room in the buffer
		while(!availableForWrite()) {
			delay(1);
		}
	}

	writeRegister(RHR_THR_REG, c);

	return 1;
}


// Note: reg is the register 0 - 15, not the shifted value with the channel select bits. Channel is always 0
// on the SC16IS740.
uint8_t SC16IS740::readRegister(uint8_t reg) {
	wire.beginTransmission(addr);
	wire.write(reg << 3);
	wire.endTransmission(false);

	wire.requestFrom(addr, 1, true);
	uint8_t value = (uint8_t) wire.read();

	log.trace("readRegister reg=%d value=%d", reg, value);

	return value;
}

// Note: reg is the register 0 - 15, not the shifted value with the channel select bits
bool SC16IS740::writeRegister(uint8_t reg, uint8_t value) {
	wire.beginTransmission(addr);
	wire.write(reg << 3);
	wire.write(value);

	int stat = wire.endTransmission(true);

	// stat:
	// 0: success
	// 1: busy timeout upon entering endTransmission()
	// 2: START bit generation timeout
	// 3: end of address transmission timeout
	// 4: data byte transfer timeout
	// 5: data byte transfer succeeded, busy timeout immediately after

	log.trace("writeRegister reg=%d value=%d stat=%d", reg, value, stat);
	// log.trace("read after write value=%d", readRegister(reg));

	return (stat == 0);
}


