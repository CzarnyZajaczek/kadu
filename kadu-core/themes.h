#ifndef KADU_THEMES_H
#define KADU_THEMES_H

#include <qmap.h>
#include <qobject.h>
#include <qregexp.h>
#include <qstring.h>
#include <qstringlist.h>

/**
	Klasa obs�uguj�ca r�ne zestawy (jak ikony i emotikony)
	\class Themes
	\brief Klasa zarz�dzaj�ca zestawami
**/

class Themes : public QObject
{
	Q_OBJECT

	friend class EmoticonsManager;

	QStringList ThemesList; //!< lista z nazwami dost�pnych zestaw�w
	QStringList ThemesPaths; //!< lista �cie�ek w kt�rych szukamy zestaw�w
	QStringList additional;
	QString ConfigName; //!< nazwa pliku z konfiguracj� zestawu
	QString Name; //!< typ zestawu (np. "icons", "emoticons")
	QString ActualTheme; //!< nazwa aktualnie wybranego zestawu
	QMap<QString, QString> entries;

	/**
		\fn QStringList getSubDirs(const QString& path, bool validate = true) const
		Wyszukuje podkatalogi w podanej �cie�ce i zwraca list� tych zawieraj�cych
		pliki konfiguracyjne zestawu. Je�li parametr validate ma warto�� false,
		zwr�cona zostanie lista wszystkich znalezionych podkatalog�w.
		\param path �cie�ka do katalogu
		\param validate sprawdzenie zawarto�ci
		\return lista podkatalog�w
	**/
	QStringList getSubDirs(const QString& path, bool validate = true) const;

	/**
		\fn bool validateDir(const QString& path) const
		Sprawdza czy w podanym katalogu, b�d� jego podkatalogach znajduj�
		si� pliki z konfiguracj� zestawu.
		\param path �cie�ka do katalogu
	**/
	bool validateDir(const QString& path) const;

	QString fixFileName(const QString& path, const QString& fn) const;

	public:
		/**
			\fn Themes(const QString& name, const QString& configname, const char *cname = 0)
			Konstruktor tworz�cy nowy zestaw
			\param name typ zestawu
			\param configname plik z konfiguracj� zestawu
			\param cname nazwa obiektu
		**/
		Themes(const QString& name, const QString& configname, const char *cname = 0);

		/**
			\fn QStringList defaultKaduPathsWithThemes() const
			Tworzy list� �cie�ek do zestaw�w znajduj�cych si� w lokalizacjach
			domy�lnych dla typu zestawu.
			\return lista �cie�ek do zestaw�w
		**/
		QStringList defaultKaduPathsWithThemes() const;

		/**
			\fn const QStringList &themes() const
			Funkcja zwraca list� z nazwami dost�pnych zestaw�w
		**/
		const QStringList &themes() const;

		/**
			\fn const QString &theme() const
			Funkcja zwraca nazw� aktualnie wybranego zestawu
		**/
		const QString &theme() const;

		/**
			\fn const QStringList &paths() const
			Funkcja zwraca list� �cie�ek, w kt�rych wyszukiwane s� zestawy
		**/
		const QStringList &paths() const;

		const QStringList &additionalPaths() const;

		/**
			\fn const QStringList &paths() const
			Funkcja zwraca �cie�k� do wskazanego zestawu
		**/
		QString themePath(const QString& theme = QString::null) const;

		QString getThemeEntry(const QString& name) const;
		const QMap<QString, QString> & getEntries() { return entries; }

	public slots:
		/**
			\fn void setTheme(const QString& theme)
			Zmienia aktualny zestaw na wskazany o ile ten istnieje
			\param theme nazwa zestawu
		**/
		void setTheme(const QString& theme);

		/**
			\fn void setPaths(const QStringList& paths)
			Ustawia list� �cie�ek, w kt�rych wyszukiwane s� zestawy
			\param paths lista �cie�ek
		**/
		void setPaths(const QStringList& paths);

	signals:
		/**
			\fn void themeChanged(const QString& theme)
		 	Sygna� jest emitowany, gdy zmieniono aktualny zestaw
			\param theme nazwa nowego zestawu
		**/
		void themeChanged(const QString& theme);

		/**
			\fn void pathsChanged(const QStringList& list)
		 	Sygna� jest emitowany, gdy zmieniono �cie�k� wyszukiwania
			\param list lista nowych �cie�ek
		**/
		void pathsChanged(const QStringList& list);
};

#endif
