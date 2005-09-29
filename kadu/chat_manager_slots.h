#ifndef KADU_CHAT_MANAGER_SLOTS_H
#define KADU_CHAT_MANAGER_SLOTS_H

#include <qobject.h>
#include <qcolor.h>
#include <qfont.h>

class QComboBox;
class QLineEdit;

/**
	\class ChatSlots
	\brief Sloty do obs�ugi okna konfiguracji
**/
class ChatManagerSlots :public QObject
{
	Q_OBJECT

	private:
		/**
			\fn void updatePreview()
			Od�wie�enie wygl�du
		**/
		void updatePreview();
	public:
		/**
			Konstruktor obiektu
			\param parent rodzic obiektu
			\param name nazwa obiektu
		**/
		ChatManagerSlots(QObject* parent=0, const char* name=0);

	public slots:
		/**
			\fn void onCreateTabChat()
			Slot jest wywo�ywany gdy otwierana jest zakladka Chat
		**/
		void onCreateTabChat();

		void onCreateTabLook();
		/**
			\fn void onApplyConfigDialog()
			Slot jest wywo�ywany gdy zamykane jest okno konfiguracji
		**/
		void onApplyTabChat();

		/**
			\fn void onApplyConfigDialog()
			Slot jest wywo�ywany gdy zamykane jest okno konfiguracji
		**/
		void onApplyTabLook();


		/**
			\fn void chooseEmoticonsStyle(int index)
			Slot jest wywo�ywany gdy wybierany jest styl ikonek
			\param index nr stylu emotikonek
		**/
		void chooseEmoticonsStyle(int index);

		/**
			\fn void onPruneChat(bool toggled)
			Slot jest wywo�ywany gdy wybierane jest czy otwierane okno
			ma by� pusty czy nie
			\param toggled
		**/
		void onPruneChat(bool toggled);

		/**
			\fn void onFoldLink(bool toggled)
			\param toggled
		**/
		void onFoldLink(bool toggled);

		/**
			\fn void onBlockClose(bool toggled)
			Slot jest wywo�ywany gdy wybierany jest maksymalny czas
			blokowania zamkni�cia okna po otrzymaniu wiadomo�ci
			\param toggled
		**/
		void onBlockClose(bool toggled);

		/**
			Slot jest wywo�ywany gdy w��czono/wy��czono wy�wietlanie nag��wk�w,
			aby uaktualni� podgl�d
		**/
		void onRemoveHeaders(bool toggled);

		/**
			\fn void chooseColor(const char* name, const QColor& color)
			Slot jest wywo�ywany gdy zmieniono kolor,
			aby uaktualni� podgl�d
			\param name nazwa koloru
			\param color kolor
		**/
		void chooseColor(const char* name, const QColor& color);

		/**
			\fn void chooseFont(const char* name, const QFont& font)
			Slot jest wywo�ywany gdy zmieniono czcionke,
			aby uaktualni� podgl�d
			\param name nazwa czcionki
			\param font czcionka
		**/
		void chooseFont(const char* name, const QFont& font);

		/**
			\fn void findAndSetWebBrowser(int selectedBrowser)
			Slot jest wywo�ywany gdy wybrano z listy przegl�dark�
			\param selectedBrowser nr przegl�darki
		**/
		void findAndSetWebBrowser(int selectedBrowser);

		/**
			\fn void findAndSetBrowserOption(int selectedOption)
			Slot jest wywo�ywany gdy aktywowane pole do wpisania opcji
			\param selectedOption wybrana opcja
		**/
		void findAndSetBrowserOption(int selectedOption);

		/**
			\fn static void initBrowserOptions(QComboBox *browserCombo, QComboBox *browserOptionsCombo, QLineEdit *browserPath)
			Slot ustawia przegl�darki wraz z opcjami
			\param browserCombo
			\param browserOptionsCombo
			\param browserPath
		**/
		static void initBrowserOptions(QComboBox *browserCombo, QComboBox *browserOptionsCombo, QLineEdit *browserPath);

		/**
			\fn static void findBrowser(int selectedBrowser, QComboBox *browserCombo, QComboBox *browserOptionsCombo, QLineEdit *browserPath)
			Slot jest wywo�ywany gdy wybrano z listy przegl�dark�
			\param selectedBrowser
			\param browserCombo
			\param browserOptionsCombo
			\param browserPath
		**/
		static void findBrowser(int selectedBrowser, QComboBox *browserCombo, QComboBox *browserOptionsCombo, QLineEdit *browserPath);

		/**
			\fn static void setBrowserOption(int selectedOption, QLineEdit *browserPathEdit, int chosenBrowser)
			Slot ustawia przegl�darki wraz z opcjami
			\param selectedOption
			\param browsePathEdit
			\param chosenBrowser
		**/
		static void setBrowserOption(int selectedOption, QLineEdit *browserPathEdit, int chosenBrowser);
};
#endif
