#ifndef MODULES_H
#define MODULES_H

#include <qstring.h>
#include <qobject.h>
#include <qstringlist.h>
#include <qdialog.h>
#include <qlistbox.h>
#include <qtranslator.h>

/**
	Zast�puje klas� QLibrary na specyficzne potrzeby Kadu
**/
class Library
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
	 jakie inne moduly sa wymagane przez ten modul
	**/
	QStringList depends;
	/**
	 opis
	**/
	QString description;
	/**
	 autor modulu
	**/
	QString author;
};

class ModulesDialog : public QDialog
{
	Q_OBJECT
	
	private:
		QListBox* InstalledListBox;
		QListBox* LoadedListBox;
		
	private slots:
		void loadItem(QListBoxItem* item);
		void unloadItem(QListBoxItem* item);
		void loadSelectedItem();
		void unloadSelectedItem();
		void getInfo();
		void refreshLists();
	
	public:
		ModulesDialog();
};

class ModulesManager : public QObject
{
	Q_OBJECT

	private:
		typedef void CloseModuleFunc();
		struct Module
		{
			Library* lib;
			CloseModuleFunc* close;
			QTranslator* translator;
			ModuleInfo info;
			int usage_counter;
		};
		QMap<QString,Module> Modules;	
		ModulesDialog* Dialog;
		void initStaticModules();
		void closeStaticModules();
		/**
			�aduje plik z t�umaczeniem. Zwraca NULL je�li wyst�pi�
			b��d.
		**/
		QTranslator* loadModuleTranslation(const QString& module_name);
		/**
			�aduje pliki t�umacze� dla wszystkich wkompilowanych
			modu��w.
		**/
		void loadStaticModulesTranslations();

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
		QStringList staticModules();
		/**
			Zwraca list� modu��w zainstalowanych jako
			dzielone biblioteki (shared libraries)
		**/
		QStringList installedModules();
		/**
			Zwraca list� modu��w za�adowanych do pami�ci
			jako dzielone biblioteki (shared libraries)
		**/
		QStringList loadedModules();
		/**
			Zwraca list� modu��w zainstalowanych jako
			dzielone biblioteki (shared libraries)
			i nie za�adowanych aktualnie do pami�ci
		**/		
		QStringList unloadedModules();
		/**
			Pobiera do info informacje o module module_name jesli
			sie to udalo zwraca true w przeciwnym wypadku false		 
		**/		
		bool moduleInfo(const QString& module_name, ModuleInfo& info);
		/**
			�aduje modu� do pami�ci i inicjalizuje go
		**/
		bool loadModule(const QString& module_name);
		/**
			Zamyka modu� i usuwa go z pami�ci
		**/		
		bool unloadModule(const QString& module_name, bool force=false);
		/**
			Sprawdza czy podany modu� jest wkompilowany statycznie.
			@param module_name nazwa modulu
		**/
		bool moduleIsStatic(const QString& module_name);
		/**
			Sprawdza czy podany modu� jest zainstalowany
			w katalogu z modu�ami zewn�trznymi.
			@param module_name nazwa modulu
		**/
		bool moduleIsInstalled(const QString& module_name);
		/**
			Sprawdza czy podany modu� zewn�trzny jest za�adowany
			@param module_name nazwa modulu
		**/		
		bool moduleIsLoaded(const QString& module_name);
		/**
			Sprawdza czy podany modu� jest aktywny.
			Dzia�a dla modu��w statycznych i za�adowanych
			zewn�trznych.
			@param module_name nazwa modulu
		**/
		bool moduleIsActive(const QString& module_name);
		/**
			Zapisuje do pliku konfiguracyjnego list� za�adowanych
			modu��w.
		**/
		void saveLoadedModules();
		
	public slots:
		void showDialog();
};

extern ModulesManager* modules_manager;

#endif
