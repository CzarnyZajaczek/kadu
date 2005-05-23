#ifndef KADU_MISC_H
#define KADU_MISC_H

#include <qcstring.h>
#include <qdatetime.h>
#include <qdialog.h>
#include <qfiledialog.h>
#include <qfont.h>
#include <qfontinfo.h>
#include <qiconset.h>
#include <qimage.h>
#include <qlabel.h>
#include <qmap.h>
#include <qpair.h>
#include <qpixmap.h>
#include <qstringlist.h>
#include <qsocket.h>
#include <qstring.h>
#include <qtextbrowser.h>
#include <qtooltip.h>
#include <qvaluelist.h>
#include <qvariant.h>

#include <inttypes.h>

#include "config_file.h"
#include "gadu.h"

class QComboBox;
class QLineEdit;
class QMenuData;

/**
	\fn QString ggPath(const QString &subpath="")
	Zmienia �cie�k� relatywn� do katalogu z ustawieniami gg
	na �cie�k� bezwzgledn� uwzgl�dniaj�c zmienne �rodowiskowe
	\c $HOME i \c $CONFIG_DIR.
	Funkcja ta jest przydatna, gdy chcemy si� odwo�a� do jakiego�
	pliku/katalogu znajduj�cego si� w katalogu ustawie� aktualnej
	sesji Kadu.
	\param subpath �cie�ka relatywna do katalogu z ustawieniami gg
	\return �cie�k� bezwzgl�dn� do pliku.
	
	<b>Przyk�ad:</b>
	\code
	MessageBox::msg(ggPath("kadu.conf"));
	\endcode
	Wy�wietlone zostanie okienko z pe�n� �cie�k� do pliku konfiguracyjnego Kadu.
**/
QString ggPath(const QString &subpath="");

/**
	\fn QString dataPath(const QString &f = "", const char *argv0 = 0)
	Zwraca bezwzgl�dn� �cie�k� do danego pliku.
	Je�eli drugi parametr jest r�ny od 0, to funkcja pr�buje najpierw ustali�
	�cie�k� na podstawie \arg argv0, kt�re ma by� r�wne argv[0], oraz zmiennej PATH.
	\param f nazwa pliku (�cie�ka relatywna do katalogu z danymi Kadu) do kt�rego
			ma zosta� zwr�cona pe�na, bezwzgl�dna �cie�ka.
	\param argv0 nazwa uruchomionego programu. Mo�e to by� zar�wno '<i>kadu</i>',
			jak i '<i>kadu-kopia2</i>'. Domy�lnie jest r�wny 0.
	\return �cie�k� bezwzgl�dn� do pliku.
**/
QString dataPath(const QString &f = "", const char *argv0 = 0);

class UserListElement;

/**
	\fn QString cp2unicode(const unsigned char *buf)
	Podmienia w danym �a�cuchu znaki z kodowania CP1250
	na kodowanie Unicode.
	\param buf �a�cuch znak�w do przekodowania
	\return przekodowany �a�cuch.
**/
QString cp2unicode(const unsigned char *);

/**
	\fn QCString unicode2cp(const QString &buf)
	Podmienia w danym �a�cuchu znaki z kodowania Unicode
	na kodowanie CP1250.
	\param buf �a�cuch znak�w do przekodowania
	\return przekodowany �a�cuch.
**/
QCString unicode2cp(const QString &);

/**
	\fn QString latin2unicode(const unsigned char *buf)
	Podmienia w danym �a�cuchu znaki z kodowania Latin2
	na kodowanie Unicode.
	\param buf �a�cuch znak�w do przekodowania
	\return przekodowany �a�cuch.
**/
QString latin2unicode(const unsigned char *);

/**
	\fn QCString unicode2latin(const QString &buf)
	Podmienia w danym �a�cuchu znaki z kodowania Latin2
	na kodowanie Unicode.
	\param buf �a�cuch znak�w do przekodowania
	\return przekodowany �a�cuch.
**/
QCString unicode2latin(const QString &buf);

/**
	\fn QString unicode2std(const QString &buf)
	Podmienia w danym �a�cuchu polskie znaki z kodowania Unicode
	na ich odpowiedniki w kodowaniu ISO8859-2, czyli zmienia
	\a � na \a n, \a � na \a o, \a � na s, itd.
	\param buf �a�cuch znak�w do przekodowania
	\return przekodowany �a�cuch.
**/
QString unicode2std(const QString &);

/**
	\fn QString unicodeUrl2latinUrl(const QString &buf)
	Zamienia kodowanie polskich znak�w z utf-8 na kodowanie latin-2
	przy pomocy QUrl::encode.
	\param buf �a�cuch znak�w do przekodowania
	\return przekodowany �a�cuch.
**/
QString unicodeUrl2latinUrl(const QString &buf);

/**
	\fn QString unicode2latinUrl(const QString &buf)
	Zamienia polskie znaki na format latin2 "URL" (czyli do postaci %XY,
	akceptowanej przez adresy URL).
	\param buf �a�cuch znak�w do przekodowania
	\return przekodowany �a�cuch.
**/
QString unicode2latinUrl(const QString &buf);

/**
	\fn QString printDateTime(const QDateTime &datetime)
	Formatuje dat� i czas do postaci czytelnej cz�owiekowi,
	czyli <tt>HH:MM:SS (DD.MM.YYYY)</tt>.
	\param datetime konkretna data, kt�r� nale�y sformatowa�
	\return sformatowan� dat�.
**/
QString printDateTime(const QDateTime &datetime);

/**
	\fn QString timestamp(time_t customtime = 0)
	Formatuje stempel czasowy w postaci 2 dat: aktualna oraz podana.
	Ma on posta�: <tt>HH:MM:SS (DD.MM.YYYY) /S HH:MM:SS (DD.MM.YYYY)</tt>,
	gdzie pierwsza data oznacza aktualn� dat� pobieran� podczas wykonywania tej funkcji,
	a druga data to data podana jako argument i sformatowatowana przez funkcj� unicode2latinUrl.
	Znacznik \c /S jest zwyk�ym �a�cuchem '/S'.
	\param customtime czas podany w formie sekund od pocz�tku roku 1970 (tzw. '<I>unixtime</i>'),
			kt�ry jest sformatowany i zwr�cony jako druga data.
	\return sformatowany stempel czasowy
**/
QString timestamp(time_t = 0);

/**
	\fn QDateTime currentDateTime()
	Funkcja s�u��ca �atwemu pobieraniu aktualnej daty.
	\return aktualn� dat�.
**/
QDateTime currentDateTime();

/**
	\fn QString pwHash(const QString &tekst)
	Funkcja hashuj�ca zadany �a�cuch tak, aby nie by� od �atwy do odczytania
	dla cz�owieka, bez uprzedniego potraktowania go w odpowiedni spos�b.
	\param tekst �a�cuch kt�ry ma zosta� zahashowany
	\return zahashowany �a�cuch
**/
QString pwHash(const QString &tekst);

/**
	\fn QString translateLanguage(const QApplication *application, const QString &locale, const bool l2n)
	T�umaczy nazwy skr�towe j�zyk�w na ich pe�ne nazwy i vice versa, czyli przyk�adowo
	\c pl przet�umaczy na \c Polski, a \c Polski przet�umaczy na \c pl.
	Je�li podane wyra�enie nie mo�e zosta� odnalezione w tablicy ty�umacze�, wtedy zwracany jest
	odpowiednia nazwa dla j�zyka angielskiego.
	\param application obiekt aplikacji Qt, kt�ry ma przet�umaczy�. Zwykle jest to \c qApp
	\param locale wyra�enie, kt�re ma zosta� odpowiednio przet�umaczone. Mo�e to by�:
			\c pl, \c en, \c de, \c it, ale r�wnie�:
			\c Polski, \c Angielski, \c Niemiecki, \c W�oski
	\param l2n okre�la, czy parametr \a locale jest wersj� 2-literow� (\c true), czy pe�n� nazw� (\c false)
	\return s�owo odpowiadaj�ce podanemu argumentowi \a locale.
**/
QString translateLanguage(const QApplication *application, const QString &locale, const bool l2n);

/**
	\fn void openWebBrowser(const QString &link)
	Otwiera domy�ln� przegl�dark� i wybiera w niej podany adres.
	\param link adres URL, kt�ry ma by� otwarty w przegl�darce
**/
void openWebBrowser(const QString &link);

/**
	\fn QString formatGGMessage(const QString &msg, int formats_length, void *formats, UinType sender)
	Formatuje wiadomo�� protoko�u GG do postaci HTML (pogr�bienie, kursywa, podkre�lenie, kolor).
	\param msg tre�� wiadomo�ci
	\param formats_length d�ugo�� parametr�w formatuj�cych (szczeg�y w dokumentacji libgadu)
	\param formats wska�nik do struktur formatuj�cych (szczeg�y w dokumentacji libgadu)
	\param sender UIN kontaktu, kt�ry ma by� u�yty do sformatowania wiadomo�ci (przychodz�cej)
	\return gotow� wiadomo�� sformatowan� w HTMLu.
**/
QString formatGGMessage(const QString &msg, int formats_length, void *formats, UinType sender);

/**
	\fn QString unformatGGMessage(const QString &msg, int &formats_length, void *&formats)
	Formatuje wiadomo�� z postaci HTML do formatu akceptowanego przez protok� GG.
	\param msg tre�� wiadomo�ci ze znacznikami HTML
	\param formats_length zmienna typu integer, do kt�rej zapisana zostanie d�ugo��
			parametr�w formatuj�cych (szczeg�y w dokumentacji libgadu)
	\param formats wska�nik, pod kt�rym zapisane zostan� struktury formatuj�ce
			wiadomo�� (szczeg�y w dokumentacji libgadu)
	\return tre�� wiadomo�ci z usuni�tymi znacznikami HTML (s� one teraz zakodowane
			w \a formats i \a formats_length ).
**/
QString unformatGGMessage(const QString &msg, int &formats_length, void *&formats);

/**
	\fn QString parse(const QString &s, const UserListElement &ule, bool escape = true)
	Podmienia odpowiednie znaczniki w danym �a�cuchu wed�ug danych podanego kontaktu.
	Dozwolone s� nast�puj�ce znaczniki podstawowe:
	\arg \c %%s status
	\arg \c %%t status w orginalnej (nie przet�umaczonej) formie.
	\arg \c %%d opis (lub pusty �a�cuch je�li opisu nie ma)
	\arg \c %%i adres IP (je�li znany, inaczej pusty �a�cuch)
	\arg \c %%v adres DNS (je�li znany, inaczej pusty �a�cuch)
	\arg \c %%o je�li kontakt ma u�ytkownika Kadu na li�cie, to ten znacznik podmieniany jest na pojedyncz� spacj�
	\arg \c %%p port (je�li znany, inaczej pusty �a�cuch)
	\arg \c %%u numer UIN
	\arg \c %%n pseudonim
	\arg \c %%a nazwa wy�wietlana
	\arg \c %%f imi�
	\arg \c %%r nazwisko
	\arg \c %%m numer telefonu kom�rkowego
	\arg \c %%g grupa (lub kilka grup oddzielonych przecinkiem)
	\arg \c %%e adres e-mail
	\arg \c %%x maksymalny rozmiar obrazka w KB
	\arg \c %%% pojedynczy znak '%'

	Ponadto mo�na stosowa� nawiasy \c [] i \c {}, oraz apostrofy: \c `'.

	Nawiasy kwadratowe pozwalaj� na u�ywanie swego rodzaju instrukcji warunkowej,
	kt�ra sprawdza czy wszystkie (bez wyj�tku) znaczniki zawarte pomi�dzy tymi
	nawiasami zwr�ci�y warto�� niepust�. Je�li tak, to w miejsce nawias�w wstawiany
	jest po prostu �a�cuch zawarty mi�dzy nimi (z podmienionymi ju� znacznikami),
	je�li natomiast kt�ry� ze znacznik�w zwr�ci� warto�� pust� (np. %o, co oznacza�o
	by �e kontakt nie ma u�ytkownika Kadu na li�cie), to w miejsce nawias�w wstawiana
	jest warto�� pusta.

	Nawiasy klamrowe natomiast sprawdzaj�, czy nazwa pliku - podana jako pierwszy wyraz zawarty
	w tych nawiasach - istnieje. Je�li tak, to w miejsce tych nawias�w wstawiany jest ca�y �a�cuch
	mi�dzy nimi zawarty, za wyj�tkiem tej pierwszej nazwy pliku (i odst�pu po niej). Je�li natomiast
	plik nie istnieje, to wstawiany jest pusty �a�cuch.

	Wreszcie apostrofy ` i '. Dzi�ki nim mo�na wykona� polecenie w pow�oce systemu, a to co zwr�ci
	ta komenda, zostanie wstawione w miejsce ca�ego wyra�enia mi�dzy apostrofami (��cznie z nimi samymi).

	\param s �a�cuch ze znacznikami do podmiany
	\param ule obiekt reprezentuj�cy kontakt, kt�rego dane b�d� podstawiane pod znaczniki
	\param escape warto�� logiczna decyduj�ca o tym, czy znaczniki HTML maj� zosta� zinterpretowane (\c false)
			czy te� potraktowa� je jako zwyk�y tekst (\c true).
	\return �a�cuch z podstawionymi danymi zamiast znacznik�w
**/
QString parse(const QString &s, const UserListElement &ule, bool escape = true);

/**
	\fn void stringHeapSort(QStringList &c)
	\internal
	Funkcja sortuj�ca odpowiednio list� �a�cuch�w.
	\todo Jakie to dok�adnie sortowanie?
**/
void stringHeapSort(QStringList &c);

/**
	\fn QStringList toStringList(const QString &e1, const QString &e2=QString(), const QString &e3=QString(), const QString &e4=QString(), const QString &e5=QString())
	��czy dowoln� liczb� (nie wi�ksz� ni� 5) �a�cuch�w QString w jedn� list� QStringList.
	\param e1 pierwszy �a�cuch, wymagany
	\param e2 drugi �a�cuch opcjonalny
	\param e3 trzeci �a�cuch opcjonalny
	\param e4 czwarty �a�cuch opcjonalny
	\param e5 pi�ty �a�cuch opcjonalny
	\return obiekt reprezentuj�cy list� podanych �a�cuch�w
**/
QStringList toStringList(const QString &e1, const QString &e2=QString(), const QString &e3=QString(), const QString &e4=QString(), const QString &e5=QString());

/**
	\fn void saveGeometry(const QWidget *w, const QString &section, const QString &name)
	Zapisuje wymiary i po�o�enie wskazanego okna do danej sekcji konfiguracyjnej,
	do klucza o podanej nazwie.
	\param w wska�nik do obiektu, kt�rego wymiary i po�o�enie maj� zosta� zapisane
	\param section sekcja pliku konfiguracyjnego, w kt�rej ma si� znale�� wpis
	\param name nazwa klucza, w jakim maj� by� zapisane te dane
**/
void saveGeometry(const QWidget *w, const QString &section, const QString &name);

/**
	\fn void loadGeometry(QWidget *w, const QString &section, const QString &name, int defaultX, int defaultY, int defaultWidth, int defaultHeight)
	�aduje wymiary i po�o�enie z podanej sekcji i klucza i aplikuje je do podanego okna.
	\param w wska�nik do obiektu, do kt�rego odczytane wymiary i po�o�enie maj� zosta� zastosowane
	\param section sekcja pliku konfiguracyjnego, z kt�rego dane b�d� czytane
	\param name nazwa klucza, z kt�rego dane b�d� czytane
	\param defaultX domy�lna warto�� pozycji lewego g�rnego rogu okna w poziomie (gdy brakuje wpisu w pliku konfiguracyjnym)
	\param defaultY domy�lna warto�� pozycji lewego g�rnego rogu okna w pionie (gdy brakuje wpisu w pliku konfiguracyjnym)
	\param defaultWidth domy�lna warto�� szeroko�ci okna (gdy brakuje wpisu w pliku konfiguracyjnym)
	\param defaultHeight domy�lna warto�� wysoko�ci okna (gdy brakuje wpisu w pliku konfiguracyjnym)
**/
void loadGeometry(QWidget *w, const QString &section, const QString &name, int defaultX, int defaultY, int defaultWidth, int defaultHeight);

/**
	\fn QString toPlainText(const QString &text)
	Usuwa znaki nowego wiersza, oraz tagi HTMLa (wszystko co da si� dopasowa� jako wyra�enie regularne: \c <.*> ).
	\param text �a�cuch do przetworzenia
	\return �a�cuch z usuni�tymi odpowiednimi znakami.
**/
QString toPlainText(const QString &text);

/**
	\var extern QFont *defaultFont
	Domy�lna czcionka Kadu.
**/
extern QFont *defaultFont;

/**
	\var extern QFontInfo *defaultFontInfo
	Wska�nik do obiekt zawieraj�cego informacje dotycz�ce domy�lnej czcionki Kadu
**/
extern QFontInfo *defaultFontInfo;

/**
	\var extern QTextCodec *codec_cp1250
	Wska�nik do kodeka CP1250.
**/
extern QTextCodec *codec_cp1250;

/**
	\var extern QTextCodec *codec_latin2
	Wska�nik do kodeka Latin2.
**/
extern QTextCodec *codec_latin2;

/**
	Klasa reprezentuj�ca okno wybierania opis�w do status�w.
	\class ChooseDescription
	\brief Okno wyboru opisu
**/
class ChooseDescription : public QDialog
{
	Q_OBJECT
	public:
		/**
			\fn ChooseDescription(int nr, QWidget * parent=0, const char * name=0)
			Konstruktor tworz�cy okno, wczytuj�cy ostatnio ustawiane opisy,
			oraz ustawiaj�cy odpowiedni� dla statusu ikon� w tym oknie.
			\param nr numer statusu, gdzie:
				\arg \c 1 oznacza status <tt>Dost�pny z opisem</tt>
				\arg \c 3 oznacza status <tt>Zaj�ty z opisem</tt>
				\arg \c 5 oznacza status <tt>Ukryty z opisem</tt>
				\arg \c 7 oznacza status <tt>Niedost�pny z opisem</tt>
				\arg <tt>Ka�dy inny</tt> oznacza status <tt>Niedost�pny z opisem</tt>
			\param parent rodzic kontrolki (domy�lnie: 0)
			\param name nazwa kontrolki (domy�lnie: 0)
		**/
		ChooseDescription ( int nr, QWidget * parent=0, const char * name=0);

		/**
			\fn virtual ~ChooseDescription()
			Domy�lny destruktor. Ustawia opis (je�li klikni�to OK).
		**/
		virtual ~ChooseDescription();

		/**
			\fn void getDescription(QString &dest)
			Funkcja s�u��ca do odczytu tre�ci z pola edycji opisu.
			\param dest obiekt klasy QString, pod kt�rym zapisana zostanie tre��
			\warning funkcja nie zwraca �a�cucha, a zapisuje go do podanego parametru
		**/
		void getDescription(QString &);

	private:
		QComboBox *desc;
		QLabel *l_yetlen;

	private slots:
		void okbtnPressed();
		void cancelbtnPressed();
		void updateYetLen(const QString&);
};

/**
	Klasa bardzo pomocna, gdy chcemy, aby Kadu udawa�o przez chwil� przegl�dark� WWW.
	Pozwala na stosunkowo �atwy odczyt danych ze stron, ale tak�e bardziej zaawansowane
	operacje, jak logowanie na stronach metod� POST, czy te� poprzez ciasteczka.
	\class HttpClient
	\brief Klient stron WWW
**/
class HttpClient : public QObject
{
	Q_OBJECT

	private:
		QSocket Socket;
		QString Host;
		QString Referer;
		QString Path;
		QByteArray Data;
		QByteArray PostData;
		int StatusCode;
		bool HeaderParsed;

		unsigned int ContentLength;
		bool ContentLengthNotFound;

		QMap<QString, QString> Cookies;

	private slots:
		void onConnected();
		void onReadyRead();
		void onConnectionClosed();

	public slots:
		/**
			\fn void setHost(const QString &host)
			Ustawia adres serwera, z kt�rego b�dziemy czyta� stron�.
			\param host adres hosta w postaci �a�cucha
		**/
		void setHost(const QString &host);

		/**
			\fn void get(const QString &path)
			Wysy�a zapytanie GET do ustawionego wcze�niej serwera.
			\param path �cie�ka do dokumentu, kt�ry chcemy pobra� z serwera

			<b>Przyk�ad:</b>
			\code
			http_client->get("/galeria/index.html");
			\endcode
		**/
		void get(const QString &path);

		/**
			\fn void post(const QString &path, const QByteArray &data)
			Wysy�a do serwera zapytanie typu POST, dodaj�c do niego
			odpowiednie dane.
			\param path �cie�ka do dokumentu, kt�ry chcemy pobra� z serwera
			\param data parametry, kt�re s� przekazane do serwera - zwykle zawieraj�
					warto�ci zmiennych z form na uprzednio pobranej stronie.
		**/
		void post(const QString &path, const QByteArray &data);

		/**
			\overload void post(const QString &path, const QString &data)
		**/
		void post(const QString &path, const QString &data);

	public:
		/**
			\fn HttpClient()
			Standardowy konstruktor.
		**/
		HttpClient();

		/**
			\fn int status() const
			\return kod aktualnego statu klienta WWW, zgodny z protoko�em HTTP (302, 500, itd).
		**/
		int status() const;

		/**
			\fn const QByteArray &data() const
			Funkcja daj�ca dost�p do danych reprezentuj�cych kod pobranej strony.
			\return tre�� pobranej strony.
		**/
		const QByteArray &data() const;

		/**
			\fn static QString encode(const QString &text)
			Zamienia znaki nieakceptowalne w adresie strony na ich odpowiedniki w formie %XY.
			\param text �a�cuch reprezentuj�cy adres strony, zawieraj�cy spacje, itp
			\return sformatowany �a�cuch.
		**/
		static QString encode(const QString &text);

		/**
			\fn const QString &cookie(const QString &name) const
			Funkcja daj�ca dost�p do danych zawartych w tzw. ciasteczkach.
			\param name nazwa ciasteczka
			\return warto�� dla danego ciasteczka.
		**/
		const QString &cookie(const QString &name) const;

		/**
			\fn const QString &cookie(const QString &name) const
			Funkcja daj�ca dost�p do danych zawartych w tzw. ciasteczkach.
			\return obiekt klasy QMap zawieraj�cy sparowane nazwy ciasteczek z ich warto�ciami.
		**/
		const QMap<QString, QString> &cookies() const;

		/**
			\fn void setCookie(const QString &name, const QString &value)
			Ustawia warto�� danego ciasteczka.
			\param name nazwa ciasteczka
			\param value warto�� dla danego ciasteczka
		**/
		void setCookie(const QString &name, const QString &value);

	signals:
		/**
			\fn void finished()
			Sygna� emitowany, gdy zapytanie zostanie sfinalizowane.
		**/
		void finished();

		/**
			\fn void redirected(QString link)
			Sygna� emitowany, gdy klient HTTP zostanie przekierowany przez serwer hosta
			na inny adres.
			\param link adres strony na jaki klient zosta� przekierowany
		**/
		void redirected(QString link);

		/**
			\fn void error()
			Sygna� emitowany, gdy wyst�pi� b��d podczas pr�by sfninaliowania zapytania.
		**/
		void error();
};

/**
	Klasa reprezentuj�ca dokument HTML. Przechowuje
	list� element�w. Element mo�e by� tagiem HTML
	albo kawa�kiem tekstu.
	\class HtmlDocument
	\brief Dokument HTML
**/
class HtmlDocument
{
	private:
		/**
			Struktura reprezentuj�ca pojedynczy element dokumentu.
			\struct Element
			\brief Element dokumentu
		**/
		struct Element
		{
			QString text; /*!< tre�� elementu */
			bool tag; /*!< oznacza, czy element jest takiem HTML, czy zwyk�ym tekstem */
		};
		QValueList<Element> Elements;
		void addElement(Element e);

	public:
		/**
			\fn static void escapeText(QString& text)
			Zamienia znaki interpretowane przez przegl�darki HTML (tj. \c < , \c > , itd.)
			na ich odpowiedniki zakodowane w HTML (tj. \c &lt; , \c &gt; , itd.).
			\param text obiekt klasy QString zawieraj�cy znaki do podmiany
			\warning funkcja nie zwraca �a�cucha z podmienionymi znakami,
					a zapisuje go od od razu do podanego obiektu, zmieniaj�c jego zawarto��.
		**/
		static void escapeText(QString& text);

		/**
			\fn static void unescapeText(QString& text)
			Zamienia znaki zakodowane w HTML (tj. \c &lt; , \c &gt; , itd.)
			na ich odpowiedniki interpretowane przez przegl�darki HTML (tj. \c < , \c > , itd.).
			\param text obiekt klasy QString zawieraj�cy znaki do podmiany
			\warning funkcja nie zwraca �a�cucha z podmienionymi znakami,
					a zapisuje go od od razu do podanego obiektu, zmieniaj�c jego zawarto��.
		**/
		static void unescapeText(QString& text);

		/**
			\fn void addTag(const QString &text)
			Dodaje podany tekst do listy element�w jako
			tag HTML.
			\param text tekst do dodania
		**/
		void addTag(const QString &text);

		/**
			\fn void addText(const QString &text)
			Dodaje podany tekst do listy element�w jako
			zwyk�y tekst.
			\param text tekst do dodania
		**/
		void addText(const QString &text);

		/**
			\fn void insertTag(const int pos,const QString &text)
			Wstawia podany tekst do listy element�w jako
			tag HTML przed innym elementem.
			\param pos indeks elementu, przed kt�rym wstawiamy
			\param text tekst do dodania
		**/
		void insertTag(const int pos,const QString &text);

		/**
			\fn void insertText(const int pos,const QString &text)
			Wstawia podany tekst do listy element�w jako
			zwykly tekst przed innym elementem.
			\param pos indeks elementu, przed kt�rym wstawiamy
			\param text tekst do dodania
		**/
		void insertText(const int pos,const QString &text);

		/**
			\fn void parseHtml(const QString& html)
			Parsuje podany napis zgodny ze struktur� HTML
			i otrzymane dane przypisuje do dokumentu.
			@param html napis do skonwertowania
		**/
		void parseHtml(const QString& html);

		/**
			\fn QString generateHtml() const
			Na podstawie zawarto�ci dokumentu generuje
			napis zgodny ze struktur� HTML. Znaki specjalne
			wchodz�ce w sk�ad element�w nie b�d�cych tagami
			HTML s� escapowane.
			\return zawarto�� dokumentu w formacie HTML.
		**/
		QString generateHtml() const;

		/**
			\fn int countElements() const
			\return ilo�� element�w wchodz�cych w sk�ad
			dokumentu.
		**/
		int countElements() const;

		/**
			\fn bool isTagElement(int index) const
			Sprawdza czy element o podanym indeksie jest
			tagiem HTML czy zwyk�ym tekstem.
			\return \c true je�li jest tagiem HTML, lub \c false gdy jest zwyk�ym tekstem.
		**/
		bool isTagElement(int index) const;

		/**
			\fn const QString &elementText(int index) const
			\return tekst elementu o podanym indeksie.
		**/
		const QString &elementText(int index) const;

		/**
			\overload QString &elementText(int index)
		**/
		QString &elementText(int index);

		/**
			\fn void setElementValue(int index,const QString& text,bool tag=false)
			Ustawia tekst i typ elementu o podanym indeksie.
			\param index indeks pozycji elementu
			\param text tre�� elementu
			\param tag czy element ma by� interpretowany jako tag HTML (\c true ),
				czy jako zwyk�y tekst (\c false ). Domy�lnie: \c false.
		**/
		void setElementValue(int index,const QString& text,bool tag=false);

		/**
			\fn void splitElement(int& index,int start,int length)
			Wydziela z elementu podany fragment tekstu.
			Element mo�e pozosta� jeden albo
			ulec podzieleniu na dwa lub nawet trzy elementy.
			\param index indeks pozycji elementu kt�ry b�dziemy dzieli�
			\param start numer znaku, od kt�rego zaczyna� ma si� wydzielony element
			\param length liczba znak�w wchodz�cych w wydzielony element, poczynaj�c od pozycji \c start
			\note Funkcja zmienia warto�� argumentu index, aby
			indeks wydzielonego elementu by� aktualny.
		**/
		void splitElement(int& index,int start,int length);

		/**
			\fn void convertUrlsToHtml()
			Skanuje tekst dokumentu w poszukiwaniu adres�w WWW,
			np. "www.kadu.net" i zmienia je w linki HTML.
		**/
		void convertUrlsToHtml();
};

/*
	nie u�ywane - patrz opis w pliku .cpp

HtmlDocument GGMessageToHtmlDocument(const QString &msg, int formats_length, void *formats);
void HtmlDocumentToGGMessage(HtmlDocument &htmldoc, QString &msg,
	int &formats_length, void *&formats);
*/

/**
	Klasa reprezentuj�ca kontrolki obrazk�w.
	\class ImageWidget
	\brief Obrazki
**/
class ImageWidget : public QWidget
{
	Q_OBJECT

	private:
		QImage Image;

	protected:
		/**
			\fn virtual void paintEvent(QPaintEvent *e)
			Funkcja rysuj�ca t� kontrolk�.
			\param e wska�nik do obiektu reprezentuj�cego zdarzenie rysowania.
		**/
		virtual void paintEvent(QPaintEvent *e);

	public:
		/**
			\fn ImageWidget(QWidget *parent)
			Konstruktor tworz�cy kontrolk� w innej - podanej kontrolce.
			\param parent kontrolka, w kt�rej nale�y obsadzi�
		**/
		ImageWidget(QWidget *parent);

		/**
			\overload ImageWidget(QWidget *parent,const QByteArray &image)
			Konstruktor tworz�cy kontrolk� w innej - podanej kontrolce,
			oraz rysuj�cy od razu dany obrazek.
			\param parent kontrolka, w kt�rej nale�y obsadzi�
			\param image dane zawieraj�ce obrazek
		**/
		ImageWidget(QWidget *parent,const QByteArray &image);

		/**
			\fn void setImage(const QByteArray &image)
			Ustawia obrazek kontrolki na zadany.
			\param image dane zawieraj�ce obrazek.
		**/
		void setImage(const QByteArray &image);

		/**
			\overload void setImage(const QPixmap &image)
		**/
		void setImage(const QPixmap &image);
};

/**
	Klasa reprezentuj�ca okienko z obrazkiem i polem edycji.
	Jest ona u�ywana do wy�wietlania obrazka z bramki SMS Idei,
	aby u�ytkownik m�g� odczyta� z niego tre�� i wpisa� j� do pola edycji.
	\brief Odczytywanie tre�ci z obrazka WWW
	\class TokenDialog
**/
class TokenDialog : public QDialog
{
	Q_OBJECT

	public:
		/**
			\fn TokenDialog(QPixmap tokenImage, QDialog *parent = 0, const char *name = 0)
			Konstruktor tworz�cy okienko wraz z gotowym obrazkiem.
			\param tokenImage obrazek do wy�wietlenia
			\param parent rodzic kontrolki (domy�lnie: 0)
			\param name nazwa kontrolki (domy�lnie: 0)
		**/
		TokenDialog(QPixmap tokenImage, QDialog *parent = 0, const char *name = 0);

		/**
			\fn void getValue(QString &tokenValue)
			Pobiera tre�� pola edycji spod obrazka.
			\param tokenValue obiekt klasy QString, pod kt�rym zostanie zapisana pobrana tre��.
		**/
		void getValue(QString &tokenValue);

	private:
		QLineEdit *tokenedit;
};

/**
	Klasa zarz�dzaj�ca stylami wygl�du Kadu.
	\class Themes
	\brief Style Kadu
**/
class Themes : public QObject
{
	Q_OBJECT
	private:
		QStringList ThemesList;
		QStringList ThemesPaths;
		QStringList additional;
		QString ConfigName, Name, ActualTheme;
		QMap<QString, QString> entries;
		QStringList getSubDirs(const QString& path) const;
		QString fixFileName(const QString& path,const QString& fn) const;

	public:
		/**
			\fn Themes(const QString& name, const QString& configname, const char *name=0)
			Konstruktor tworz�cy grup� styli Kadu.
			\param name nazwa grupy styli (\c emoticons , \c sounds , \c icons )
			\param configname nazwa pliku konfiguracyjnego
			\param name nazwa obiektu (domy�lnie: 0)
		**/
		Themes(const QString& name, const QString& configname, const char *name=0);

		/**
			\fn QStringList defaultKaduPathsWithThemes() const
			\return domy�ln� list� �cie�ek, w kt�rych Kadu trzyma grupy styli.
		**/
		QStringList defaultKaduPathsWithThemes() const;

		/**
			\fn const QStringList &themes() const
			\return list� styli Kadu.
		**/
		const QStringList &themes() const;

		/**
			\fn const QStringList &themes() const
			\return bierz�cy styl.
		**/
		const QString &theme() const;

		/**
			\fn const QStringList &paths() const
			\return list� �cie�ek, w kt�rych Kadu trzyma grupy styli.
		**/
		const QStringList &paths() const;

		/**
			\fn const QStringList &additionalPaths() const
			\return list� dodatkowych �cie�ek, w kt�rych Kadu trzyma grupy styli.
		**/
		const QStringList &additionalPaths() const;

		/**
			\fn QString themePath(const QString& theme="") const
			Pobiera �cie�k� do danego stylu.
			\param theme nazwa stylu (domy�lnie: "", co oznacza styl domy�lny)
			\return �cie�k� do katalogu podanego stylu
		**/
		QString themePath(const QString& theme="") const;

		/**
			QString getThemeEntry(const QString& name) const
			Pobiera warto�� danego wpisu.
			\param name nazwa klucza, dla kt�rego nale�y pobra� wpis
			\return wpis z danego stylu, o podanym kluczu
			\todo opisa� dok�adniej co to za wpisy.
		**/
		QString getThemeEntry(const QString& name) const;

	public slots:
		/**
			\fn void setTheme(const QString& theme)
			Ustawia aktualny styl.
			\param theme styl, kt�ry ma by� ustawiony
		**/
		void setTheme(const QString& theme);

		/**
			\fn void setPaths(const QStringList& paths)
			Ustawia �cie�ki dla styli.
			\param paths lista �cie�ek
		**/
		void setPaths(const QStringList& paths);

	signals:
		/**
			\fn void themeChanged(const QString& theme)
			Sygna� emitowany, gdy zmieniony zosta� styl.
			\param theme nazwa nowego stylu
		**/
		void themeChanged(const QString& theme);

		/**
			\fn void pathsChanged(const QStringList& list)
			Sygna� emitowany, gdy �cie�ki do styli zosta�y zmienione.
			\param list lista nowych �cie�ek
		**/
		void pathsChanged(const QStringList& list);
};

/**
	Klasa zarz�dzaj�ca zestawami ikon, oraz samymi ikonami.
	Istnieje globalna zmienna - icons_manager_ptr - tak wi�c tworzenie
	nowych instancji tego obiektu nie jest potrzebne.
	\class IconsManager
	\brief Menad�er ikon
**/
class IconsManager :public Themes
{
	Q_OBJECT
	public:
		/**
			\fn IconsManager(const QString& name, const QString& configname)
			Konstruktor tworz�cy grup� styli ikon.
			\param name nazwa grupy
			\param configname nazwa pliku konfiguracyjnego
		**/
		IconsManager(const QString& name, const QString& configname);

		/**
			\fn QString iconPath(const QString &name) const
			T�umaczy nazw� ikony z zestawu na �cie�k� do niej i na odwr�t.
			\param name nazwa ikony z zestawu lub scie�ka do pliku
					(je�li zawiera znak '/' to jest interpretowana jako
					scie�ka).
			\return pe�n� �cie�k� do ikony z aktualnego zestawu,
					lub bezposrednio nazw� je�li jest to pe�na �cie�ka.
		**/
		QString iconPath(const QString &name) const;

		/**
			\fn const QPixmap & loadIcon(const QString &name)
			�aduje ikon� z aktualnego zestawu lub z podanego pliku.
			\param name nazwa ikony z zestawu lub �cie�ka do pliku
					(je�li zawiera znak '/' to jest interpretowana jako
					�cie�ka).
			\return obiekt klasy QPixmap reprezentuj�cy dan� ikon�.
		**/
		const QPixmap & loadIcon(const QString &name);

		/**
			\fn static void initModule()
			Inicjalizuje modu� menad�era ikon.
		**/
		static void initModule();

		/**
			\fn void registerMenu(QMenuData *menu)
			Rejestruje nowe menu.
			\param menu wska�nik do obiektu danych dla menu
			\todo opisa� dok�adniej co to za menu.
		**/
		void registerMenu(QMenuData *menu);

		/**
			\fn void registerMenu(QMenuData *menu)
			Wyrejestruje menu.
			\param menu wska�nik do obiektu danych menu
			\todo opisa� dok�adniej co to za menu.
		**/
		void unregisterMenu(QMenuData *menu);

		/**
			\fn void registerMenuItem(QMenuData *menu, const QString &caption, const QString &iconName)
			Rejestruje nowy element w menu.
			\param menu wska�nik do obiektu danych menu
			\param caption tytu� nowego elementu
			\param iconName nazwa ikony dla nowego elementu
		**/
		void registerMenuItem(QMenuData *menu, const QString &caption, const QString &iconName);

		/**
			\fn void unregisterMenuItem(QMenuData *menu, const QString &caption)
			Wyrejestrowuje element z menu.
			\param menu wska�nik do obiektu danych menu
			\param caption tytu� elementu
		**/
		void unregisterMenuItem(QMenuData *menu, const QString &caption);

	public slots:
		/**
			\fn void clear()
			Czy�ci zestaw ikon.
		**/
		void clear();

		/**
			\fn void refreshMenus()
			Od�wierza menu.
		**/
		void refreshMenus();

	private:
		QMap<QString, QPixmap> icons;
		
		QValueList<QPair<QMenuData *, QValueList<QPair<QString, QString> > > > menus;

	private slots:
		void selectedPaths(const QStringList& paths);
		void onCreateConfigDialog();
		void onApplyConfigDialog();
};

/**
	\var extern IconsManager *icons_manager_ptr
	Wska�nik do obiektu generalnego zarz�dcy ikonek Kadu.
**/
//TODO: po wydaniu 0.4 trzeba zmieni� nazw� na icons_manager i wywali� define'a
extern IconsManager *icons_manager_ptr;
/**
	\def icons_manager
	Tymczasowy skr�t bezpo�rednio do obiektu generalnego zarz�dcy ikonek Kadu,
	zamiast wska�nika icons_manager_ptr.
**/
#define icons_manager (*icons_manager_ptr)


/**
	Klasa rozwiazujaca problem z powiadomieniem
	o utworzeniu nowej instancji danej klasy.
	Umieszczamy w klasie publiczna statyczna
	zmienna createNotifier klasy CreateNotifier,
	do ktorej mog� si� pod��cza� pozosta�e cze�ci kodu.
	Przed wyj�ciem z konstruktora wywo�ujemy metod�:
	\code createNotifier.notify(this); \endcode
	\class CreateNotifier
	\brief Powiadamianie o nowych instancjach klas
**/
class CreateNotifier : public QObject
{
	Q_OBJECT

	public:
		/**
			\fn void notify(QObject* new_object)
			Emituje sygna� do wszystkich obiekt�w pod��czonych do tego obiektu
			informuj�cy o tym, �e dany obiekt zosta� utworzony. Funkcja ta powinna
			by� wywo�ana przez nowotworzony obiekt, w jego konstruktorze.
			\param new_object wska�nik do obiekty, kt�ry zosta� utworzony (zwykle podaje si� \c this )
		**/
		void notify(QObject* new_object);

	signals:
		/**
			\fn void objectCreated(QObject* new_object)
			Sygna� emitowany, gdy jaki� obiekt wywo�a metod� notify() (zwykle z parametrem \c this ).
			\param new_object wska�nik do nowoutworzonego obiektu
		**/
		void objectCreated(QObject* new_object);
};

/**
	Klasa zarz�dzaj�ca obrazkami przesy�anymi przez protok� GG.
	\class GaduImagesManager
	\brief Zarz�dca obrazk�w protoko�u GG
**/
class GaduImagesManager
{
	private:
		struct ImageToSend
		{
			uint32_t size;
			uint32_t crc32;
			QString file_name;
			QDateTime lastSent;
			char *data;
		};
		QMap<QPair<uint32_t, uint32_t>, ImageToSend> ImagesToSend;//size,crc32 -> 

		struct SavedImage
		{
			uint32_t size;
			uint32_t crc32;
			QString file_name;
		};
		QValueList<SavedImage> SavedImages;

	public:
		/**
			\fn static void setBackgroundsForAnimatedImages(HtmlDocument &doc, const QColor &col)
			Ustawia kolor t�a dla animowanych obrazk�w w danym dokumencie.
			\param doc dokument, w kt�rym kolory zostan� ustawione
			\param col kolor t�a
		**/
		static void setBackgroundsForAnimatedImages(HtmlDocument &doc, const QColor &col);

		/**
			\fn static QString loadingImageHtml(UinType uin, uint32_t size, uint32_t crc32)
			Generuje odpowiedni kod HTML, kt�ry nale�y wstawi� do okna rozmowy, aby pokaza�,
			�e obrazek jest w trakcie �adowania (przesy�ania), oraz ustawia
			odpowiednie parametry znacznika &lt;img&gt; tak, aby mo�na by�o potem �atwo
			zidentyfikowa�, jaki obrazek mia� by� wstawiony w to miejsce.
			W praktyce daje nam znan� klepsydr�.
			\param uin UIN kontaktu wysy�aj�cego obrazek
			\param size rozmiar obrazka
			\param crc32 suma kontrolna obrazka
			\return �a�cuch stanowi�cy kod HTML
		**/
		static QString loadingImageHtml(UinType uin, uint32_t size, uint32_t crc32);

		/**
			\fn static QString imageHtml(const QString& file_name)
			Generuje kod HTML, kt�ry nale�y wstawi� do okna rozmowy,
			aby pokaza� w nim obrazek.
			\param file_name nazwa pliku obrazka
			\return �a�cuch stanowi�cy kod HTML
		**/
		static QString imageHtml(const QString& file_name);

		/**
			\fn void addImageToSend(const QString& file_name, uint32_t& size, uint32_t& crc32)
			Dodaje obrazek do listy obrazk�w, kt�re maj� by� wys�ane z kolejn� wiadomo�ci�.
			\param file_name nazwa pliku obrazka
			\param size rozmiar obrazka
			\param crc32 suma kontrolna obrazka
		**/
		void addImageToSend(const QString& file_name, uint32_t& size, uint32_t& crc32);

		/**
			\fn void sendImage(UinType uin, uint32_t size, uint32_t crc32)
			Wysy�a jeden z obrazk�w znajduj�cych si� na uprzednio przygotowanej li�cie.
			\param uin numer UIN do kt�re ma by� wys�any obrazek
			\param size rozmiar obrazka, kt�ry chcemy wys�a�
			\param crc32 suma kontrolna obrazka, kt�ry chcemy wys�a�
		**/
		void sendImage(UinType uin, uint32_t size, uint32_t crc32);

		/**
			\fn QString getImageToSendFileName(uint32_t size, uint32_t crc32)
			Szuka zakolejkowanego obrazka.
			\return nazw� pliku znalezionego obrazka. Zwraca ci�g pusty,
			je�li obrazek nie zosta� w tej sesji zakolejkowany do wys�ania.
		**/
		QString getImageToSendFileName(uint32_t size, uint32_t crc32);

		/**
			\fn QString saveImage(UinType sender, uint32_t size, uint32_t crc32, const QString& filename, const char* data)
			Zapisuje obrazek w katalogu .gg/images.
			\return pe�n� �cie�k� do zapisanego obrazka.
		**/
		QString saveImage(UinType sender, uint32_t size, uint32_t crc32, const QString& filename, const char* data);

		/**
			\fn QString getSavedImageFileName(uint32_t size, uint32_t crc32)
			Szuka zapisanego obrazka.
			\return zwraca jego nazw� pliku wraz ze �cie�k�. Zwraca ci�g pusty,
			je�li obrazek nie zosta� w tej sesji zapisany.
		**/
		QString getSavedImageFileName(uint32_t size, uint32_t crc32);

		/**
			\fn QString replaceLoadingImages(const QString& text, UinType sender, uint32_t size, uint32_t crc32)
			Podmienia obrazki klepsydr na w�a�ciwy obrazek, o ile zosta� on ju� w ca�o�ci pobrany.
			Funkcj� t� nale�y wywo�a� dla ka�dego niepowtarzalnego obrazka w tre�ci dokumentu.
			\param text tre�� dokumentu ze znacznikami HTML oznaczaj�cymi klepsydry
			\param sender UIN kontaktu kt�ry przysy�a obrazki
			\param size rozmiar obrazka kt�ry chcemy wstawi�
			\param crc32 suma kontrolna obrazka kt�ry chcemy wstawi�
			\return tre�� dokumentu z podmienionymi znacznikami HTML
		**/
		QString replaceLoadingImages(const QString& text, UinType sender, uint32_t size, uint32_t crc32);
};

/**
	\var extern GaduImagesManager gadu_images_manager
	Obiekt zarz�dzaj�cy obrazkami przesy�anymi przez protok� Gadu-Gadu.
**/
extern GaduImagesManager gadu_images_manager;

/**
	Klasa s�u�aca do pogl�du obrazk�w podczas wyboru pliku obrazka.
	\class PixmapPreview
	\brief Podgl�d obrazk�w
**/
class PixmapPreview : public QLabel, public QFilePreview
{
	public:
		/**
			\fn PixmapPreview()
			Domy�lny konstruktor.
		**/
		PixmapPreview();

		/**
			\fn void previewUrl(const QUrl& url)
			Ustawia podgl�d dla danego pliku.
			\param url adres (�cie�ka lokalna lub plik w sieci) pliku obrazka
		**/
		void previewUrl(const QUrl& url);
};

/**
	Klasa reprezentuj�ca okno wyboru obrazka do wstawiania.
	\class ImageDialog
	\brief Okno wyboru obrazka
**/
class ImageDialog : public QFileDialog
{
	public:
		/**
			\fn ImageDialog(QWidget* parent)
			Konstruktor tworz�cy okno danym rodzicem.
			\param parent rodzic kontrolki
		**/
		ImageDialog(QWidget* parent);
};

/**
	Zmodyfikowany QTextBrowser specjalnie na potrzeby Kadu.
	Klikni�cie na linku otwiera ustawion� w konfiguracji przegl�dark�.
	W menu kontekstowym jest dodatkowa opcja "Kopiuj lokacj� odno�nika".
	Dodatkowo poprawka b��du w Qt.
	\class KaduTextBrowser
	\brief Wy�wietlanie wiadomo�ci
**/
class KaduTextBrowser : public QTextBrowser, QToolTip
{
	Q_OBJECT

	private:
		QString anchor;
		int level;
		/**
			this value stores the current highlighted link
			for use with maybeTip(), or is null
		**/
		QString highlightedlink; 	

	private slots:
		void copyLinkLocation();
		void hyperlinkClicked(const QString& link) const;
		void linkHighlighted(const QString &);
		
	protected:
		/**
			\fn QPopupMenu *createPopupMenu(const QPoint &point)
			Wy�wietla menu kontekstowe.
			\param point wsp�rz�dne punktu, w kt�rym ma znale�� si� lewy g�rny r�g menu kontekstowego
			\return wska�nik do obiektu menu
		**/
		QPopupMenu *createPopupMenu(const QPoint &point);

		/**
			\fn virtual void drawContents(QPainter * p, int clipx, int clipy, int clipw, int cliph)
			Rysuje zawarto�� kontrolki (wiadomo�ci, separatory, itp).
			\param p wska�nik do obiektu rysuj�cego
			\param clipx wsp�rz�dne poziome lewego g�rnego rogu prostok�ta kt�ry zostanie narysowany
			\param clipy wsp�rz�dne pionowe lewego g�rnego rogu prostok�ta kt�ry zostanie narysowany
			\param clipw szeroko�� prostok�ta kt�ry zostanie narysowany
			\param cliph wysoko�� prostok�ta kt�ry zostanie narysowany
		**/
		virtual void drawContents(QPainter * p, int clipx, int clipy, int clipw, int cliph);

		/**
			\fn virtual void maybeTip(const QPoint& point)
			Funkcja wy�wietlaj�ca podpowied�.
			\param point wsp�rz�dne okre�laj�ce lewy g�rny r�g podpowiedzi
		**/
		virtual void maybeTip(const QPoint&);

		/**
			\fn virtual void contentsMouseReleaseEvent(QMouseEvent * e)
			Emituje sygna� puszczenia przycisku myszy.
			\param e wska�nik do obiektu reprezentuj�cego zdarzenie myszy
		**/
		virtual void contentsMouseReleaseEvent(QMouseEvent * e);

	public:
		/**
			\fn KaduTextBrowser(QWidget *parent = 0, const char *name = 0)
			Konstruktor standardowy.
			\param parent rodzic kontrolki (domy�lnie: 0)
			\param name nazwa kontrolki (domy�lnie: 0)
		**/
		KaduTextBrowser(QWidget *parent = 0, const char *name = 0);

		/**
			\fn void setSource(const QString &name)
			Ustawia �r�d�o danych.
			\param name nazwa �r�d�a danych
			\note Tak na prawd� funkcja ta na obecn� chwil� nie robi nic.
		**/
		void setSource(const QString &name);

		/**
			\fn void clear()
			Czy�ci zawarto��.
		**/
		void clear() 
		{ 
			setText("");
			//dla QT<3.2 clear() powodowa�o b��d 
		}

		/**
			\fn void setFont(const QFont& f)
			Ustawia czcionk�.
			\param f obiekt czcionki do ustawienia
		**/
		void setFont(const QFont& f) 	{ QTextBrowser::setFont(f); }

		/**
			\fn void setMargin(int width)
			Ustawia szeroko�� marginesu.
			\param width liczba pikseli
		**/
		void setMargin(int width);

	public slots:
		/**
			\fn virtual void copy()
			Kopiuje zaznaczon� cz�� do schowka.
		**/
		virtual void copy();

	signals:
		/**
			\fn void mouseReleased(QMouseEvent *e, KaduTextBrowser *sender)
			Dowolny przycisk myszy zosta� zwolniony.
			\param e wska�nik do obiektu reprezentuj�cego zdarzenie myszy
			\param sender wska�nik do obiektu, kt�ry wyemitowa� ten sygna�, czyli \c this
		**/
		void mouseReleased(QMouseEvent *e, KaduTextBrowser *sender);
};

/**
	\fn QValueList<int> toIntList(const QValueList<QVariant> &in)
	Konwertuje ca�� list� wariant�w do listy liczb ca�kowitych za jednym zamachem.
**/
QValueList<int> toIntList(const QValueList<QVariant> &in);

/**
	\fn QValueList<int> toIntList(const QValueList<QVariant> &in)
	Konwertuje ca�� list� liczb ca�kowitych do listy wariant�w za jednym zamachem.
**/
QValueList<QVariant> toVariantList(const QValueList<int> &in);

/**
	\fn template<class T, class X> QValueList<T> keys(const QMap<T, X> &m)
	Funkcja ta jest reimplementacj� funkcji z Qt - QMap::keys(), w celu
	omini�cia b��du krytycznego w Qt.
**/
template<class T, class X> QValueList<T> keys(const QMap<T, X> &m)
{
#if QT_VERSION < 0x030005
    QValueList<T> ret;
    for(QMap<T,X>::const_iterator it = m.begin(), map_end = m.end(); it != map_end; ++it)
        ret.append(it.key());
    return ret;
#else
    return m.keys();
#endif
}

/**
	\fn template<class T, class X> QValueList<X> values(const QMap<T, X> &m)
	Funkcja ta jest reimplementacj� funkcji z Qt - QMap::values(), w celu
	omini�cia b��du krytycznego w Qt.
**/
template<class T, class X> QValueList<X> values(const QMap<T, X> &m)
{
#if QT_VERSION < 0x030005
    QValueList<X> ret;
    for(QMap<T,X>::const_iterator it = m.begin(), map_end = m.end(); it != map_end; ++it)
        ret.append(it.data());
    return ret;
#else
    return m.values();
#endif
}

/**
	\fn QString narg(const QString &s, const QString &arg1, const QString &arg2, const QString &arg3=QString(), const QString &arg4=QString())
	Zast�pstwo dla arga z QString, kt�re podmienia kolejne %[1-4] w miejscu.

	W QStringu efektem:
	\code QString("%1 odst�p %2").arg("pierwszy %1 tekst").arg("drugi tekst") \endcode
	jest <tt>"pierwszy drugi tekst tekst odst�p %2"</tt>
	a chcieliby�my �eby by�o
	<tt>"pierwszy %1 tekst odst�p drugi tekst"</tt>
	co robi w�a�nie ta funkcja.
**/
QString narg(const QString &s, const QString &arg1, const QString &arg2,
				const QString &arg3=QString(), const QString &arg4=QString());

/**
	\overload QString narg(const QString &s, const QString &arg1, const QString &arg2, const QString &arg3, const QString &arg4, const QString &arg5, const QString &arg6=QString(), const QString &arg7=QString(),const QString &arg8=QString(), const QString &arg9=QString())
**/
QString narg(const QString &s, const QString &arg1, const QString &arg2,
				const QString &arg3, const QString &arg4,
				const QString &arg5, const QString &arg6=QString(),
				const QString &arg7=QString(),const QString &arg8=QString(),
				const QString &arg9=QString());

/**
	\overload QString narg(const QString &s, const QString **tab, int count)
	Uog�lniony narg() na wi�ksz� liczb� parametr�w.
	\param count musi by� mniejszy lub r�wny 9
	\param tab tablica \c count wska�nik�w do QString
**/
QString narg(const QString &s, const QString **tab, int count);

/**
	\fn void printBacktrace(const QString &header="")
	Wypisuje na wyj�ciu standardowym tzw. <i>backtrace</i>.
	\param header opcjonalna nazwa nag��wka
**/
void printBacktrace(const QString &header="");

/**
	\def VAR(v,x)
	Definiuje now� zmienn� v o takim samym typie jak x jednocze�nie inicjuj�c j� warto�ci� x.
**/
#define VAR(v,x)	__typeof(x) v=x

/**
	\def FOREACH(i,c)
	Szybsza w zapisaniu p�tla \c for .
**/
#define FOREACH(i,c) for(VAR(i, (c).begin()); i!=(c).end(); ++i)

/**
	\def CONST_FOREACH(i,c)
	Szybsza w zapisaniu p�tla \c for , operuj�ca na iteratorach sta�ych lub dynamicznych, w zale�no�ci od wersji Qt.
**/
#if QT_VERSION >= 0x030200
#define CONST_FOREACH(i,c) for(VAR(i, (c).constBegin()), _kadu__##i##__end = (c).constEnd(); i!=_kadu__##i##__end; ++i)
#else
#define CONST_FOREACH(i,c) for(VAR(i, (c).begin()), _kadu__##i##__end = (c).end(); i!=_kadu__##i##__end; ++i)
#endif

#endif
