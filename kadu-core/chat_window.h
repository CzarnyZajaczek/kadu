#ifndef KADU_CHAT_WINDOW_H
#define KADU_CHAT_WINDOW_H

#include <qmainwindow.h>
#include <qstring.h>

#include "chat_widget.h"

class ChatWindow : public QMainWindow
{
	Q_OBJECT

private:
	ChatWidget* currentChatWidget;
	QTimer* title_timer;  /*!< zmienna przechowuj�ca czas od ostatniego od�wie�enia tytu�u okna */

	void storeGeometry();
	void restoreGeometry();

	bool showNewMessagesNum;
	bool blinkChatTitle;

private slots:
	void updateTitle();

protected:
	/**
		\fn virtual void closeEvent(QCloseEvent* e)
		Funkcja obs�uguj�ca zamkni�cie okna
	**/
	virtual void closeEvent(QCloseEvent* e);

	/**
		\fn virtual void windowActivationChange(bool oldActive)
		Funkcja steruj�ca mruganiem napisu okna
	**/
	virtual void windowActivationChange(bool oldActive);
	/**
		\fn virtual void keyReleaseEvent(QKeyEvent* e)
		Funkcja obs�uguj�ca zwolnienie przycisku
		\param e wska�nik do obiektu obs�uguj�cego klawisze
	**/
	virtual void mouseReleaseEvent(QMouseEvent *e);

public:
	ChatWindow(QWidget *parent = 0, const char *name = 0);
	virtual ~ChatWindow();

	void setChatWidget(ChatWidget *chatWidget);
	ChatWidget* chatWidget();

public slots:
	void blinkTitle();
	void showNewMessagesNumInTitle();

	/**
		\fn void alertNewMessage()
		Slot informuj�cy o nowej wiadomo�ci
		mruganie je�li okno jest nieaktywne
	**/
	void alertNewMessage();

signals:
	void chatWidgetActivated(ChatWidget *);

};

#endif
