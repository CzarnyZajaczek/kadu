/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 * Copyright 2011 Wojciech Treter (juzefwt@gmail.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtCore/QFileInfo>
#include <QtCore/QProcess>
#include <QtCore/QStringList>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>

#include "misc/path-conversion.h"

#include "sql-restore.h"

#define RECOVERY_SCRIPT "plugins/data/sql_history/scripts/history-database-recovery.sh"

bool SqlRestore::isCorrupted(const QSqlDatabase &database)
{
	if (!database.isOpen()) // do not restore closed database
		return false;

	if (database.isOpenError()) // restore every database that was not properly opened
		return true;

	const QStringList &tables = database.tables();
	if (QSqlError::NoError != database.lastError().type())
		return true;

	return tables.isEmpty();
}

SqlRestore::RestoreError SqlRestore::performRestore(const QString &databaseFilePath)
{
	QString recoveryScriptPath = dataPath(RECOVERY_SCRIPT);

	QFileInfo recoveryScriptFileInfo(recoveryScriptPath);
	if (!recoveryScriptFileInfo.exists())
		return ErrorNoRestoreScriptExecutable;

	QProcess restoreProcess;
	restoreProcess.execute("/bin/bash", QStringList() << recoveryScriptPath << databaseFilePath);
	restoreProcess.waitForFinished(-1);

	if (restoreProcess.exitCode() < 0 || restoreProcess.exitCode() > ErrorRecovering)
		return ErrorRecovering;

	return static_cast<RestoreError>(restoreProcess.exitCode());
}
