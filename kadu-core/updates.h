#ifndef KADU_UPDATES_H
#define KADU_UPDATES_H

#include <qstring.h>
#include <qdatetime.h>
#include <qobject.h>

#include "gadu.h"

class QUrlOperator;
class QNetworkOperation;

class Updates : public QObject
{
	Q_OBJECT
	public:
		static void initModule();
		static void closeModule();

	private slots:
		void gotUpdatesInfo(const QByteArray &data, QNetworkOperation *op);
		void run();

	private:
		static bool UpdateChecked;
		static Updates *instance;
		static QDateTime LastUpdateCheck;
		
		QString query;
		QUrlOperator *op;

		Updates(UinType uin);
		virtual ~Updates();
		static bool ifNewerVersion(const QString &newestversion);
		static QString stripVersion(const QString stripversion);
};

#endif
