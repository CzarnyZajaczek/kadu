#ifndef MISC_H
#define MISC_H

#include <qvaluelist.h>
#include <qdialog.h>
#include <qcombobox.h>
#include <qstring.h>
#include <qpixmap.h>
#include <qiconset.h>
#include <qlabel.h>
#include <qcstring.h>
#include <qdatetime.h>
#include <qlineedit.h>
#include <qimage.h>
#include <qstringlist.h>
#include <qsocket.h>

#include "libgadu.h"
#include "userlist.h"
#include "config_file.h"

/*
	Zmienia �cie�k� relatywn� do katalogu z ustawieniami gg
	na �cie�k� bezwzgledn� uwzgl�dniaj�c zmienne �rodowiskowe
	$HOME i $CONFIG_DIR
*/
QString ggPath(const QString &subpath);

QString dataPath(const QString &);

QString cp2unicode(const unsigned char *);
QCString unicode2cp(const QString &);
QString latin2unicode(const unsigned char *);
QCString unicode2latin(const QString &);
QString unicode2std(const QString &);
QString printDateTime(const QDateTime &datetime);
QString timestamp(time_t = 0);
QDateTime currentDateTime();
QString pwHash(const QString &tekst);
QString translateLanguage(const QApplication *application, const QString &locale, const bool l2n);
void openWebBrowser(const QString &link);
void escapeSpecialCharacters(QString &msg);
QString formatGGMessage(const QString &msg, int formats_length, void *formats);
QString unformatGGMessage(const QString &msg, int &formats_length, void *&formats);
QString parse(const QString &s, const UserListElement &ule, bool escape = true);
void stringHeapSort(QStringList &c);

class UinsList : public QValueList<uin_t>
{
	public:
		UinsList();
		bool equals(const UinsList &uins) const;
		void sort();
};

class ChooseDescription : public QDialog {
	Q_OBJECT
	public:
		ChooseDescription ( int nr, QWidget * parent=0, const char * name=0);

	private:
		QComboBox *desc;
		QLabel *l_yetlen;

	private slots:
		void okbtnPressed();
		void cancelbtnPressed();
		void updateYetLen(const QString&);

};

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
		int Status;
		bool HeaderParsed;
		int ContentLength;
		QMap<QString,QString> Cookies;
		
	private slots:
		void onConnected();
		void onReadyRead();
		void onConnectionClosed();
		
	public:
		HttpClient();
		void setHost(QString host);
		void get(QString path);
		void post(QString path,const QByteArray& data);
		void post(QString path,const QString& data);
		int status();
		const QByteArray& data();
		QString encode(const QString& text);
		
	signals:
		void finished();
		void redirected(QString link);
		void error();
};

/**
	Klasa reprezentuj�ca dokument html. Przechowuje
	list� element�w. Element mo�e by� tagiem html
	albo kawa�kiem tekstu.
**/
class HtmlDocument
{
	private:
		struct Element
		{
			QString text;
			bool tag;
		};
		QValueList<Element> Elements;
		void escapeText(QString& text);
		void unescapeText(QString& text);
		void addElement(Element e);

	public:
		/**
			Dodaje podany tekst do listy element�w jako
			tag html.
			@param text tekst do dodania
		**/
		void addTag(const QString &text);
		/**
			Dodaje podany tekst do listy element�w jako
			zwyk�y tekst.
			@param text tekst do dodania
		**/
		void addText(const QString &text);
		/**
			Wstawia podany tekst do listy element�w jako
			tag html przed innym elementem.
			@param pos indeks elementu przed ktorym wstawiamy
			@param text tekst do dodania
		**/		
		void insertTag(const int pos,const QString &text);
		/**
			Wstawia podany tekst do listy element�w jako
			zwykly tekst przed innym elementem.
			@param pos indeks elementu przed ktorym wstawiamy
			@param text tekst do dodania
		**/				
		void insertText(const int pos,const QString &text);
		/**
			Parsuje podany napis zgodny ze struktur� html
			i otrzymane dane przypisuje do dokumentu.
			@param html napis do skonwertowania
		**/
		void parseHtml(const QString& html);
		/**
			Na podstawie zawarto�ci dokumentu generuje
			napis zgodny ze struktur� html. Znaki specjalne
			wchodz�ce w sk�ad element�w nie b�d�cych tagami
			html s� escapowane.
		**/
		QString generateHtml();
		/**
			Zwraca ilo�� element�w wchodz�cych w sk�ad
			dokumentu.
		**/
		int countElements();
		/**
			Sprawdza czy element o podanym indeksie jest
			tagiem html czy zwyk�ym tekstem.
		**/
		bool isTagElement(int index);
		/**
			Zwraca tekst elementu o podanym indeksie.
		**/
		QString elementText(int index);
		/**
			Ustawia tekst i typ elementu o podanym indeksie.
		**/			
		void setElementValue(int index,const QString& text,bool tag=false);
		/**
			Wydziela z elementu podany fragment tekstu.
			Element mo�e pozosta� jeden albo
			ulec podzieleniu na dwa lub nawet trzy elementy.
			Funkcja zmienia warto�� argumentu index, aby
			indeks wydzielonego elementu by� aktualny.
		**/
		void splitElement(int& index,int start,int length);
};

HtmlDocument GGMessageToHtmlDocument(const QString &msg, int formats_length, void *formats);
void HtmlDocumentToGGMessage(HtmlDocument &htmldoc, QString &msg,
	int &formats_length, void *&formats);

class ImageWidget : public QWidget
{
	Q_OBJECT

	private:
		QImage Image;

	protected:
		virtual void paintEvent(QPaintEvent *e);

	public:
		ImageWidget(QWidget *parent);
		ImageWidget(QWidget *parent,const QByteArray &image);
		void setImage(const QByteArray &image);
};

class token : public QObject {
	Q_OBJECT

	public:
		token();
		~token();
		void getToken();

	private:
		struct gg_http *h;
		QSocketNotifier *snr;
		QSocketNotifier *snw;

		void deleteSocketNotifiers();
		void createSocketNotifiers();

	private slots:
		void socketEvent();
		void dataReceived();
		void dataSent();

	signals:
		void gotToken(struct gg_http *h);
		void tokenError();
};

class TokenDialog : public QDialog {
	Q_OBJECT

	public:
		TokenDialog(QDialog *parent = 0, const char *name = 0);
		void getToken(QString &Tokenid, QString &Tokenval);

	private slots:
		void gotTokenReceived(struct gg_http *h);
		void tokenErrorReceived();

	private:
		token Token;
		ImageWidget *tokenimage;
		QLineEdit *tokenedit;
		QString tokenid;
};

class Themes : public QObject
{
	Q_OBJECT
	private:
		QStringList ThemesList;
		QStringList ThemesPaths;
		QString ConfigName, Name, ActualTheme;
		QMap<QString, QString> entries;
		QStringList getSubDirs(const QString& path);
		QString fixFileName(const QString& path,const QString& fn);

	public:
		Themes(const QString& name, const QString& configname);
		QStringList defaultKaduPathsWithThemes();
		const QStringList themes();
		void setTheme(const QString& theme);
		QString theme();
		void setPaths(const QStringList& paths);
		QStringList paths();
		QString themePath(const QString& theme="");
		QString getThemeEntry(const QString& name);

	signals:
		void themeChanged(const QString& theme);
		void pathsChanged(const QStringList& list);
};


struct iconhandle {
	QString name;
	QIconSet picture;
};

class IconsManager :public Themes 
{
	Q_OBJECT
	public:
		IconsManager(const QString& name, const QString& configname);
		/**
			Laduje ikone z aktualnego zestawu lub z podanego pliku.
			@param name nazwa ikony z zestawu lub sciezka do pliku
			(jesli zawiera znak '/' to jest interpretowana jako
			sciezka).
		**/
		QPixmap loadIcon(const QString &name);
		static void initModule();

	private:
		QValueList<iconhandle> icons;
	private slots:
		void changed(const QString& theme);
		void chooseIconTheme(const QString& string);
		void selectedPaths(const QStringList& paths);
		void onCreateConfigDialog();
		void onDestroyConfigDialog();
};

extern IconsManager icons_manager;


/**
	klasa rozwiazujaca problem z powiadomieniem
	o utworzeniu nowej instancji danej klasy.
	umieszczamy w klasie publiczna statyczna
	zmienna createNotifier klasy CreateNotifier
	do ktorej moga sie podlaczac pozostale czesci kodu.
	przed wyjsciem z konstruktora wywolujemy metode:
	createNotifier.notify(this);
**/
class CreateNotifier : public QObject
{
	Q_OBJECT
	
	public:
		void notify(QObject* new_object);
		
	signals:
		void objectCreated(QObject* new_object);
};

#endif
