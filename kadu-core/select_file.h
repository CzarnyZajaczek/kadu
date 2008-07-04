/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SELECT_FILE_H
#define SELECT_FILE_H

#include <QtGui/QWidget>

class QLineEdit;

class SelectFile : public QWidget
{
	Q_OBJECT

	QString Type;
	QLineEdit *LineEdit;

private slots:
	void selectFileClicked();

public:
	SelectFile(const QString &type, QWidget *parent = 0);
	SelectFile(QWidget *parent = 0);

	void setType(const QString &type) { Type = type; }

	QString file() const;
	void setFile(const QString &file);

};

#endif // SELECT_FILE_H
