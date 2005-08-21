#ifndef KADU_TEXT_BROWSER_H
#define KADU_TEXT_BROWSER_H

#include <qfont.h>
#include <qpoint.h>
#include <qstring.h>
#include <qtextbrowser.h>
#include <qtooltip.h>

class QPopupMenu;
class QPainter;
class QMouseEvent;

/**
	Zmodyfikowany QTextBrowser specjalnie na potrzeby Kadu.
	Klikni�cie na linku otwiera ustawion� w konfiguracji przegl�dark�.
	W menu kontekstowym jest dodatkowa opcja "Kopiuj lokacj� odno�nika".
	Dodatkowo poprawka b��du w Qt.
**/
class KaduTextBrowser : public QTextBrowser, QToolTip
{
	Q_OBJECT

	private:
		QString anchor;
		int level;
		/**
			this value stores the current highlighted link
			for use with maybeTip(), or is null
		**/
		QString highlightedlink; 	

	private slots:
		void copyLinkLocation();
		void hyperlinkClicked(const QString& link) const;
		void linkHighlighted(const QString &);
		
	protected:
		QPopupMenu *createPopupMenu(const QPoint &point);
		virtual void drawContents(QPainter * p, int clipx, int clipy, int clipw, int cliph);
		virtual void maybeTip(const QPoint&);
		virtual void contentsMouseReleaseEvent(QMouseEvent * e);

	public:
		KaduTextBrowser(QWidget *parent = 0, const char *name = 0);
		void setSource(const QString &name);
		/**
			Nadpisane dla wyja�nienia wieloznaczno�ci
		**/
		void clear() 
		{ 
			setText("");
			//dla QT<3.2 clear() powodowa�o b��d 
		}
		/**
			Nadpisane dla wyja�nienia wieloznaczno�ci		**/

		void setFont(const QFont& f) 	{ QTextBrowser::setFont(f); }
		void setMargin(int width);

	public slots:
		virtual void copy();

	signals:
		/**
			Dowolny przycisk myszy zosta� zwolniony
			Przekazany zostaje tak�e obiekt, kt�ry wywo�a� akcj� - czyli this.
		**/
		void mouseReleased(QMouseEvent *e, KaduTextBrowser *sender);
};

#endif
