/*
 * Copyright (c) 2010, Joshua Lackey
 * Copyright (c) 2010-2011, Thomas Tsou
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


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include <complex>
#include <iostream>

#include "usrp_source.h"

static bool loop_file = false;

extern int g_verbosity;
FILE *fp = NULL;
char *g_filename = NULL;

bool init_file()
{
	fprintf(stdout, "Opening file %s\n", g_filename);
	fp = fopen(g_filename, "r");
	if (!fp) {
		fprintf(stderr, "Could not open file %s\n", g_filename);
		return false;
	}

	return true;
}

bool reopen_file()
{
	fclose(fp);
	fp = fopen(g_filename, "r");
}

usrp_source::usrp_source(float sample_rate,
			long int fpga_master_clock_freq,
			bool external_ref, char *filename) {

	m_desired_sample_rate = sample_rate;
	m_fpga_master_clock_freq = fpga_master_clock_freq;
	m_sample_rate = sample_rate;
	m_cb = new circular_buffer(CB_LEN, sizeof(complex), 0);
	m_recv_samples_per_packet = 512;
	g_filename = filename;
	pthread_mutex_init(&m_u_mutex, 0);
}


usrp_source::~usrp_source() {

	stop();
	delete m_cb;
	pthread_mutex_destroy(&m_u_mutex);
}


void usrp_source::stop() {

}


void usrp_source::start() {

}


float usrp_source::sample_rate() {

	return m_sample_rate;
}


int usrp_source::tune(double freq) {

	return freq;
}


void usrp_source::set_antenna(const std::string) {

}


void usrp_source::set_antenna(int) {

}


std::vector<std::string> usrp_source::get_antennas() {

	std::vector<std::string> vec;

	return vec;
}


bool usrp_source::set_gain(float) {

	return true;
}


/*
 * open() should be called before multiple threads access usrp_source.
 */
int usrp_source::open(char *) {

	return 0;
}

/*
 * Fill buffer with file data. Loop at end of file.
 */
bool fill_from_file(float *buf, size_t len)
{
	size_t num;

	if (!fp && !init_file())
		return false;

	num = fread((void *) buf, 2 * sizeof(float), len, fp);
	if (num != len) {
		if (loop_file)
			reopen_file();
		else
			return false;
	}

	return true;
}

int usrp_source::fill(unsigned int num_samples, unsigned int *overrun) {

	unsigned char ubuf[m_recv_samples_per_packet * 2 * sizeof(float)];
	float *s = (float *)ubuf;
	unsigned int i, j, space, overrun_cnt;
	complex *c;
	bool overrun_pkt;

	overrun_cnt = 0;

	while ((m_cb->data_available() < num_samples)
			&& m_cb->space_available() > 0) {

		pthread_mutex_lock(&m_u_mutex);

		if (!fill_from_file(s, m_recv_samples_per_packet)) {
			fprintf(stderr, "End of file or error\n");
			return -1;
		}

		pthread_mutex_unlock(&m_u_mutex);

		// write complex<short> input to complex<float> output
		c = (complex *)m_cb->poke(&space);

		// set space to number of complex items to copy
		if(space > m_recv_samples_per_packet)
			space = m_recv_samples_per_packet;

		// write data
		for(i = 0, j = 0; i < space; i += 1, j += 2)
			c[i] = complex(s[j], s[j + 1]);

		// update cb
		m_cb->wrote(i);
	}

	// if the cb is full, we left behind data from the usb packet
	if(m_cb->space_available() == 0) {
		fprintf(stderr, "warning: local overrun\n");
	}

	if (overrun)
		*overrun = overrun_cnt;

	return 0;
}


int usrp_source::read(complex *buf, unsigned int num_samples, unsigned int *samples_read) {

	unsigned int n;

	if(fill(num_samples, 0))
		return -1;

	n = m_cb->read(buf, num_samples);

	if(samples_read)
		*samples_read = n;

	return 0;
}


/*
 * Don't hold a lock on this and use the usrp at the same time.
 */
circular_buffer *usrp_source::get_buffer() {

	return m_cb;
}


int usrp_source::flush(unsigned int flush_count) {

	m_cb->flush();
	fill(flush_count * m_recv_samples_per_packet * 2 * sizeof(short), 0);
	m_cb->flush();

	return 0;
}
