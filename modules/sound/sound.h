#ifndef KADU_SOUND_H
#define KADU_SOUND_H

#include <qobject.h>
#include <qstring.h>
#include <qdatetime.h>
#include <qstringlist.h>
#include <qmap.h>

#include "config_file.h"
#include "modules.h"
#include "gadu.h"
#include "userlist.h"
#include "misc.h"

class SoundSlots : public QObject
{
	Q_OBJECT
	private:
		int muteitem;
		QMap<QString, QString> soundfiles;
		QStringList soundNames;
		QStringList soundTexts;

	private slots:
		void soundPlayer(bool value, bool toolbarChanged=false);
		void onCreateConfigDialog();
		void onApplyConfigDialog();
		void chooseSoundTheme(const QString& string);
		void chooseSoundFile();
		void clearSoundFile();
		void testSoundFile();
		void selectedPaths(const QStringList& paths);
		void muteUnmuteSounds();
	public:
		SoundSlots(QObject *parent=0, const char *name=0);
		~SoundSlots();
};

class SoundManager : public Themes
{
    Q_OBJECT
	private:
		QTime lastsoundtime;
		bool mute;

	private slots:
		void newChat(const UinsList &senders, const QString& msg, time_t time);
		void newMessage(const UinsList &senders, const QString& msg, time_t time, bool &grab);
		void connectionError(const QString &message);
		void userChangedStatusToAvailable(const UserListElement &ule);
		void userChangedStatusToBusy(const UserListElement &ule);
		void userChangedStatusToNotAvailable(const UserListElement &ule);
		/* from i ule s� ignorowane, message wskazuje na plik z d�wi�kiem do odtworzenia
		 * je�eli message==QString::null, to odtwarzany jest standardowy d�wi�k dla tego typu 
		 * je�eli mapa jest!=NULL brane s� z niej nast�puj�ce warto�ci:
		 *		"Force"           - bool (wymuszenie odtwarzania mimo wyciszenia)
		 */
		void message(const QString &from, const QString &message, const QMap<QString, QVariant> *parameters, const UserListElement *ule);

	public slots:
		void play(const QString &path, bool force=false);
		void setMute(const bool& enable);

	public:
		SoundManager(const QString& name, const QString& configname);
		~SoundManager();
		bool isMuted();
		int timeAfterLastSound();		

	signals:
		void playSound(const QString &sound, bool volCntrl, double vol);
};

extern SoundManager* sound_manager;

#endif
