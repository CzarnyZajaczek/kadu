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

typedef QValueList<Chat*> ChatList;

/**
	Klasa przechowuj�ca informacje o wiadomo�ci,
	kt�ra ma si� pojawi� w oknie Chat
**/
class ChatMessage
{
	public:
	QString nick;
	QDateTime date;
	QDateTime sdate;
	QString unformattedMessage;
	bool isMyMessage;
	QColor backgroundColor;
	QColor textColor;
	
	/**
		Mo�e s�u�y� do przechowywania informacji o tym
		czy wiadomo�� by�a potwierdzona, szyfrowana, ...
		W zamy�le ma s�u�y� do okre�lania czy jaki� obrazek
		ma by� dodawany do wiadomo�ci czy nie - jaki obrazek -
		ta informacja b�dzie gdzie indziej
	**/
	QMap<QString, bool> attributes; 
	
	//inne atrybuty?
	//QMap<QString, QString> stringAttributes;
	
	bool needsToBeFormatted;
	
	/**
		Sformatowana wiadomo�� (razem z <p> lub <table>)
	**/
	QString message;
	/**
		Konstruktor ustawiaj�cy nick, niesformatowan� wiadomo��,
		czy wiadomo�� pochodzi od nas, dat� wiadomo�ci,
		dat� nadania wiadomo�ci przez serwer
	**/
	ChatMessage(const QString &nick, const QString &unformattedMessage, bool myMessage, QDateTime date, QDateTime sdate=QDateTime());
	/**
		Konstruktor ustawiaj�cy sformatowan� wiadomo��, kolor t�a i liter wiadomo�ci
	**/
	ChatMessage(const QString &formattedMessage, const QColor &bgColor=Qt::white, const QColor &textColor=Qt::black);
};
/**
	Klasa zarz�dzaj�ca oknami Chat
**/
class ChatManager : public QObject
{
	Q_OBJECT

	private:
		ChatList Chats;
		int openPendingMsg(int index, ChatMessage &msg);
		
		struct ChatInfo
		{
			UinsList uins;
			QMap<QString, QVariant> map;
		};
		QValueList<ChatInfo> addons;

	public:	
		ChatManager(QObject* parent=NULL, const char* name=NULL);
		~ChatManager();
		/**
			Zwraca liste otwartych okien Chat.
		**/
		const ChatList& chats() const;
		/**
			Zwraca wska�nik do okna z list� u�ytkownik�w uins
			@param uins lista u�ytkownik�w
			@return wska�nik do okna je�li istnieje w przeciwnym wypadku zwraca NULL
		**/
		Chat* findChatByUins(const UinsList &uins) const;
		/**
			zwraca warto�� w�asno�ci "name" okna okre�lonego przez uins
			@param uins lista u�ytkownik�w identyfikuj�ca okno
			@param name nazwa w�asno�ci
			@return zwraca warto�� w�asno�ci je�li okre�lone okno istnieje,
			je�li nie to tworzy tak� w�asno�� (ustawia na pust�)
		**/
		QVariant& getChatProperty(const UinsList &uins, const QString &name);
		
	public slots:
		/**
			Slot informuj�cy o odebraniu wiadomo�ci
		**/
		void chatMsgReceived(UinsList senders,const QString& msg,time_t time,bool& grab);

		/**
			Otwiera nowe okno Chat z wymienionymi rozmowcami.
			Parametr time sluzy do sprawdzenia ile wiadomosci
			z historii ma sie pojawic w oknie.
		**/
		int openChat(UinsList senders,time_t time=0);		
		/**
			Wpisuje zakolejkowane wiadomo�ci do okna z u�ytkownikami "uins"
		**/
		void openPendingMsgs(UinsList uins);
		/**
			Wpisuje wszystkie zakolejkowane wiadomo�ci do odpowiednich okien
		**/
		void openPendingMsgs();
		/*
			Niebardzo rozumiem tej funkcji (czemu jest uin i uins)	
		*/
		void sendMessage(UinType uin,UinsList selected_uins);
		/**
			Zamyka wszystkie okna chat
		**/
		void closeAllWindows();
		/**
			Dodaje okno do managera
			@param chat wska�nik do okna ktore chcemy doda�
			@return zwraca numer naszego okna po zarejestrowaniu
		**/
		int registerChat(Chat* chat);
		/**
			Wyrejestrowuje okno z managera
			Zapisuje w�asno�ci okna
			wysy�a sygna� chatDestroying i chatDestroyed
		**/
		void unregisterChat(Chat* chat);
		/**
			Od�wie�a tytu�y wszystkich okien
		**/
		void refreshTitles();
		/**
			Od�wie�a tytu�y okien kt�re zawieraj� uin
		**/
		void refreshTitlesForUin(UinType uin);
		/**
			Od�wie�a kolory i czcionki we wszystkich oknach
		**/
		void changeAppearance();
		/**
			pozwala przypisa� okre�lonemu czatowi (nawet je�eli on jeszcze nie istnieje)
			pewne w�asno�ci
		**/
		void setChatProperty(const UinsList &uins, const QString &name, const QVariant &value);
		
	signals:
		/**
		 	Sygna� ten jest wysy�any po utworzeniu nowego okna chat
			@param senders lista uin-�w z kt�rymi tworzymy nowy chat
		**/
		void chatCreated(const UinsList& senders);
		/**
		 	Sygna� ten jest wysy�any przed zamnkni�ciem okna chat
			@param senders lista uin-�w z kt�re wyst�puj� w tym chacie
		**/
		void chatDestroying(const UinsList& senders);
		/**
		 	Sygna� ten jest wysy�any po zamnkni�ciem okna chat
			@param senders lista uin-�w z kt�re wyst�powa�y w tym chacie
		**/
		void chatDestroyed(const UinsList& senders);
		/**
			Sygna� ten jest wysy�aniy podczas ka�dej pr�by otwarcia nowego okna chcat
			nawet je�li ju� taki istnieje
			@param senders lista uin-�w
		**/
		void chatOpen(const UinsList& senders);
};

extern ChatManager* chat_manager;

class EmoticonSelector;
class ColorSelector;
/**
	Klasa umo�liwiaj�ca wpisywanie wiadomo�ci
**/
class CustomInput : public QMultiLineEdit
{
	Q_OBJECT

	protected:
		bool autosend_enabled;
		virtual void keyPressEvent(QKeyEvent *e);
		virtual void keyReleaseEvent(QKeyEvent* e);

	public:
		enum
		{
			KEY_BOLD,
			KEY_ITALIC,
			KEY_UNDERLINE
		};
		CustomInput(QWidget* parent = 0, const char* name = 0);

	public slots:
		void paste();
		/**
			Ustawia/wy��cza automatyczne wysy�anie wiadomo�ci
		**/
		void setAutosend(bool);

	signals:
		/**
			Sygna� zostaje wys�any kiedy naci�ni�to 
			klawisz wys�ania wiadomo�ci
		**/
		void sendMessage();
		/**
			Sygna� zostaje wys�any gdy naci�ni�to specjalny klawisz (skr�t)
			odpowiadaj�cy np KEY_BOLD
		**/
		void specialKeyPressed(int key);
		/**
			Dowolny przycisk klawiatury zosta� naci�ni�ty.
			Przekazany zostaje tak�e obiekt, kt�ry wywo�a� akcj� - czyli this.
		**/
		void keyPressed(QKeyEvent* e, CustomInput* sender);
		/**
			Dowolny przycisk klawiatury zosta� zwolniony.
			Przekazany zostaje tak�e obiekt, kt�ry wywo�a� akcj� - czyli this.
		**/
		void keyReleased(QKeyEvent* e, CustomInput* sender);
};

/**
	Klasa wykorzystana do oddzielenia listy u�ytkownik�w od historii
	i pola do wpisywania wiadomo�ci
**/
class KaduSplitter : public QSplitter
{
	protected:
		QValueList<KaduTextBrowser*> textbrowsers;
		void drawContents(QPainter* p);
		void childEvent(QChildEvent* c);

	public:
		KaduSplitter(QWidget* parent = 0, const char* name = 0);
		KaduSplitter(Orientation o, QWidget* parent = 0, const char* name = 0);
};

class QMimeSourceFactory;
/**
	Okno rozmowy
**/
class Chat : public QWidget
{
	Q_OBJECT

	private:
		friend class ChatManager;
		struct RegisteredButton
		{
			QString name;
			QObject* receiver;
			QString slot;
		};
		static QValueList<RegisteredButton> RegisteredButtons;
		QMap<QString,QPushButton*> Buttons;
		
		QValueList<ChatMessage *> ChatMessages;
	
		UinsList Uins;
		int index;
		QString title_buffer;
		QTimer* title_timer;  
		QColor actcolor;
		
		QMimeSourceFactory *bodyformat;
		EmoticonSelector* emoticon_selector;
		ColorSelector* color_selector;
		QPushButton* boldbtn;
		QPushButton* italicbtn;
		QPushButton* underlinebtn;
		QPushButton* colorbtn;
		QPushButton* iconsel;
		QPushButton* autosend;
		QPushButton* lockscroll;
		QAccel* acc;
		QPushButton* sendbtn;
		UserBox* userbox;
		QString myLastMessage;
		int myLastFormatsLength;
		void* myLastFormats;
		int seq;
		KaduSplitter *vertSplit, *horizSplit;
		int ParagraphSeparator;
		QDateTime lastMsgTime;

		void pruneWindow();

	private slots:
		void userWhois();
		void emoticonSelectorClicked();
		void changeColor();
		void addMyMessageToHistory();
		void clearChatWindow();
		void pageUp();
		void pageDown();
		void insertImage();
		void imageReceivedAndSaved(UinType sender,uint32_t size,uint32_t crc32,const QString& path);

		void connectAcknowledgeSlots();
		void disconnectAcknowledgeSlots();

	protected:
		virtual void closeEvent(QCloseEvent*);
		QString convertCharacters(QString text, const QColor &bgcolor, EmoticonsStyle style);
		virtual void windowActivationChange(bool oldActive);
		virtual void keyPressEvent(QKeyEvent* e);
		virtual void mouseReleaseEvent(QMouseEvent *e);

	public:
		// FIXME - nie powinno byc publicznych zmiennych
		KaduTextBrowser* body;
		CustomInput* edit;
		QHBox* buttontray;
		/**
			Rejestruje opcje modulu Chat w oknie konfiguracji.
		**/
		static void initModule();
		Chat(UinsList uins, QWidget* parent = 0, const char* name = 0);
		~Chat();
		/**
			Rejestruje przycisk w oknie chat, ustawia funkcje do obs�ugi 
			sygna�u "clicked"
			@param name nazwa przycisku aby moc si� do niego odwo�a�
			@param receiver obiekt kt�ry zawiera funkcje do obs�ugi zdarzenia
			@param slot funkcja kt�ra b�dzie obs�ugiwa� zdarzenie "clicked"
		**/
		static void registerButton(const QString& name, QObject* receiver, const QString& slot);
		/**
			Wyrejestrowuje przycisk ze wszystkich okien
			@param name nazwa przycisku do wyrejestrowania
		**/
		static void unregisterButton(const QString& name);
		/**
			Zwraca wska�nik do przycisku 
			@param name nazwa przycisku
			@return wska�nik do przycisku, je�li przycisk nie istnieje
			zwraca NULL
		**/
		QPushButton* button(const QString& name) const;
		/**
			Formatuje wszystkie wiadomo�ci na html'a,
			dodaje kolory t�a i czcionek
			@param msgs lista wiadomo�ci do sformatowania
		**/
		void formatMessages(QValueList<ChatMessage *> &msgs);
		/**
			Formatuje wiadomo�� na html'a
			@param msg wiadomo�� kt�ra b�dzie sformatowana
			@param myBgColor kolor kt�ry b�dzie ustawiony jako t�o
			w naszych wiadomo�ciach
			@param usrBgColor kolor, kt�ry b�dzie ustawiony jako t�o
			wiadomo�ci naszego rozm�wcy
			@param myFontColor kolor naszej czcionki
			@param usrFontColor kolor czcionki naszego rozm�wcy
			@param style styl ikonek 
		**/
		void formatMessage(ChatMessage &msg,
				QColor myBgColor=QColor(), QColor usrBgColor=QColor(),
				QColor myFontColor=QColor(), QColor usrFontColor=QColor(),
				EmoticonsStyle style=(EmoticonsStyle)config_file.readNumEntry("Chat","EmoticonsStyle"));
		/**
			Dodaje wiadomo�� do okna
		**/
		void checkPresence(UinsList, const QString&, time_t, QValueList<ChatMessage *> &messages);
		/**
			Wpisuje do okna histori� rozmowy
			@param senders lista u�ytkownik�w identyfikuj�cych okno
			@param time ..
		**/
		void writeMessagesFromHistory(UinsList, time_t);
		/**
			Zwraca list� numer�w rozmowc�w.
		**/
		const UinsList& uins() const;
		/**
			Daje dost�p do wiadomo�ci aktualnie przechowywanych
			w oknie chat. Metody tej mo�na u�y� do zmiany tre�ci
			lub w�a�ciwo�ci kt�rej� z wiadomo�ci w odpowiedzi
			na jakie� zdarzenie.
		**/
		QValueList<ChatMessage*>& chatMessages();
		/**
			Od�wie�a zawarto�� okna uwzgl�dniaj�c ewentualne
			zmiany dokonane w kt�rej� wiadomo�ci z listy
			uzyskanej za pomoc� metody chatMessages(),
			dodanie nowych wiadomo�ci lub usuni�cie istniej�cych.
		**/
		void repaintMessages();
		/**
			Zwraca aktualny tytu� okna
		**/
		const QString& title() const;

	public slots:
		/**
			Slot zmieniaj�cy kolory i czcionki w oknie
		**/
		void changeAppearance();
		/**
			Slot ustawiaj�cy tytu� okna zgodnie z konfiguracj�
		**/
		void setTitle();
		/**
			Slot dodaje ikonke do pola wpisywania wiadomo�ci
			@param emot ikonka np. <lol>
		**/
		void addEmoticon(QString);
		/**
			Slot dodaj wiadomo�ci do okna
			@param messages lista wiadomo�ci
		**/
		void scrollMessages(const QValueList<ChatMessage *> &);
		/**
			Slot informuj�cy o nowej wiadomo�ci 
			mruganie je�li okno jest nieaktywne
		**/
		void alertNewMessage();
		/**
			Slot wywo�ywany po naci�ni�ciu przycisku historii
		**/
		void HistoryBox();
		/**
			Slot wywo�ywany po naci�ni�ciu przycisku do wysy�ania wiadomo�ci
		**/
		void sendMessage();
		/**
			Slot wywo�ywany po naci�nieciu przycisku anulowania
			wysy�aniu wiadomo�ci
		**/
		void cancelMessage();
		/**
			Slot wpisuj�cy wiadomo�� do okna
			@see sendMessage
		**/
		void writeMyMessage();
		/**
		    	Slot zmieniaj�cy tytu� 
		    	@see setTitle
		**/
		void changeTitle();
		/**
			Slot wywo�ywany przy naci�ni�ciu przycisku pogrubienia
		**/
		void toggledBold(bool on);
		/**
			Slot wywo�ywany przy naci�ni�ciu przycisku kursywy
		**/
		void toggledItalic(bool on);
		/**
			Slot wywo�ywany przy naci�ni�ciu przycisku podkre�lenia
		**/
		void toggledUnderline(bool on);
		/**
			Slot wywo�ywany kiedy pozycja kursosa zmieni�a si�
		**/
		void curPosChanged(int para, int pos);
		/**
			Slot zostaje wywo�any gdy naci�ni�to specjalny klawisz (skr�t)
			odpowiadaj�cy np KEY_BOLD
		**/
		void specialKeyPressed(int key);
		/**
			Slot zostaje wywo�wany gdy u�ytkownik zmieni� kolor czcionki
			kt�r� b�dzie pisa�
		**/
		void colorChanged(const QColor& color);
		/**
			Slot zostaje wywo�any przy zamykaniu okna wyboru ikonek
		**/
		void colorSelectorAboutToClose();
		/**
			Slot zostaje wywo�any gdy nasza wiadomo�� zosta�a zablokowana
			przez serwer
		**/
		void messageBlockedSlot(int seq, UinType uin);
		/**
			Slot zostaje wywo�any gdy skrzynka na wiadomo�ci
			zosta�a zape�niona
		**/
		void messageBoxFullSlot(int seq, UinType uin);
		/**
			Slot zostaje wywo�any gdy wiadomo�� nie zosta�a dostarczona
		**/
		void messageNotDeliveredSlot(int seq, UinType uin);
		/**
			Slot zostaje wywo�any gdy wiadomo�� zosta�a poprawnie wys�ana
		**/
		void messageAcceptedSlot(int seq, UinType uin);
		
	signals:
		/**
			Sygnal jest emitowany gdy uzytkownik wyda polecenie
			wyslania wiadomosci, np klikajac na guzik "wyslij".
		**/
		void messageSendRequested(Chat* chat);
		/**
			Sygnal daje mozliwosc operowania na wiadomoci
			ktora ma byc wyslana do serwera juz w jej docelowej
			formie po konwersji z unicode i innymi zabiegami.
			Tresc wiadomosci mozna zmienic podmieniajac wskaznik
			msg na nowy bufor i zwalniajac stary (za pomoca free).
			Mozna tez przerwac dalsza jej obrobke ustawiajac
			wskaznik stop na true.
		**/
		void messageFiltering(const UinsList& uins, QCString& msg, bool& stop);
		/**
			Sygnal jest emitowany gdy zakonczy sie proces
			wysylania wiadomosci i zwiazanych z tym czynnosci.
			Oczywiscie nie wiemy czy wiadomosc dotarla.
		**/		
		void messageSent(Chat* chat);
};

class ColorSelectorButton : public QPushButton
{
	Q_OBJECT

	private:
		QColor color;

	private slots:
		void buttonClicked();

	public:
		ColorSelectorButton(QWidget* parent, const QColor& qcolor, int width=1, const char *name=0);

	signals:
		/**
			Sygna� zostaje wys�any po wybraniu koloru
		**/
		void clicked(const QColor& color);
};

class ColorSelector : public QWidget
{
	Q_OBJECT

	private slots:
		void iconClicked(const QColor& color);

	protected:
		void closeEvent(QCloseEvent*);

	public:
		ColorSelector(const QColor &defColor, QWidget* parent = 0, const char* name = 0);

	public slots:
		/**
		  	Slot wyr�wnuje pozycj� do widgeta
		**/
		void alignTo(QWidget* w);

	signals:
		/**
			Sygna� jest emitowany gdy zamykany jest wyb�r kolor�w
		**/
		void aboutToClose();
		/**
			Sygna� jest emitowany gdy wybrano kolor
		**/
		void colorSelect(const QColor&);
};

class ChatSlots :public QObject
{
	Q_OBJECT

	private:
		void updatePreview();
	public:
		ChatSlots(QObject* parent=0, const char* name=0);

	public slots:
		/**
			Slot jest wywo�ywany gdy otwierane jest okno konfiguracji
		**/
		void onCreateConfigDialog();
		/**
			Slot jest wywo�ywany gdy zamykane jest okno konfiguracji
		**/
		void onDestroyConfigDialog();
		/**
			Slot jest wywo�ywany gdy wybierany jest styl ikonek
		**/
		void chooseEmoticonsStyle(int index);
		/**
			Slot jest wywo�ywany gdy wybierane jest czy otwierane okno
			ma by� pusty czy nie
		**/
		void onPruneChat(bool toggled);
		/**
		**/
		void onFoldLink(bool toggled);
		/**
			Slot jest wywo�ywany gdy wybierany jest maksymalny czas
			blokowania zamkni�cia okna po otrzymaniu wiadomo�ci
		**/
		void onBlockClose(bool toggled);
		/**
			Slot jest wywo�ywany gdy zmieniono kolor,
			aby uaktualni� podgl�d 
		**/
		void chooseColor(const char* name, const QColor& color);
		/**
			Slot jest wywo�ywany gdy zmieniono czcionke,
			aby uaktualni� podgl�d 
		**/
		void chooseFont(const char* name, const QFont& font);
		/**
			Slot jest wywo�ywany gdy wybrano z listy przegl�dark�
		**/
		void findAndSetWebBrowser(int selectedBrowser);
		/**
			Slot jest wywo�ywany gdy aktywowane pole do wpisania opcji
		**/
		void findAndSetBrowserOption(int selectedOption);
		/**
			Slot ustawia przegl�darki wraz z opcjami
		**/
		static void initBrowserOptions(QComboBox *browserCombo, QComboBox *browserOptionsCombo, QLineEdit *browserPath);
		/**
			Slot jest wywo�ywany gdy wybrano z listy przegl�dark�
		**/
		static void findBrowser(int selectedBrowser, QComboBox *browserCombo, QComboBox *browserOptionsCombo, QLineEdit *browserPath);
		/**
			Slot ustawia przegl�darki wraz z opcjami
		**/
		static void setBrowserOption(int selectedOption, QLineEdit *browserPathEdit, int chosenBrowser);
};

#endif
