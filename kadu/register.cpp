/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qlayout.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <errno.h>
#include <stdlib.h>
#include <qfile.h>

// to mam nadzieje kiedys usuniemy
#include "kadu.h"
#include "adduser.h"
#include "config_file.h"
#include "debug.h"
#include "register.h"
//

Register::Register(QDialog *parent, const char *name) : QDialog (parent, name, FALSE, Qt::WDestructiveClose) {
	kdebug("Register::Register()\n");

	QGridLayout *grid = new QGridLayout(this, 5, 2, 6, 5);

	QLabel *l_pwd = new QLabel(i18n("Password"),this);
	pwd = new QLineEdit(this);
	pwd->setEchoMode(QLineEdit::Password);

	QLabel *l_pwd2 = new QLabel(i18n("Retype password"),this);
	pwd2 = new QLineEdit(this);
	pwd2->setEchoMode(QLineEdit::Password);

	QLabel *l_mail = new QLabel(i18n("E-mail"),this);
	mail = new QLineEdit(this);

	QPushButton *snd = new QPushButton(this);
	snd->setText(i18n("Register"));
	QObject::connect(snd, SIGNAL(clicked()), this, SLOT(doRegister()));

	status = new QLabel(this);

	updateconfig = new QCheckBox(this);
	QLabel *l_updateconfig = new QLabel(i18n("Create config file"),this);
	updateconfig->setChecked(true);
	QToolTip::add(l_updateconfig, i18n("Write the newly obtained UIN and password into a clean configuration file\nThis will erase your current config file contents if you have one"));

	grid->addWidget(l_pwd, 0, 0);
	grid->addWidget(pwd, 0, 1);
	grid->addWidget(l_pwd2, 1, 0);
	grid->addWidget(pwd2, 1, 1);
	grid->addWidget(l_mail, 2, 0);
	grid->addWidget(mail, 2, 1);
	grid->addWidget(updateconfig, 3, 0, Qt::AlignRight);
	grid->addWidget(l_updateconfig, 3, 1);
	grid->addWidget(status, 4, 0);
	grid->addWidget(snd, 4, 1);
	grid->addRowSpacing(3, 20);

	setCaption(i18n("Register user"));
	resize(240, 150);

	snr = snw = NULL;
	h = NULL;
}

void Register::doRegister() {
	kdebug("Register::doRegister()\n");
	if (pwd->text() != pwd2->text()) {
		QMessageBox::warning(this, "Kadu", i18n("Passwords do not match"), i18n("OK"), 0, 0, 1);
		return;
		}

	if (!pwd->text().length()) {
		QMessageBox::warning(this, "Kadu", i18n("Please fill out all fields"), i18n("OK"), 0, 0, 1);
		return;
		}

	if (!(h = gg_register(iso_to_cp(mail->text()).data(), iso_to_cp(pwd->text()).data(), 1))) {
		status->setText(i18n("Error"));
		return;
		}

	status->setText(i18n("Registering"));

	setEnabled(false);
	createSocketNotifiers();
}

void Register::closeEvent(QCloseEvent *e) {
	kdebug("Register::closeEvent()\n");
	deleteSocketNotifiers();
	if (h) {
		gg_free_register(h);
		h = NULL;
		}
	QDialog::closeEvent(e);
	kdebug("Register::closeEvent(): end\n");
}

void Register::createSocketNotifiers() {
	kdebug("Register::createSocketNotifiers()\n");

	snr = new QSocketNotifier(h->fd, QSocketNotifier::Read, kadu);
	QObject::connect(snr, SIGNAL(activated(int)), this, SLOT(dataReceived()));

	snw = new QSocketNotifier(h->fd, QSocketNotifier::Write, kadu);
	QObject::connect(snw, SIGNAL(activated(int)), this, SLOT(dataSent()));
}

void Register::deleteSocketNotifiers() {
	kdebug("Register::deleteSocketNotifiers()\n");
	if (snr) {
		snr->setEnabled(false);
		snr->deleteLater();
		snr = NULL;
		}
	if (snw) {
		snw->setEnabled(false);
		snw->deleteLater();
		snw = NULL;
		}
}

void Register::dataReceived() {
	kdebug("Register::dataReceived()\n");
	if (h->check && GG_CHECK_READ)
		socketEvent();
}

void Register::dataSent() {
	kdebug("Register::dataSent()\n");
	snw->setEnabled(false);
	if (h->check && GG_CHECK_WRITE)
		socketEvent();
}

void Register::socketEvent() {
	kdebug("Register::socketEvent()\n");
	if (gg_register_watch_fd(h) == -1) {
		deleteSocketNotifiers();
		gg_free_register(h);
		h = NULL;
		kdebug("Register::socketEvent(): error registering\n");
		status->setText(i18n("Error"));
		setEnabled(true);
		return;
		}
	struct gg_pubdir *p = (struct gg_pubdir *)h->data;
	switch (h->state) {
		case GG_STATE_CONNECTING:
			kdebug("Register::socketEvent(): changing QSocketNotifiers.\n");
			deleteSocketNotifiers();
			createSocketNotifiers();
			if (h->check & GG_CHECK_WRITE)
				snw->setEnabled(true);
			break;
		case GG_STATE_ERROR:
			deleteSocketNotifiers();
			gg_free_register(h);
			h = NULL;
			kdebug("Register::socketEvent(): error registering\n");
			status->setText(i18n("Error"));
			setEnabled(true);
			break;
		case GG_STATE_DONE:
			deleteSocketNotifiers();
			kdebug("Register::socketEvent(): success=%d, uin=%ld\n", p->success, p->uin);
			if (p->success) {
				status->setText(i18n("Success!"));
				uin = p->uin;
				QMessageBox::information(this, "Kadu", i18n("Registration was successful. Your new number is %1.\nStore it in a safe place along with the password.\nNow add your friends to the userlist.").arg(uin),
					i18n("OK"), 0, 0, 1);
				ask();
				kdebug("Register::socketEvent() before close()\n");
//				accept();
				close();
				}
			else {
				gg_free_register(h);
				h = NULL;
				kdebug("Register::socketEvent(): error registering\n");
				status->setText(i18n("Error"));
				setEnabled(true);
				}
			break;
		default:
			if (h->check & GG_CHECK_WRITE)
				snw->setEnabled(true);
		}
}

void Register::ask() {
	kdebug("Register::ask()\n");
	if (updateconfig->isChecked()) {
		config.uin = uin;
		config.password = pwd->text();
		createConfig();
		}
}


void createConfig() {
	kdebug("createConfig()\n");
	char *home = getenv("HOME");
	struct passwd *pw;

	kdebug("createConfig(): $HOME=%s\n", home);
	if (!home) {
		if (!(pw = getpwuid(getuid())))
			return;
		home = pw->pw_dir;
		}

	struct stat buf;	
	QString ggpath = ggPath("");
	stat(ggpath.local8Bit(), &buf);
	if (S_ISDIR(buf.st_mode))
		kdebug("createConfig(): Directory %s exists\n", (const char *)ggpath.local8Bit());
	else {
		kdebug("createConfig(): Creating directory\n");
		if (mkdir(ggpath.local8Bit(), 0700) != 0 ) {
			perror("mkdir");
			return;
			}
		}

	kdebug("createConfig(): Writing config files...\n");
	ConfigFile *konf;
	konf = new ConfigFile(ggPath("kadu.conf"));
	konf->setGroup("Global");
	konf->writeEntry("UIN", int(config.uin));
	konf->writeEntry("Password", pwHash(config.password));
	konf->sync();
	delete konf;

	kadu->setCaption(QString("Kadu: %1").arg(config.uin));

	kdebug("createConfig(): Config file created\n");
}

Unregister::Unregister(QDialog *parent, const char *name) : QDialog (parent, name, FALSE, Qt::WDestructiveClose) {
	kdebug("Unregister::Unregister()\n");

	QGridLayout *grid = new QGridLayout(this, 4, 2, 6, 5);

	QLabel *l_uin = new QLabel(i18n("UIN"),this);
	uin = new QLineEdit(this);

	QLabel *l_pwd = new QLabel(i18n("Password"),this);
	pwd = new QLineEdit(this);
	pwd->setEchoMode(QLineEdit::Password);

	QLabel *l_mail = new QLabel(i18n("E-mail"),this);
	mail = new QLineEdit(this);

	QPushButton *snd = new QPushButton(this);
	snd->setText(i18n("Unregister"));
	QObject::connect(snd, SIGNAL(clicked()), this, SLOT(doUnregister()));

	status = new QLabel(this);

	grid->addWidget(l_uin, 0, 0);
	grid->addWidget(uin, 0, 1);
	grid->addWidget(l_pwd, 1, 0);
	grid->addWidget(pwd, 1, 1);
	grid->addWidget(l_mail, 2, 0);
	grid->addWidget(mail, 2, 1);
	grid->addWidget(status, 3, 0);
	grid->addWidget(snd, 3, 1);
	grid->addRowSpacing(3, 20);

	setCaption(i18n("Unregister user"));
	resize(240, 150);

	snr = snw = NULL;
	h = NULL;
}

void Unregister::doUnregister() {
	kdebug("Unregister::doUnregister()\n");

	if (!uin->text().toUInt() || !pwd->text().length()) {
		QMessageBox::warning(this, "Kadu", i18n("Please fill out all fields"), i18n("OK"), 0, 0, 1);
		return;
		}

	if (!(h = gg_unregister(uin->text().toUInt(), pwd->text().local8Bit(), mail->text().local8Bit(), 1))) {
		status->setText(i18n("Error"));
		return;
		}

	status->setText(i18n("Unregistering"));

	setEnabled(false);
	createSocketNotifiers();
}

void Unregister::closeEvent(QCloseEvent *e) {
	kdebug("Unregister::closeEvent()\n");
	deleteSocketNotifiers();
	if (h) {
		gg_free_register(h);
		h = NULL;
		}
	QDialog::closeEvent(e);
	kdebug("Unregister::closeEvent(): end\n");
}

void Unregister::createSocketNotifiers() {
	kdebug("Unregister::createSocketNotifiers()\n");

	snr = new QSocketNotifier(h->fd, QSocketNotifier::Read, kadu);
	QObject::connect(snr, SIGNAL(activated(int)), this, SLOT(dataReceived()));

	snw = new QSocketNotifier(h->fd, QSocketNotifier::Write, kadu);
	QObject::connect(snw, SIGNAL(activated(int)), this, SLOT(dataSent()));
}

void Unregister::deleteSocketNotifiers() {
	kdebug("Unregister::deleteSocketNotifiers()\n");
	if (snr) {
		snr->setEnabled(false);
		snr->deleteLater();
		snr = NULL;
		}
	if (snw) {
		snw->setEnabled(false);
		snw->deleteLater();
		snw = NULL;
		}
}

void Unregister::dataReceived() {
	kdebug("Unregister::dataReceived()\n");
	if (h->check && GG_CHECK_READ)
		socketEvent();
}

void Unregister::dataSent() {
	kdebug("Unregister::dataSent()\n");
	snw->setEnabled(false);
	if (h->check && GG_CHECK_WRITE)
		socketEvent();
}

void Unregister::socketEvent() {
	kdebug("Unregister::socketEvent()\n");
	if (gg_register_watch_fd(h) == -1) {
		deleteSocketNotifiers();
		gg_free_register(h);
		h = NULL;
		kdebug("Unregister::socketEvent(): error unregistering\n");
		status->setText(i18n("Error"));
		setEnabled(true);
		return;
		}
	struct gg_pubdir *p = (struct gg_pubdir *)h->data;
	switch (h->state) {
		case GG_STATE_CONNECTING:
			kdebug("Unregister::socketEvent(): changing QSocketNotifiers.\n");
			deleteSocketNotifiers();
			createSocketNotifiers();
			if (h->check & GG_CHECK_WRITE)
				snw->setEnabled(true);
			break;
		case GG_STATE_ERROR:
			deleteSocketNotifiers();
			gg_free_register(h);
			h = NULL;
			kdebug("Unregister::socketEvent(): error unregistering\n");
			status->setText(i18n("Error"));
			setEnabled(true);
			break;
		case GG_STATE_DONE:
			deleteSocketNotifiers();
			kdebug("Unregister::socketEvent(): success\n");
			if (p->success) {
				status->setText(i18n("Success!"));
//				uin = p->uin;
//				QMessageBox::information(this, "Kadu", i18n("Registration was successful. Your new number is %1.\nStore it in a safe place along with the password.\nNow add your friends to the userlist.").arg(uin),
//					i18n("OK"), 0, 0, 1);
//				ask();
				deleteConfig();
				kdebug("Unregister::socketEvent() before close()\n");
				close();
				}
			else {
				gg_free_register(h);
				h = NULL;
				kdebug("Unregister::socketEvent(): error unregistering\n");
				status->setText(i18n("Error"));
				setEnabled(true);
				}
			break;
		default:
			if (h->check & GG_CHECK_WRITE)
				snw->setEnabled(true);
		}
}

void Unregister::deleteConfig() {
	kdebug("Unregister::deleteConfig()\n");

	kdebug("Unregister::deleteConfig(): Deleting config file...\n");
	QFile::remove(ggPath("kadu.conf"));
	config.uin = 0;

	kadu->setCaption(i18n("No user"));

	kdebug("Unregister::deleteConfig(): Config file deleted\n");
}

