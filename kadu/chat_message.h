#ifndef KADU_CHAT_MESSAGE_H
#define KADU_CHAT_MESSAGE_H

#include <qcolor.h>
#include <qdatetime.h>
#include <qmap.h>
#include <qstring.h>

/**

	Klasa przechowuj�ca informacje o wiadomo�ci, kt�ra ma si� pojawi�
	w oknie Chat.
	\class ChatMessage
	\brief Klasa przechowuj�ca informacje o wiadomo�ci.
**/
class ChatMessage
{
	public:
	QString nick;/*!< nazwa u�ytkownika */
	QDateTime date; /*!< data otrzymania wiadomo�ci */
	QDateTime sdate; /*!< data wys�ania wiadomo�ci */
	QString unformattedMessage; /*!< niesformatowana wiadomo�� */
	bool isMyMessage; /*!< zmienna mowi�ca czy wiadomo�c zosta�a
				napisana przez nas */
	QColor backgroundColor; /*!< t�o wiadomo�ci */
	QColor textColor; /*!< kolor wiadomo�ci */


	QMap<QString, bool> attributes; /*!<
		Mo�e s�u�y� do przechowywania informacji o tym
		czy wiadomo�� by�a potwierdzona, szyfrowana, ...
		W zamy�le ma s�u�y� do okre�lania czy jaki� obrazek
		ma by� dodawany do wiadomo�ci czy nie - jaki obrazek -
		ta informacja b�dzie gdzie indziej
		*/

	//inne atrybuty?
	//QMap<QString, QString> stringAttributes;

	bool needsToBeFormatted;/*!< zmienna mowi�ca czy wiadomo�c powinna
				  by� sformatowana */

	QString message;/*!<
			Sformatowana wiadomo�� (razem z \<p\> lub \<table\>)
			**/


	/**
		\fn ChatMessage(const QString &nick, const QString &unformattedMessage, bool myMessage, QDateTime date, QDateTime sdate=QDateTime())
		Konstruktor ustawiaj�cy nick, niesformatowan� wiadomo��,
		czy wiadomo�� pochodzi od nas, dat� wiadomo�ci,
		dat� nadania wiadomo�ci przez serwer
		\param nick nazwa u�ytkownika
		\param unformattedMessage niesformatowana wiadomo��
		\param myMessage zmienna mowi�ca czy wiadomo�� pochodzi od nas
		\param date data otrzymania wiadomo�ci
		\param sdate data wys�ania wiadomo�ci
	**/
	ChatMessage(const QString &nick, const QString &unformattedMessage, bool myMessage, QDateTime date, QDateTime sdate=QDateTime());
	/**
		\fn ChatMessage(const QString &formattedMessage, const QColor &bgColor=Qt::white, const QColor &textColor=Qt::black)
		Konstruktor ustawiaj�cy sformatowan� wiadomo��,
		kolor t�a i liter wiadomo�ci
		\param nick nazwa u�ytkownika
		\param unformattedMessage niesformatowana wiadomo��
		\param myMessage zmienna mowi�ca czy wiadomo�� pochodzi od nas
		\param date data otrzymania wiadomo�ci
		\param sdate data wys�ania wiadomo�ci
	**/
	ChatMessage(const QString &formattedMessage, const QColor &bgColor=Qt::white, const QColor &textColor=Qt::black);
};

#endif
