

#include "SC16IS740RK.h"

static Logger log("app.ser");

// Converts addr (0-3) corresponding to the value set by A0 and A1 to an address
// Note these values are half that of Table 32 in the data sheet because the data sheet
// values include the I2C R/W bit in bit 0, the LSB.
static const uint8_t subAddrs[4] = { 0x4d, 0x4c, 0x49, 0x48};

SC16IS740Base::SC16IS740Base() {
}

SC16IS740Base::~SC16IS740Base() {

}

bool SC16IS740Base::begin(int baudRate, uint8_t options) {

	preBegin();

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
	return true;
}

int SC16IS740Base::available() {
	return readRegister(RXLVL_REG);
}

int SC16IS740Base::availableForWrite() {
	return readRegister(TXLVL_REG);
}


int SC16IS740Base::read() {
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

int SC16IS740Base::peek() {
	if (!hasPeek) {
		peekByte = read();
		hasPeek = true;
	}
	return peekByte;
}

void SC16IS740Base::flush() {
	while(availableForWrite() < 64) {
		delay(1);
	}
}

size_t SC16IS740Base::write(uint8_t c) {

	if (writeBlocksWhenFull) {
		// Block until there is room in the buffer
		while(!availableForWrite()) {
			delay(1);
		}
	}

	writeRegister(RHR_THR_REG, c);

	return 1;
}

size_t SC16IS740Base::write(const uint8_t *buffer, size_t size) {
	size_t written = 0;
	bool done = false;

	while(size > 0 && !done) {
		size_t count = size;
		if (count > writeInternalMax()/2) {
			count = writeInternalMax()/2;
		}

		if (writeBlocksWhenFull) {
			while(true) {
				int avail = availableForWrite();
				if (count <= (size_t) avail) {
					break;
				}
			}
		}
		else {
			int avail = availableForWrite();
			if (count > (size_t) avail) {
				count = (size_t) avail;
				done = true;
			}
		}

		if (count == 0) {
			break;
		}

		if (!writeInternal(buffer, count)) {
			// Failed to write
			break;
		}
		buffer += count;
		size -= count;
		written += count;
	}

	return written;
}

/**
 * @brief Read a multiple bytes to the serial port.
 *
 * @param buffer The buffer to read data into. It will not be null terminated.
 *
 * @param size The maximum number of bytes to read (buffer size)
 *
 * @return The number of bytes actually read or -1 if there are no bytes available to read.
 *
 * This is faster than reading a single byte at time because up to 32 bytes of data can
 * be sent or received in an I2C transaction, greatly reducing overhead.
 */
int SC16IS740Base::read(uint8_t *buffer, size_t size) {
	int avail = available();
	if (avail == 0) {
		// No data to read
		return -1;
	}
	if (size > (size_t) avail) {
		size = (size_t) avail;
	}
	if (size > readInternalMax()) {
		size = readInternalMax();
	}
	if (!readInternal(buffer, size)) {
		return -1;
	}

	return (int) size;
}



SC16IS740::SC16IS740(TwoWire &wire, int addr) : wire(wire) {

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

bool SC16IS740::preBegin() {
	wire.begin();
	return true;
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


bool SC16IS740::readInternal(uint8_t *buffer, size_t size) {
	wire.beginTransmission(addr);
	wire.write(RHR_THR_REG << 3);
	wire.endTransmission(false);

	uint8_t numRcvd = wire.requestFrom(addr, size, (uint8_t)true);
	if (numRcvd < size) {
		log.info("readInternal failed numRcvd=%u size=%u", numRcvd, size);
		return false;
	}

	for(size_t ii = 0; ii < size; ii++) {
		buffer[ii] = (uint8_t) wire.read();
	}

	log.trace("readInternal %d bytes", size);

	return true;
}


bool SC16IS740::writeInternal(const uint8_t *buffer, size_t size) {
	wire.beginTransmission(addr);
	wire.write(RHR_THR_REG << 3);
	wire.write(buffer, size);

	int stat = wire.endTransmission(true);

	// stat:
	// 0: success
	// 1: busy timeout upon entering endTransmission()
	// 2: START bit generation timeout
	// 3: end of address transmission timeout
	// 4: data byte transfer timeout
	// 5: data byte transfer succeeded, busy timeout immediately after

	log.trace("writeInternal size=%d stat=%d", size, stat);

	return (stat == 0);
}

SC16IS740SPI::SC16IS740SPI(SPIClass &spi, int cs, int intPin) : spi(spi), cs(cs), intPin(intPin) {

}
SC16IS740SPI::~SC16IS740SPI() {

}

bool SC16IS740SPI::preBegin() {
	spi.begin(cs);
	digitalWrite(cs, HIGH);

	if (!sharedBus) {
		setSpiSettings();
	}
	return true;
}

// Note: reg is the register 0 - 15, not the shifted value with the channel select bits. Channel is always 0
// on the SC16IS740.
uint8_t SC16IS740SPI::readRegister(uint8_t reg) {

	beginTransaction();

	spi.transfer(0x80 | reg << 3);
	uint8_t value = (uint8_t) spi.transfer(0);

	endTransaction();


	log.trace("readRegister reg=%d value=%d", reg, value);

	return value;
}

// Note: reg is the register 0 - 15, not the shifted value with the channel select bits
bool SC16IS740SPI::writeRegister(uint8_t reg, uint8_t value) {


	beginTransaction();

	spi.transfer(reg << 3);
	spi.transfer(value);

	endTransaction();

	log.trace("writeRegister reg=%d value=%d", reg, value);
	// log.trace("read after write value=%d", readRegister(reg));

	return true;
}


bool SC16IS740SPI::readInternal(uint8_t *buffer, size_t size) {
	beginTransaction();

	spi.transfer(0x80 | RHR_THR_REG << 3);
#if HAL_PLATFORM_MESH
	// On mesh platforms, the DMA spi.transfer is causing a SOS fault. Disable for now.
	for(size_t ii = 0; ii < size; ii++) {
		buffer[ii] = spi.transfer(0);
	}
#else
	spi.transfer(NULL, buffer, size, NULL);
#endif
	endTransaction();

	log.trace("readInternal %d bytes", size);

	return true;
}

bool SC16IS740SPI::writeInternal(const uint8_t *buffer, size_t size) {
	beginTransaction();

	spi.transfer(RHR_THR_REG << 3);
#if HAL_PLATFORM_MESH
	// On mesh platforms, the DMA spi.transfer is causing a SOS fault. Disable for now.
	for(size_t ii = 0; ii < size; ii++) {
		spi.transfer(buffer[ii]);
	}
#else
	spi.transfer(const_cast<uint8_t *>(buffer), NULL, size, NULL);
#endif

	endTransaction();

	return true;
}


void SC16IS740SPI::beginTransaction() {
	if (sharedBus) {
		setSpiSettings();
		// Changing the SPI settings seems to leave the bus unstable for a period of time.
		if (sharedBusDelay != 0) {
			delayMicroseconds(sharedBusDelay);
		}
	}
	pinResetFast(cs);
}

void SC16IS740SPI::endTransaction() {
	pinSetFast(cs);
}

void SC16IS740SPI::setSpiSettings() {
	// The SC16IS7xx can only do MSBFIRST, SPI_MODE0
	spi.setBitOrder(MSBFIRST);
	spi.setClockSpeed(spiClockSpeedMHz, MHZ); // Default: 4
	spi.setDataMode(SPI_MODE0);
}
