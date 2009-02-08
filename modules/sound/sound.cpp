/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QMetaType>
#include <QtGui/QApplication>
#include <QtGui/QGridLayout>
#include <QtGui/QPushButton>

#include "gui/widgets/configuration/configuration-window.h"
#include "gui/widgets/configuration/config-combo-box.h"
#include "path_list_edit.h"
#include "chat_widget.h"
#include "chat_manager.h"
#include "config_file.h"
#include "debug.h"
#include "sound.h"
#include "kadu.h"
#include "kadu_parser.h"
#include "misc.h"
#include "../notify/notify.h"
#include "sound_file.h"

#include "sound_slots.h"

/**
 * @ingroup sound
 * @{
 */
SoundManager* sound_manager = NULL;
SoundSlots* sound_slots;

extern "C" KADU_EXPORT int sound_init(bool firstLoad)
{
	kdebugf();

	new SoundManager(firstLoad, "sounds", "sound.conf");
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/sound.ui"), sound_manager);

	qRegisterMetaType<SoundDevice>("SoundDevice");
	qRegisterMetaType<SoundDeviceType>("SoundDeviceType");

	kdebugf2();
	return 0;
}

extern "C" KADU_EXPORT void sound_close()
{
	kdebugf();
	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/sound.ui"), sound_manager);

	delete sound_manager;
	sound_manager = 0;
	kdebugf2();
}

SamplePlayThread::SamplePlayThread(SoundDevice device)
	: Device(device), Sample(0), SampleLen(0), Stopped(false),
	PlayingSemaphore(1), SampleSemaphore(1)
{
	kdebugf();
	PlayingSemaphore.acquire();
	kdebugf2();
}

void SamplePlayThread::run()
{
	kdebugf();
	for(;;)
	{
		PlayingSemaphore.acquire();
		if (Stopped)
		{
			SampleSemaphore.release();
			break;
		}
		bool result;
		emit sound_manager->playSampleImpl(Device, Sample, SampleLen, &result);
		QApplication::postEvent(this, new QCustomEvent(QEvent::User, Device));
		SampleSemaphore.release();
	}
	kdebugf2();
}

bool SamplePlayThread::event(QEvent* event)
{
	if (event->type() == QEvent::User)
		emit samplePlayed((SoundDevice)(((QCustomEvent *)event)->data()));
	return true;
}

void SamplePlayThread::playSample(const int16_t* data, int length)
{
	kdebugf();

	SampleSemaphore.acquire();
	Sample = data;
	SampleLen = length;
	PlayingSemaphore.release();
	kdebugf2();
}

void SamplePlayThread::stop()
{
	kdebugf();

	SampleSemaphore.acquire();
	Stopped = true;
	PlayingSemaphore.release();
	if (!wait(5000))
	{
		kdebugm(KDEBUG_ERROR, "deadlock :|, terminating SamplePlayThread\n");
		this->terminate();
		wait(1000);
	}
	kdebugf2();
}

SampleRecordThread::SampleRecordThread(SoundDevice device)
	: Device(device), Sample(0), SampleLen(0), Stopped(false),
	RecordingSemaphore(1), SampleSemaphore(1)
{
	kdebugf();
	RecordingSemaphore.acquire();
	kdebugf2();
}

void SampleRecordThread::run()
{
	kdebugf();
	for(;;)
	{
		RecordingSemaphore.acquire();
		if (Stopped)
		{
			SampleSemaphore.release();
			break;
		}
		bool result;
		emit sound_manager->recordSampleImpl(Device, Sample, SampleLen, &result);
		QApplication::postEvent(this, new QCustomEvent(QEvent::User, Device));
		SampleSemaphore.release();
	}
	kdebugf2();
}

bool SampleRecordThread::event(QEvent* event)
{
	if (event->type() == QEvent::User)
		emit sampleRecorded((SoundDevice)(((QCustomEvent *)event)->data()));
	return true;
}

void SampleRecordThread::recordSample(int16_t* data, int length)
{
	kdebugf();
	SampleSemaphore.acquire();
	Sample = data;
	SampleLen = length;
	RecordingSemaphore.release();
	kdebugf2();
}

void SampleRecordThread::stop()
{
	kdebugf();
	SampleSemaphore.acquire();
	Stopped = true;
	RecordingSemaphore.release();
	if (!wait(5000))
	{
		kdebugm(KDEBUG_ERROR, "deadlock :|, terminating SampleRecordThread\n");
		this->terminate();
		wait(1000);
	}
	kdebugf2();
}

SoundManager::SoundManager(bool firstLoad, const QString& name, const QString& configname) : Notifier(),
	themes(new Themes(name, configname)),
	lastsoundtime(), mute(false), PlayingThreads(), RecordingThreads(),
	play_thread(new SoundPlayThread()), simple_player_count(0)
{
	kdebugf();

	createDefaultConfiguration();

	lastsoundtime.start();

	play_thread->start();

	sound_manager = this;
	sound_slots = new SoundSlots(firstLoad, this);

	themes->setPaths(QStringList::split(QRegExp("(;|:)"), config_file.readEntry("Sounds", "SoundPaths")));

	QStringList soundThemes = themes->themes();
	QString soundTheme = config_file.readEntry("Sounds", "SoundTheme");
	if (!soundThemes.isEmpty() && (soundTheme != "Custom") && !soundThemes.contains(soundTheme))
	{
		soundTheme = "default";
		config_file.writeEntry("Sounds", "SoundTheme", "default");
	}

	if (soundTheme != "custom")
		applyTheme(soundTheme);

	notification_manager->registerNotifier(QT_TRANSLATE_NOOP("@default", "Sound"), this);

	kdebugf2();
}

SoundManager::~SoundManager()
{
	kdebugf();
	play_thread->endThread();

	delete sound_slots;
	sound_slots = NULL;

	notification_manager->unregisterNotifier("Sound");

	play_thread->wait(2000);
	if (play_thread->running())
	{
		kdebugm(KDEBUG_WARNING, "terminating play_thread!\n");
		play_thread->terminate();
	}
	delete play_thread;
	delete themes;

	kdebugf2();
}

void SoundManager::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	connect(mainConfigurationWindow, SIGNAL(configurationWindowApplied()), this, SLOT(configurationWindowApplied()));

	connect(mainConfigurationWindow->widgetById("sound/use"), SIGNAL(toggled(bool)),
		mainConfigurationWindow->widgetById("sound/theme"), SLOT(setEnabled(bool)));
	connect(mainConfigurationWindow->widgetById("sound/use"), SIGNAL(toggled(bool)),
		mainConfigurationWindow->widgetById("sound/volume"), SLOT(setEnabled(bool)));
	connect(mainConfigurationWindow->widgetById("sound/use"), SIGNAL(toggled(bool)),
		mainConfigurationWindow->widgetById("sound/samples"), SLOT(setEnabled(bool)));

	connect(mainConfigurationWindow->widgetById("sound/enableVolumeControl"), SIGNAL(toggled(bool)),
		mainConfigurationWindow->widgetById("sound/volumeControl"), SLOT(setEnabled(bool)));

	connect(mainConfigurationWindow->widgetById("sound/testPlay"), SIGNAL(clicked()), sound_slots, SLOT(testSamplePlaying()));
	connect(mainConfigurationWindow->widgetById("sound/testRecord"), SIGNAL(clicked()), sound_slots, SLOT(testSampleRecording()));
	connect(mainConfigurationWindow->widgetById("sound/testDuplex"), SIGNAL(clicked()), sound_slots, SLOT(testFullDuplex()));

	themesComboBox = dynamic_cast<ConfigComboBox *>(mainConfigurationWindow->widgetById("sound/themes"));
	connect(themesComboBox, SIGNAL(activated(int)), configurationWidget, SLOT(themeChanged(int)));
	connect(themesComboBox, SIGNAL(activated(const QString &)), sound_slots, SLOT(themeChanged(const QString &)));
	configurationWidget->themeChanged(themesComboBox->currentItem());

	themesPaths = dynamic_cast<PathListEdit *>(mainConfigurationWindow->widgetById("soundPaths"));
	connect(themesPaths, SIGNAL(changed()), sound_manager, SLOT(setSoundThemes()));

	setSoundThemes();
}

NotifierConfigurationWidget *SoundManager::createConfigurationWidget(QWidget *parent, char *name)
{
	configurationWidget = new SoundConfigurationWidget(parent, name);
	return configurationWidget;
}

void SoundManager::setSoundThemes()
{
	themes->setPaths(themesPaths->pathList());

	QStringList soundThemeNames = themes->themes();
	soundThemeNames.sort();

	QStringList soundThemeValues = soundThemeNames;

	soundThemeNames.prepend(tr("Custom"));
	soundThemeValues.prepend("Custom");

	themesComboBox->setItems(soundThemeValues, soundThemeNames);
	themesComboBox->setCurrentText(themes->theme());
}

void SoundManager::configurationWindowApplied()
{
	kdebugf();

	if (themesComboBox->currentItem() != 0)
		applyTheme(themesComboBox->currentText());

	configurationWidget->themeChanged(themesComboBox->currentIndex());
}

void SoundManager::createDefaultConfiguration()
{
	config_file.addVariable("Notify", "ConnectionError_Sound", true);
	config_file.addVariable("Notify", "NewChat_Sound", true);
	config_file.addVariable("Notify", "NewMessage_Sound", true);
	config_file.addVariable("Notify", "StatusChanged/ToOnline_Sound", true);
	config_file.addVariable("Notify", "StatusChanged/ToBusy_Sound", true);
	config_file.addVariable("Notify", "FileTransfer/IncomingFile_Sound", true);

	config_file.addVariable("Sounds", "PlaySound", true);
	config_file.addVariable("Sounds", "SoundPaths", "");
	config_file.addVariable("Sounds", "SoundTheme", "default");
	config_file.addVariable("Sounds", "SoundVolume", 100);
	config_file.addVariable("Sounds", "VolumeControl", false);
}

void SoundManager::applyTheme(const QString &themeName)
{
	themes->setTheme(themeName);
	QMap<QString, QString> entries = themes->getEntries();
	QMap<QString, QString>::const_iterator i = entries.constBegin();

	while (i != entries.constEnd()) {
		config_file.writeEntry("Sounds", i.key() + "_sound", themes->themePath() + i.value());
		++i;
	}
}

Themes *SoundManager::theme()
{
	return themes;
}

bool SoundManager::isMuted() const
{
	return mute;
}

void SoundManager::setMute(const bool& enable)
{
	mute = enable;
}

void SoundManager::playSound(const QString &soundName)
{
	if (isMuted())
	{
		kdebugmf(KDEBUG_FUNCTION_END, "end: muted\n");
		return;
	}

	if (timeAfterLastSound() < 500)
	{
		kdebugmf(KDEBUG_FUNCTION_END, "end: too often, exiting\n");
		return;
	}

	QString sound = config_file.readEntry("Sounds", soundName + "_sound");

	if (QFile::exists(sound))
	{
		play(sound, config_file.readBoolEntry("Sounds","VolumeControl"), 1.0 * config_file.readDoubleNumEntry("Sounds", "SoundVolume") / 100);
		lastsoundtime.restart();
	}
	else
		fprintf(stderr, "file (%s) not found\n", qPrintable(sound));
}

void SoundManager::notify(Notification *notification)
{
	kdebugf();

	playSound(notification->type());

	kdebugf2();
}

void SoundManager::play(const QString &path, bool force)
{
	kdebugf();

	if (isMuted() && !force)
	{
		kdebugmf(KDEBUG_FUNCTION_END, "end: muted\n");
		return;
	}

	if (QFile::exists(path))
		play(path, config_file.readBoolEntry("Sounds","VolumeControl"), 1.0 * config_file.readDoubleNumEntry("Sounds", "SoundVolume") / 100);
	else
		fprintf(stderr, "file (%s) not found\n", qPrintable(path));

	kdebugf2();
}

int SoundManager::timeAfterLastSound() const
{
	return lastsoundtime.elapsed();
}

SoundDevice SoundManager::openDevice(SoundDeviceType type, int sample_rate, int channels)
{
	kdebugf();
	SoundDevice device=NULL;
	emit openDeviceImpl(type, sample_rate, channels, &device);
	kdebugf2();
	return device;
}

void SoundManager::closeDevice(SoundDevice device)
{
	kdebugf();
	if (PlayingThreads.contains(device))
	{
		SamplePlayThread* playing_thread = PlayingThreads[device];
		disconnect(playing_thread, SIGNAL(samplePlayed(SoundDevice)), this, SIGNAL(samplePlayed(SoundDevice)));
		playing_thread->stop();
		PlayingThreads.remove(device);
		playing_thread->deleteLater();
	}
	if (RecordingThreads.contains(device))
	{
		SampleRecordThread* recording_thread = RecordingThreads[device];
		disconnect(recording_thread, SIGNAL(sampleRecorded(SoundDevice)), this, SIGNAL(sampleRecorded(SoundDevice)));
		recording_thread->stop();
		RecordingThreads.remove(device);
		recording_thread->deleteLater();
	}
	emit closeDeviceImpl(device);

	kdebugf2();
}

void SoundManager::enableThreading(SoundDevice device)
{
	kdebugf();
	if (!PlayingThreads.contains(device))
	{
		SamplePlayThread* playing_thread = new SamplePlayThread(device);
		connect(playing_thread, SIGNAL(samplePlayed(SoundDevice)), this, SIGNAL(samplePlayed(SoundDevice)));
		playing_thread->start();
		PlayingThreads.insert(device, playing_thread);
	}
	if (!RecordingThreads.contains(device))
	{
		SampleRecordThread* recording_thread = new SampleRecordThread(device);
		connect(recording_thread, SIGNAL(sampleRecorded(SoundDevice)), this, SIGNAL(sampleRecorded(SoundDevice)));
		recording_thread->start();
		RecordingThreads.insert(device, recording_thread);
	}
	kdebugf2();
}

void SoundManager::setFlushingEnabled(SoundDevice device, bool enabled)
{
	kdebugf();
	emit setFlushingEnabledImpl(device, enabled);
	kdebugf2();
}

bool SoundManager::playSample(SoundDevice device, const int16_t* data, int length)
{
	kdebugf();
	bool result;
	if (PlayingThreads.contains(device))
	{
		PlayingThreads[device]->playSample(data, length);
		result = true;
	}
	else
		emit playSampleImpl(device, data, length, &result);
	kdebugf2();
	return result;
}

bool SoundManager::recordSample(SoundDevice device, int16_t* data, int length)
{
	kdebugf();
	bool result;
	if (RecordingThreads.contains(device))
	{
		RecordingThreads[device]->recordSample(data, length);
		result = true;
	}
	else
		emit recordSampleImpl(device, data, length, &result);
	kdebugf2();
	return result;
}

// stupid Qt, yes this code work
void SoundManager::connectNotify(const char *signal)
{
//	kdebugm(KDEBUG_INFO, ">>> %s %s\n", signal, SIGNAL(playSound(QString&,bool,double)) );
	if (strcmp(signal,SIGNAL(playSound(QString,bool,double)))==0)
		++simple_player_count;
}

void SoundManager::disconnectNotify(const char *signal)
{
//	kdebugm(KDEBUG_INFO, ">>> %s %s\n", signal, SIGNAL(playSound(QString&,bool,double)) );
	if (strcmp(signal,SIGNAL(playSound(QString,bool,double)))==0)
		--simple_player_count;
}

void SoundManager::play(const QString &path, bool volCntrl, double vol)
{
	kdebugf();
	if (simple_player_count>0)
		emit playSound(path, volCntrl, vol);
	else
		play_thread->tryPlay(qPrintable(path), volCntrl, vol);
	kdebugf2();
}

void SoundManager::stop()
{
	kdebugf();
// 	if (simple_player_count>0)
// 		emit playStop();
// 	else
	{
		play_thread->terminate();
		play_thread->wait();

		// TODO: fix it, let play_thread exists only if needed
		delete play_thread;
		play_thread = new SoundPlayThread();
		play_thread->start();
	}
	kdebugf2();
}

SoundPlayThread::SoundPlayThread() : QThread(),
	mutex(), semaphore(new QSemaphore(100)), end(false), list()
{
	(*semaphore).acquire(100);
}

SoundPlayThread::~SoundPlayThread()
{
	if (semaphore)
	{
		delete semaphore;
		semaphore = 0;
	}
}

void SoundPlayThread::tryPlay(const char *path, bool volumeControl, float volume)
{
	kdebugf();
	if (mutex.tryLock())
	{
		list.push_back(SndParams(path, volumeControl, volume));
		mutex.unlock();
		(*semaphore).release();
	}
	kdebugf2();
}

void SoundPlayThread::run()
{
	kdebugf();
	while (!end)
	{
		(*semaphore).acquire();
		mutex.lock();
		kdebugmf(KDEBUG_INFO, "locked\n");
		if (end)
		{
			mutex.unlock();
			break;
		}
		SndParams params=list.first();
		list.pop_front();

		play(qPrintable(params.filename),
				params.volumeControl, params.volume);
		mutex.unlock();
		kdebugmf(KDEBUG_INFO, "unlocked\n");
	}//end while(!end)
	kdebugf2();
}

bool SoundPlayThread::play(const char *path, bool volumeControl, float volume)
{
	bool ret = false;
	SoundFile *sound = new SoundFile(path);

	if (!sound->isOk())
	{
		fprintf(stderr, "broken sound file?\n");
		delete sound;
		return false;
	}

	kdebugm(KDEBUG_INFO, "\n");
	kdebugm(KDEBUG_INFO, "length:   %d\n", sound->length);
	kdebugm(KDEBUG_INFO, "speed:    %d\n", sound->speed);
	kdebugm(KDEBUG_INFO, "channels: %d\n", sound->channels);

	if (volumeControl)
		sound->setVolume(volume);

	SoundDevice dev;
	dev = sound_manager->openDevice(PLAY_ONLY, sound->speed, sound->channels);
	sound_manager->setFlushingEnabled(dev, true);
	ret = sound_manager->playSample(dev, sound->data, sound->length*sizeof(sound->data[0]));
	sound_manager->closeDevice(dev);

	delete sound;
	return ret;
}

void SoundPlayThread::endThread()
{
	mutex.lock();
	end = true;
	mutex.unlock();
	(*semaphore).release();
}

SndParams::SndParams(QString fm, bool volCntrl, float vol) :
			filename(fm), volumeControl(volCntrl), volume(vol)
{
}

SndParams::SndParams(const SndParams &p) : filename(p.filename),
						volumeControl(p.volumeControl), volume(p.volume)
{
}

/** @} */

