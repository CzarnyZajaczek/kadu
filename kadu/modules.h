#ifndef MODULES_H
#define MODULES_H

#include <qstring.h>
#include <qobject.h>
#include <qstringlist.h>
#include <qdialog.h>
#include <qlistview.h>
#include <qtranslator.h>
#include <qlabel.h>
#include <qhbox.h>

/**
	Zast�puje klas� QLibrary na specyficzne potrzeby Kadu
**/
class Library : public QObject
{
	private:
		QString FileName;
		void* Handle;
		
	public:
		Library(const QString& file_name);
		~Library();
		bool load();
		void* resolve(const QString& symbol_name);
		QString error();
};

/**
 informacje o module
**/
struct ModuleInfo
{
	/**
	 jakie inne modu�y s� wymagane przez ten modu�
	**/
	QStringList depends;
	/**
	 z jakimi modu�ami ten modu� konfliktuje
	**/
	QStringList conflicts;
	/**
	 jak� cech� dostarcza ten modu�
	**/
	QStringList provides;
	/**
	 opis
	**/
	QString description;
	/**
	 autor modu�u
	**/
	QString author;
};

class ModulesDialog : public QHBox
{
	Q_OBJECT
	
	private:
		QListView* lv_modules;
		QLabel* l_moduleinfo;
		
	private slots:
		void loadItem();
		void unloadItem();
		void moduleAction(QListViewItem *);
		void itemsChanging();
		void getInfo();
		void refreshList();
		void keyPressEvent(QKeyEvent *);
	
	public:
		ModulesDialog();
		~ModulesDialog();
};

class ModulesManager : public QObject
{
	Q_OBJECT

	private:
		typedef int InitModuleFunc();
		typedef void CloseModuleFunc();
		/**
			Informacje o statycznym module.
		**/
		struct StaticModule
		{
			InitModuleFunc* init;
			CloseModuleFunc* close;
		};
		/**
			Lista statycznych modu��w wype�niania
			przez kod generowany przez configure.
		**/
		QMap<QString,StaticModule> StaticModules;
		/**
			Informacje o aktywnym module
			statycznym b�d� zewn�trznym.
			Dla modu�u statycznego lib==NULL.
		**/
		struct Module
		{
			Library* lib;
			CloseModuleFunc* close;
			QTranslator* translator;
			ModuleInfo info;
			int usage_counter;
		};
		/**
			Lista aktywnych modu��w
			statycznych b�d� zewn�trznych.
		**/
		QMap<QString,Module> Modules;	
		/**
		**/
		ModulesDialog* Dialog;
		/**
			�aduje plik z t�umaczeniem. Zwraca NULL je�li wyst�pi�
			b��d.
		**/
		QTranslator* loadModuleTranslation(const QString& module_name);
		/**
			Sprawdza czy dost�pne s� modu�y z listy
			zale�no�ci danego modu�u. W razie czego
			stara si� je za�adowa� je�li s� dost�pne.
			@param module_info informacje o module
		**/
		bool satisfyModuleDependencies(const ModuleInfo& module_info);
		/**
			Zwi�ksza liczniki u�ycia modu��w u�ywanych
			przez dany modu�.
			@param module_info informacje o module
		**/
		void incDependenciesUsageCount(const ModuleInfo& module_info);
		/**
			Rejestruje modu� statyczny. Funcja wywo�ywana
			dla wszystkich modu��w statycznych przez kod
			wygenerowany przez configure.
		**/
		void registerStaticModule(const QString& module_name,
			InitModuleFunc* init,CloseModuleFunc* close);
		/**
			Rejestruje modu�y statyczne. Kod funkcji jest
			generowany przez configure.
		**/
		void registerStaticModules();

	private slots:
		void dialogDestroyed();
	
	public:	
		static void initModule();
		static void closeModule();
		ModulesManager();
		~ModulesManager();
		/**
			Zwraca list� modu��w wkompilowanych
			statycznie w plik wykonywalny kadu
		**/
		QStringList staticModules() const;
		/**
			Zwraca list� modu��w zainstalowanych jako
			dzielone biblioteki (shared libraries)
		**/
		QStringList installedModules() const;
		/**
			Zwraca list� modu��w za�adowanych do pami�ci
			jako dzielone biblioteki (shared libraries)
		**/
		QStringList loadedModules() const;
		/**
			Zwraca list� modu��w zainstalowanych jako
			dzielone biblioteki (shared libraries)
			i nie za�adowanych aktualnie do pami�ci
		**/		
		QStringList unloadedModules() const;
		/**
			Zwraca list� aktywnych modu��w.
			Uwzgl�dniane s� zar�wno aktywne modu�y
			statyczne jak i zewn�trzne.
		**/
		QStringList activeModules() const;
		/**
			Pobiera do info informacje o module module_name jesli
			sie to udalo zwraca true w przeciwnym wypadku false		 
		**/		
		bool moduleInfo(const QString& module_name, ModuleInfo& info) const;
		/**
			Sprawdza czy podany modu� jest wkompilowany statycznie.
			@param module_name nazwa modu�u
		**/
		bool moduleIsStatic(const QString& module_name) const;
		/**
			Sprawdza czy podany modu� jest zainstalowany
			w katalogu z modu�ami zewn�trznymi.
			@param module_name nazwa modu�u
		**/
		bool moduleIsInstalled(const QString& module_name) const;
		/**
			Sprawdza czy podany modu� zewn�trzny jest za�adowany
			@param module_name nazwa modu�u
		**/		
		bool moduleIsLoaded(const QString& module_name) const;
		/**
			Sprawdza czy podany modu� jest aktywny.
			Dzia�a dla modu��w statycznych i za�adowanych
			zewn�trznych.
			@param module_name nazwa modu�u
		**/
		bool moduleIsActive(const QString& module_name) const;
		/**
			Sprawdza czy podany modu� konfliktuje
			z jakim� innym za�adowanym modu�em
			@param module_name nazwa modu�u
			@param module_info informacje o module
		**/
		bool conflictsWithLoaded(const QString &module_name, const ModuleInfo& module_info) const;
		
	public slots:
		/**
			Aktywuje modu� statyczny je�li jest dost�pny
			lub �aduje do pami�ci i aktywuje modu� zewn�trzny.
			@param module_name nazwa modu�u
		**/
		bool activateModule(const QString& module_name);
		/**
			Deaktywuje modu� statyczny lub
			deaktywuje i usuwa z pami�ci modu� zewn�trzny.
			@param module_name nazwa modu�u
		**/		
		bool deactivateModule(const QString& module_name, bool force=false);
		void showDialog();
		/**
		 	Zwi�ksza licznik u�ycia modu�u o 1
			@param module_name nazwa modu�u
		**/
		void moduleIncUsageCount(const QString& module_name);
		/**
		 	Zmniejsza licznik u�ycia modu�u o 1
			@param module_name nazwa modu�u
		**/
		void moduleDecUsageCount(const QString& module_name);
		/**
			Zapisuje do pliku konfiguracyjnego list� za�adowanych
			modu��w.
		**/
		void saveLoadedModules();
};

extern ModulesManager* modules_manager;

#endif
