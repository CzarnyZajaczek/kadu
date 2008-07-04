/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SYNTAX_EDITOR_H
#define SYNTAX_EDITOR_H

#include <QtCore/QMap>
#include <QtGui/QWidget>

#include "userlistelement.h"

class QComboBox;
class QLineEdit;
class QPushButton;
class QTextEdit;

class Preview;
class SyntaxEditorWindow;

struct SyntaxInfo
{
	bool global;
};

class SyntaxList : public QObject, public QMap<QString, SyntaxInfo>
{
	Q_OBJECT

	QString category;

public:
	SyntaxList(const QString &category);
	virtual ~SyntaxList() {}

	static QString readSyntax(const QString &category, const QString &name, const QString &defaultSyntax);

	void reload();

	bool updateSyntax(const QString &name, const QString &syntax);
	QString readSyntax(const QString &name);
	bool deleteSyntax(const QString &name);

	bool isGlobal(const QString &name);

signals:
	void updated();

};

class SyntaxEditor : public QWidget
{
	Q_OBJECT

	UserListElement example;

	SyntaxList *syntaxList;
	QComboBox *syntaxListCombo;
	QPushButton *deleteButton;

	QString category;
	QString syntaxHint;

	void updateSyntaxList();

private slots:
	void editClicked();
	void deleteClicked();

	void syntaxChangedSlot(const QString &newSyntax);
	void syntaxListUpdated();

public:
	SyntaxEditor(QWidget *parent = 0);
	virtual ~SyntaxEditor();

	QString currentSyntax();

	void setCategory(const QString &category);
	void setSyntaxHint(const QString &syntaxHint);

public slots:
	void setCurrentSyntax(const QString &syntax);

signals:
	void syntaxChanged(const QString &newSyntax);
	void onSyntaxEditorWindowCreated(SyntaxEditorWindow *syntaxEditorWindow);

};

class SyntaxEditorWindow : public QWidget
{
	Q_OBJECT

	SyntaxList *syntaxList;

	QLineEdit *nameEdit;
	QTextEdit *editor;
	Preview *previewPanel;

	QString category;
	QString syntaxName;

private slots:
	void save();
	void saveAs();

protected:
	virtual void keyPressEvent(QKeyEvent *e);

public:
	SyntaxEditorWindow(SyntaxList *syntaxList, const QString &syntaxName, const QString &category, const QString &syntaxHint, QWidget* parent = 0);
	~SyntaxEditorWindow();

	Preview * preview() { return previewPanel; }

public slots:
	void refreshPreview();

signals:
	void updated(const QString &syntaxName);

};

#endif // SYNTAX_EDITOR
