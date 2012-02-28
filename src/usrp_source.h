/*
 * Copyright (c) 2010, Joshua Lackey
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *     *  Redistributions of source code must retain the above copyright
 *        notice, this list of conditions and the following disclaimer.
 *
 *     *  Redistributions in binary form must reproduce the above copyright
 *        notice, this list of conditions and the following disclaimer in the
 *        documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"

#include <uhd/usrp/multi_usrp.hpp>

#include "usrp_complex.h"
#include "circular_buffer.h"


class usrp_source {
public:
	usrp_source(float sample_rate,
		long int fpga_master_clock_freq = 100000000,
		bool external_ref = false);

	~usrp_source();

	int open(unsigned int subdev);
	int read(complex *buf,
		unsigned int num_samples,
		unsigned int *samples_read);

	int fill(unsigned int num_samples, unsigned int *overrun);
	int tune(double freq);
	void set_antenna(int antenna);
	void set_antenna(const std::string antenna);
	std::vector<std::string> get_antennas();
	bool set_gain(float gain);
	void start();
	void stop();
	int flush(unsigned int flush_count = FLUSH_COUNT);
	circular_buffer *get_buffer();

	float sample_rate();

private:
	uhd::usrp::multi_usrp::sptr	m_dev;
#ifdef UHD_3_4_X
	uhd::rx_streamer::sptr		m_rx_stream;
#endif

	float				m_sample_rate;
	float				m_desired_sample_rate;
	bool				m_external_ref;
	unsigned int			m_recv_samples_per_packet;
	long int			m_fpga_master_clock_freq;

	circular_buffer *		m_cb;

	/*
	 * This mutex protects access to the USRP and daughterboards but not
	 * necessarily to any fields in this class.
	 */
	pthread_mutex_t			m_u_mutex;

	static const unsigned int	FLUSH_COUNT	= 10;
	static const unsigned int	CB_LEN		= (1 << 20);
	static const int		NCHAN		= 1;
};
