#ifndef CHAT_H
#define CHAT_H

#include <qcolor.h>
#include <qdialog.h>
#include <qmap.h>
#include <qmultilineedit.h>
#include <qpushbutton.h>
#include <qrect.h>
#include <qsplitter.h>
#include <qvariant.h>
#include <qvaluelist.h>

#include "emoticons.h"
#include "misc.h"
#include "userbox.h"

class Chat;
class QHBox;
class ChatSlots;
/** \typedef QValueList<Chat*> ChatList */
typedef QValueList<Chat*> ChatList;

/**

	Klasa przechowuj�ca informacje o wiadomo�ci, kt�ra ma si� pojawi�
	w oknie Chat.
	\class ChatMessage
	\brief Klasa przechowuj�ca informacje o wiadomo�ci.
**/
class ChatMessage
{
	public:
	QString nick;/*!< nazwa u�ytkownika */
	QDateTime date; /*!< data otrzymania wiadomo�ci */
	QDateTime sdate; /*!< data wys�ania wiadomo�ci */
	QString unformattedMessage; /*!< niesformatowana wiadomo�� */
	bool isMyMessage; /*!< zmienna mowi�ca czy wiadomo�c zosta�a 
				napisana przez nas */
	QColor backgroundColor; /*!< t�o wiadomo�ci */
	QColor textColor; /*!< kolor wiadomo�ci */
	
	
	QMap<QString, bool> attributes; /*!<
		Mo�e s�u�y� do przechowywania informacji o tym
		czy wiadomo�� by�a potwierdzona, szyfrowana, ...
		W zamy�le ma s�u�y� do okre�lania czy jaki� obrazek
		ma by� dodawany do wiadomo�ci czy nie - jaki obrazek -
		ta informacja b�dzie gdzie indziej
		*/
	
	//inne atrybuty?
	//QMap<QString, QString> stringAttributes;
	
	bool needsToBeFormatted;/*!< zmienna mowi�ca czy wiadomo�c powinna 
				  by� sformatowana */
	
	QString message;/*!<
			Sformatowana wiadomo�� (razem z \<p\> lub \<table\>)
			**/


	/**
		\fn ChatMessage(const QString &nick, const QString &unformattedMessage, bool myMessage, QDateTime date, QDateTime sdate=QDateTime())
		Konstruktor ustawiaj�cy nick, niesformatowan� wiadomo��,
		czy wiadomo�� pochodzi od nas, dat� wiadomo�ci,
		dat� nadania wiadomo�ci przez serwer
		\param nick nazwa u�ytkownika
		\param unformattedMessage niesformatowana wiadomo��
		\param myMessage zmienna mowi�ca czy wiadomo�� pochodzi od nas
		\param date data otrzymania wiadomo�ci
		\param sdate data wys�ania wiadomo�ci
	**/
	ChatMessage(const QString &nick, const QString &unformattedMessage, bool myMessage, QDateTime date, QDateTime sdate=QDateTime());
	/**
		\fn ChatMessage(const QString &formattedMessage, const QColor &bgColor=Qt::white, const QColor &textColor=Qt::black)
		Konstruktor ustawiaj�cy sformatowan� wiadomo��,
		kolor t�a i liter wiadomo�ci
		\param nick nazwa u�ytkownika
		\param unformattedMessage niesformatowana wiadomo��
		\param myMessage zmienna mowi�ca czy wiadomo�� pochodzi od nas
		\param date data otrzymania wiadomo�ci
		\param sdate data wys�ania wiadomo�ci
	**/
	ChatMessage(const QString &formattedMessage, const QColor &bgColor=Qt::white, const QColor &textColor=Qt::black);
};
/**
	Klasa pozwalaj�ca zarz�dza� otwartymi oknami rozm�w: otwiera�,
	zamykac, szuka� okna ze wgl�du na list� u�ytkownik�w itp.
	\class ChatManager
	\brief Klasa zarz�dzaj�ca oknami Chat
**/
class ChatManager : public QObject
{
	Q_OBJECT

	private:
		ChatList Chats;/*!< lista okien*/
		
		static ChatSlots *chatslots;
		/**
		\fn int openPendingMsg(int index, ChatMessage &msg)
		Funkcja otwieraj�ca zakolejkowan� wiadomo��
		\param index nr wiadomo�ci z kolejki
		\param msg wiadomo�� z kolejki
		\return zwracany jest nr okna w kt�rym zosta�a otwarta wiadomo��
		**/
		int openPendingMsg(int index, ChatMessage &msg);
		
		/** 
		\struct ChatInfo
		Struktura przechowuje informacje o danym oknie
		**/
		struct ChatInfo
		{
			UinsList uins;/*!< lista u�ytkownik�w 
					identyfikuj�ca okno */
			QMap<QString, QVariant> map; /*!< parametry 
							danego okna */
		};
		QValueList<ChatInfo> addons; /*!< lista parametr�w okien */

	public:	
		/**
		\fn ChatManager(QObject* parent=NULL, const char* name=NULL)
		Konstruktor tworz�cy obiekt zarz�dzaj�cy oknami
		\param parent rodzic okna
		\param name nazwa obiektu
		**/
		ChatManager(QObject* parent=NULL, const char* name=NULL);
		
		/**
			\fn ~ChatManager()
			Destruktor zamyka wszystkie otwarte okna
		**/
		
		~ChatManager();
		
		/**
			\fn const ChatList& chats() const
			Funkcja zwraca liste otwartych okien Chat
		**/

		/**
			\fn static void initModule()
			Rejestruje opcje modulu Chat w oknie konfiguracji
		**/
		static void initModule();
		static void closeModule();

		const ChatList& chats() const;
		
		/**
			\fn Chat* findChatByUins(const UinsList &uins) const
			Funkcja zwraca wska�nik do okna z list� 
			u�ytkownik�w uins
			\param uins lista u�ytkownik�w
			\return wska�nik do okna je�li istnieje w przeciwnym
			 wypadku zwraca NULL
		**/
		Chat* findChatByUins(const UinsList &uins) const;
		
		/**
			\fn QVariant& getChatProperty(const UinsList &uins, const QString &name)
			Funkcja zwraca warto�� w�asno�ci "name" okna 
			okre�lonego przez uins
			\param uins lista u�ytkownik�w identyfikuj�ca okno
			\param name nazwa w�asno�ci
			\return zwraca warto�� w�asno�ci je�li okre�lone okno 
			istnieje,\n je�li nie to tworzy tak� 
			w�asno�� (ustawia na pust�)
		**/
		QVariant& getChatProperty(const UinsList &uins, const QString &name);
		
	public slots:
	
		/**
			\fn void chatMsgReceived(UinsList senders,const QString& msg,time_t time,bool& grab)
			Slot informuj�cy o odebraniu wiadomo�ci
			\param senders lista u�ytkownik�w identyfikuj�cych okno
			\param msg tre�� otrzymanej wiadomo�ci
			\param time czas otrzymania wiadomo�ci
			\param grab 
		**/
		void chatMsgReceived(UinsList senders,const QString& msg,time_t time,bool& grab);

		/**
			\fn int openChat(UinsList senders,time_t time=0)
			Funkcja otwiera nowe okno Chat z wymienionymi rozmowcami.
			\param senders lista u�ytkownik�w identyfikuj�cych okno
			\param time parametr sluzy do sprawdzenia ile wiadomosci
			z historii ma sie pojawic w oknie.
			\return zwracany jest numer otwartego okna 
		**/
		int openChat(UinsList senders,time_t time=0);		
		
		/**
			\fn void openPendingMsgs(UinsList uins)
			Funkcja wpisuje zakolejkowane wiadomo�ci do okna
			z u�ytkownikami "uins"
			\param uins lista u�ytkownik�w identyfikuj�cych okno
		**/
		void openPendingMsgs(UinsList uins);
		
		/**
			\fn void openPendingMsgs()
			Funkcja wpisuje wszystkie zakolejkowane wiadomo�ci 
			do odpowiednich okien
		**/
		void openPendingMsgs();
		
		//TODO: opisac funkcje sendMessage(..)
		/*
			Niebardzo rozumiem tej funkcji (czemu jest uin i uins)	
		*/
		void sendMessage(UinType uin,UinsList selected_uins);
		
		/**
			\fn void closeAllWindows()
			Funkcja zamyka wszystkie okna chat
		**/
		void closeAllWindows();
		
		/**
			\fn int registerChat(Chat* chat)
			Dodaje okno do menad�era
			\param chat wska�nik do okna ktore chcemy doda�
			\return zwraca numer naszego okna po zarejestrowaniu
		**/
		int registerChat(Chat* chat);
		
		/**
			\fn void unregisterChat(Chat* chat)
			Funkcja wyrejestrowuje okno z managera \n
			Zapisuje w�asno�ci okna \n
			wysy�a sygna� chatDestroying i chatDestroyed
			\param chat okno kt�re b�dzie wyrejestrowane
		**/
		void unregisterChat(Chat* chat);
		
		/**
			\fn void refreshTitles()
			Funkcja od�wie�a tytu�y wszystkich okien
		**/
		void refreshTitles();
		
		/**
			\fn void refreshTitlesForUin(UinType uin)
			Funkcja od�wie�a tytu�y okien kt�re zawieraj� uin
			\param uin nr u�ytkownika, kt�rego 
			opis/status b�dzie od�wie�any
		**/
		void refreshTitlesForUin(UinType uin);
		
		/**
			\fn void changeAppearance()
			Funkcja od�wie�a kolory i czcionki we wszystkich oknach
		**/
		void changeAppearance();
		
		/**
			\fn void setChatProperty(const UinsList &uins, const QString &name, const QVariant &value)
			Funkcja pozwala przypisa� okre�lonemu czatowi 
			(nawet je�eli on jeszcze nie istnieje) pewne w�asno�ci
			\param uins lista u�ytkownik�w identyfikuj�cych okno
			\param name nazwa w�asno�ci
			\param value warto�� w�asno�ci
		**/
		void setChatProperty(const UinsList &uins, const QString &name, const QVariant &value);
		
	signals:
		
		/**
			\fn void chatCreated(const UinsList& senders)
		 	Sygna� ten jest wysy�any po utworzeniu nowego okna chat
			\param senders lista uin-�w z kt�rymi tworzymy nowy chat
		**/
		void chatCreated(const UinsList& senders);
		
		/**
			\fn void chatDestroying(const UinsList& senders)
		 	Sygna� ten jest wysy�any przed zamnkni�ciem okna chat
			\param senders lista uin-�w kt�re wyst�puj� w tym chacie
		**/
		void chatDestroying(const UinsList& senders);
		
		/**
			\fn void chatDestroyed(const UinsList& senders)
		 	Sygna� ten jest wysy�any po zamnkni�ciem okna chat
			\param senders lista uin-�w kt�re 
			wyst�powa�y w tym chacie
		**/
		void chatDestroyed(const UinsList& senders);
		
		/**
			\fn void chatOpen(const UinsList& senders)
			Sygna� ten jest wysy�aniy podczas ka�dej pr�by 
			otwarcia nowego okna chat nawet je�li ju� taki istnieje
			\param senders lista uin-�w
		**/
		void chatOpen(const UinsList& senders);
};

/** 
	wska�nik do obiektu ChatManager'a 
**/
extern ChatManager* chat_manager; 

class EmoticonSelector;
class ColorSelector;
/**
	\class CustomInput
	\brief Klasa umo�liwiaj�ca wpisywanie wiadomo�ci
**/
class CustomInput : public QMultiLineEdit
{
	Q_OBJECT

	protected:
		
		bool autosend_enabled;/*!< zmienna informuj�ca czy w��czone jest 
					automatyczne wysy�anie */
		/**
			\fn virtual void keyPressEvent(QKeyEvent *e)
			Funkcja obs�uguj�ca naci�ni�cie przycisku
			\param e wska�nik do obiektu obs�uguj�cego klawisze
		**/
		virtual void keyPressEvent(QKeyEvent *e);
		
		/**
			\fn virtual void keyReleaseEvent(QKeyEvent *e)
			Funkcja obs�uguj�ca zwolnienie przycisku
			\param e wska�nik do obiektu obs�uguj�cego klawisze
		**/
		virtual void keyReleaseEvent(QKeyEvent* e);

	public:
		
		/*! Typ wyliczeniowy mowi�cy o rodzaju pisanych znak�w */
		enum
		{
			KEY_BOLD,/*!< pogrubienie */
			KEY_ITALIC,/*!< kursywa */
			KEY_UNDERLINE/*!< podkre�lenie */
		};
		
		/**
			\fn CustomInput(QWidget* parent = 0, const char* name = 0)
			Konstruktor tworz�cy obiekt 
			i ustawiaj�cy odpowiedni styl
			\param parent rodzic okna
			\param name nazwa obiektu
		**/
		CustomInput(QWidget* parent = 0, const char* name = 0);

	public slots:
		
		/**
			\fn void paste()
			Slot wkleja czysty tekst
		**/
		void paste();
		
		/**
			\fn void setAutosend(bool on)
			Ustawia/wy��cza automatyczne wysy�anie wiadomo�ci
			\param on zmienna ustawiaj�ca autosend
		**/
		void setAutosend(bool on);

	signals:
		
		/**
			\fn void sendMessage()
			Sygna� zostaje wys�any kiedy naci�ni�to 
			klawisz wys�ania wiadomo�ci
		**/
		void sendMessage();
		
		/**
			\fn void specialKeyPressed(int key)
			Sygna� zostaje wys�any gdy naci�ni�to specjalny 
			klawisz (skr�t) odpowiadaj�cy np KEY_BOLD
			\param key warto�� z typu wyliczeniowego KEY_*
		**/
		void specialKeyPressed(int key);
		
		/**
			\fn void keyPressed(QKeyEvent* e, CustomInput* sender)
			Dowolny przycisk klawiatury zosta� naci�ni�ty.
			Przekazany zostaje tak�e obiekt, kt�ry wywo�a� akcj� -
			 czyli this
			\param e wska�nik do obiektu obs�uguj�cego klawisze
			\param sender wska�nik do obiektu, kt�ry
			wywo�a� ten sygna�
		**/
		void keyPressed(QKeyEvent* e, CustomInput* sender);
		
		/**
			\fn void keyReleased(QKeyEvent* e, CustomInput* sender)
			Dowolny przycisk klawiatury zosta� zwolniony.
			Przekazany zostaje tak�e obiekt, kt�ry wywo�a� akcj� -
			 czyli this.
			\param e wska�nik do obiektu obs�uguj�cego klawisze
			\param sender wska�nik do obiektu, kt�ry
			wywo�a� ten sygna�
		**/
		void keyReleased(QKeyEvent* e, CustomInput* sender);
};

/**
	Klasa wykorzystana do oddzielenia listy u�ytkownik�w od historii
	i pola do wpisywania wiadomo�ci
	\class KaduSplitter
	\brief Klasa rozdzielaj�ca kontrolki

**/
class KaduSplitter : public QSplitter
{
	protected:
		QValueList<KaduTextBrowser*> textbrowsers;
		void drawContents(QPainter* p);
		void childEvent(QChildEvent* c);

	public:
	/**
		Konstruktor tworz�cy obiekt
		\fn KaduSplitter(QWidget* parent = 0, const char* name = 0)
		\param parent rodzic okna
		\param name nazwa obiektu

	**/
		KaduSplitter(QWidget* parent = 0, const char* name = 0);
	
	/**
		Konstruktor tworz�cy obiekt o danej orientacji
		\fn KaduSplitter(Orientation o, QWidget* parent = 0, const char* name = 0)
		\param parent rodzic okna
		\param name nazwa obiektu
		\param o orientacja separatora
	**/
		KaduSplitter(Orientation o, QWidget* parent = 0, const char* name = 0);
};

class QMimeSourceFactory;

/**
	Klasa tworz�ca okno rozmowy, rejestruj�ca przyciski,
	formatuj�ca wiadomo�� itp.
	\class Chat
	\brief Okno rozmowy
**/
class Chat : public QWidget
{
	Q_OBJECT

	private:
		friend class ChatManager;
		/**
		\struct RegisteredButton struktura przechowuj�ca informacje o zarejestrowanym przycisku
		**/
		struct RegisteredButton
		{
			QString name; /*!< nazwa przycisku */
			QObject* receiver; /*!< obiekt z kt�rego wykorzystana
					b�dzie funkcja do obs�ugi przycisku */
			QString slot; /*!< funkcja podpi�ta pod obs�ug�
						tego przycisku */
		};
		static QValueList<RegisteredButton> RegisteredButtons;/*!<
				zarejestrowane przyciski w oknie rozmowy */
		QMap<QString,QPushButton*> Buttons; /*!< przyciski z okna */
		
		QValueList<ChatMessage *> ChatMessages; /*!< wiadomo�ci wpisane 
							w oknie rozmowy */
	
		UinsList Uins; /*!< lista u�ytkownik�w w danym oknie */
		int index;	/*!< nr okna (z chat menad�era) */
		QString title_buffer; /*!< tytu� okna */
		QTimer* title_timer;  /*!< zmienna przechowuj�ca czas od ostatniego 
					od�wie�enia tytu�u okna */
		QColor actcolor; /*!< zmienna przechowuj�ca aktualny kolor */
		
		QMimeSourceFactory *bodyformat; /*!< zmienna ustawiaj�cy format */
		EmoticonSelector* emoticon_selector; /*!< okienko do 
							wyboru emotikonek */
		ColorSelector* color_selector; /*!< okienko do wyboru koloru */
		QPushButton* boldbtn; /*!< przycisk pogrubiaj�cy czcionk� */
		QPushButton* italicbtn; /*!< przycisk ustawiaj�cy 
					czcionk� na kursyw� */			
		QPushButton* underlinebtn; /*!< przycisk podkre�laj�cy tekst */
		QPushButton* colorbtn; /*!< przycisk umo�liwiaj�cy 
					wyb�r koloru czcionki*/
		QPushButton* iconsel;
		QPushButton* autosend; /*!< przycisk automatycznego wysy�ania */
		QPushButton* lockscroll; /*!< przisk blokowania 
						przesuwania rozmowy */
		QAccel* acc; /*!< zmienna potrzebna do
				 rejestracji skr�t�w klawiszowych*/
		QPushButton* sendbtn; /*!< przycisk do wysy�ania wiadomo�ci */
		UserBox* userbox; /*!< lista kontakt�w przydatna gdy jeste�my w
						 konferencji */
		QString myLastMessage;/*!< zmienna przechowuj�ca 
					nasz� ostatni� wiadomo�� */
		int myLastFormatsLength; /* ?? */
		void* myLastFormats; /* ?? */
		int seq; /* ?? */
		KaduSplitter *vertSplit, *horizSplit; /*!< obiekty oddzielaj�ce
							 kontrolki od siebie */
		int ParagraphSeparator; /* ?? */
		QDateTime lastMsgTime; /*!< czas ostatniej wiadomo�ci */

		/**
			\fn void pruneWindow()
			Funkcja czyszcz�ca okno rozmowy
		**/
		void pruneWindow(); 

	private slots:
		
		/**
			\fn void userWhois()
			Slot wy�wietlaj�cy informacje o zaznaczonym kontakcie
		**/
		void userWhois();
		
		/**
			\fn void emoticonSelectorClicked()
			Slot wy�wietlaj�cy okno z wyborem emotikonek
		**/
		void emoticonSelectorClicked();
		
		/**
			\fn void changeColor()
			Slot zmieniaj�cy kolor czcionki
		**/
		void changeColor();
		
		/**
			\fn void addMyMessageToHistory()
			Slot dodaje wys�an� wiadomo�� do historii rozmowy
		**/
		void addMyMessageToHistory();
		
		/**
			\fn void clearChatWindow()
			Slot czyszcz�cy okno rozmowy
		**/
		void clearChatWindow();
		
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
			\fn void insertImage()
			Slot wklejaj�cy obrazek do okna rozmowy (wyb�r obrazka)
		**/
		void insertImage();
		
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
		CustomInput* edit; /*!< okno do wpisywania wiadomo�ci */
		QHBox* buttontray;/* ?? */
		
		/**
			Konstruktor okna rozmowy
			\fn Chat(UinsList uins, QWidget* parent = 0, const char* name = 0)
			\param parent rodzic okna
			\param name nazwa obiektu
		**/
		Chat(UinsList uins, QWidget* parent = 0, const char* name = 0);
		
		/**
			\fn ~Chat()
			Destruktor okna rozmowy
		**/
		~Chat();
		
		/**
			\fn static void registerButton(const QString& name, QObject* receiver, const QString& slot)
			Rejestruje przycisk w oknie chat, ustawia funkcje do obs�ugi 
			sygna�u "clicked"
			\param name nazwa przycisku aby moc si� do niego odwo�a�
			\param receiver obiekt kt�ry zawiera funkcje do obs�ugi zdarzenia
			\param slot funkcja kt�ra b�dzie obs�ugiwa� zdarzenie "clicked"
		**/
		static void registerButton(const QString& name, QObject* receiver, const QString& slot);
		
		/**
			\fn static void unregisterButton(const QString& name)
			Wyrejestrowuje przycisk ze wszystkich okien
			\param name nazwa przycisku do wyrejestrowania
		**/
		static void unregisterButton(const QString& name);
		
		/**
			\fn QPushButton* button(const QString& name) const
			Zwraca wska�nik do przycisku 
			\param name nazwa przycisku
			\return wska�nik do przycisku, je�li przycisk 
			nie istnieje zwraca NULL
		**/
		QPushButton* button(const QString& name) const;
		
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
			\fn void checkPresence(UinsList, const QString&, time_t, QValueList<ChatMessage *> &messages)
			Dodaje wiadomo�� do okna
			\param senders lista u�ytkownik�w
			\param msg wiadomo�c
			\param time czas 
			\param messages lista wiadomo�ci do wpisania do okna
		**/
		void checkPresence(UinsList senders, const QString &msg, time_t time, QValueList<ChatMessage *> &messages);
		
		/**
			\fn void writeMessagesFromHistory(UinsList senders, time_t time)
			Wpisuje do okna histori� rozmowy
			\param senders lista u�ytkownik�w identyfikuj�cych okno
			\param time ..
		**/
		void writeMessagesFromHistory(UinsList senders, time_t time);
		
		/**
			\fn const UinsList& uins() const
			Zwraca list� numer�w rozmowc�w.
		**/
		const UinsList& uins() const;
		
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

	public slots:
		
		/**
			\fn void changeAppearance()
			Slot zmieniaj�cy kolory i czcionki w oknie
		**/
		void changeAppearance();
		
		/**
			\fn void setTitle()
			Slot ustawiaj�cy tytu� okna zgodnie z konfiguracj�
		**/
		void setTitle();
		
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
			\fn void HistoryBox()
			Slot wywo�ywany po naci�ni�ciu przycisku historii
		**/
		void HistoryBox();
		
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
			\fn void toggledBold(bool on)
			Slot wywo�ywany przy naci�ni�ciu przycisku pogrubienia
			\param on ustawia pogrubienie
		**/
		void toggledBold(bool on);
		
		/**
			\fn void toggledItalic(bool on)
			Slot wywo�ywany przy naci�ni�ciu przycisku kursywy
			\param on ustawia kursyw�
		**/
		void toggledItalic(bool on);
		
		/**
			\fn void toggledUnderline(bool on)
			Slot wywo�ywany przy naci�ni�ciu przycisku podkre�lenia
			\param on ustawia podkre�lenie
		**/
		void toggledUnderline(bool on);
		
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
		
	signals:
		/**
			\fn void messageSendRequested(Chat* chat)
			Sygnal jest emitowany gdy uzytkownik wyda polecenie
			wyslania wiadomosci, np klikajac na guzik "wyslij".
			\param chat wska�nik do okna kt�re emituje sygna�
		**/
		void messageSendRequested(Chat* chat);
		
		/**
			\fn void messageFiltering(const UinsList& uins, QCString& msg, bool& stop)
			Sygnal daje mozliwosc operowania na wiadomoci
			ktora ma byc wyslana do serwera juz w jej docelowej
			formie po konwersji z unicode i innymi zabiegami.
			Tresc wiadomosci mozna zmienic podmieniajac wskaznik
			msg na nowy bufor i zwalniajac stary (za pomoca free).
			Mozna tez przerwac dalsza jej obrobke ustawiajac
			wskaznik stop na true.
			\param uins lista u�ytkownik�w
			\param msg wiadomo�� 
			\param stop zako�czenie dalszej obr�bki sygna�u
		**/
		void messageFiltering(const UinsList& uins, QCString& msg, bool& stop);
		
		/**
			\fn void messageSent(Chat* chat)
			Sygnal jest emitowany gdy zakonczy sie proces
			wysylania wiadomosci i zwiazanych z tym czynnosci.
			Oczywiscie nie wiemy czy wiadomosc dotarla.
			\param chat wska�nik do okna rozmowy,
			 kt�re emitowa�o sygna�
		**/		
		void messageSent(Chat* chat);
};

/**
	\class ColoSelectorButton
	\brief Klasa do wyboru koloru za pomoc� przycisku
**/
class ColorSelectorButton : public QPushButton
{
	Q_OBJECT

	private:
		QColor color;/*!< kolor czcionki */

	private slots:
		/**
			\fn void buttonClicked()
			Slot obs�uguj�cy naci�ni�cie przycisku
		**/
		void buttonClicked();

	public:
		/**
			\fn ColorSelectorButton(QWidget* parent, const QColor& qcolor, int width=1, const char *name=0)
			Konstruktor tworz�cy okno do wyboru koloru
		**/
		ColorSelectorButton(QWidget* parent, const QColor& qcolor, int width=1, const char *name=0);

	signals:
		/**
			\fn void clicked(const QColor& color)
			Sygna� zostaje wys�any po wybraniu koloru
		**/
		void clicked(const QColor& color);
};

/**
	\clas ColorSelector
	\brief Klasa do wyboru kolor�w
**/
class ColorSelector : public QWidget
{
	Q_OBJECT

	private slots:
		/**
			\fn void iconClicked(const QColor& color);
			Slot obs�uguj�cy wybranie koloru
		**/
		void iconClicked(const QColor& color);

	protected:
		/**
			\fn void closeEvent(QCloseEvent*)
			Funkcja obs�uguj�ca zamkni�cie okna wyboru kolor�w
		**/
		void closeEvent(QCloseEvent*);

	public:
		/**
			\fn ColorSelector(const QColor &defColor, QWidget* parent = 0, const char* name = 0)
			Konstruktor obiektu do wyboru kolor�w
		**/
		ColorSelector(const QColor &defColor, QWidget* parent = 0, const char* name = 0);

	public slots:
		/**
			\fn void alignTo(QWidget* w)
		  	Slot wyr�wnuje pozycj� do widgeta
			\param w wka�nik do obiektu ktorego b�dziemy wyr�wnywac
		**/
		void alignTo(QWidget* w);

	signals:
		/**
			\fn void aboutToClose()
			Sygna� jest emitowany gdy zamykany jest wyb�r kolor�w
		**/
		void aboutToClose();
		
		/**
			\fn void colorSelect(const QColor&)
			Sygna� jest emitowany gdy wybrano kolor
			
		**/
		void colorSelect(const QColor&);
};
/**
	\class ChatSlots
	\brief Sloty do obs�ugi okna konfiguracji
**/
class ChatSlots :public QObject
{
	Q_OBJECT

	private:
		/**
			\fn void updatePreview()
			Od�wie�enie wygl�du
		**/
		void updatePreview();
	public:
		/**
			Konstruktor obiektu
			\param parent rodzic obiektu
			\param name nazwa obiektu
		**/
		ChatSlots(QObject* parent=0, const char* name=0);

	public slots:
		/**
			\fn void onCreateConfigDialog()
			Slot jest wywo�ywany gdy otwierane jest okno konfiguracji
		**/
		void onCreateConfigDialog();
		
		/**
			\fn void onDestroyConfigDialog()
			Slot jest wywo�ywany gdy zamykane jest okno konfiguracji
		**/
		void onDestroyConfigDialog();
		
		/**
			\fn void chooseEmoticonsStyle(int index)
			Slot jest wywo�ywany gdy wybierany jest styl ikonek
			\param index nr stylu emotikonek
		**/
		void chooseEmoticonsStyle(int index);
		
		/**
			\fn void onPruneChat(bool toggled)
			Slot jest wywo�ywany gdy wybierane jest czy otwierane okno
			ma by� pusty czy nie
			\param toggled 
		**/
		void onPruneChat(bool toggled);
		
		/**
			\fn void onFoldLink(bool toggled)
			\param toggled
		**/
		void onFoldLink(bool toggled);
		
		/**
			\fn void onBlockClose(bool toggled)
			Slot jest wywo�ywany gdy wybierany jest maksymalny czas
			blokowania zamkni�cia okna po otrzymaniu wiadomo�ci
			\param toggled
		**/
		void onBlockClose(bool toggled);
		
		/**
			\fn void chooseColor(const char* name, const QColor& color)
			Slot jest wywo�ywany gdy zmieniono kolor,
			aby uaktualni� podgl�d 
			\param name nazwa koloru
			\param color kolor
		**/
		void chooseColor(const char* name, const QColor& color);
		
		/**
			\fn void chooseFont(const char* name, const QFont& font)
			Slot jest wywo�ywany gdy zmieniono czcionke,
			aby uaktualni� podgl�d 
			\param name nazwa czcionki
			\param font czcionka
		**/
		void chooseFont(const char* name, const QFont& font);
		
		/**
			\fn void findAndSetWebBrowser(int selectedBrowser)
			Slot jest wywo�ywany gdy wybrano z listy przegl�dark�
			\param selectedBrowser nr przegl�darki
		**/
		void findAndSetWebBrowser(int selectedBrowser);
		
		/**
			\fn void findAndSetBrowserOption(int selectedOption)
			Slot jest wywo�ywany gdy aktywowane pole do wpisania opcji
			\param selectedOption wybrana opcja
		**/
		void findAndSetBrowserOption(int selectedOption);
		
		/**
			\fn static void initBrowserOptions(QComboBox *browserCombo, QComboBox *browserOptionsCombo, QLineEdit *browserPath)
			Slot ustawia przegl�darki wraz z opcjami
			\param browserCombo
			\param browserOptionsCombo
			\param browserPath
		**/
		static void initBrowserOptions(QComboBox *browserCombo, QComboBox *browserOptionsCombo, QLineEdit *browserPath);
		
		/**
			\fn static void findBrowser(int selectedBrowser, QComboBox *browserCombo, QComboBox *browserOptionsCombo, QLineEdit *browserPath)
			Slot jest wywo�ywany gdy wybrano z listy przegl�dark�
			\param selectedBrowser
			\param browserCombo
			\param browserOptionsCombo
			\param browserPath
		**/
		static void findBrowser(int selectedBrowser, QComboBox *browserCombo, QComboBox *browserOptionsCombo, QLineEdit *browserPath);
		
		/**
			\fn static void setBrowserOption(int selectedOption, QLineEdit *browserPathEdit, int chosenBrowser)
			Slot ustawia przegl�darki wraz z opcjami
			\param selectedOption
			\param browsePathEdit
			\param chosenBrowser
		**/
		static void setBrowserOption(int selectedOption, QLineEdit *browserPathEdit, int chosenBrowser);
};

#endif
