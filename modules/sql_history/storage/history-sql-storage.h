#ifndef HISTORY_SQL_STORAGE_H
#define HISTORY_SQL_STORAGE_H

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlTableModel>

#include "../../history/history.h"
#include "../../history/storage/history-storage.h"

/**
	@class HistorySqlStorage
	@author Juzef, Adrian
	@short Klasa obs�ugi zapisu historii rozm�w w bazach danych SQLite, MySQL i PostgreSQL.
**/

class HistorySqlStorage : public HistoryStorage
{
	Q_OBJECT

	QSqlDatabase Database; /*!< Obiekt reprezentuj�cy bie��c� baz� danych. */
	QSqlTableModel *MessagesModel;
	QString DbPrefix;

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
		
	void executeQuery(QSqlQuery query);
	/**
		\fn QString prepareText(const QString &text);
		Funkcja zabezpiecza w tek�cie znaki mog�ce potencjalnie rozwali� zapytanie do bazy.
		\param text tekst do przeczyszczenia
		\return wyczyszczony tekst
	**/
	QString prepareText(const QString &text);	
 
private slots:
	virtual void messageReceived(const Message &message);
	virtual void messageSent(const Message &message);
	void initializeDatabase();

public:
	HistorySqlStorage();
	~HistorySqlStorage();

	virtual QList<Chat *> chatsList();
	virtual QList<QDate> datesForChat(Chat *chat);
	virtual QList<ChatMessage *> getMessages(Chat *chat, QDate date = QDate(), int limit = 0);
	virtual int getMessagesCount(Chat *chat, QDate date = QDate());

	QList<QDate> getAllDates();
	QList<QDate> historyDates(const ContactList& uids);
	QList<QDate> historyStatusDates(const ContactList& uids);
	QList<QDate> historySmsDates(const ContactList& uids);

	QList<ChatMessage*> historyMessages(const ContactList& uids, QDate date = QDate());
	QList<ChatMessage*> getStatusEntries(const ContactList& uids, QDate date = QDate());
	QList<ChatMessage*> getSmsEntries(const ContactList& uids, QDate date = QDate());

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
	void appendMessageEntry(const Message &message);
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
	
public slots:
	void appendSms(const QString &mobile, const QString &msg);
	void appendStatus(Contact elem, QString protocolName);
	HistorySearchResult searchHistory(ContactList users, HistorySearchParameters params);

};

#endif
