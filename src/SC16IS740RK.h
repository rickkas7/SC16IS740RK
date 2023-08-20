#ifndef __SC16IS740RK_H
#define __SC16IS740RK_H

#include "Particle.h"

/**
 * @brief Library for using the SC16IS740 UART on the Particle platform
 * 
 * It is recommended that you use https://github.com/rickkas7/SC16IS7xxRK, which is an updated
 * version of this library, instead. 
 */
class SC16IS740Base : public Stream {
public:
	SC16IS740Base();
	virtual ~SC16IS740Base();

	/**
	 * @brief Set the oscillator frequency in Hz (default: 1843200)
	 *
	 * You must call this before begin
	 */
	inline SC16IS740Base &withOscillatorHz(int value) { oscillatorHz = value; return *this; };

	/**
	 * @brief Set up the chip. You must do this before reading or writing.
	 *
	 * @param baudRate the baud rate (see below)
	 *
	 * @param options The number of data bits, parity, and stop bits
	 *
	 * You can call begin more than once if you want to change the baud rate. The FIFOs are
	 * cleared when you call begin.
	 *
	 * Available baud rates depend on your oscillator, but with a 1.8432 MHz oscillator, the following are supported:
	 * 50, 75, 110, 134.5, 150, 300, 600, 1200, 1800, 2000, 2400, 3600, 4800, 7200, 9600, 19200, 38400, 57600, 115200
	 *
	 * The valid options in standard number of bits; none=N, even=E, odd=O; number of stop bits format:
	 * OPTIONS_8N1, OPTIONS_8E1, OPTIONS_8O1
	 * OPTIONS_8N2, OPTIONS_8E2, OPTIONS_8O2
	 * OPTIONS_7N1, OPTIONS_7E1, OPTIONS_7O1
	 * OPTIONS_7N2, OPTIONS_7E2, OPTIONS_7O2
	 */
	bool begin(int baudRate, uint8_t options = OPTIONS_8N1);

	/**
	 * @brief Defines what should happen when calls to write()/print()/println()/printlnf() that would overrun the buffer.
	 *
	 * blockOnOverrun(true) - this is the default setting. When there is no room in the buffer for the data to be written,
	 * the program waits/blocks until there is room. This avoids buffer overrun, where data that has not yet been sent over
	 * serial is overwritten by new data. Use this option for increased data integrity at the cost of slowing down realtime
	 * code execution when lots of serial data is sent at once.
	 *
	 * blockOnOverrun(false) - when there is no room in the buffer for data to be written, the data is written anyway,
	 * causing the new data to replace the old data. This option is provided when performance is more important than data
	 * integrity.
	 */
	inline void blockOnOverrun(bool value = true) { writeBlocksWhenFull = value; };

	/**
	 * @brief Returns the number of bytes available to read from the serial port
	 *
	 * This is a standard Arduino/Wiring method for Stream objects.
	 */
    virtual int available();

	/**
	 * @brief Returns the number of bytes available to write into the TX FIFO
	 */
    virtual int availableForWrite();

	/**
	 * @brief Read a single byte from the serial port
	 *
	 * @return a byte value 0 - 255 or -1 if no data is available.
	 *
	 * This is a standard Arduino/Wiring method for Stream objects.
	 */
    virtual int read();

	/**
	 * @brief Read a single byte from the serial port, but do not remove it so it can be read again.
	 *
	 * @return a byte value 0 - 255 or -1 if no data is available.
	 *
	 * This is a standard Arduino/Wiring method for Stream objects.
	 */
    virtual int peek();

	/**
	 * @brief Block until all serial data is sent.
	 *
	 * This is a standard Arduino/Wiring method for Stream objects.
	 */
    virtual void flush();

	/**
	 * @brief Write a single byte to the serial port.
	 *
	 * @param c The byte to write. Can write binary or text data.
	 *
	 * This is a standard Arduino/Wiring method for Stream objects.
	 */
    virtual size_t write(uint8_t c);

	/**
	 * @brief Write a multiple bytes to the serial port.
	 *
	 * @param buffer The buffer to write. Can write binary or text data.
	 *
	 * @param size The number of bytes to write
	 *
	 * @return The number of bytes written.
	 *
	 * This is faster than writing a single byte at time because up to 32 bytes of data can
	 * be sent or received in an I2C transaction, greatly reducing overhead.
	 */
	virtual size_t write(const uint8_t *buffer, size_t size);

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
	virtual int read(uint8_t *buffer, size_t size);


    /**
     * @brief Read a register
     *
     * @param reg The register number to read. Note that this should be the register 0 - 16, before shifting for channel.
     */
	virtual uint8_t readRegister(uint8_t reg) = 0;

	/**
     * @brief Write a register
     *
     * @param reg The register number to write. Note that this should be the register 0 - 16, before shifting for channel.
     *
     * @param value The value to write
     */
	virtual bool writeRegister(uint8_t reg, uint8_t value) = 0;


	static const uint8_t OPTIONS_8N1 = 0b000011;
	static const uint8_t OPTIONS_8E1 = 0b011011;
	static const uint8_t OPTIONS_8O1 = 0b001011;

	static const uint8_t OPTIONS_8N2 = 0b000111;
	static const uint8_t OPTIONS_8E2 = 0b011111;
	static const uint8_t OPTIONS_8O2 = 0b001111;

	static const uint8_t OPTIONS_7N1 = 0b000010;
	static const uint8_t OPTIONS_7E1 = 0b011010;
	static const uint8_t OPTIONS_7O1 = 0b001010;

	static const uint8_t OPTIONS_7N2 = 0b000110;
	static const uint8_t OPTIONS_7E2 = 0b011110;
	static const uint8_t OPTIONS_7O2 = 0b001110;

	static const uint8_t RHR_THR_REG = 0x00;
	static const uint8_t IEF_REG = 0x01;
	static const uint8_t FCR_IIR_REG = 0x02;
	static const uint8_t LCR_REG = 0x03;
	static const uint8_t MCR_REG = 0x04;
	static const uint8_t LSR_REG = 0x05;
	static const uint8_t MSR_REG = 0x06;
	static const uint8_t SPR_REG = 0x07;
	static const uint8_t TXLVL_REG = 0x08;
	static const uint8_t RXLVL_REG = 0x09;
	static const uint8_t IODIR_REG = 0x0a;
	static const uint8_t IOSTATE_REG = 0x0b;
	static const uint8_t IOINTENA_REG = 0x0c;
	static const uint8_t IOCONTROL_REG = 0x0e;
	static const uint8_t EFCR_REG = 0x0f;

	// Special register block
	static const uint8_t LCR_SPECIAL_START = 0x80;
	static const uint8_t LCR_SPECIAL_END = 0xbf;
	static const uint8_t DLL_REG = 0x00;
	static const uint8_t DLH_REG = 0x01;

	// Enhanced register set
	static const uint8_t EFR_REG = 0x02;
	static const uint8_t XON1_REG = 0x04;
	static const uint8_t XON2_REG = 0x05;
	static const uint8_t XOFF1_REG = 0x06;
	static const uint8_t XOFF2_REG = 0x07;


protected:
	/**
	 * @brief Called from begin to allow things like wire.begin()
	 */
	virtual bool preBegin() { return true; };

	/**
	 * @brief Maximum number of bytes that can be read by readInternal
	 */
	virtual size_t readInternalMax() const = 0;

	/**
	 * @brief Internal function to read data
	 *
	 * It can only read 32 bytes at a time, the maximum that will fit in a 32 byte I2C transaction with
	 * the register address in the first byte.
	 */
	virtual bool readInternal(uint8_t *buffer, size_t size) = 0;

	/**
	 * @brief Maximum number of bytes that can be written by writeInternal
	 */
	virtual size_t writeInternalMax() const = 0;

	/**
	 * @brief Internal function to write data
	 *
	 * It can only write 31 bytes at a time, the maximum that will fit in a 32 byte I2C transaction with
	 * the register address in the first byte.
	 */
	virtual bool writeInternal(const uint8_t *buffer, size_t size) = 0;





	int oscillatorHz = 1843200;
	bool hasPeek = false;
	uint8_t peekByte = 0;
	bool writeBlocksWhenFull = true;
};

class SC16IS740 : public SC16IS740Base {
public:
	/**
	 * @brief Construct the UART object. Typically done as a global variable.
	 *
	 * @param wire The I2C port to use, typically Wire.
	 *
	 * @param addr The address you've set using the A0 and A1 pins, 0-3. This will be converted to the
	 * appropriate I2C address. Or you can directly specify the actual I2C address 0-127.
	 */
	SC16IS740(TwoWire &wire, int addr);

	/**
	 * @brief Destructor. You typically don't delete one of these as it's normally a global variable.
	 */
	virtual ~SC16IS740();

    /**
     * @brief Read a register
     *
     * @param reg The register number to read. Note that this should be the register 0 - 16, before shifting for channel.
     */
	virtual uint8_t readRegister(uint8_t reg);

	/**
     * @brief Write a register
     *
     * @param reg The register number to write. Note that this should be the register 0 - 16, before shifting for channel.
     *
     * @param value The value to write
     */
	virtual bool writeRegister(uint8_t reg, uint8_t value);

protected:
	/**
	 * Called during begin to initialize Wire (I2C)
	 */
	virtual bool preBegin();

	/**
	 * @brief Maximum number of bytes that can be read by readInternal
	 */
	inline size_t readInternalMax() const { return 32; };

	/**
	 * @brief Internal function to read data
	 *
	 * It can only read 32 bytes at a time, the maximum that will fit in a 32 byte I2C transaction with
	 * the register address in the first byte.
	 */
	virtual bool readInternal(uint8_t *buffer, size_t size);

	/**
	 * @brief Maximum number of bytes that can be written by writeInternal
	 */
	inline size_t writeInternalMax() const { return 31; };

	/**
	 * @brief Internal function to write data
	 *
	 * It can only write 31 bytes at a time, the maximum that will fit in a 32 byte I2C transaction with
	 * the register address in the first byte.
	 */
	virtual bool writeInternal(const uint8_t *buffer, size_t size);

	TwoWire &wire;
	uint8_t addr; // This is the actual I2C address

};

class SC16IS740SPI : public SC16IS740Base {
public:
	/**
	 * @brief Construct the UART object. Typically done as a global variable.
	 *
	 * @param spi The SPI port to use, typically SPI (A pins) or SPI1 (D pins).
	 *
	 * @param cs The pin to use for the CS (chip select) or SS (slave select) pin. Often the
	 * pin A2 is used for SPI and D5 for SPI1, but any free GPIO pin can be used.
	 *
	 * @param intPin The pin to use for interrupts from the SC16IS740. Not currently used.
	 * If not using interrupts, omit this parameter or pass -1.
	 */
	SC16IS740SPI(SPIClass &spi, int cs, int intPin = -1);

#ifdef SYSTEM_VERSION_v151RC1
	// In 1.5.0-rc.1, SPI interfaces are handled differently. You can still pass in SPI, SPI1, etc.
	// but the code to handle it varies
	SC16IS740SPI(::particle::SpiProxy<HAL_SPI_INTERFACE1> &spiProxy, int cs = A2, int intPin = -1) : 
		spi(spiProxy.instance()), cs(cs), intPin(intPin) {};

#if Wiring_SPI1
	SC16IS740SPI(::particle::SpiProxy<HAL_SPI_INTERFACE2> &spiProxy, int cs = A2, int intPin = -1) : 
		spi(spiProxy.instance()), cs(cs), intPin(intPin) {};
#endif

#if Wiring_SPI2
	SC16IS740SPI(::particle::SpiProxy<HAL_SPI_INTERFACE3> &spiProxy, int cs = A2, int intPin = -1) : 
		spi(spiProxy.instance()), cs(cs), intPin(intPin) {};
#endif

#endif

	/**
	 * @brief Destructor. You typically don't delete one of these as it's normally a global variable.
	 */
	virtual ~SC16IS740SPI();

    /**
     * @brief Read a register
     *
     * @param reg The register number to read. Note that this should be the register 0 - 16, before shifting for channel.
     */
	virtual uint8_t readRegister(uint8_t reg);

	/**
     * @brief Write a register
     *
     * @param reg The register number to write. Note that this should be the register 0 - 16, before shifting for channel.
     *
     * @param value The value to write
     */
	virtual bool writeRegister(uint8_t reg, uint8_t value);

	/**
	 * @brief Sets the SPI clock speed (default: 4 MHz)
	 */
	inline SC16IS740SPI &withSpiClockSpeedMHz(uint8_t value) { spiClockSpeedMHz = value; return *this; };

	/**
	 * @brief Sets shared bus mode
	 *
	 * In shared bus mode, every SPI transaction will reset the SPI mode, speed, and bit order. This is useful if you
	 * have multiple devices on a single SPI bus with different settings. The problem is that this appears to cause
	 * data corruption on the STM32F205 unless you wait a bit after changing the settings. This apparently required
	 * delay is set using the delayus parameter.
	 *
	 * @param delayus Amount of time in microseconds to delay after changing SPI settings to allow the bus to settle.
	 */
	inline SC16IS740SPI &withSharedBus(unsigned long delayus) { sharedBus = true; sharedBusDelay = delayus; return *this;};

protected:
	/**
	 * Called during begin to initialize SPI
	 */
	virtual bool preBegin();

	/**
	 * @brief Maximum number of bytes that can be read by readInternal
	 */
	inline size_t readInternalMax() const { return 64; };

	/**
	 * @brief Internal function to read data
	 *
	 * It can only read 32 bytes at a time, the maximum that will fit in a 32 byte I2C transaction with
	 * the register address in the first byte.
	 */
	virtual bool readInternal(uint8_t *buffer, size_t size);

	/**
	 * @brief Maximum number of bytes that can be written by writeInternal
	 */
	inline size_t writeInternalMax() const { return 64; };

	/**
	 * @brief Internal function to write data
	 *
	 * It can only write 31 bytes at a time, the maximum that will fit in a 32 byte I2C transaction with
	 * the register address in the first byte.
	 */
	virtual bool writeInternal(const uint8_t *buffer, size_t size);

	/**
	 * @brief Begins an SPI transaction, setting the CS line LOW.
	 * Also sets the SPI speed and mode settings if sharedBus == true
	 */
	void beginTransaction();

	/**
	 * @brief Ends an SPI transaction, basically just setting the CS line high.
	 */
	void endTransaction();

	/**
	 * @brief Sets the SPI bus speed, mode and byte order
	 *
	 * This is done in begin() normally or in beginTransaction() if sharedBus == true.
	 *
	 * The issue is that changing the bus speed and settings requires a delay for things to
	 * sync back up. If the SPI flash is the only thing on that bus, the delay is unnecessary
	 * because the speed and mode can be set during begin() instead and just left that way.
	 */
	void setSpiSettings();


	SPIClass &spi;
	int cs;
	int intPin;

	/**
	 * @brief The maximum speed to use. Most flash modules can handle 60 MHz without difficulties.
	 *
	 * Flash-chip-specific subclasses can override this if they need a slower speed.
	 */
	uint8_t spiClockSpeedMHz = 4;

	bool sharedBus = false;
	unsigned long sharedBusDelay = 200; // microseconds

};



#endif /* __SC16IS740RK_H */
