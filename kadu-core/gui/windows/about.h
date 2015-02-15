/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010, 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef ABOUT_H
#define ABOUT_H

#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>

#include "os/generic/desktop-aware-object.h"

class QUrl;
class QKeyEvent;
class QMouseEvent;
class QString;

/**
	\class About
	\brief Okno z informacjami o programie.
**/
class About : public QWidget, DesktopAwareObject
{
	Q_OBJECT

	QString loadFile(const QString &name);

private slots:
	void openUrl(const QUrl &url);

protected:
	virtual void keyPressEvent(QKeyEvent *event);

public:
	/**
		\fn About(QWidget *parent = 0)
		Konstruktor tworzy okno, ustawia zak�adki z informacjami o autorach, podzi�kowaniach, licencji oraz list� zmian w programie. \n
		Domy�lnie okno ma rozmiar 640x420.
		\param parent rodzic okna
		\param name nazwa obiektu
	**/
	explicit About(QWidget *parent = 0);

	/**
		\fn ~About()
		Destruktor zapisuje rozmiar okna.
	**/
	virtual ~About();

};

class KaduLink : public QLabel
{
	Q_OBJECT

	QByteArray Link;

protected:
	virtual void mousePressEvent(QMouseEvent *);

public:
	explicit KaduLink(const QByteArray &link, QWidget *parent = 0);
	virtual ~KaduLink();

};

#endif
