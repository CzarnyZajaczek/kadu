/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qcursor.h>
#include <qdragobject.h>
#include <qfontmetrics.h>
#include <qpen.h>
#include <qpopupmenu.h>
#include <qregexp.h>
#include <qspinbox.h>

#include "config_dialog.h"
#include "config_file.h"
#include "debug.h"
#include "ignore.h"
#include "misc.h"
#include "pending_msgs.h"
#include "status.h"
#include "userbox.h"
#include "kadu.h"

QFontMetrics* KaduListBoxPixmap::descriptionFontMetrics=NULL;
UinType KaduListBoxPixmap::myUIN;
bool KaduListBoxPixmap::ShowDesc;
bool KaduListBoxPixmap::AlignUserboxIconsTop;
bool KaduListBoxPixmap::ShowMultilineDesc;
bool KaduListBoxPixmap::MultiColumn;
int  KaduListBoxPixmap::MultiColumnWidth;
QColor KaduListBoxPixmap::descColor;

void KaduListBoxPixmap::setFont(const QFont &f)
{
	kdebugf();
	QFont newFont = QFont(f);
	newFont.setPointSize(f.pointSize() - 2);
	if (descriptionFontMetrics)
		delete descriptionFontMetrics;
	descriptionFontMetrics= new QFontMetrics(newFont);
	kdebugf2();
}

KaduListBoxPixmap::KaduListBoxPixmap(const QPixmap &pix, const QString &text)
	: QListBoxItem()
{
	buf_width=-1;
	pm = pix;
	setText(text);
}

KaduListBoxPixmap::KaduListBoxPixmap(const QPixmap &pix, const QString &text, const QString &descr, bool bold)
	: QListBoxItem()
{
	buf_width=-1;
	pm = pix;
	setText(text);
	setDescription(descr);
	setBold(bold);
}

void KaduListBoxPixmap::setMyUIN(UinType u)
{
	myUIN=u;
}

void KaduListBoxPixmap::setShowDesc(bool sd)
{
	ShowDesc=sd;
}

void KaduListBoxPixmap::setAlignTop(bool at)
{
	AlignUserboxIconsTop=at;
}

void KaduListBoxPixmap::setShowMultilineDesc(bool m)
{
	ShowMultilineDesc=m;
}

void KaduListBoxPixmap::setMultiColumn(bool m)
{
	MultiColumn=m;
}

void KaduListBoxPixmap::setMultiColumnWidth(int w)
{
	MultiColumnWidth=w;
}

void KaduListBoxPixmap::setDescriptionColor(const QColor &col)
{
	descColor = col;
}

void KaduListBoxPixmap::paint(QPainter *painter)
{
//	kdebugf();
	UserListElement &user = userlist.byAltNick(text());
//	kdebugm(KDEBUG_INFO, "%d\n", (int)&user);
	bool isMyUin=(myUIN == user.uin());
	QColor origColor = painter->pen().color();
	if (user.uin())
	{
		UinsList uins(user.uin());
		if (user.blocking())
			painter->setPen(QColor(255, 0, 0));
		else if (isIgnored(uins))
			painter->setPen(QColor(192, 192, 0));
		else if (user.offlineTo())
			painter->setPen(QColor(128, 128, 128));
	}

	int itemHeight = AlignUserboxIconsTop ? lineHeight(listBox()):height(listBox());
	int yPos;
	QString descr=isMyUin ? gadu->status().description() : description();
	bool hasDescription=isMyUin ? gadu->status().hasDescription() : !descr.isEmpty();

	if (!pm.isNull())
	{
		yPos = (itemHeight - pm.height()) / 2;
		painter->drawPixmap(3, yPos, pm);
	}

	if (!text().isEmpty())
	{
		QFont oldFont = painter->font();

		if (bold)
		{
			QFont newFont = QFont(oldFont);
			newFont.setWeight(QFont::Bold);
			painter->setFont(newFont);
		}

		QFontMetrics fm = painter->fontMetrics();

		if (ShowDesc && hasDescription)
			yPos = fm.ascent() + 1;
		else
			yPos = ((itemHeight - fm.height()) / 2) + fm.ascent();

		painter->drawText(pm.width() + 5, yPos, text());

		if (bold)
			painter->setFont(oldFont);

//		kdebugmf(KDEBUG_INFO, "isMyUin = %d, own_description = %s\n",
//			isMyUin, (const char *)unicode2latin(own_description));
		if (ShowDesc && hasDescription)
		{
			yPos += fm.height() - fm.descent();

			QFont newFont = QFont(oldFont);
			newFont.setPointSize(oldFont.pointSize() - 2);
			painter->setFont(newFont);

			if (!ShowMultilineDesc)
#if QT_VERSION < 0x030100
				descr.replace(QRegExp("\n"), " ");
#else
				descr.replace("\n", " ");
#endif
			int h;
			QStringList out;
			calculateSize(descr, width(listBox())-5-pm.width(), out, h);
			if (!out.empty() && !isSelected())
				painter->setPen(descColor);
			else
				painter->setPen(origColor);
			CONST_FOREACH(text, out)
			{
				painter->drawText(pm.width() + 5, yPos, *text);
				yPos += descriptionFontMetrics->lineSpacing();
			}

			painter->setFont(oldFont);
		}
	}
//	kdebugf2();
}

int KaduListBoxPixmap::height(const QListBox* lb) const
{
//	kdebugf();
	UserListElement &user = userlist.byAltNick(text());
	bool isMyUin=(myUIN == user.uin());
	QString descr=isMyUin ? gadu->status().description() : description();
	bool hasDescription=isMyUin ? gadu->status().hasDescription() : !descr.isEmpty();

	int height=lb->fontMetrics().lineSpacing()+3;
	if (hasDescription && ShowDesc)
	{
		if (!ShowMultilineDesc)
#if QT_VERSION < 0x030100
			descr.replace(QRegExp("\n"), " ");
#else
			descr.replace("\n", " ");
#endif
		QStringList out;
		int h;
		calculateSize(descr, width(lb)-5-pm.width(), out, h);
		height+=h;
	}
//	kdebugf2();
	return QMAX(pm.height(), height);
}

int KaduListBoxPixmap::lineHeight(const QListBox* lb) const
{
	int height=lb->fontMetrics().lineSpacing()+3;
	return QMAX(pm.height(), height);
}


int KaduListBoxPixmap::width(const QListBox* lb) const
{
//	kdebugf();
	if (MultiColumn)
		return MultiColumnWidth;
	else
		return QMAX(pm.width(), lb->width()-20);
/*
   joi:
   trzeba da� lb->width()-20, �eby zmie�ci� si� suwak - nie mo�na pobra� jego
   szeroko�ci ze stylu, bo np Keramik zwraca b��dn� warto�� (=100)  (ze standardowych
   styl�w KDE tylko SGI ma szeroko�� >20 i na nim kadu b�dzie kiepsko wygl�da�:( )

   nie mo�na te� wzi�� lb->visibleWidth(), bo gdy w jednej zak�adce mamy tyle kontakt�w,
   �e si� nie mieszcz� na 1 ekranie, a w drugiej tyle �e si� mieszcz�, to przy
   prze��czaniu si� z jednej zak�adki do drugiej dostajemy poziomy suwak... :|
*/
}

//#include <sys/time.h>
void KaduListBoxPixmap::calculateSize(const QString &text, int width, QStringList &out, int &height) const
{
//	kdebugf();
	if (text==buf_text && width==buf_width)	//ju� to liczyli�my ;)
	{
		out=buf_out;
		height=buf_height;
		return;
	}

	int tmplen;

	out.clear();
	height=0;

/*	struct timeval t1,t2;
	gettimeofday(&t1, NULL);
	for(int j=0; j<1000; ++j)
	{
		out.clear();
		height=0;
*/
	QStringList tmpout=QStringList::split('\n', text, true);
	int wsize=descriptionFontMetrics->width('W'); //zdaje si� najszersza litera to 'w'
	int initialLength=width/wsize; // spr�bujmy przewidzie� szeroko��

	if (initialLength<1) //sytuacja tragiczna: nie uda�o si� ani jednego znaku zmie�ci�
	{
		kdebugm(KDEBUG_WARNING, "no space for description!\n");
		height=0;
		out=QStringList();
		return;
	}

	while (!tmpout.isEmpty())
	{
		QString curtext=tmpout.front();
		int textlen=curtext.length();
		int len=initialLength;
		bool tooWide=false;
		while (1)
		{
			tooWide=(descriptionFontMetrics->width(curtext.left(len))>=width);
			if (!tooWide && len<textlen)
				len+=5; //przesuwamy si� po 5 znak�w �eby by�o szybciej
			else
				break;
		}
		if (tooWide) //przekroczyli�my szeroko�� listy
		{
			while(descriptionFontMetrics->width(curtext.left(len))>=width) //skracamy �eby w og�le si� zmie�ci�
				--len;
			tmplen=len;
			while (len>0 && !curtext[len-1].isSpace()) //nie b�d�my chamy i dzielmy opisy na granicach s��w
				--len;
			if (len==0)//no ale mo�e kto� wpisa� bez spacji?
				len=tmplen-1;
		}
		if (len<1)
		{
			kdebugm(KDEBUG_WARNING, "no space for description ! (2)\n");
			height=0;
			out=QStringList();
			return;
		}
		QString next=curtext.mid(len);//przenosimy do nast�pnego wiersza
		out.push_back(curtext.left(len));
		tmpout.pop_front();
		++height;
		if (tooWide)
		{
			if (next[0].isSpace())//je�eli obcinamy na bia�ym znaku, to w nast�pnej linii ten znak nie jest potrzebny
				tmpout.push_front(next.mid(1));
			else
				tmpout.push_front(next);
		}
	}

/*	}
	gettimeofday(&t2, NULL);
	kdebugm(KDEBUG_INFO, "czas: %ld\n", (t2.tv_usec-t1.tv_usec)+(t2.tv_sec*1000000)-(t1.tv_sec*1000000));
*/
	height*=descriptionFontMetrics->lineSpacing();

	buf_text=text;
	buf_width=width;
	buf_out=out;
	buf_height=height;
//	kdebugm(KDEBUG_DUMP, "h:%d txt:%s\n", height, text.local8Bit().data());
//	for(QStringList::Iterator it = out.begin(); it != out.end(); ++it )
//		kdebugm(KDEBUG_DUMP, ">>%s\n", (*it).local8Bit().data());
}

void KaduListBoxPixmap::changeText(const QString &text)
{
	setText(text);
}

UserBoxMenu *UserBox::userboxmenu = NULL;

UserBox::UserBox(QWidget* parent,const char* name,WFlags f)
	: QListBox(parent, name, f),QToolTip(viewport())

{
	kdebugf();

	if (!userboxmenu)
		userboxmenu= new UserBoxMenu(this);
	UserBoxes.append(this);

	if (config_file.readBoolEntry("Look", "MultiColumnUserbox"))
		setColumnMode(QListBox::FitToWidth);
	setPaletteBackgroundColor(config_file.readColorEntry("Look", "UserboxBgColor"));
	setPaletteForegroundColor(config_file.readColorEntry("Look", "UserboxFgColor"));
	QListBox::setFont(config_file.readFontEntry("Look", "UserboxFont"));
	setMinimumWidth(20);
	setSelectionMode(QListBox::Extended);
	connect(this, SIGNAL(doubleClicked(QListBoxItem *)), this, SLOT(doubleClickedSlot(QListBoxItem *)));
	connect(this, SIGNAL(returnPressed(QListBoxItem *)), this, SLOT(returnPressedSlot(QListBoxItem *)));

	kdebugf2();
}

UserBox::~UserBox()
{
	kdebugf();
	UserBoxes.remove(this);
	kdebugf2();
}

void UserBox::maybeTip(const QPoint &c)
{
	kdebugf();
	if (!config_file.readBoolEntry("General", "ShowTooltipOnUserbox"))
		return;

	QListBoxItem* item = static_cast<QListBoxItem*>(itemAt(c));
	if(item)
	{
		QRect r(itemRect(item));
		QString s;
		const UserStatus & status = userlist.byAltNick(item->text()).status();
		QString description = status.description();
		QString name = qApp->translate("@default", UserStatus::name(UserStatus::index(status.status(), false)));

		if (description.isEmpty())
		{
			if (status.isOffline() && !userlist.byAltNick(item->text()).uin())
				s = tr("<i>Mobile:</i> <b>%1</b>").arg(userlist.byAltNick(item->text()).mobile());
			else
				s = tr("<nobr><i>%1</i></nobr>").arg(name);
		}
		else
		{
			HtmlDocument::escapeText(description);
			description.replace(QRegExp(" "), "&nbsp;");
			description.replace(QRegExp("\n"), "<br/>");

			s = narg(tr("<nobr><i>%1</i> <b>(d.)</b></nobr><br/><br/><b>Description:</b><br/>%2"), name, description);
		}
		tip(r, s);
	}
	kdebugf2();
}

void UserBox::mousePressEvent(QMouseEvent *e)
{
	kdebugf();
	if (e->button() != RightButton)
		QListBox::mousePressEvent(e);
	else
	{
		QListBoxItem *item = itemAt(e->pos());
		if (item)
		{
			if (!item->isSelected())
				if (!(e->state() & Qt::ControlButton))
					for (unsigned int i = 0, count2 = count(); i < count2; ++i)
						setSelected(i, FALSE);
			setSelected(item, TRUE);
			setCurrentItem(item);
			emit rightButtonPressed(item, e->globalPos());
		}
	}
	kdebugf2();
}

void UserBox::mouseMoveEvent(QMouseEvent* e)
{
//	kdebugf();
	if ((e->state() & LeftButton)&&itemAt(e->pos()))
	{
		QString drag_text;
		for(unsigned int i = 0, count1 = count(); i < count1; ++i)
			if (isSelected(i))
			{
				if (!drag_text.isEmpty())
					drag_text += "\n";
				drag_text += item(i)->text();
			}
		QDragObject* d = new QTextDrag(drag_text,this);
		d->dragCopy();
	}
	else
		QListBox::mouseMoveEvent(e);
//	kdebugf2();
}

void UserBox::keyPressEvent(QKeyEvent *e)
{
//	kdebugf();
	QListBox::keyPressEvent(e);
	QWidget::keyPressEvent(e);
	emit currentChanged(item(currentItem()));
//	kdebugf2();
}

void UserBox::sortUsersByAltNick(QStringList &users)
{
	stringHeapSort(users);
}

void UserBox::refresh()
{
	kdebugf();
	KaduListBoxPixmap *lbp;

	// Zapami�tujemy zaznaczonych u�ytkownik�w
	QStringList s_users;
	for (unsigned int i = 0, count2 = count(); i < count2; ++i)
		if (isSelected(i))
			s_users.append(item(i)->text());
	QString s_user = currentText();

	//zapami�tajmy po�o�enie pionowego suwaka
	int vScrollValue = verticalScrollBar()->value();

	// Najpierw dzielimy uzytkownikow na cztery grupy
	QStringList a_users;
	QStringList i_users;
	QStringList n_users;
	QStringList b_users;

	UinType myUin = config_file.readNumEntry("General", "UIN");
	CONST_FOREACH(username, Users)
	{
		UserListElement &user = userlist.byAltNick(*username);
		if (user.uin())
		{
			if (user.uin() == myUin)
			{
				user.status().setStatus(gadu->status());
				user.setMaxImageSize(config_file.readNumEntry("Chat", "MaxImageSize"));
			}
			switch (user.status().status())
			{
				case Offline:
					n_users.append(user.altNick());
					break;
				case Invisible:
					i_users.append(user.altNick());
					break;
				default:
					a_users.append(user.altNick());
			}
		}
		else
			b_users.append(user.altNick());//bez uin�w
	}
	sortUsersByAltNick(a_users);
	sortUsersByAltNick(i_users);
	sortUsersByAltNick(n_users);
	sortUsersByAltNick(b_users);
	// Czyscimy list�
	clear();

	bool showBold = config_file.readBoolEntry("Look", "ShowBold");
	bool showOnlyDesc = config_file.readBoolEntry("General", "ShowOnlyDescriptionUsers");
	bool showBlocking = config_file.readBoolEntry("General", "ShowBlocking");
	bool showBlocked = config_file.readBoolEntry("General", "ShowBlocked");

	// Dodajemy aktywnych
	CONST_FOREACH(username, a_users)
	{
		UserListElement &user = userlist.byAltNick(*username);
		if (user.blocking() && !showBlocked)
			continue;
		if (user.status().isBlocking() && !showBlocking)
			continue;

		if (!showOnlyDesc || user.status().hasDescription())
		{
			bool has_mobile = !user.mobile().isEmpty();
			bool bold = showBold ? (user.status().isOnline() || user.status().isBusy()) : false;
			if (pending.pendingMsgs(user.uin()))
				lbp = new KaduListBoxPixmap(icons_manager.loadIcon("Message"), user.altNick(),
					user.status().description(), bold);
			else
			{
				const QPixmap &pix = user.status().pixmap(has_mobile);
				if (!pix.isNull())
					lbp = new KaduListBoxPixmap(pix, user.altNick(), user.status().description(), bold);
				else
					lbp = new KaduListBoxPixmap(icons_manager.loadIcon("Online"), user.altNick(),
						user.status().description(), bold);
			}
			insertItem(lbp);
		}
	}

	// Dodajemy niewidocznych
	CONST_FOREACH(username, i_users)
	{
		UserListElement &user = userlist.byAltNick(*username);
		if (user.blocking() && !showBlocked)
			continue;

		if (!showOnlyDesc || user.status().hasDescription())
		{
			bool has_mobile = !user.mobile().isEmpty();
			if (pending.pendingMsgs(user.uin()))
				lbp = new KaduListBoxPixmap(icons_manager.loadIcon("Message"), user.altNick(),
					user.status().description(), 0);
			else
			{
				const QPixmap &pix = user.status().pixmap(has_mobile);
				if (!pix.isNull())
					lbp = new KaduListBoxPixmap(pix, user.altNick(), user.status().description(), 0);
				else
					lbp = new KaduListBoxPixmap(icons_manager.loadIcon("Invisible"), user.altNick(),
						user.status().description(), 0);
			}
			insertItem(lbp);
		}
	}

	// Dodajemy nieaktywnych
	if (config_file.readBoolEntry("General","ShowHideInactive"))
	CONST_FOREACH(username, n_users)
	{
		UserListElement &user = userlist.byAltNick(*username);
		if (user.blocking() && !showBlocked)
			continue;

		if (!showOnlyDesc || user.status().hasDescription())
		{
			bool has_mobile = !user.mobile().isEmpty();
			if (pending.pendingMsgs(user.uin()))
				lbp = new KaduListBoxPixmap(icons_manager.loadIcon("Message"), user.altNick(),
					user.status().description(), 0);
			else
			{
				const QPixmap &pix = user.status().pixmap(has_mobile);
				if (!pix.isNull())
					lbp = new KaduListBoxPixmap(pix, user.altNick(), user.status().description(), 0);
				else
					lbp = new KaduListBoxPixmap(icons_manager.loadIcon("Offline"), user.altNick(),
						user.status().description(), 0);
			}
			insertItem(lbp);
		}
	}

	// Dodajemy uzytkownikow bez numerow GG
	if(!showOnlyDesc)
		CONST_FOREACH(username, b_users)
		{
			UserListElement &user = userlist.byAltNick(*username);
			lbp = new KaduListBoxPixmap(icons_manager.loadIcon("Mobile"), user.altNick(),
				user.status().description(), 0);
			insertItem(lbp);
		}

	// Przywracamy zaznaczenie wczesniej zaznaczonych uzytkownikow
	CONST_FOREACH(username, s_users)
		setSelected(findItem(*username), true);
	setCurrentItem(findItem(s_user));

	//przywracamy po�o�enie pionowego suwaka
	verticalScrollBar()->setValue(vScrollValue);

	kdebugf2();
}

void UserBox::addUser(const QString &altnick)
{
	kdebugf();
	Users.append(altnick);
}

void UserBox::removeUser(const QString &altnick)
{
	kdebugf();
	Users.remove(altnick);
}

void UserBox::renameUser(const QString &oldaltnick, const QString &newaltnick)
{
	kdebugf();
	QStringList::iterator it = Users.find(oldaltnick);
	if (it != Users.end())
	{
		(*it) = newaltnick;
		KaduListBoxPixmap *pix=(KaduListBoxPixmap*)findItem(oldaltnick, Qt::ExactMatch|Qt::CaseSensitive);
		if (pix)
			pix->changeText(newaltnick);
	}
	else
		kdebugmf(KDEBUG_WARNING, "userbox does not contain: %s\n", oldaltnick.local8Bit().data());
	kdebugf2();
}

bool UserBox::containsAltNick(const QString &altnick) const
{
	kdebugf();
	CONST_FOREACH(username, Users)
		if ((*username).lower() == altnick.lower())
			return true;
	kdebugmf(KDEBUG_INFO, "userbox does not contain: %s\n", altnick.lower().local8Bit().data());
	return false;
}

void UserBox::changeAllToInactive()
{
	kdebugf();
	QPixmap qp_inact = icons_manager.loadIcon("Offline");
	for(unsigned int i = 0, count2 = count(); i < count2; ++i)
		changeItem(qp_inact, item(i)->text(), i);
	kdebugf2();
}

void UserBox::showHideInactive()
{
	kdebugf();
	config_file.writeEntry("General","ShowHideInactive",!config_file.readBoolEntry("General","ShowHideInactive"));
	all_refresh();
	kdebugf2();
}

void UserBox::showHideDescriptions()
{
	kdebugf();
	config_file.writeEntry("General","ShowOnlyDescriptionUsers",!config_file.readBoolEntry("General","ShowOnlyDescriptionUsers"));
	all_refresh();
}

UinsList UserBox::getSelectedUins() const
{
	kdebugf();
	UinsList uins;
	for (unsigned int i = 0, count2 = count(); i < count2; ++i)
		if (isSelected(i))
		{
			UserListElement &user = userlist.byAltNick(text(i));
			if (user.uin())
				uins.append(user.uin());
		}
	kdebugf2();
	return uins;
}

UserList UserBox::getSelectedUsers() const
{
	kdebugf();
	UserList users;
	for (unsigned int i = 0, count2 = count(); i < count2; ++i)
		if (isSelected(i))
			users.addUser(userlist.byAltNick(text(i)));
	kdebugf2();
	return users;
}

UserBox* UserBox::getActiveUserBox()
{
	kdebugf();
	FOREACH(box, UserBoxes)
	{
		if ((*box)->isActiveWindow())
		{
			kdebugf2();
			return *box;
		}
	}
	kdebugmf(KDEBUG_PANIC, "return NULL!\n");
	return NULL;
}

QStringList UserBox::getSelectedAltNicks() const
{
	kdebugf();
	QStringList nicks;
	for (unsigned int i = 0, count2 = count(); i < count2; ++i)
		if (isSelected(i))
			nicks.append(text(i));
	kdebugf2();
	return nicks;
}
/////////////////////////////////////////////////////////

void UserBox::all_refresh()
{
	kdebugf();
	FOREACH(box, UserBoxes)
		(*box)->refresh();
}

void UserBox::all_removeUser(const QString &altnick)
{
	kdebugf();
	FOREACH(box, UserBoxes)
		(*box)->removeUser(altnick);
	kdebugf2();
}

void UserBox::all_changeAllToInactive()
{
	kdebugf();
	FOREACH(box, UserBoxes)
		(*box)->changeAllToInactive();
	kdebugf2();
}

void UserBox::all_renameUser(const QString &oldaltnick, const QString &newaltnick)
{
	kdebugf();
	FOREACH(box, UserBoxes)
		(*box)->renameUser(oldaltnick, newaltnick);
	kdebugf2();
}

void UserBox::initModule()
{
	kdebugf();
	ConfigDialog::addTab(QT_TRANSLATE_NOOP("@default", "General"), "GeneralTab");
	ConfigDialog::addCheckBox("General", "grid", QT_TRANSLATE_NOOP("@default", "Show inactive contacts"), "ShowHideInactive", true, QT_TRANSLATE_NOOP("@default", "Display contacts who are offline"), "", Advanced);
	ConfigDialog::addCheckBox("General", "grid", QT_TRANSLATE_NOOP("@default", "Show contacts with description"), "ShowOnlyDescriptionUsers", false, QT_TRANSLATE_NOOP("@default", "Display contacts that have a desciption"), "", Advanced);
	ConfigDialog::addCheckBox("General", "grid", QT_TRANSLATE_NOOP("@default", "Show contacts that you are blocking"), "ShowBlocked", true, "", "", Expert);
	ConfigDialog::addCheckBox("General", "grid", QT_TRANSLATE_NOOP("@default", "Show contacts that are blocking you"), "ShowBlocking", true, "", "", Expert);

	// dodanie wpisow do konfiga (pierwsze uruchomienie)
	QWidget w;
	config_file.addVariable("Look", "InfoPanelBgColor", w.paletteBackgroundColor());
	config_file.addVariable("Look", "InfoPanelFgColor", w.paletteForegroundColor());
	config_file.addVariable("Look", "UserboxBgColor", w.paletteBackgroundColor());
	config_file.addVariable("Look", "UserboxFgColor", w.paletteForegroundColor());
	config_file.addVariable("Look", "AlignUserboxIconsTop", false);
	config_file.addVariable("Look", "DescriptionColor", w.paletteForegroundColor());

	if (config_file.readEntry("Look", "MultiColumnUserboxWidth").isEmpty())//operacje na czcionkach s� powolne, wystrzegamy si� ich kiedy mozemy
		config_file.addVariable("Look", "MultiColumnUserboxWidth", int(QFontMetrics(*defaultFont).width("Imie i Nazwisko")*1.5));

	ConfigDialog::addTab(QT_TRANSLATE_NOOP("@default", "Look"), "LookTab");

	ConfigDialog::addCheckBox("Look", "varOpts", QT_TRANSLATE_NOOP("@default", "Align icon next to contact name"), "AlignUserboxIconsTop", config_file.readBoolEntry("Look", "AlignUserboxIconsTop"), "", "", Advanced);

	ConfigDialog::addVGroupBox("Look", "varOpts2", QT_TRANSLATE_NOOP("@default", "Columns"), "", Advanced);
	ConfigDialog::addCheckBox("Look", "Columns", QT_TRANSLATE_NOOP("@default", "Multicolumn userbox"), "MultiColumnUserbox", false);
	ConfigDialog::addSpinBox("Look", "Columns", QT_TRANSLATE_NOOP("@default", "Userbox width when multi column"), "MultiColumnUserboxWidth", 1, 1000, 1);

	ConfigDialog::addVGroupBox("Look", "Look", QT_TRANSLATE_NOOP("@default", "Colors"), "", Advanced);
		ConfigDialog::addVGroupBox("Look", "Colors", QT_TRANSLATE_NOOP("@default", "Main window"));
			ConfigDialog::addColorButton("Look", "Main window", QT_TRANSLATE_NOOP("@default", "Userbox background color"), "UserboxBgColor", config_file.readColorEntry("Look","UserboxBgColor"), "", "userbox_bg_color");
			ConfigDialog::addColorButton("Look", "Main window", QT_TRANSLATE_NOOP("@default", "Userbox font color"), "UserboxFgColor", config_file.readColorEntry("Look","UserboxFgColor"), "", "userbox_font_color");
			ConfigDialog::addColorButton("Look", "Main window", QT_TRANSLATE_NOOP("@default", "Description font color"), "DescriptionColor", config_file.readColorEntry("Look","DescriptionColor"), "", "userbox_desc_color");

	ConfigDialog::addVGroupBox("Look", "Look", QT_TRANSLATE_NOOP("@default", "Fonts"), "", Advanced);
		ConfigDialog::addSelectFont("Look", "Fonts", QT_TRANSLATE_NOOP("@default", "Font in userbox"), "UserboxFont", defaultFont->toString(), "", "userbox_font_box");

	ConfigDialog::addVGroupBox("Look", "Look", QT_TRANSLATE_NOOP("@default", "Previews"), "", Advanced);
		ConfigDialog::addHBox("Look", "Previews", "othr_prvws");
			ConfigDialog::addVGroupBox("Look", "othr_prvws", QT_TRANSLATE_NOOP("@default", "Preview userbox"));
				ConfigDialog::addLabel("Look", "Preview userbox", "<b>Text</b> preview", "preview_userbox");
			ConfigDialog::addVGroupBox("Look", "othr_prvws", QT_TRANSLATE_NOOP("@default", "Preview panel"));
				ConfigDialog::addLabel("Look", "Preview panel", "<b>Text</b> preview", "preview_panel");

	ConfigDialog::addCheckBox("General", "grid", QT_TRANSLATE_NOOP("@default", "Show tooltip on userbox"), "ShowTooltipOnUserbox", true, "", "", Expert);

	KaduListBoxPixmap::setFont(config_file.readFontEntry("Look","UserboxFont"));
	KaduListBoxPixmap::setShowDesc(config_file.readBoolEntry("Look", "ShowDesc"));
	KaduListBoxPixmap::setAlignTop(config_file.readBoolEntry("Look", "AlignUserboxIconsTop"));
	KaduListBoxPixmap::setShowMultilineDesc(config_file.readBoolEntry("Look", "ShowMultilineDesc"));
	KaduListBoxPixmap::setMultiColumn(config_file.readBoolEntry("Look", "MultiColumnUserbox"));
	KaduListBoxPixmap::setMultiColumnWidth(config_file.readNumEntry("Look", "MultiColumnUserboxWidth", 230));
	KaduListBoxPixmap::setMyUIN(config_file.readNumEntry("General", "UIN"));
	KaduListBoxPixmap::setDescriptionColor(config_file.readColorEntry("Look", "DescriptionColor"));

	UserBoxSlots *userboxslots= new UserBoxSlots();
	ConfigDialog::registerSlotOnCreate(userboxslots, SLOT(onCreateConfigDialog()));
	ConfigDialog::registerSlotOnApply(userboxslots, SLOT(onDestroyConfigDialog()));

	ConfigDialog::connectSlot("Look", "Userbox background color", SIGNAL(changed(const char *, const QColor&)), userboxslots, SLOT(chooseColor(const char *, const QColor&)), "userbox_bg_color");
	ConfigDialog::connectSlot("Look", "Userbox font color", SIGNAL(changed(const char *, const QColor&)), userboxslots, SLOT(chooseColor(const char *, const QColor&)), "userbox_font_color");

	ConfigDialog::connectSlot("Look", "Font in userbox", SIGNAL(changed(const char *, const QFont&)), userboxslots, SLOT(chooseFont(const char *, const QFont&)), "userbox_font_box");

	ConfigDialog::connectSlot("Look", "Multicolumn userbox", SIGNAL(toggled(bool)), userboxslots, SLOT(onMultiColumnUserbox(bool)));

	kdebugf2();
}

void UserBox::resizeEvent(QResizeEvent *r)
{
//	kdebugf();
	QListBox::resizeEvent(r);
	refresh();
}

void UserBox::doubleClickedSlot(QListBoxItem *item)
{
	emit doubleClicked(item->text());
}

void UserBox::returnPressedSlot(QListBoxItem *item)
{
	emit returnPressed(item->text());
}

UserBoxMenu::UserBoxMenu(QWidget *parent, const char *name): QPopupMenu(parent, name)
{
	connect(this, SIGNAL(aboutToHide()), this, SLOT(restoreLook()));
}

int UserBoxMenu::addItem(const QString &text, const QObject* receiver, const char* member, const QKeySequence accel, int id)
{
	return insertItem(text, receiver, member, accel, id);
}

int UserBoxMenu::addItem(const QString &iconname, const QString &text, const QObject* receiver, const char* member, const QKeySequence accel, int id)
{
	iconNames.append(qMakePair(text,iconname));
	return insertItem( QIconSet(icons_manager.loadIcon(iconname)) , text, receiver, member, accel, id);
}

int UserBoxMenu::addItemAtPos(int index,const QString &iconname, const QString &text, const QObject* receiver, const char* member, const QKeySequence accel, int id)
{
	iconNames.append(qMakePair(text,iconname));
	return insertItem( QIconSet(icons_manager.loadIcon(iconname)) , text, receiver, member, accel, id, index);
}

int UserBoxMenu::getItem(const QString &caption) const
{
	for (unsigned int i = 0, count2 = count(); i < count2; ++i)
		if (!QString::localeAwareCompare(caption, text(idAt(i)).left(caption.length())))
			return idAt(i);
	return -1;
}

void UserBoxMenu::restoreLook()
{
	for (unsigned int i = 0, count2 = count(); i < count2; ++i)
	{
		setItemEnabled(idAt(i), true);
		setItemChecked(idAt(i), false);
//		setItemVisible(idAt(i),true);
// nie ma takiej funkcji w qt 3.0.*
	}
}

void UserBoxMenu::show(QListBoxItem *item)
{
	if (item == NULL)
		return;

	emit popup();
	exec(QCursor::pos());
}

void UserBoxMenu::refreshIcons()
{
	kdebugf();
	for (unsigned int i = 0, count2 = count(); i < count2; ++i)
	{
		int id = idAt(i);
		QString t = text(id);

		CONST_FOREACH(it, iconNames)
			if (t.startsWith((*it).first))
			{
				bool enabled=isItemEnabled(id);
				bool checked=isItemChecked(id);
				changeItem(id, icons_manager.loadIcon((*it).second), t);
				setItemEnabled(id, enabled);
				setItemChecked(id, checked);
			}
	}
	kdebugf2();
}

void UserBoxSlots::onCreateConfigDialog()
{
	kdebugf();

	QSpinBox *multi=ConfigDialog::getSpinBox("Look", "Userbox width when multi column");
	multi->setSuffix(" px");
	multi->setEnabled(config_file.readBoolEntry("Look", "MultiColumnUserbox"));

	updatePreview();
	kdebugf2();
}

void UserBoxSlots::onDestroyConfigDialog()
{
	kdebugf();
	UserBox *userbox = kadu->userbox();

	if (config_file.readBoolEntry("Look", "MultiColumnUserbox"))
		userbox->setColumnMode(QListBox::FitToWidth);
	else
		userbox->setColumnMode(1);

	userbox->setPaletteBackgroundColor(config_file.readColorEntry("Look", "UserboxBgColor"));
	userbox->setPaletteForegroundColor(config_file.readColorEntry("Look", "UserboxFgColor"));
	userbox->QListBox::setFont(config_file.readFontEntry("Look", "UserboxFont"));

	KaduListBoxPixmap::setFont(config_file.readFontEntry("Look","UserboxFont"));
	KaduListBoxPixmap::setShowDesc(config_file.readBoolEntry("Look", "ShowDesc"));
	KaduListBoxPixmap::setAlignTop(config_file.readBoolEntry("Look", "AlignUserboxIconsTop"));
	KaduListBoxPixmap::setShowMultilineDesc(config_file.readBoolEntry("Look", "ShowMultilineDesc"));
	KaduListBoxPixmap::setMultiColumn(config_file.readBoolEntry("Look", "MultiColumnUserbox"));
	KaduListBoxPixmap::setMultiColumnWidth(config_file.readNumEntry("Look", "MultiColumnUserboxWidth", 230));
	KaduListBoxPixmap::setMyUIN(config_file.readNumEntry("General", "UIN"));
	KaduListBoxPixmap::setDescriptionColor(config_file.readColorEntry("Look", "DescriptionColor"));

	UserBox::all_refresh();
	kdebugf2();
}

void UserBoxSlots::chooseColor(const char *name, const QColor &color)
{
	kdebugf();
	QLabel *preview= ConfigDialog::getLabel("Look", "<b>Text</b> preview", "preview_userbox");
	if (QString(name)=="userbox_bg_color")
		preview->setPaletteBackgroundColor(color);
	else if (QString(name)=="userbox_font_color")
		preview->setPaletteBackgroundColor(color);
	else
		kdebugmf(KDEBUG_ERROR, "label '%s' not known\n", name);
	kdebugf2();
}

void UserBoxSlots::chooseFont(const char *name, const QFont &font)
{
	kdebugf();
	QLabel *preview= ConfigDialog::getLabel("Look", "<b>Text</b> preview", "preview_userbox");
	if (QString(name)=="userbox_font_box")
		preview->setFont(font);
	kdebugf2();
}

void UserBoxSlots::updatePreview()
{
	kdebugf();
	QLabel *preview= ConfigDialog::getLabel("Look", "<b>Text</b> preview", "preview_userbox");
	preview->setFont(config_file.readFontEntry("Look", "UserboxFont"));
	preview->setPaletteForegroundColor(config_file.readColorEntry("Look", "UserboxFgColor"));
	preview->setPaletteBackgroundColor(config_file.readColorEntry("Look", "UserboxBgColor"));
	preview->setAlignment(Qt::AlignLeft);
	kdebugf2();
}

void UserBoxSlots::onMultiColumnUserbox(bool toggled)
{
	ConfigDialog::getSpinBox("Look", "Userbox width when multi column")->setEnabled(toggled);
}

QValueList<UserBox *> UserBox::UserBoxes;
