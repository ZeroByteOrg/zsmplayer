#include "opm.h"
#include "ymfm_opm.h"
#include "ymfm_fm.ipp"
#include <ctime>
#include <iostream>
#include <cmath>
#include <queue>

#define BUFFERSIZE	64000

typedef struct {
	uint8_t reg;
	uint8_t val;
} command;

class ym2151_interface : public ymfm::ymfm_interface
{
public:
	ym2151_interface(uint32_t ymclock = 3579545)
	    : m_chip(*this),
	      m_chip_sample_rate(m_chip.sample_rate(ymclock)),
	      m_warning(false)
	{
	}

	void render(int16_t *stream, uint32_t samples)
	{
		command c;
		ymfm::ym2151::output_data ym0;
		for (uint32_t i = 0 ; i < samples*2 ; i+=2) {
			if (m_writes.size()) {
				c=m_writes.front();
				m_writes.pop();
				m_chip.write_address(c.reg);
				m_chip.write_data(c.val, true); // bool = debug_write mode
			}
			m_chip.generate(&ym0, 1);
			stream[i] = ym0.data[0];
			stream[i+1] = ym0.data[1];
		}
	}

	void write(uint8_t addr, uint8_t value)
	{
		command c;
		c.reg=addr;
		c.val=value;
		m_writes.push(c);
//		m_chip.write_address(addr);
//		m_chip.write_data(value, true); // bool = debug_write mode
	}

	void reset()
	{
		m_chip.reset();
	}

	uint32_t sample_rate(uint32_t clock)
	{
		return m_chip.sample_rate(clock);
	}


private:
	ymfm::ym2151 m_chip;
	uint32_t m_chip_clock_speed;
	uint32_t m_chip_sample_rate;
	bool m_warning;
	std::queue<command> m_writes;
};

static ym2151_interface Ym_interface;

// present C-style accessor functions for python module
extern "C" {
	void YM_render(int16_t *stream, unsigned samples)
	{
		Ym_interface.render(stream, samples);
	}

	void YM_write(uint8_t reg, uint8_t val)
	{
		Ym_interface.write(reg, val);
	}

	void YM_reset()
	{
		Ym_interface.reset();
	}

	uint32_t YM_samplerate(uint32_t clock)
	{
		return Ym_interface.sample_rate(clock);
	}
}
