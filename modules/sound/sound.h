#ifndef KADU_SOUND_H
#define KADU_SOUND_H

#include <qobject.h>
#include <qstring.h>
#include <qdatetime.h>
#include <qstringlist.h>
#include <qmap.h>
#include <qthread.h>

#include "config_file.h"
#include "modules.h"
#include "gadu.h"
#include "userlist.h"
#include "misc.h"
#include "message_box.h"

/**
	Uog�lniony deskryptor urz�dzenia d�wi�kowego.
**/
typedef void* SoundDevice;

/**
	To jest klasa u�ywana wewn�trznie przez klas� SoundManager
	i nie powiniene� mie� potrzeby jej u�ywania.
**/
class SoundPlayThread : public QObject, public QThread
{
	Q_OBJECT

	private:
		SoundDevice Device;
		const int16_t* Sample;
		int SampleLen;
		bool Stopped;
		QSemaphore PlayingSemaphore;
		QSemaphore SampleSemaphore;

	protected:
		virtual void run();
		virtual void customEvent(QCustomEvent* event);

	public:
		SoundPlayThread(SoundDevice device);
		void playSample(const int16_t* data, int length);
		void stop();
		
	signals:
		void samplePlayed(SoundDevice device);
};

/**
	To jest klasa u�ywana wewn�trznie przez klas� SoundManager
	i nie powiniene� mie� potrzeby jej u�ywania.
**/
class SoundRecordThread : public QObject, public QThread
{
	Q_OBJECT

	private:
		SoundDevice Device;
		int16_t* Sample;
		int SampleLen;
		bool Stopped;
		QSemaphore RecordingSemaphore;
		QSemaphore SampleSemaphore;

	protected:
		virtual void run();
		virtual void customEvent(QCustomEvent* event);

	public:
		SoundRecordThread(SoundDevice device);
		void recordSample(int16_t* data, int length);
		void stop();
		
	signals:
		void sampleRecorded(SoundDevice device);
};

class SoundSlots : public QObject
{
	Q_OBJECT
	private:
		int muteitem;
		QMap<QString, QString> soundfiles;
		QStringList soundNames;
		QStringList soundTexts;
		MessageBox* SamplePlayingTestMsgBox;
		SoundDevice SamplePlayingTestDevice;
		int16_t* SamplePlayingTestSample;
		MessageBox* SampleRecordingTestMsgBox;
		SoundDevice SampleRecordingTestDevice;
		int16_t* SampleRecordingTestSample;
		MessageBox* FullDuplexTestMsgBox;
		SoundDevice FullDuplexTestDevice;
		int16_t* FullDuplexTestSample;

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
		void testSamplePlaying();
		void samplePlayingTestSamplePlayed(SoundDevice device);
		void testSampleRecording();
		void sampleRecordingTestSampleRecorded(SoundDevice device);
		void sampleRecordingTestSamplePlayed(SoundDevice device);
		void testFullDuplex();
		void fullDuplexTestSampleRecorded(SoundDevice device);
		void closeFullDuplexTest();

	public:
		SoundSlots(QObject *parent=0, const char *name=0);
		~SoundSlots();
};

class SoundManager : public Themes
{
    Q_OBJECT
	private:
		friend class SoundPlayThread;
		friend class SoundRecordThread;
		QTime lastsoundtime;
		bool mute;
		QMap<SoundDevice, SoundPlayThread*> PlayingThreads;
		QMap<SoundDevice, SoundRecordThread*> RecordingThreads;

	private slots:
		void newChat(const UinsList &senders, const QString& msg, time_t time);
		void newMessage(const UinsList &senders, const QString& msg, time_t time, bool &grab);
		void connectionError(const QString &message);
		void userChangedStatusToAvailable(const UserListElement &ule);
		void userChangedStatusToBusy(const UserListElement &ule);
		void userChangedStatusToInvisible(const UserListElement &ule);
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
		/**
			Otwiera urz�dzenie d�wi�kowe do operacji
			odtwarzania i nagrywania sampli.
			Niekt�re implementacje pozwalaj� na otwarcie
			wielu niezale�nie dzia�aj�cych "po��cze�"
			z urz�dzeniami. Wystarczy wtedy kilkukrotnie
			wywo�a� t� metod�.
			Emituje sygna� openDeviceImpl() w celu
			przekazania ��dania do konkrentego modu�u
			d�wi�kowego.
			@param sample_rate sample rate - np. 8000 lub 48000
			@channels ilo�� kana��w: 1 - mono, 2 - stereo
			@return uog�lniony deskryptor urz�dzenia lub NULL je�li otwarcie si� nie powiod�o.
		**/
		SoundDevice openDevice(int sample_rate, int channels = 1);
		/**
			Zamyka urz�dzenie d�wi�kowe otwarte za pomoc�
			metody openDevice().
			Niekt�re implementacje pozwalaj� na otwarcie
			wielu niezale�nie dzia�aj�cych "po��cze�"
			z urz�dzeniami. Ka�de otwarte po��czenie nale�y
			zamkn�� za pomoc� tej metody.
			Je�li w��czyli�my operacje nieblokuj�ce to
			metoda ta czeka na zako�czenie trwaj�cych operacji
			i ko�czy dzia�anie w�tk�w.
			Emituje sygna� closeDeviceImpl() w celu
			przekazania ��dania do konkrentego modu�u
			d�wi�kowego.
			@param device uog�lniony deskryptor urz�dzenia.
		**/
		void closeDevice(SoundDevice device);
		/**
			Powo�uje do �ycia w�tki zajmuj�ce si� odtwarzaniem
			i nagrywaniem pr�bek dla danego po��czenia z
			urz�dzeniem d�wi�kowym.
			Od tej chwili playSample() i recordSample()
			b�d� operacjami nieblokuj�cymi.
			@param device uog�lniony deskryptor urz�dzenia.
		**/
		void enableThreading(SoundDevice device);
		/**
			Odtwarza pr�bk� d�wi�kow�. Standardowo jest to
			operacja blokuj�ca. Mo�e by� wywo�ana z innego
			w�tku (a nawet powinna).
			Emituje sygna� playSampleImpl() w celu
			przekazania ��dania do konkrentego modu�u
			d�wi�kowego.
			Po uprzednim wywo�aniu enableThreading() dzia�anie
			metoda jest nieblokuj�ca i przekazuje jedynie polecenie
			odtwarzania do w�tku.
			W takim wypadku nale�y uwa�a�, aby nie zwolni� pami�ci
			zajmowanej przez dane sampla zanim odtwarzanie si� nie
			zako�czy.
			@param device uog�lniony deskryptor urz�dzenia
			@data wska�nik do danych sampla
			@length d�ugo�� danych sampla
			@return true je�li odtwarzanie zako�czy�o si� powodzeniem.
		**/
		bool playSample(SoundDevice device, const int16_t* data, int length);
		/**
			Nagrywa pr�bk� d�wi�kow�. Standardowo jest to
			operacja blokuj�ca. Mo�e by� wywo�ana z innego
			w�tku (a nawet powinna).
			Emituje sygna� recordSampleImpl() w celu
			przekazania ��dania do konkrentego modu�u
			d�wi�kowego.
			Po uprzednim wywo�aniu enableThreading() dzia�anie
			metoda jest nieblokuj�ca i przekazuje jedynie polecenie
			nagrywania do w�tku.
			W takim wypadku nale�y uwa�a�, aby nie zwolni� pami�ci
			bufora na dane sampla zanim nagrywanie si� nie
			zako�czy.			
			@param device uog�lniony deskryptor urz�dzenia
			@data wska�nik na bufor dla danych sampla
			@length d�ugo�� sampla do nagrania (wielko�� bufora)
			@return true je�li nagrywanie zako�czy�o si� powodzeniem.
		**/
		bool recordSample(SoundDevice device, int16_t* data, int length);

	signals:
		void playSound(const QString &sound, bool volCntrl, double vol);
		/**
			Sygna� emitowany gdy odtwarzanie sampla si�
			zako�czy�o (odnosi si� tylko do sytuacji gdy
			w��czone s� operacje nieblokuj�ce).
		**/
		void samplePlayed(SoundDevice device);
		/**
			Sygna� emitowany gdy nagrywanie sampla si�
			zako�czy�o (odnosi si� tylko do sytuacji gdy
			w��czone s� operacje nieblokuj�ce).
		**/
		void sampleRecorded(SoundDevice device);
		/**
			Pod ten sygna� powinien podpi�� si� modu�
			d�wi�kowy je�li obs�uguje funkcj� odtwarzania
			pr�bki d�wi�kowej.
			Wyemitowanie sygna�u oznacza ��danie
			otwarcia urz�dzenia d�wi�kowego do operacji
			odtwarzania i nagrywania sampli.
			@param sample_rate sample rate - np. 8000 lub 48000
			@channels ilo�� kana��w: 1 - mono, 2 - stereo
			@device zwr�cony uog�lniony deskryptor urz�dzenia lub NULL je�li otwarcie si� nie powiod�o.
		**/
		void openDeviceImpl(int sample_rate, int channels, SoundDevice& device);
		/**
			Pod ten sygna� powinien podpi�� si� modu�
			d�wi�kowy je�li obs�uguje funkcj� odtwarzania
			pr�bki d�wi�kowej.
			Wyemitowanie sygna�u oznacza ��danie
			Zamkni�cia urz�dzenia d�wi�kowego otwartegp za pomoc�
			metody openDevice().
			@param device uog�lniony deskryptor urz�dzenia.
		**/
		void closeDeviceImpl(SoundDevice device);
		/**
			Pod ten sygna� powinien podpi�� si� modu�
			d�wi�kowy je�li obs�uguje funkcj� odtwarzania
			pr�bki d�wi�kowej.
			Wyemitowanie sygna�u oznacza ��danie
			odtworzenia pr�bki d�wi�kowej.
			Modu� powinien odda� sterowanie dopiero po
			odtworzeniu pr�bki.
			Sygna� zazwyczaj b�dzie emitowany z innego
			w�tku i slot musi by� do tego przystosowany.
			@param device uog�lniony deskryptor urz�dzenia
			@data wska�nik do danych sampla
			@length d�ugo�� danych sampla
			@result zwr�cony rezultat operacji - true je�li odtwarzanie zako�czy�o si� powodzeniem.
		**/
		void playSampleImpl(SoundDevice device, const int16_t* data, int length, bool& result);
		/**
			Pod ten sygna� powinien podpi�� si� modu�
			d�wi�kowy je�li obs�uguje funkcj� odtwarzania
			pr�bki d�wi�kowej.
			Wyemitowanie sygna�u oznacza ��danie
			nagrania pr�bki d�wi�kowej.
			Modu� powinien odda� sterowanie dopiero po
			nagraniu pr�bki.
			Sygna� zazwyczaj b�dzie emitowany z innego
			w�tku i slot musi by� do tego przystosowany.
			@param device uog�lniony deskryptor urz�dzenia
			@data wska�nik na bufor dla danych sampla
			@length d�ugo�� sampla do nagrania (wielko�� bufora)
			@result zwr�cony rezultat operacji - true je�li nagrywanie zako�czy�o si� powodzeniem.
		**/
		void recordSampleImpl(SoundDevice device, int16_t* data, int length, bool& result);		
};

extern SoundManager* sound_manager;

#endif
