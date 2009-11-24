/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGlobal>

#ifndef Q_OS_WIN
#include <sndfile.h>
#endif
#include <string.h>

#include "sound-file.h"
#include "debug.h"

/**
 * @ingroup sound
 * @{
 */
SoundFile::SoundFile(const char *path):length(0),data(NULL),channels(-1),speed(0)
{
#ifndef Q_OS_WIN
	SF_INFO info;
	memset(&info, 0, sizeof(info));
	SNDFILE *f = sf_open(path, SFM_READ, &info);
	if (!f)
	{
		fprintf(stderr, "cannot open file '%s'\n", path);
		return;
	}
	kdebugm(KDEBUG_INFO, "frames:\t\t%lu\n", info.frames);
	kdebugm(KDEBUG_INFO, "samplerate:\t%d\n", info.samplerate);
	kdebugm(KDEBUG_INFO, "channels:\t%d\n", info.channels);
	kdebugm(KDEBUG_INFO, "format:\t\t0x%x\n", info.format);
	kdebugm(KDEBUG_INFO, "sections:\t%d\n", info.sections);
	kdebugm(KDEBUG_INFO, "seekable:\t%d\n", info.seekable);
	
	length = info.frames;
	channels = info.channels;
	speed = info.samplerate;
	
	long format = info.format & SF_FORMAT_SUBMASK;

	if (format == SF_FORMAT_FLOAT || format == SF_FORMAT_DOUBLE)
	{
		length *= channels;
		data = new qint16[length];
		float *buffer = new float [length];
		double scale;

		sf_command (f, SFC_CALC_SIGNAL_MAX, &scale, sizeof (scale)) ;
		if (scale < 1e-10)
			scale = 1.0 ;
		else
			scale = 32700.0 / scale;

		int readcount = sf_read_float (f, buffer, length);
		for (int m = 0; m < readcount; ++m)
			data [m] = qint16(scale * buffer [m]);
		delete buffer;
	}
	else
	{
		length *= channels;
		data = new qint16[length];
		sf_read_short (f, data, length);
	}

	sf_close(f);
#endif
}

#define SAMPLE_MAX 0x7fff
#define SAMPLE_MIN -0x7ffe

void SoundFile::setVolume(qint16 *data, int length, float vol)
{
	short *end=data+length;
	while (data!=end)
	{
		float tmp = vol * *data;
		if (tmp > SAMPLE_MAX)
			tmp = SAMPLE_MAX;
		else if (tmp < SAMPLE_MIN)
			tmp = SAMPLE_MIN;
		*data++=(qint16)tmp;
	}
}

void SoundFile::setVolume(float vol)
{
	setVolume(data, length, vol);
}

SoundFile::~SoundFile()
{
	if (data != 0)
	{
		delete [] data;
		data = NULL;
	}
}

bool SoundFile::isOk()
{
	return (length!=0);
}

/** @} */

