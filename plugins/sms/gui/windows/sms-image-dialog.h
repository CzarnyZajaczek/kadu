/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
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

#ifndef SMS_IMAGE_DIALOG_H
#define SMS_IMAGE_DIALOG_H

#include <QtGui/QDialog>
#include <QtScript/QScriptValue>

class QLabel;
class QLineEdit;
class QNetworkReply;

class SmsImageDialog : public QDialog
{
	Q_OBJECT

	QLineEdit *TokenEdit;
	QLabel *PixmapLabel;
	QNetworkReply *TokenNetworkReply;

	QScriptValue CallbackObject;
	QScriptValue CallbackMethod;

	void createGui();
	void loadTokenImage(const QString &tokenImageUrl);
	void result(const QString &value);

private slots:
	void tokenImageDownloaded();

public:
	SmsImageDialog(const QString &tokenImageUrl, QScriptValue callbackObject, QScriptValue callbackMethod, QWidget *parent = 0);
	virtual ~SmsImageDialog();

public slots:
	virtual void accept();
	virtual void reject();

};

#endif //SMS_IMAGE_DIALOG_H
