#include "opm.h"
#include "ymfm_opm.h"
#include "ymfm_fm.ipp"
#include <ctime>
#include <iostream>
#include <chrono>
#include <cmath>

// reference for timing = https://www.cplusplus.com/reference/chrono/high_resolution_clock/now/

#define BUFFERSIZE	64000

using namespace std::chrono;

class ym2151_interface : public ymfm::ymfm_interface
{
public:
	ym2151_interface(uint32_t ymclock = 3579545)
	    : m_chip(*this),
	      m_lastrender(high_resolution_clock::now()),
	      m_buffer_used(0),
	      m_chip_sample_rate(m_chip.sample_rate(ymclock)),
	      m_warning(false)
	{
	}

	void render(int16_t *stream, uint32_t samples)
	{
		m_lastrender = high_resolution_clock::now();
		ymfm::ym2151::output_data ym0;

		uint32_t i = 0;
		// if buffer has data, send those first
		while ((i < m_buffer_used) && (i < samples))
		{
			ym0 = m_buffer[i++];
			*stream++ = ym0.data[0];
			*stream++ = ym0.data[1];
		}
		// then generate any remaining samples
		generate(stream, samples - i);
		m_buffer_used = 0;
	}
	

	void write(uint8_t addr, uint8_t value)
	{
		prerender(high_resolution_clock::now());
		m_chip.write_address(addr);
		m_chip.write_data(value, true); // bool = debug_write mode
	}

	void reset()
	{
		prerender(high_resolution_clock::now());
		m_chip.reset();
	}

	uint32_t sample_rate(uint32_t clock)
	{
		return m_chip.sample_rate(clock);
	}
	
	void debug(high_resolution_clock::time_point now)
	{
		double whole, frac;

		// numsamples = samples-per-unit-time * delta-t
		double dt = (duration_cast<duration<double>>(now - m_lastrender)).count();
		frac = std::modf(m_chip_sample_rate * dt, &whole);
		uint32_t numsamples = static_cast<uint32_t>(whole);
		printf ("---------------------------\n");
		printf ("rate    = %d\n", m_chip_sample_rate);
		printf ("period  = %f\n", 1.0/m_chip_sample_rate);
		printf ("dt      = %f\n", dt);
		printf ("nSample = %d\n", numsamples);
		printf ("frac    = %f\n", frac);
		printf ("adjust  = %f sec\n", frac / m_chip_sample_rate);

		// now set the timestamp of the last-rendered sample:
		//duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
		//std::chrono::duration<double>(timeToSleep)
		//dt = (frac / m_chip_sample_rate);
	}

/*
	void debug_write(uint8_t addr, uint8_t value)
	{
		// do a direct write without triggering the busy timer
		m_chip.write_address(addr);
		m_chip.write_data(value, true);
	}

	uint8_t debug_read(uint8_t addr)
	{
		return m_chip.get_registers().get_register_data(addr);
	}

	uint8_t read_status()
	{
		return m_chip.read_status();
	}
*/

private:
	ymfm::ym2151              m_chip;
	uint32_t m_chip_clock_speed;
	uint32_t m_chip_sample_rate;
	high_resolution_clock::time_point m_lastrender;
	bool m_warning;
	
	ymfm::ym2151::output_data m_buffer[BUFFERSIZE];
	uint32_t                  m_buffer_size;
	uint32_t                  m_buffer_used;
	
	void prerender(high_resolution_clock::time_point now)
	{
		static float frameerror = 0;

		double whole, frac;

		ymfm::ym2151::output_data *buffer = &m_buffer[m_buffer_used];

		// numsamples = samples-per-unit-time * delta-t + int(frameerror)
		double dt = (duration_cast<duration<double>>(now - m_lastrender)).count();
		frameerror += std::modf(m_chip_sample_rate * dt, &whole);
		uint32_t numsamples = static_cast<uint32_t>(whole) + std::floor(frameerror);
		frameerror -= std::floor(frameerror);
		
		// now set the timestamp of the last-rendered sample:
		//duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
		//std::chrono::duration<double>(timeToSleep)
		// dt = (frac / m_chip_sample_rate);
		
		//m_lastrender = now - duration<double>dt;  // <---- want this

		m_lastrender = now; // <--- accumulates error over time. Fix.
		
		if (numsamples > BUFFERSIZE - m_buffer_used) {
			if (! m_warning) {
				std::cerr << "Warning: YMFM Buffer Overflow" << std::endl;
				m_warning = true;
			}
			numsamples = BUFFERSIZE - m_buffer_used;
		}
		else if (m_warning) {
				std::cerr << "Info: YMFM Buffer Overflow clear" << std::endl;
				m_warning = false;
		}
		
		while (numsamples > 0) {
			m_chip.generate(&m_buffer[m_buffer_used++],1);
			--numsamples;
		}
	}

	void generate(int16_t *stream, uint32_t samples)
	{
		ymfm::ym2151::output_data ym0;
		for (uint32_t i = 0 ; i < samples*2 ; i+=2) {
			m_chip.generate(&ym0, 1);
			stream[i] = ym0.data[0];
			stream[i+1] = ym0.data[1];
		}
	}
};

static ym2151_interface Ym_interface;

// present C-style accessor functions for python module
extern "C" { 
	void YM_render(int16_t *stream, uint32_t samples)
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
	
	void debug()
	{
		Ym_interface.debug(high_resolution_clock::now());
	}
}
