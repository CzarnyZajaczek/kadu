#ifndef KADU_CHAT_COLORS_H
#define KADU_CHAT_COLORS_H

#include <qcolor.h>

/**
	Klasa opisuj�ca kolory wykorzystywane
	w oknie rozmowy.
	\class ChatColors
	\brief Kolory w oknie Chat
**/
class ChatColors
{
	private:
		QColor BackgroundColor; /*!< kolor t�a */
		QColor FontColor; /*!< kolor czcionki */
		QColor NickColor; /*!< kolor nicku */

	public:
		/**
			Konstruktor kolor�w dla okna Chat
			\fn ChatColors(const QColor& background_color,
				const QColor& font_color,
				const QColor& nick_color)
			\param background_color kolor t�a
			\param font_color kolor czcionki
			\param nick_color kolor nicku
		**/
		ChatColors(const QColor& background_color,
			const QColor& font_color,
			const QColor& nick_color);

		/**
			\fn const QColor& backgroundColor() const
			Zwraca kolor t�a
		**/
		const QColor& backgroundColor() const;

		/**
			\fn const QColor& fontColor() const
			Zwraca kolor czcionki
		**/
		const QColor& fontColor() const;

		/**
			\fn const QColor& nickColor() const
			Zwraca kolor nicku
		**/
		const QColor& nickColor() const;
};

/**
	Klasa opisuj�ca kolory w�asne u�ytkownika
	\class OwnChatColors
	\brief Kolory w�asne
**/
class OwnChatColors : public ChatColors
{
	public:
		/**
			\fn UserChatColors()
			Konstruktor kolor�w w�asnych
		**/
		OwnChatColors();
};

/**
	Klasa opisuj�ca kolory rozm�wcy
	\class UserChatColors
	\brief Kolory rozm�wcy
**/
class UserChatColors : public ChatColors
{
	public:
		/**
			\fn UserChatColors()
			Konstruktor kolor�w rozm�wcy
		**/
		UserChatColors();
};

#endif
