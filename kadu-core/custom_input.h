#ifndef KADU_CUSTOM_INPUT_H
#define KADU_CUSTOM_INPUT_H

#include <qglobal.h>

#include <q3multilineedit.h>
//Added by qt3to4:
#include <QKeyEvent>

class QKeyEvent;
class QWidget;

/**
	\class CustomInput
	\brief Klasa umo�liwiaj�ca wpisywanie wiadomo�ci
**/
class CustomInput : public Q3MultiLineEdit
{
	Q_OBJECT

	protected:

		bool autosend_enabled;/*!< zmienna informuj�ca czy w��czone jest
					automatyczne wysy�anie */
		/**
			\fn virtual void keyPressEvent(QKeyEvent *e)
			Funkcja obs�uguj�ca naci�ni�cie przycisku
			\param e wska�nik do obiektu obs�uguj�cego klawisze
		**/
		virtual void keyPressEvent(QKeyEvent *e);

		/**
			\fn virtual void keyReleaseEvent(QKeyEvent *e)
			Funkcja obs�uguj�ca zwolnienie przycisku
			\param e wska�nik do obiektu obs�uguj�cego klawisze
		**/
		virtual void keyReleaseEvent(QKeyEvent* e);

	public:

		/*! Typ wyliczeniowy mowi�cy o rodzaju pisanych znak�w */
		enum
		{
			KEY_BOLD,/*!< pogrubienie */
			KEY_ITALIC,/*!< kursywa */
			KEY_UNDERLINE/*!< podkre�lenie */
		};

		/**
			\fn CustomInput(QWidget* parent = 0, const char* name = 0)
			Konstruktor tworz�cy obiekt
			i ustawiaj�cy odpowiedni styl
			\param parent rodzic okna
			\param name nazwa obiektu
		**/
		CustomInput(QWidget* parent = 0, const char* name = 0);

	public slots:

		/**
			\fn void paste()
			Slot wkleja czysty tekst
		**/
		void paste();

		/**
			\fn void setAutosend(bool on)
			Ustawia/wy��cza automatyczne wysy�anie wiadomo�ci
			\param on zmienna ustawiaj�ca autosend
		**/
		void setAutosend(bool on);

	signals:

		/**
			\fn void sendMessage()
			Sygna� zostaje wys�any kiedy naci�ni�to
			klawisz wys�ania wiadomo�ci
		**/
		void sendMessage();

		/**
			\fn void specialKeyPressed(int key)
			Sygna� zostaje wys�any gdy naci�ni�to specjalny
			klawisz (skr�t) odpowiadaj�cy np KEY_BOLD
			\param key warto�� z typu wyliczeniowego KEY_*
		**/
		void specialKeyPressed(int key);

		/**
			\fn void keyPressed(QKeyEvent* e, CustomInput* sender, bool &handled)
			Dowolny przycisk klawiatury zosta� naci�ni�ty.
			Przekazany zostaje tak�e obiekt, kt�ry wywo�a� akcj� -
			 czyli this
			\param e wska�nik do obiektu obs�uguj�cego klawisze
			\param sender wska�nik do obiektu, kt�ry
			wywo�a� ten sygna�
			\param handled Slot osb�uguj�cy ten sygna� powinien ten parametr ustawi� na true,
			je�li zdarzenie zosta�o ju� obs�u�one i �adne czynno�ci zwi�zane
			ze standardow� obs�ug� kontrolek Qt nie powinny by� podj�te.
		**/
		void keyPressed(QKeyEvent* e, CustomInput* sender, bool &handled);

		/**
			\fn void keyReleased(QKeyEvent* e, CustomInput* sender, bool &handled)
			Dowolny przycisk klawiatury zosta� zwolniony.
			Przekazany zostaje tak�e obiekt, kt�ry wywo�a� akcj� -
			 czyli this.
			\param e wska�nik do obiektu obs�uguj�cego klawisze
			\param sender wska�nik do obiektu, kt�ry
			wywo�a� ten sygna�
			\param handled Slot osb�uguj�cy ten sygna� powinien ten parametr ustawi� na true,
			je�li zdarzenie zosta�o ju� obs�u�one i �adne czynno�ci zwi�zane
			ze standardow� obs�ug� kontrolek Qt nie powinny by� podj�te.
		**/
		void keyReleased(QKeyEvent* e, CustomInput* sender, bool &handled);
};

#endif
