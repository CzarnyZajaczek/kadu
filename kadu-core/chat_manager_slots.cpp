/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qdir.h>
#include <qhgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qtextedit.h>
#include <qtooltip.h>

#include <stdlib.h>

#include "chat_manager.h"
#include "chat_manager_slots.h"
// #include "config_dialog.h"
#include "config_file.h"
#include "debug.h"
#include "message_box.h"
#include "misc.h"

ChatManagerSlots::ChatManagerSlots(QObject* parent, const char* name)
	: QObject(parent, name)
{
}

void ChatManagerSlots::setMailPath(int mailNumber)
{
// 	kdebugf();

// 	QLineEdit *mailPathEdit = ConfigDialog::getLineEdit("Chat", "Custom Mail client");

// 	switch (mailNumber)
// 	{
// 		case 0:
// 		{
// 			mailPathEdit->setBackgroundMode(PaletteBase);
// 			mailPathEdit->setReadOnly(false);
// 			return;
// 		}
// 		case 1:
// 		{
// 			mailPathEdit->setText("kmail %1");
// 			mailPathEdit->setBackgroundMode(PaletteButton);
// 			mailPathEdit->setReadOnly(true);
// 			break;
// 		}
// 		case 2:
// 		{
// 			mailPathEdit->setText("thunderbird -compose mailto:%1");
// 			mailPathEdit->setBackgroundMode(PaletteButton);
// 			mailPathEdit->setReadOnly(true);
// 			break;
// 		}
// 		case 3:
// 		{
// 			mailPathEdit->setText("seamonkey -compose mailto:%1");
// 			mailPathEdit->setBackgroundMode(PaletteButton);
// 			mailPathEdit->setReadOnly(true);
// 			break;
// 		}
//		case 4:
//		{
//			mailPathEdit->setText("evolution mailto:%1");
//			mailPathEdit->setBackgroundMode(PaletteButton);
//			mailPathEdit->setReadOnly(true);
//			break;
//		}
// 		default:
// 			return;
// 	}

// 	findMailClient(mailNumber);

// 	kdebugf2();
}

void ChatManagerSlots::findMailClient(const unsigned int mailNumber)
{
	kdebugf();
	QStringList searchPath = QStringList::split(":", QString(getenv("PATH")));
	QString mailClient;
	QString mailClientAlt; // nazwa alternatywna, przydatna gdy dystrybucja zmieni�a nazw� binarki

	switch (mailNumber)
	{
		case 1:
		{
			searchPath.append("/opt/kde/bin");
			searchPath.append("/opt/kde3/bin");
			mailClient = "kmail";
			break;
		}
		case 2:
		{
			searchPath.append("/usr/local/Mozilla");
			searchPath.append("/usr/local/mozilla");
			searchPath.append("/usr/local/Thunderbird");
			searchPath.append("/usr/local/thunderbird");
			searchPath.append("/opt/thunderbird");
			mailClient = "thunderbird";
			mailClientAlt = "mozilla-thunderbird"; // debian i pochodne
			break;
		}
		case 3:
		{
			searchPath.append("/usr/local/Mozilla");
			searchPath.append("/usr/local/mozilla");
			searchPath.append("/usr/local/Seamonkey");
			searchPath.append("/usr/local/seamonkey");
			searchPath.append("/opt/seamonkey");
			mailClient = "seamonkey";
			break;
		}
		case 4:
		{
			searchPath.append("/opt/evolution");
			searchPath.append("/usr/local/evolution");
			searchPath.append("/usr/local/Evolution");
			mailClient = "evolution";
			break;
		}
		default:
			return;
	}

	QString mailClientPath;
	CONST_FOREACH(path, searchPath)
	{
		if (QFile::exists((*path) + '/' + mailClient) || (!mailClientAlt.isEmpty() && QFile::exists((*path) + '/' + mailClientAlt)))
		{
			mailClientPath = (*path);
			break;
		}
	}

	if (mailClientPath.isEmpty())
	{
		MessageBox::msg(tr("Selected mail client was not found in your system. The path to it doesn't exist in $PATH variable.\nYou may add it to $PATH or specify location using Specify path option"));

// 		ConfigDialog::getComboBox("Chat", "Choose your mail client")->setCurrentItem(0);
// 		setMailPath(0);

		return;
	}

// 	QLineEdit *mailPathEdit = ConfigDialog::getLineEdit("Chat", "Custom Mail client");
// 	mailPathEdit->setText(mailClientPath + '/' + mailPathEdit->text());

	kdebugf2();
}

void ChatManagerSlots::setMailClients(QComboBox *mailCombo)
{
	mailCombo->insertItem(tr("Specify path"));
	mailCombo->insertItem("KMail");
	mailCombo->insertItem("Thunderbird");
	mailCombo->insertItem("SeaMonkey");
	mailCombo->insertItem("Evolution");
}

void ChatManagerSlots::onCreateTabChat()
{
	kdebugf();

// 	if ((EmoticonsStyle)config_file.readNumEntry("Chat", "EmoticonsStyle") == EMOTS_NONE)
// 		(cb_emoticons_theme)->setEnabled(false);

	//ustawienie p�l w combo wyboru przegladarki
// 	QComboBox *browserCombo= ConfigDialog::getComboBox("Chat", "Choose your browser");
// 	QComboBox *browserOptionsCombo=ConfigDialog::getComboBox("Chat", "Browser options");
// 	QLineEdit *browserPath= ConfigDialog::getLineEdit("Chat", "Custom Web browser");
// 	initBrowserOptions(browserCombo, browserOptionsCombo, browserPath);

	//podpiecie pod zmiane w combo
// 	connect(browserCombo, SIGNAL(activated (int)), this, SLOT(findAndSetWebBrowser(int)));
// 	connect(browserOptionsCombo, SIGNAL(activated (int)), this, SLOT(findAndSetBrowserOption(int)));

	//ustawienie p�l w combo wyboru klienta poczty i podpi�cie pod zmian�
// 	QComboBox *mailCombo = ConfigDialog::getComboBox("Chat", "Choose your mail client");
// 	setMailClients(mailCombo);
// 	connect(mailCombo, SIGNAL(activated(int)), this, SLOT(setMailPath(int)));
	//ustawienie wybranego klienta w combo i wpisanie jego �cie�ki do pola edycji
	unsigned int mailClientNo = config_file.readUnsignedNumEntry("Chat", "MailClientNo", 0);
// 	mailCombo->setCurrentItem(mailClientNo);
	setMailPath(mailClientNo);

	onPruneChat(config_file.readBoolEntry("Chat", "ChatPrune"));

// 	QCheckBox *c_foldlink = ConfigDialog::getCheckBox("Chat", "Automatically fold links");
// 	QHGroupBox *h_fold = ConfigDialog::getHGroupBox("Chat", "Link folding");
	onFoldLink(config_file.readBoolEntry("Chat", "FoldLink"));

// 	QToolTip::add(h_fold, tr("URLs longer than this value will be shown truncated to this length"));
// 	QToolTip::add(c_foldlink, tr("This will show a long URL as http://www.start...end.com/\nto protect the chat window from a mess"));
// 	ConfigDialog::getSpinBox("Chat", "Max image size")->setSuffix(" kB");

// 	QSpinBox *blockCloseTime=ConfigDialog::getSpinBox("Chat", "Max time to block window close");
// 	blockCloseTime->setEnabled(config_file.readBoolEntry("Chat", "ChatCloseTimer"));
// 	blockCloseTime->setSuffix(" s");
// 	QCheckBox *shortcutSends = ConfigDialog::getCheckBox("Chat", "\"%1\" in chat sends message by default");
// 	shortcutSends->setText(shortcutSends->text().arg(config_file.readEntry("ShortCuts", "chat_newline")));

	kdebugf2();
}

void ChatManagerSlots::onCreateTabLook()
{
	kdebugf();
	//deaktywacja opcji wylaczenia separatorow
// 	QCheckBox *b_noHeadersRepeat= ConfigDialog::getCheckBox("Look", "Remove chat header repetitions");

// 	QSpinBox *s_headersSeparatorHeight= ConfigDialog::getSpinBox("Look", "Chat header separators height:");
// 	QSpinBox *s_noHeadersInterval= ConfigDialog::getSpinBox("Look", "Interval between header removal:");

// 	QSpinBox *s_SeparatorHeight= ConfigDialog::getSpinBox("Look", "Message separators height:");

// 	s_headersSeparatorHeight->setEnabled(b_noHeadersRepeat->isChecked());
// 	s_noHeadersInterval->setEnabled(b_noHeadersRepeat->isChecked());

	//dodanie suffiksu w spinboksach
// 	s_headersSeparatorHeight->setSuffix(" px");
// 	s_noHeadersInterval->setSuffix(" min");
// 	s_SeparatorHeight->setSuffix(" px");

	//opcja usuwania czasu z serwera
// 	QSpinBox *s_noServerTimeDiff= ConfigDialog::getSpinBox("Look", "Maximum time difference");

// 	s_noServerTimeDiff->setEnabled(ConfigDialog::getCheckBox("Look", "Remove server time")->isChecked());
// 	s_noServerTimeDiff->setSuffix(" s");

// 	onChatThemeChanged(ConfigDialog::getComboBox("Look", "Select chat style")->currentText());
// 	updatePreview();

	kdebugf2();
}

void ChatManagerSlots::onChatThemeChanged(const QString &name)
{
	kdebugf();
// 	ConfigDialog::getTextEdit("Look", "Full chat style:")->setEnabled(name == qApp->translate("Chat", "Custom"));
}

void ChatManagerSlots::onBlockClose(bool toggled)
{
// 	ConfigDialog::getSpinBox("Chat", "Max time to block window close")->setEnabled(toggled);
}

void ChatManagerSlots::onPruneChat(bool toggled)
{
// 	ConfigDialog::getHGroupBox("Chat", "Message pruning")->setEnabled(toggled);
}

void ChatManagerSlots::onFoldLink(bool toggled)
{
// 	ConfigDialog::getHGroupBox("Chat", "Link folding")->setEnabled(toggled);
}

void ChatManagerSlots::onRemoveHeaders(bool toggled)
{
// 	ConfigDialog::getSpinBox("Look", "Chat header separators height:")->setEnabled(toggled);
// 	ConfigDialog::getSpinBox("Look", "Interval between header removal:")->setEnabled(toggled);
}

void ChatManagerSlots::onRemoveServerTime(bool toggled)
{
// 	ConfigDialog::getSpinBox("Look", "Maximum time difference")->setEnabled(toggled);
}

void ChatManagerSlots::onApplyTabLook()
{
	kdebugf();
/*
	Aby unikn�c problem�w z niepoprawnymi localesami i pozniejszymi
	k�opotami kt�re moga wynikn�c z tego, musimy zamienic dwie mozliwe
	mozliwo�ci na _puste_pole_ przez co uzyskamy ze kadu i tak bedzie
	dynamicznie reagowac na zmiany locales�w nie zaleznie jaka wersja
	by�a zapisana przed ustawieniem ustawien domyslnych(moze nie za
	dobrze to wyjasnione, ale konieczne. Nie dotyczy to dw�ch zmiennych
	config.panelsyntax i config.conferencesyntax, bo pierwotnie zawieraj
	TYLKO sam� sk�adnie)
*/

// 	QLineEdit *e_chatsyntax= ConfigDialog::getLineEdit("Look", "Chat window title syntax:");
// 	QLineEdit *e_conferenceprefix= ConfigDialog::getLineEdit("Look", "Conference window title prefix:");

// 	if (e_chatsyntax->text() == tr("Chat with ")+"%a (%s[: %d])" || e_chatsyntax->text() == "Chat with %a (%s[: %d])")
// 		config_file.writeEntry("Look", "ChatContents", QString::null);

// 	if (e_conferenceprefix->text() == tr("Conference with ") || e_conferenceprefix->text() == "Conference with ")
// 		config_file.writeEntry("Look", "ConferencePrefix", QString::null);

	kdebugf2();
}

void ChatManagerSlots::chooseColor(const char* name, const QColor& color)
{
	kdebugf();
// 	QLabel *preview1= ConfigDialog::getLabel("Look", "<b>Me</b> 00:00:00", "chat_me");
// 	QLabel *preview2= ConfigDialog::getLabel("Look", "<b>Other party</b> 00:00:02", "chat_other");
// 	if (QString(name)=="own_bg_color")
// 		preview1->setPaletteBackgroundColor(color);
// 	else if (QString(name)=="his_bg_color")
// 		preview2->setPaletteBackgroundColor(color);
// 	else if (QString(name)=="own_font_color")
// 		preview1->setPaletteForegroundColor(color);
// 	else if (QString(name)=="his_font_color")
// 		preview2->setPaletteForegroundColor(color);
// 	else
// 		kdebugm(KDEBUG_ERROR, "chooseColor: label '%s' not known!\n", name);
	kdebugf2();
}

void ChatManagerSlots::updatePreview()
{
	kdebugf();
// 	QLabel *preview1= ConfigDialog::getLabel("Look", "<b>Me</b> 00:00:00", "chat_me");
// 	QLabel *preview2= ConfigDialog::getLabel("Look", "<b>Other party</b> 00:00:02", "chat_other");
// 	preview1->setFont(config_file.readFontEntry("Look", "ChatFont"));
// 	preview1->setPaletteForegroundColor(config_file.readColorEntry("Look", "ChatMyFontColor"));
// 	preview1->setPaletteBackgroundColor(config_file.readColorEntry("Look", "ChatMyBgColor"));
// 	preview1->setAlignment(Qt::AlignLeft);
// 	preview2->setFont(config_file.readFontEntry("Look", "ChatFont"));
// 	preview2->setPaletteForegroundColor(config_file.readColorEntry("Look", "ChatUsrFontColor"));
// 	preview2->setPaletteBackgroundColor(config_file.readColorEntry("Look", "ChatUsrBgColor"));
// 	preview2->setAlignment(Qt::AlignLeft);
	kdebugf2();
}

void ChatManagerSlots::chooseBackgroundFile()
{
// 	QLineEdit *bg_path = ConfigDialog::getLineEdit("Look", "Chat background image");
// 	QWidget w;
// 	ImageDialog *bg_dlg = new ImageDialog(&w);
// 	bg_dlg->setDir(bg_path->text());
// 	bg_dlg->setCaption(tr("Insert image"));
// 	if (bg_dlg->exec() == QDialog::Accepted)
// 		bg_path->setText(bg_dlg->selectedFile());
// 	delete bg_dlg;
}

ChatManagerSlots* ChatManager::chatslots=NULL;
