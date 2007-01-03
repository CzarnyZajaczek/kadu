#ifndef KADU_CHAT_STYLES_H
#define KADU_CHAT_STYLES_H

#include <qstring.h>

/**
	Klasa okre�laj�ca wygl�d styli
	wykorzystywanych w oknie rozmowy.
	\class ChatStyle
	\brief Styl okna Chat
**/
class ChatStyle
{
	private:
		QString FormatStringFull; /*!< �a�cuch opisuj�cy pe�en styl */
		QString FormatStringPure; /*!< �a�cuch opisuj�cy styl bez nag��wka */

		bool noServerTime; /*!< czy czas serwera powinien by� usuwany */
		int noServerTimeDiff; /*!< maksymalna r�nica czasu dla kt�rej usuwamy */

	public:
		/**
			Konstruktor stylu okna rozmowy
			\fn ChatStyle(const QString& format_string_full,
				const QString& format_string_pure)
			\param format_string_full pe�ny styl okna
			\param format_string_pure styl bez nag��wka
		**/
		ChatStyle(const QString& format_string_full,
			const QString& format_string_pure);

		/**
			Konstruktor stylu okna rozmowy
			\fn ChatStyle(const QString& format_string_full)
			\param format_string_full pe�ny styl okna
		**/
		ChatStyle(const QString& format_string_full);

		/**
			\fn const QString& formatStringFull() const
			Zwraca �a�cuch dla pe�nego stylu
		**/
		const QString& formatStringFull() const;

		/**
			\fn const QString& formatStringPure() const
			Zwraca �a�cuch dla stylu bez nag��wka
		**/
		const QString& formatStringPure() const;

		/**
			\fn const bool noServerTimes() const
			Zwraca warto�� okre�laj�c�, czy czas serwera powinien
			by� usuwany dla wiadomo�ci przychodz�cych.
		**/
		const bool noServerTimes() const;

		/**
			\fn const int noServerTimesDiff() const
			Zwraca maksymaln� r�nic� czasu, dla kt�rej czas
			serwera powinien by� pomijany.
		**/
		const int noServerTimesDiff() const;

		/**
			Destruktor styli w oknie Chat
			\fn ~ChatStyle()
		**/
		virtual ~ChatStyle();
};

/**
	Klasa opisuj�ca wygl�d stylu Kadu.
	\class KaduChatStyle
	\brief Styl Kadu dla okna Chat
**/
class KaduChatStyle : public ChatStyle
{
	public:
		/**
			Konstruktor stylu Kadu
			\fn KaduChatStyle()
		**/
		KaduChatStyle();
};

/**
	Klasa opisuj�ca wygl�d stylu Hapi.
	\class HapiChatStyle
	\brief Styl Hapi dla okna Chat
**/
class HapiChatStyle : public ChatStyle
{
	public:
		/**
			Konstruktor stylu Hapi
			\fn HapiChatStyle()
		**/
		HapiChatStyle();
};

/**
	Klasa opisuj�ca wygl�d stylu Irc.
	\class IrcChatStyle
	\brief Styl Irc dla okna Chat
**/
class IrcChatStyle : public ChatStyle
{
	public:
		/**
			Konstruktor stylu Irc
			\fn IrcChatStyle()
		**/
		IrcChatStyle();
};

/**
	Klasa opisuj�ca wygl�d stylu stworzonego
	przez u�ytkownika.
	\class CustomChatStyle
	\brief Styl Custom dla okna Chat
**/
class CustomChatStyle : public ChatStyle
{
	public:
		/**
			Konstruktor stylu Custom
			\fn CustomChatStyle()
			\param full_style w�asna sk�adnia stylu
		**/
		CustomChatStyle(const QString& full_style);
};

#endif
