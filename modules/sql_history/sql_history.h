#ifndef SQL_HISTORY_H
#define SQL_HISTORY_H

#include <QtCore/QObject>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QKeyEvent>
#include <QtSql/QtSql>
#include <QtGui/QMenu>
#include <QtGui/qcheckbox.h>
#include <QtGui/QSpinBox>
#include <QtCore/qmap.h>
#include <QtCore/qpair.h>
#include <QtCore/qstring.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qvariant.h>
#include <QtCore/qdatetime.h>
#include <QtGui/qdialog.h>

#include "protocols/protocol.h"
#include "../history/history.h"

#include "chat/chat_message.h"
#include "../history/storage/history-storage.h"

class Account;

/**
	@class HistorySqlStorage
	@author Juzef, Adrian
	@short Klasa obs�ugi zapisu historii rozm�w w bazach danych SQLite, MySQL i PostgreSQL.
**/

class HistorySqlStorage : public HistoryStorage, ConfigurationUiHandler, ConfigurationAwareObject
{
	Q_OBJECT

	QSpinBox *portSpinBox;	
	QComboBox *driverComboBox;
	QLineEdit* hostLineEdit;
	QLineEdit* userLineEdit; 
	QLineEdit* nameLineEdit;  
	QLineEdit* passLineEdit;
	QLineEdit* prefixLineEdit;

	QSqlDatabase debe; /*!< Obiekt reprezentuj�cy bie��c� baz� danych. */
	QString tableNamePrefix;
	/**
		\fn QString findNewUidGroupId();
		Funkcja zwraca ID dla nowo tworzonego pola grupy u�ytkownik�w
	**/
	QString findNewUidGroupId();

	void createDefaultConfiguration(); 
	/**
		\fn void appendHistory(ChatWidget *chat);
		Funkcja dodaje zapis historii rozm�w do nowo tworzonego okna rozmowy.
		\param chat okno rozmowy
	**/
	void appendHistory(ChatWidget *chat);

	/**
		\fn void removeHistory(const ContactList& uids);
		Funkcja usuwa ca�� histori� dla podanej listy u�ytkownik�w.
		\param uids lista u�ytkownik�w
	**/
	//void removeHistory(const ContactList& uids);
	QList<ContactList> getUidGroups(QString queryString);
		
	void executeQuery(const QString &query);
	/**
		\fn QString prepareText(const QString &text);
		Funkcja zabezpiecza w tek�cie znaki mog�ce potencjalnie rozwali� zapytanie do bazy.
		\param text tekst do przeczyszczenia
		\return wyczyszczony tekst
	**/
	QString prepareText(const QString &text);	
	/**
		\fn QString addUidGroup(ContactList users);
		Funkcja tworzy rekord nowej grupy u�ytkownik�w.
		\param users lista u�ytkownik�w
		\return string z ID pola bazy danych nowo utworzonej grupy u�ytkownik�w
	**/
	QString addUidGroup(ContactList users);	

virtual void configurationUpdated();

private slots:
	void driverComboBoxValueChanged(int index);
		
	void initializeDatabase();
	void configurationWindowApplied();
	void portSpinBoxValueChanged(int value);

	virtual void messageReceived(Chat *chat, Contact contact, const QString &message);


public:
	HistorySqlStorage(bool firstLoad);
	~HistorySqlStorage();

	QList<ContactList> getAllUidGroups();
	QList<ContactList> getChatUidGroups();
	QList<ContactList> getStatusUidGroups();
	QList<ContactList> getSmsUidGroups();

	QList<QDate> getAllDates();
	QList<QDate> historyDates(const ContactList& uids);
	QList<QDate> historyStatusDates(const ContactList& uids);
	QList<QDate> historySmsDates(const ContactList& uids);

	QList<ChatMessage*> historyMessages(const ContactList& uids, QDate date = QDate());
	QList<ChatMessage*> getStatusEntries(const ContactList& uids, QDate date = QDate());
	QList<ChatMessage*> getSmsEntries(const ContactList& uids, QDate date = QDate());

	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);
	QString findUidGroup(ContactList users);
	int getEntriesCount(const QList<ContactList> &uids, HistoryEntryType type);
	/**
		\fn void appendMessageEntry(ContactList list, const QString &msg, bool outgoing, time_t send_time, time_t receive_time);
		Zapisuje wiadomo�ci w bazie. Z za�o�enia interfejs dla modu��w importu/eksportu historii.
		\param list lista u�ytkownik�w
		\param msg tre�� wiadomo�ci
		\param outgoing wiadomo�� wys�ana
		\param send_time czas wys�ania wiadomo�ci
		\param receive_time czas odebrania 
	**/
	void appendMessageEntry(ContactList list, const QString &msg, bool outgoing, time_t send_time, time_t receive_time);
	/**
		\fn void appendSmsEntry(ContactList list, const QString &msg, bool outgoing, time_t send_time, time_t receive_time);
		Zapisuje smsy w bazie. Z za�o�enia interfejs dla modu��w importu/eksportu historii.
		\param list lista u�ytkownik�w
		\param msg tre�� wiadomo�ci
		\param outgoing wiadomo�� wys�ana
		\param send_time czas wys�ania wiadomo�ci
		\param receive_time czas odebrania 
	**/
	void appendSmsEntry(ContactList list, const QString &msg, bool outgoing, time_t send_time, time_t receive_time);
	/**
		\fn void appendStatusEntry(ContactList list, const QString &status, const QString &desc, time_t time, const QString &adds);
		Zapisuje statusy w bazie. Z za�o�enia interfejs dla modu��w importu/eksportu historii.
		\param list lista u�ytkownik�w
		\param msg tre�� wiadomo�ci
		\param outgoing wiadomo�� wys�ana
		\param send_time czas wys�ania wiadomo�ci
		\param receive_time czas odebrania 
	**/
	void appendStatusEntry(ContactList list, const QString &status, const QString &desc, time_t time, const QString &adds);

	/**
		\fn void removeHistory(const ContactList& uids, const QDate &date = 0);
		Funkcja usuwa ca�� histori� dla podanej listy u�ytkownik�w lub tylko dla podanej daty.
		\param uids lista u�ytkownik�w
	**/
	void removeHistory(const ContactList& uids, const QDate &date = QDate(), HistoryEntryType type = EntryTypeMessage);
		
	bool beginTransaction();
	bool commitTransaction();
	bool rollbackTransaction();


public slots:
	void appendSms(const QString &mobile, const QString &msg);
	void appendStatus(Contact elem, QString protocolName);
	void chatMsgReceived(Protocol *protocol, ContactList senders, const QString& msg, time_t time);
	void messageSentAndConfirmed(ContactList, const QString&, time_t time = 0);
	HistorySearchResult searchHistory(ContactList users, HistorySearchParameters params);

};

extern HistorySqlStorage *historySqlStorage;

#endif
