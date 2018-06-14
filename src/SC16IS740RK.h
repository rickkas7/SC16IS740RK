#ifndef __SC16IS740RK_H
#define __SC16IS740RK_H

#include "Particle.h"

/**
 * @brief Library for using the SC16IS740 UART on the Particle platform
 *
 */
class SC16IS740 : public Stream {
public:
	/**
	 * @brief Construct the UART object. Typically done as a global variable.
	 *
	 * @param wire The I2C port to use, typically Wire.
	 *
	 * @param addr The address you've set using the A0 and A1 pins, 0-3. This will be converted to the
	 * appropriate I2C address. Or you can directly specify the actual I2C address 0-127.
	 *
	 * @param intPin The interrupt pin (-1 = not used). Note: interrupts are not currently supported.
	 */
	SC16IS740(TwoWire &wire, int addr, int intPin = -1);
	virtual ~SC16IS740();

	/**
	 * @brief Set the oscillator frequency in Hz (default: 1843200)
	 *
	 * You must call this before begin
	 */
	inline SC16IS740 &withOscillatorHz(int value) { oscillatorHz = value; return *this; };

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
	void begin(int baudRate, uint8_t options = OPTIONS_8N1);

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
	 * @brief Write a single byte to the seria; port.
	 *
	 * @param c The byte to write. Can write binary or text data.
	 *
	 * This is a standard Arduino/Wiring method for Stream objects.
	 */
    virtual size_t write(uint8_t c);

    /**
     * @brief Read a register
     *
     * @param reg The register number to read. Note that this should be the register 0 - 16, before shifting for channel.
     */
	uint8_t readRegister(uint8_t reg);

	/**
     * @brief Write a register
     *
     * @param reg The register number to write. Note that this should be the register 0 - 16, before shifting for channel.
     *
     * @param value The value to write
     */
	bool writeRegister(uint8_t reg, uint8_t value);


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
	TwoWire &wire;
	int addr; // This is the actual I2C address
	int intPin; // Pin used for interrupts or -1
	int oscillatorHz = 1843200;
	bool hasPeek = false;
	uint8_t peekByte = 0;
	bool writeBlocksWhenFull = true;
};

#endif /* __SC16IS740RK_H */
