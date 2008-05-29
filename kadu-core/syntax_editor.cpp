/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QApplication>
#include <QComboBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

#include "config_file.h"
#include "icons_manager.h"
#include "message_box.h"
#include "misc.h"
#include "preview.h"

#include "syntax_editor.h"

SyntaxList::SyntaxList(const QString &category)
	: category(category)
{
	reload();
}

QString SyntaxList::readSyntax(const QString &category, const QString &name, const QString &defaultSyntax)
{
	QString path;
	QFile syntaxFile;
	path = dataPath("kadu") + "/syntax/" + category + "/" + name + ".syntax";

	syntaxFile.setName(path);
	if (!syntaxFile.open(QIODevice::ReadOnly))
	{
		path = ggPath() + "/syntax/" + category + "/" + name + ".syntax";

		syntaxFile.setName(path);
		if (!syntaxFile.open(QIODevice::ReadOnly))
			return defaultSyntax;
	}

	QString result;
	QTextStream stream(&syntaxFile);
	stream.setCodec("UTF-8");
	result = stream.read();
	syntaxFile.close();

	if (result.isEmpty())
		return defaultSyntax;

	return result;
}

void SyntaxList::reload()
{
	QDir dir;
	QString path;
	QFileInfo fi;
	QStringList files;

	SyntaxInfo info;

	info.global = false;
	path = ggPath() + "/syntax/" + category + "/";
	dir.setPath(path);

	dir.setNameFilter("*.syntax");
	files = dir.entryList();

	CONST_FOREACH(file, files)
	{
		fi.setFile(path + *file);
		if (fi.isReadable())
			insert(fi.baseName(), info);
	}

	info.global = true;
	path = dataPath("kadu") + "/syntax/" + category + "/";
	dir.setPath(path);

	files = dir.entryList();

	CONST_FOREACH(file, files)
	{
		fi.setFile(path + *file);
		if (fi.isReadable() && !contains(*file))
			insert(fi.baseName(), info);
	}

	if (count() == 0)
	{
		info.global = false;
		insert("custom", info);

		updateSyntax("custom", "");
	}
}

bool SyntaxList::updateSyntax(const QString &name, const QString &syntax)
{
	QString path = ggPath() + "/syntax/";
	QDir dir(path);
	if (!dir.exists())
		if (!dir.mkdir(path))
			return false;

	path = ggPath() + "/syntax/" + category + "/";
	dir.setPath(path);
	if (!dir.exists())
		if (!dir.mkdir(path))
			return false;

	QFile syntaxFile;
	syntaxFile.setName(path + name + ".syntax");
	if (!syntaxFile.open(QIODevice::WriteOnly))
		return false;

	QTextStream stream(&syntaxFile);
	stream.setCodec("UTF-8");
	stream << syntax;
	syntaxFile.close();

	SyntaxInfo info;
	info.global = false;
	insert(name, info);

	emit updated();

	return true;
}

QString SyntaxList::readSyntax(const QString &name)
{
	if (!contains(name))
		return QString();

	SyntaxInfo info = *(find(name));
	QString path;
	if (info.global)
		path = dataPath("kadu") + "/syntax/" + category + "/" + name + ".syntax";
	else
		path = ggPath() + "/syntax/" + category + "/" + name + ".syntax";

	QFile syntaxFile;
	syntaxFile.setName(path);
	if (!syntaxFile.open(QIODevice::ReadOnly))
		return QString();

	QString result;
	QTextStream stream(&syntaxFile);
	stream.setCodec("UTF-8");
	result = stream.read();
	syntaxFile.close();

	return result;
}

bool SyntaxList::deleteSyntax(const QString &name)
{
	if (!contains(name))
		return false;

	SyntaxInfo info = *(find(name));
	if (info.global)
		return false;

	QString path = ggPath() + "/syntax/" + category + "/" + name + ".syntax";
	QFile file;
	file.setName(path);

	if (!file.remove())
		return false;

	remove(name);
	emit updated();

	return true;
}

bool SyntaxList::isGlobal(const QString &name)
{
	if (!contains(name))
		return false;

	SyntaxInfo info = *(find(name));
	return info.global;
}

SyntaxEditor::SyntaxEditor(QWidget *parent)
	: QWidget(parent), syntaxList(0)
{
	UserStatus status;
	status.setBusy(qApp->translate("@default", "Description"));

	example.addProtocol("Gadu", "999999");
	example.setStatus("Gadu", status);
	example.setFirstName(qApp->translate("@default", "Mark"));
	example.setLastName(qApp->translate("@default", "Smith"));
	example.setNickName(qApp->translate("@default", "Jimbo"));
	example.setAltNick(qApp->translate("@default", "Jimbo"));
	example.setMobile("+48123456789");
	example.setEmail("jimbo@mail.server.net");
	example.setHomePhone("+481234567890");
	example.setAddressAndPort("Gadu", QHostAddress(2130706433), 80);
	example.setDNSName("Gadu", "host.server.net");

	QHBoxLayout *layout = new QHBoxLayout(this);

	syntaxListCombo = new QComboBox(this);
	connect(syntaxListCombo, SIGNAL(activated(const QString &)), this, SLOT(syntaxChangedSlot(const QString &)));

	QPushButton *editButton = new QPushButton(tr("Edit"), this);
	deleteButton = new QPushButton(tr("Delete"), this);
	connect(editButton, SIGNAL(clicked()), this, SLOT(editClicked()));
	connect(deleteButton, SIGNAL(clicked()), this, SLOT(deleteClicked()));

	layout->addWidget(syntaxListCombo, 100);
	layout->addWidget(editButton);
	layout->addWidget(deleteButton);
}

SyntaxEditor::~SyntaxEditor()
{
	if (syntaxList)
	{
		delete syntaxList;
		syntaxList = 0;
	}
}

void SyntaxEditor::setCurrentSyntax(const QString &syntax)
{
	syntaxListCombo->setCurrentText(syntax);
	syntaxChangedSlot(syntax);
}

QString SyntaxEditor::currentSyntax()
{
	return syntaxListCombo->currentText();
}

void SyntaxEditor::setCategory(const QString &category)
{
	this->category = category;
	updateSyntaxList();
}

void SyntaxEditor::setSyntaxHint(const QString &syntaxHint)
{
	this->syntaxHint = syntaxHint;
}

void SyntaxEditor::editClicked()
{
	SyntaxEditorWindow *editor = new SyntaxEditorWindow(syntaxList, syntaxListCombo->currentText(), category, syntaxHint);
	connect(editor, SIGNAL(updated(const QString &)), this, SLOT(setCurrentSyntax(const QString &)));

	emit onSyntaxEditorWindowCreated(editor);
	editor->refreshPreview();
	editor->show();
}

void SyntaxEditor::deleteClicked()
{
	if (!syntaxList)
		return;

	if (syntaxList->deleteSyntax(currentSyntax()))
		setCurrentSyntax(*(syntaxList->keys().begin()));
	else
		MessageBox::msg(tr("Unable to remove syntax: %1").arg(currentSyntax()), true, "Warning");
}

void SyntaxEditor::syntaxChangedSlot(const QString &newSyntax)
{
	if (!syntaxList)
		return;

	if (!syntaxList->contains(newSyntax))
		return;

	QFile file;
	QString fileName;
	QString content;

	SyntaxInfo info = (*syntaxList)[newSyntax];
	if (info.global)
		fileName = dataPath("kadu") + "/syntax/" + category.lower() + "/" + newSyntax + ".syntax";
	else
		fileName = ggPath() + "/syntax/" + category.lower() + "/" + newSyntax + ".syntax";

	file.setName(fileName);
	if (!file.open(QIODevice::ReadOnly))
		return;

	QTextStream stream(&file);
	stream.setCodec("UTF-8");
	content = stream.read();
	file.close();

	content.replace(QRegExp("%o"),  " ");

	deleteButton->setEnabled(!info.global);
	emit syntaxChanged(content);
}

void SyntaxEditor::updateSyntaxList()
{
	if (syntaxList)
		delete syntaxList;
	syntaxList = new SyntaxList(category.lower());

	syntaxListCombo->clear();
	syntaxListCombo->insertStringList(syntaxList->keys());

	connect(syntaxList, SIGNAL(updated()), this, SLOT(syntaxListUpdated()));
}

void SyntaxEditor::syntaxListUpdated()
{
	syntaxListCombo->clear();
	syntaxListCombo->insertStringList(syntaxList->keys());
}

SyntaxEditorWindow::SyntaxEditorWindow(SyntaxList *syntaxList, const QString &syntaxName, const QString &category, const QString &syntaxHint, QWidget* parent)
	: QWidget(parent), syntaxList(syntaxList), syntaxName(syntaxName)
{
	setWindowTitle(tr("Kadu syntax editor"));
	setAttribute(Qt::WA_DeleteOnClose);

	QFrame *syntax = new QFrame();

	QGridLayout *syntax_layout = new QGridLayout(syntax);
	syntax_layout->setColStretch(0, 2);
	syntax_layout->setColStretch(1, 1);
	syntax_layout->setSpacing(5);

	editor = new QTextEdit(syntax);
	editor->setTextFormat(Qt::PlainText);
	editor->setText(syntaxList->readSyntax(syntaxName));

	if (!syntaxHint.isEmpty())
		editor->setToolTip(syntaxHint);

	syntax_layout->addMultiCellWidget(editor, 0, 1, 0, 0);

	previewPanel = new Preview(syntax);
	previewPanel->setResetBackgroundColor(config_file.readEntry("Look", category + "BgColor"));
	syntax_layout->addWidget(previewPanel, 0, 1);

	QPushButton *preview = new QPushButton(tr("Preview"), syntax);
	connect(preview, SIGNAL(clicked()), this, SLOT(refreshPreview()));
	syntax_layout->addWidget(preview, 1, 1);

	QWidget *buttons = new QWidget();
	QHBoxLayout *buttons_layout = new QHBoxLayout;
	buttons->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
	buttons_layout->setSpacing(5);

	(new QWidget(buttons))->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));
	QPushButton *saveSyntax = new QPushButton(icons_manager->loadIcon("OkWindowButton"), tr("Save"), 0);
	QPushButton *saveAsSyntax = new QPushButton(icons_manager->loadIcon("OkWindowButton"), tr("Save as..."), 0);
	QPushButton *cancel = new QPushButton(icons_manager->loadIcon("CloseWindowButton"), tr("Cancel"), 0);

	buttons_layout->addWidget(saveSyntax);
	buttons_layout->addWidget(saveAsSyntax);
	buttons_layout->addWidget(cancel);
	buttons->setLayout(buttons_layout);

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->addWidget(syntax);
	layout->addWidget(buttons);

	if (syntaxList->isGlobal(syntaxName))
		saveSyntax->setDisabled(true);
	else
		connect(saveSyntax, SIGNAL(clicked()), this, SLOT(save()));

	connect(saveAsSyntax, SIGNAL(clicked()), this, SLOT(saveAs()));
	connect(cancel, SIGNAL(clicked()), this, SLOT(close()));

	loadGeometry(this, "Look", "SyntaxEditorGeometry", 0, 30, 790, 480);
}

SyntaxEditorWindow::~SyntaxEditorWindow()
{
// 	saveGeometry(this, "Look", "SyntaxEditorGeometry");
}

void SyntaxEditorWindow::refreshPreview()
{
	QString content = editor->text();
	previewPanel->syntaxChanged(content);
}

void SyntaxEditorWindow::save()
{
	syntaxList->updateSyntax(syntaxName, editor->text());
	emit updated(syntaxName);
	close();
}

void SyntaxEditorWindow::saveAs()
{
	QString newSyntaxName = syntaxName;
	bool ok;

	while (true)
	{
		newSyntaxName = QInputDialog::getText(tr("New syntax name"), tr("Enter new syntax name"), QLineEdit::Normal, newSyntaxName, &ok);
		if (!ok)
			return;

		if (newSyntaxName.isEmpty())
			continue;

		if (newSyntaxName == syntaxName)
			break;

		if (!syntaxList->contains(newSyntaxName))
			break;

		if (syntaxList->isGlobal(newSyntaxName))
		{
			MessageBox::msg(tr("Syntax %1 already exists and cannot be modified").arg(newSyntaxName), true, "Warning");
			continue;
		}
		else
		{
			if (MessageBox::ask(tr("Overwrite %1 syntax?").arg(newSyntaxName)))
				break;
		}
	}

	syntaxList->updateSyntax(newSyntaxName, editor->text());
	emit updated(newSyntaxName);
	close();
}

void SyntaxEditorWindow::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Escape)
	{
		e->accept();
		close();
	}
	else
		QWidget::keyPressEvent(e);
}
