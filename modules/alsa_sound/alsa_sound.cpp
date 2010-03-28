/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2005, 2006 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2007, 2008, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <sys/time.h>

#include "alsa_sound.h"
#include "configuration/configuration-file.h"
#include "debug.h"
#include "../sound/sound.h"

/**
 * @ingroup alsa_sound
 * @{
 */
struct ALSADevice
{
	snd_pcm_t *player;

	int channels;

	ALSADevice() : player(NULL), channels(-1)
	{
	}
};

extern "C" int alsa_sound_init(bool firstLoad)
{
	Q_UNUSED(firstLoad
		 )
	kdebugf();

	alsa_player_slots = new ALSAPlayerSlots;
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/alsa_sound.ui"));

	kdebugf2();
	return 0;
}

extern "C" void alsa_sound_close()
{
	kdebugf();

	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/alsa_sound.ui"));
	delete alsa_player_slots;
	alsa_player_slots = 0;

	kdebugf2();
}

snd_pcm_t *ALSAPlayerSlots::alsa_open (const char *device, int channels, int samplerate, bool play)
{
	kdebugf();
	snd_pcm_t *alsa_dev;
	snd_pcm_hw_params_t *hw_params;
	snd_pcm_uframes_t buffer_size, start_threshold;
	snd_pcm_uframes_t alsa_period_size, alsa_buffer_frames;
	snd_pcm_sw_params_t *sw_params;

	int err;

	alsa_period_size = 512;
	alsa_buffer_frames = 3 * alsa_period_size;

	// we need non-blocking behaviour only while opening device
	if ((err = snd_pcm_open (&alsa_dev, device, play?SND_PCM_STREAM_PLAYBACK:SND_PCM_STREAM_CAPTURE, SND_PCM_NONBLOCK)) < 0)
	{
		fprintf(stderr, "cannot open audio device \"%s\" (%s)\n", device, snd_strerror (err));
		fflush(stderr);
		return NULL;
	}
	// so after open we are immediately setting device to blocking mode
	snd_pcm_nonblock(alsa_dev, 0);
	kdebugm(KDEBUG_INFO, "device opened\n");

	if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0)
	{
		fprintf(stderr, "cannot allocate hardware parameter structure (%s)\n", snd_strerror (err));
		fflush(stderr);
		snd_pcm_close (alsa_dev);
		return NULL;
	}
	kdebugm(KDEBUG_INFO, "struct allocated\n");

	if ((err = snd_pcm_hw_params_any (alsa_dev, hw_params)) < 0)
	{
		fprintf(stderr, "cannot initialize hardware parameter structure (%s)\n", snd_strerror (err));
		fflush(stderr);
		snd_pcm_close (alsa_dev);
		return NULL;
	}
	kdebugm(KDEBUG_INFO, "struct initialized\n");

	if ((err = snd_pcm_hw_params_set_access (alsa_dev, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0)
	{
		fprintf(stderr, "cannot set access type (%s)\n", snd_strerror (err));
		fflush(stderr);
		snd_pcm_close (alsa_dev);
		return NULL;
	}
	kdebugm(KDEBUG_INFO, "access type set\n");

	if ((err = snd_pcm_hw_params_set_format (alsa_dev, hw_params, SND_PCM_FORMAT_S16_LE)) < 0)
	{
		fprintf(stderr, "cannot set sample format (%s)\n", snd_strerror (err));
		fflush(stderr);
		snd_pcm_close (alsa_dev);
		return NULL;
	}
	kdebugm(KDEBUG_INFO, "sample format set\n");

	if ((err = snd_pcm_hw_params_set_rate_near (alsa_dev, hw_params, (unsigned int*)&samplerate, 0)) < 0)
	{
		fprintf(stderr, "cannot set sample rate (%s)\n", snd_strerror (err));
		fflush(stderr);
		snd_pcm_close (alsa_dev);
		return NULL;
	}
	kdebugm(KDEBUG_INFO, "sample rate set\n");

	if ((err = snd_pcm_hw_params_set_channels (alsa_dev, hw_params, channels)) < 0)
	{
		fprintf(stderr, "cannot set channel count (%s)\n", snd_strerror (err));
		fflush(stderr);
		snd_pcm_close (alsa_dev);
		return NULL;
	}
	kdebugm(KDEBUG_INFO, "channel count set\n");

	if ((err = snd_pcm_hw_params_set_buffer_size_near (alsa_dev, hw_params, &alsa_buffer_frames)) < 0)
	{
		fprintf(stderr, "cannot set buffer size (%s)\n", snd_strerror (err));
		fflush(stderr);
		snd_pcm_close (alsa_dev);
		return NULL;
	}
	kdebugm(KDEBUG_INFO, "buffer size set\n");

	if ((err = snd_pcm_hw_params_set_period_size_near (alsa_dev, hw_params, &alsa_period_size, 0)) < 0)
	{
		fprintf(stderr, "cannot set period size (%s)\n", snd_strerror (err));
		fflush(stderr);
		snd_pcm_close (alsa_dev);
		return NULL;
	}
	kdebugm(KDEBUG_INFO, "period size set\n");

	if ((err = snd_pcm_hw_params (alsa_dev, hw_params)) < 0)
	{
		fprintf(stderr, "cannot set parameters (%s)\n", snd_strerror (err));
		fflush(stderr);
		snd_pcm_close (alsa_dev);
		return NULL;
	}
	kdebugm(KDEBUG_INFO, "parameters set\n");

	/* extra check: if we have only one period, this code won't work */
	snd_pcm_hw_params_get_period_size (hw_params, &alsa_period_size, 0);
	snd_pcm_hw_params_get_buffer_size (hw_params, &buffer_size);
	if (alsa_period_size == buffer_size)
	{
		fprintf(stderr, "Can't use period equal to buffer size (%lu == %lu)\n", alsa_period_size, buffer_size);
		fflush(stderr);
		snd_pcm_close (alsa_dev);
		return NULL;
	}
	kdebugm(KDEBUG_INFO, "period correctly set\n");


	snd_pcm_hw_params_free (hw_params);
	kdebugm(KDEBUG_INFO, "struct freed\n");

	if ((err = snd_pcm_sw_params_malloc (&sw_params)) != 0)
	{
		fprintf(stderr, "snd_pcm_sw_params_malloc: %s\n", snd_strerror (err));
		fflush(stderr);
		snd_pcm_close (alsa_dev);
		return NULL;
	}
	kdebugm(KDEBUG_INFO, "another struct allocated\n");

	if ((err = snd_pcm_sw_params_current (alsa_dev, sw_params)) != 0)
	{
		fprintf(stderr, "snd_pcm_sw_params_current: %s\n", snd_strerror (err));
		fflush(stderr);
		snd_pcm_close (alsa_dev);
		return NULL;
	}
	kdebugm(KDEBUG_INFO, "params got\n");

	/* round up to closest transfer boundary */
	start_threshold = 1;
	if ((err = snd_pcm_sw_params_set_start_threshold (alsa_dev, sw_params, start_threshold)) < 0)
	{
		fprintf(stderr, "cannot set start threshold (%s)\n", snd_strerror (err));
		fflush(stderr);
		snd_pcm_close (alsa_dev);
		return NULL;
	}
	kdebugm(KDEBUG_INFO, "threshold set\n");

	if ((err = snd_pcm_sw_params (alsa_dev, sw_params)) != 0)
	{
		fprintf(stderr, "snd_pcm_sw_params: %s\n", snd_strerror (err));
		fflush(stderr);
		snd_pcm_close (alsa_dev);
		return NULL;
	}
	kdebugm(KDEBUG_INFO, "params ok\n");

	snd_pcm_sw_params_free (sw_params);
	kdebugm(KDEBUG_INFO, "struct freed\n");

	snd_pcm_reset (alsa_dev);

	kdebugf2();
	return alsa_dev;
}

ALSAPlayerSlots::ALSAPlayerSlots(QObject *parent) : QObject(parent)
{
	kdebugf();

	createDefaultConfiguration();

	kdebugf2();
}

ALSAPlayerSlots::~ALSAPlayerSlots()
{
	kdebugf();

	kdebugf2();
}

void ALSAPlayerSlots::createDefaultConfiguration()
{
	config_file.addVariable("Sounds", "ALSAOutputDevice", "default");
}

bool ALSAPlayerSlots::isOk()
{
	kdebugf();
	snd_pcm_t *dev = alsa_open(qPrintable(config_file.readEntry("Sounds", "ALSAOutputDevice")), 1, 8000);
	bool ret = dev!=NULL;
	if (dev)
		snd_pcm_close (dev);
	kdebugf2();
	return ret;
}

void ALSAPlayerSlots::openDevice(SoundDeviceType type, int sample_rate, int channels, SoundDevice* device)
{
	kdebugf();

	ALSADevice *dev = new ALSADevice();
	if (!dev)
		return;

	if (type == SoundDevicePlayOnly)
	{
		dev->player = alsa_open (qPrintable(config_file.readEntry("Sounds", "ALSAOutputDevice")), channels, sample_rate, true);
		if (!dev->player)
		{
			delete dev;
			device = 0;
			kdebugmf(KDEBUG_FUNCTION_END|KDEBUG_WARNING,"end: cannot open play device\n");
			return;
		}
	}

	dev->channels = channels;
	*device = (SoundDevice)dev;
	kdebugf2();
}

void ALSAPlayerSlots::closeDevice(SoundDevice device)
{
	kdebugf();
	ALSADevice *dev = (ALSADevice*)device;
	if (!dev)
		return;
	if (dev->player)
		snd_pcm_close (dev->player);
	delete dev;
	kdebugf2();
}

int xrun_recovery(snd_pcm_t *handle, int err)
{
	kdebugmf(KDEBUG_FUNCTION_START, "start: err:%d -EPIPE:%d -ESTRPIPE:%d\n", err, -EPIPE, -ESTRPIPE);
	if (err == -EPIPE)
	{
		err = snd_pcm_prepare(handle);
		if (err < 0)
			kdebugm(KDEBUG_WARNING, "can't recovery from underrun, prepare failed: %s\n", snd_strerror(err));
		kdebugf2();
		return 0;
	}
	else if (err == -ESTRPIPE)
	{
		while ((err = snd_pcm_resume(handle)) == -EAGAIN)
			sleep(1);   /* wait until the suspend flag is released */
		if (err < 0)
		{
			err = snd_pcm_prepare(handle);
			if (err < 0)
				kdebugm(KDEBUG_WARNING, "can't recovery from suspend, prepare failed: %s\n", snd_strerror(err));
		}
		kdebugf2();
		return 0;
	}
	kdebugf2();
	return err;
}

void ALSAPlayerSlots::playSample(SoundDevice device, const int16_t* data, int length, bool* result)
{
//	kdebugf();
	ALSADevice *dev = (ALSADevice*)device;
	*result = (dev!=NULL && dev->player!=NULL);
	const char *cdata = (const char *)data;
	if (*result)
	{
		int res, written = 0;
		int availErrorsCount = 0;
		while (written < length)
		{
			if ((res = snd_pcm_wait(dev->player, 100)) < 0)
				xrun_recovery(dev->player, res);
			kdebugm(KDEBUG_DUMP, "snd_pcm_wait(player): %d\n", res);
			int towrite = (length - written) / (2 * dev->channels);
			int avail = snd_pcm_avail_update(dev->player);
			kdebugm(KDEBUG_DUMP, "snd_pcm_avail_update(player): %d\n", avail);
			if (avail < 0)
			{
				xrun_recovery(dev->player, avail);
				avail = snd_pcm_avail_update(dev->player);
				kdebugm(KDEBUG_DUMP, "snd_pcm_avail_update(player): %d\n", avail);
			}

			if (avail <= 0)
			{
				kdebugm(KDEBUG_WARNING, "player avail error: %d\n", avail);
				++availErrorsCount;
				avail = 0;
			}
			else
				availErrorsCount = 0;

			if (availErrorsCount > 10)
			{
				*result = false;
				break;
			}

			if (avail < towrite)
				towrite = avail;

			kdebugm(KDEBUG_DUMP, "playing %d frames, bytes already played: %d\n", towrite, written);
			res = snd_pcm_writei(dev->player, cdata + written, towrite);
			kdebugm(KDEBUG_DUMP, "played: %d\n", res);
			if (res == -EAGAIN || res == -EINVAL)
				// don't know why it is needed when we get EINVAL, but it works...
				continue;
			if (res < 0)
			{
				if (xrun_recovery(dev->player, res) < 0)
				{
					fprintf(stderr, "alsa write error: %s\n", snd_strerror(res));
					fflush(stderr);
					*result = false;
					break;
				}
			}
			else
				written += res * 2 * dev->channels;
		}
	}
	else
		kdebugmf(KDEBUG_ERROR, "device closed!\n");
}

void ALSAPlayerSlots::setFlushingEnabled(SoundDevice device, bool /*enabled*/)
{
	kdebugf();
	ALSADevice *dev = (ALSADevice*)device;
	if (!dev)
		return;
/*	if (dev->player)
		snd_pcm_nonblock (dev->player, !enabled);*/
}

ALSAPlayerSlots *alsa_player_slots;

/** @} */

