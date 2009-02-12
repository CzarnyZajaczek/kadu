/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GADU_CONTACT_ACCOUNT_DATA_WIDGET
#define GADU_CONTACT_ACCOUNT_DATA_WIDGET

#include <QtGui/QDialog>
#include <QtGui/QLineEdit>

#include "gui/widgets/configuration/configuration-window.h"

class GaduContactAccountData;

class GaduContactAccountDataWidget : public QWidget
{
	Q_OBJECT

	GaduContactAccountData *ContactAccountData;
	void createGui();
public:
	GaduContactAccountDataWidget(GaduContactAccountData *contactAccountData, QWidget *parent = 0);
	~GaduContactAccountDataWidget();

};

#endif // GADU_CONTACT_ACCOUNT_DATA_WIDGET
