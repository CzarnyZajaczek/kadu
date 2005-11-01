#ifndef KADU_CHAT_H
#define KADU_CHAT_H

#include <qdatetime.h>
#include <qstring.h>
#include <qmainwindow.h>
#include <qvaluelist.h>

#include "config_file.h"
#include "custom_input.h"
#include "emoticons.h"
#include "gadu.h"
#include "usergroup.h"

class QAccel;
class QHBox;
class QMimeSourceFactory;
class QObject;
class QPushButton;

class Chat;
class ChatManager;
class ChatMessage;
class ColorSelector;
class EmoticonSelector;
class KaduSplitter;
class KaduTextBrowser;
class UserBox;

/** \typedef QValueList<Chat*> ChatList */
typedef QValueList<Chat*> ChatList;

/**
	Klasa tworz�ca okno rozmowy, rejestruj�ca przyciski,
	formatuj�ca wiadomo�� itp.
	\class Chat
	\brief Okno rozmowy
**/

class Chat : public QMainWindow
{
	Q_OBJECT

	private:
		friend class ChatManager;

		QValueList<ChatMessage *> ChatMessages; /*!< wiadomo�ci wpisane
							w oknie rozmowy */

		UserGroup *Users; /*!< lista u�ytkownik�w w danym oknie */
		int index;	/*!< nr okna (z chat menad�era) */
		QString title_buffer; /*!< tytu� okna */
		QTimer* title_timer;  /*!< zmienna przechowuj�ca czas od ostatniego
					od�wie�enia tytu�u okna */
		QColor actcolor; /*!< zmienna przechowuj�ca aktualny kolor */

		CustomInput* Edit; /*!< okno do wpisywania wiadomo�ci */
		QMimeSourceFactory *bodyformat; /*!< zmienna ustawiaj�cy format */
		EmoticonSelector* emoticon_selector; /*!< okienko do
							wyboru emotikonek */
		ColorSelector* color_selector; /*!< okienko do wyboru koloru */
		bool AutoSend;
		bool ScrollLocked;
		bool WaitingForACK;
		QAccel* acc; /*!< zmienna potrzebna do
				 rejestracji skr�t�w klawiszowych*/
		UserBox* userbox; /*!< lista kontakt�w przydatna gdy jeste�my w
						 konferencji */
		QString myLastMessage;/*!< zmienna przechowuj�ca
					nasz� ostatni� wiadomo�� */
		int myLastFormatsLength; /* ?? */
		void* myLastFormats; /* ?? */
		int seq; /* ?? */
		KaduSplitter *vertSplit, *horizSplit; /*!< obiekty oddzielaj�ce
							 kontrolki od siebie */
		int ParagraphSeparator; /* odst�p mi�dzy kolejnymi wypowiedziami */

		QDateTime lastMsgTime; /*!< czas ostatniej wiadomo�ci */

		//pomijanie nag��wk�w w wiadomo�ciach
		QString PreviousMessage;           //pami�tamy od kogo by�a ostatnia wiadomo��
		bool CfgNoHeaderRepeat;         //czy chcemy w og�le u�ywa� okrojonych nag��wk�w ?
		int CfgHeaderSeparatorHeight;      //jaki wysoki chcemy mie� separator ?
		int CfgNoHeaderInterval;        //co ile chcemy przywr�ci� nag��wek ?

		time_t LastTime;

		/**
			\fn void pruneWindow()
			Funkcja czyszcz�ca okno rozmowy
		**/
		void pruneWindow();

	private slots:

		/**
			\fn void changeColor()
			Slot zmieniaj�cy kolor czcionki
		**/
		void changeColor(const QWidget* activating_widget);

		/**
			\fn void addMyMessageToHistory()
			Slot dodaje wys�an� wiadomo�� do historii rozmowy
		**/
		void addMyMessageToHistory();

		/**
			\fn void pageUp()
			Slot przewijaj�cy histori� rozmowy w g�r�
		**/
		void pageUp();

		/**
			\fn void pageDown()
			Slot przewijaj�cy histori� rozmowy w d�
		**/
		void pageDown();

		/**
			\fn void imageReceivedAndSaved(UinType sender,uint32_t size,uint32_t crc32,const QString& path)
			Slot obs�uguj�cy odebranie i zapis obrazka
			\param sender osoba, kt�ra wys�a�a obrazek
			\param size rozmiar obrazka
			\param crc32 suma kontrolna obrazka
			\param path �cie�ka do obrazka
		**/
		void imageReceivedAndSaved(UinType sender,uint32_t size,uint32_t crc32,const QString& path);

		/**
			\fn void connectAcknowledgeSlots();
			Slot pod��czaj�cy potwierdzenie dostarczenia wiadomo�ci
		**/
		void connectAcknowledgeSlots();

		/**
			\fn void disconnectAcknowledgeSlots();
			Slot od��czaj�cy potwierdzenie dostarczenia wiadomo�ci
		**/
		void disconnectAcknowledgeSlots();

		void selectedUsersNeeded(const UserGroup*& user_group);

	protected:
		/**
			\fn virtual void closeEvent(QCloseEvent* e)
			Funkcja obs�uguj�ca zamkni�cie okna
		**/
		virtual void closeEvent(QCloseEvent* e);

		/**
			\fn QString convertCharacters(QString text, const QColor &bgcolor, EmoticonsStyle style)
			Funkcja zamienia specjalne tagi na emotikonki,
			html na czysty tekst itp.
			\param text tekst do konwersji
			\param bgcolor kolor t�a
			\param style styl emotikonek
		**/
		QString convertCharacters(QString text, const QColor &bgcolor, EmoticonsStyle style);

		/**
			\fn virtual void windowActivationChange(bool oldActive)
			Funkcja steruj�ca mruganiem napisu okna
		**/
		virtual void windowActivationChange(bool oldActive);

		/**
			\fn virtual void keyPressEvent(QKeyEvent* e)
			Funkcja obs�uguj�ca naci�ni�cie przycisku
			\param e wska�nik do obiektu obs�uguj�cego klawisze
		**/
		virtual void keyPressEvent(QKeyEvent* e);

		/**
			\fn virtual void keyReleaseEvent(QKeyEvent* e)
			Funkcja obs�uguj�ca zwolnienie przycisku
			\param e wska�nik do obiektu obs�uguj�cego klawisze
		**/
		virtual void mouseReleaseEvent(QMouseEvent *e);

	public:
		// FIXME - nie powinno byc publicznych zmiennych
		KaduTextBrowser* body; /*!< historia rozmowy */

		/**
			Konstruktor okna rozmowy
			\fn Chat(UserListElements usrs, QWidget* parent = 0, const char* name = 0)
			\param usrs lista kontakt�w, z kt�rymi prowadzona jest rozmowa
			\param parent rodzic okna
			\param name nazwa obiektu
		**/
		Chat(UserListElements usrs, QWidget* parent = 0, const char* name = 0);

		/**
			\fn ~Chat()
			Destruktor okna rozmowy
		**/
		~Chat();

		/**
			\fn void formatMessages(QValueList<ChatMessage *> &msgs)
			Formatuje wszystkie wiadomo�ci na html'a,
			dodaje kolory t�a i czcionek
			\param msgs lista wiadomo�ci do sformatowania
		**/
		void formatMessages(QValueList<ChatMessage *> &msgs);
		/**
			\fn void formatMessage(ChatMessage &msg,
				QColor myBgColor=QColor(), QColor usrBgColor=QColor(),
				QColor myFontColor=QColor(), QColor usrFontColor=QColor(),
				EmoticonsStyle style=(EmoticonsStyle)config_file.readNumEntry("Chat","EmoticonsStyle"))

			Formatuje wiadomo�� na html'a
			\param msg wiadomo�� kt�ra b�dzie sformatowana
			\param myBgColor kolor kt�ry b�dzie ustawiony jako t�o
			w naszych wiadomo�ciach
			\param usrBgColor kolor, kt�ry b�dzie ustawiony jako t�o
			wiadomo�ci naszego rozm�wcy
			\param myFontColor kolor naszej czcionki
			\param usrFontColor kolor czcionki naszego rozm�wcy
			\param style styl ikonek
		**/
		void formatMessage(ChatMessage &msg,
				QColor myBgColor=QColor(), QColor usrBgColor=QColor(),
				QColor myFontColor=QColor(), QColor usrFontColor=QColor(),
				EmoticonsStyle style=(EmoticonsStyle)config_file.readNumEntry("Chat","EmoticonsStyle"));

		/**
			\fn void newMessage(const QString &protocolName, UserListElements senders, const QString &msg, time_t time)
			Dodaje wiadomo�� do okna
			\param protocolName nazwa protoko�u
			\param senders lista u�ytkownik�w
			\param msg wiadomo�c
			\param time czas
		**/
		void newMessage(const QString &protocolName, UserListElements senders, const QString &msg, time_t time);

		/**
			\fn void writeMessagesFromHistory(UserListElements senders, time_t time)
			Wpisuje do okna histori� rozmowy
			\param senders lista u�ytkownik�w identyfikuj�cych okno
			\param time ..
		**/
		void writeMessagesFromHistory(UserListElements senders, time_t time);

		/**
			\fn const UserGroup *users() const
			Zwraca list� numer�w rozmowc�w.
		**/
		const UserGroup *users() const;

		/**
			\fn QValueList<ChatMessage*>& chatMessages()
			Daje dost�p do wiadomo�ci aktualnie przechowywanych
			w oknie chat. Metody tej mo�na u�y� do zmiany tre�ci
			lub w�a�ciwo�ci kt�rej� z wiadomo�ci w odpowiedzi
			na jakie� zdarzenie.
		**/
		QValueList<ChatMessage*>& chatMessages();

		/**
			\fn void repaintMessages()
			Od�wie�a zawarto�� okna uwzgl�dniaj�c ewentualne
			zmiany dokonane w kt�rej� wiadomo�ci z listy
			uzyskanej za pomoc� metody chatMessages(),
			dodanie nowych wiadomo�ci lub usuni�cie istniej�cych.
		**/
		void repaintMessages();

		/**
			\fn const QString& title() const
			Zwraca aktualny tytu� okna
		**/
		const QString& title() const;

		CustomInput* edit();

		bool autoSend() const;
		bool waitingForACK() const;

		virtual void dragEnterEvent(QDragEnterEvent *e);
		virtual void dropEvent(QDropEvent *e);
		virtual void dragMoveEvent(QDragMoveEvent *e);

	public slots:

		/**
			\fn void changeAppearance()
			Slot zmieniaj�cy kolory i czcionki w oknie
		**/
		void changeAppearance();

		/**
			\fn void refreshTitle()
			Slot ustawiaj�cy tytu� okna zgodnie z konfiguracj�
		**/
		void refreshTitle();

		/**
			\fn void addEmoticon(QString)
			Slot dodaje ikonke do pola wpisywania wiadomo�ci
			\param emot ikonka np. <lol>
		**/
		void addEmoticon(QString);

		/**
			\fn void scrollMessages(const QValueList<ChatMessage *> &)
			Slot dodaj wiadomo�ci do okna
			\param messages lista wiadomo�ci
		**/
		void scrollMessages(const QValueList<ChatMessage *> &);

		/**
			\fn void alertNewMessage()
			Slot informuj�cy o nowej wiadomo�ci
			mruganie je�li okno jest nieaktywne
		**/
		void alertNewMessage();

		/**
			\fn void sendMessage()
			Slot wywo�ywany po naci�ni�ciu przycisku
			do wysy�ania wiadomo�ci
		**/
		void sendMessage();

		/**
			\fn void cancelMessage()
			Slot wywo�ywany po naci�nieciu przycisku anulowania
			wysy�aniu wiadomo�ci
		**/
		void cancelMessage();

		/**
			\fn void writeMyMessage()
			Slot wpisuj�cy wiadomo�� do okna
			\see sendMessage
		**/
		void writeMyMessage();

		/**
			\fn void changeTitle()
			Slot zmieniaj�cy tytu�
			\see setTitle
		**/
		void changeTitle();

		/**
			\fn void curPosChanged(int para, int pos)
			Slot wywo�ywany kiedy pozycja kursosa zmieni�a si�
			\param  para nie obslugiwane
			\param pos nie obslugiwane
		**/
		void curPosChanged(int para, int pos);

		/**
			\fn void specialKeyPressed(int key)
			Slot zostaje wywo�any gdy naci�ni�to specjalny klawisz (skr�t)
			odpowiadaj�cy np KEY_BOLD
			\param key warto�� naci�ni�tego specjalnego klawisza
		**/
		void specialKeyPressed(int key);

		/**
			\fn void colorChanged(const QColor& color)
			Slot zostaje wywo�wany gdy u�ytkownik zmieni� kolor czcionki
			kt�r� b�dzie pisa�
			\param color warto�� zmienionego koloru
		**/
		void colorChanged(const QColor& color);

		/**
			\fn void colorSelectorAboutToClose()
			Slot zostaje wywo�any przy zamykaniu okna wyboru ikonek
		**/
		void colorSelectorAboutToClose();

		/**
			\fn void messageBlockedSlot(int seq, UinType uin)
			Slot zostaje wywo�any gdy nasza wiadomo�� zosta�a zablokowana
			przez serwer
			\param seq
			\param uin
		**/
		void messageBlockedSlot(int seq, UinType uin);

		/**
			\fn void messageBoxFullSlot(int seq, UinType uin)
			Slot zostaje wywo�any gdy skrzynka na wiadomo�ci
			zosta�a zape�niona
			\param seq
			\param uin
		**/
		void messageBoxFullSlot(int seq, UinType uin);

		/**
			\fn void messageNotDeliveredSlot(int seq, UinType uin)
			Slot zostaje wywo�any gdy wiadomo�� nie zosta�a dostarczona
			\param seq
			\param uin
		**/
		void messageNotDeliveredSlot(int seq, UinType uin);

		/**
			\fn void messageAcceptedSlot(int seq, UinType uin)
			Slot zostaje wywo�any gdy wiadomo�� zosta�a poprawnie wys�ana
			\param seq
			\param uin
		**/
		void messageAcceptedSlot(int seq, UinType uin);

		/**
			\fn void clearChatWindow()
			Slot czyszcz�cy okno rozmowy
		**/
		void clearChatWindow();

		/**
			\fn void setAutoSend(bool auto_send)
			Ustaw autowysylanie
		**/
		void setAutoSend(bool auto_send);

		/**
			\fn void setScrollLocked(bool locked)
			Ustaw blokade przewijania
		**/
		void setScrollLocked(bool locked);

		/**
			\fn void emoticonSelectorClicked()
			Slot wy�wietlaj�cy okno z wyborem emotikonek
		**/
		void openEmoticonSelector(const QWidget* activating_widget);

		/**
			\fn void insertImage()
			Slot wklejaj�cy obrazek do okna rozmowy (wyb�r obrazka)
		**/
		void insertImage();

	signals:
		/**
			\fn void messageSendRequested(Chat* chat)
			Sygnal jest emitowany gdy uzytkownik wyda polecenie
			wyslania wiadomosci, np klikajac na guzik "wyslij".
			\param chat wska�nik do okna kt�re emituje sygna�
		**/
		void messageSendRequested(Chat* chat);

		/**
			\fn void messageFiltering(const UserGroup *users, QCString& msg, bool& stop)
			Sygnal daje mozliwosc operowania na wiadomoci
			ktora ma byc wyslana do serwera juz w jej docelowej
			formie po konwersji z unicode i innymi zabiegami.
			Tresc wiadomosci mozna zmienic podmieniajac wskaznik
			msg na nowy bufor i zwalniajac stary (za pomoca free).
			Mozna tez przerwac dalsza jej obrobke ustawiajac
			wskaznik stop na true.
			\param users lista u�ytkownik�w
			\param msg wiadomo��
			\param stop zako�czenie dalszej obr�bki sygna�u
		**/
		void messageFiltering(const UserGroup *users, QCString &msg, bool &stop);

		/**
			\fn void messageSent(Chat* chat)
			Sygnal jest emitowany gdy zakonczy sie proces
			wysylania wiadomosci i zwiazanych z tym czynnosci.
			Oczywiscie nie wiemy czy wiadomosc dotarla.
			\param chat wska�nik do okna rozmowy,
			 kt�re emitowa�o sygna�
		**/
		void messageSent(Chat* chat);

		void fileDropped(const UserGroup *users, const QString &fileName);
};

#endif
