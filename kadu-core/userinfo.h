#ifndef KADU_USERINFO_H
#define KADU_USERINFO_H

#include <qglobal.h>

#include <q3hbox.h>
#include <q3valuelist.h>
//Added by qt3to4:
#include <QLabel>
#include <QKeyEvent>

#include "userlistelement.h"
#include "misc.h"

class QCheckBox;
class Q3Dns;
class QLineEdit;
class QPushButton;
class Q3ScrollView;
class QTabWidget;
class Q3VBox;
class Q3VGroupBox;

/**
	Klasa ta reprezentuje okno, z kt�rym mamy do czynienia podczas dodawania nowego kontaktu,
	lub podczas edycji danych istniej�cego ju� kontaktu.
	\class UserInfo
	\brief Okno dialogowe pokazuj�ce informacje o wybranym kontakcie
**/
class UserInfo : public QWidget
{
	Q_OBJECT
	public:
		/**
			\var static CreateNotifier createNotifier
			Statyczny obiekt wysy�aj�cy sygna� CreateNotifier::objectCreated
			je�li powstanie nowa instancja okienka.
		**/
		static CreateNotifier createNotifier;

		/**
			\fn UserInfo(const QString &altnick, bool addUser = false, QDialog* parent=0, const char *name=0)
			Konstruktor tworz�cy okno dialogowe z informacjami o danym kontakcie.
			\param altnick pseudonim kontaktu wy�wietlany na li�cie.
			\param addUser TRUE gdy dodajemy kontakt, FALSE gdy edytujemy dane istniej�cego. Domy�lnie FALSE.
			\param parent wska�nik do obiektu stanowi�cego rodzica. Domy�lnie 0.
			\param name nazwa kontrolki. Domy�lnie 0.
		**/
		UserInfo(UserListElement user, QWidget* parent = 0, const char *name = 0);
		~UserInfo();

		QTabWidget *tabs() {return tw_main;}
		UserListElement user() const {return User;}
		static bool acceptableGroupName(const QString &groupName);

	private:
		UserListElement User;
		QLineEdit *e_firstname;
		QLineEdit *e_lastname;
		QLineEdit *e_nickname;
		QLineEdit *e_altnick;
		QLineEdit *e_mobile;
		QLineEdit *e_uin;
		QLineEdit *e_addr;
		QLineEdit *e_ver;
		QLineEdit *e_email;
		QLineEdit *e_dnsname;
		QCheckBox *c_blocking;
		QCheckBox *c_offtouser;
		QCheckBox *c_notify;
		QPushButton *pb_addapply;
		QTabWidget *tw_main;
		Q3VGroupBox *vgb_general;
		Q3Dns *dns;
		Q3ScrollView* scrollView;

		QList<QCheckBox *> groups;
		QList<QCheckBox *> hiddenCheckBoxes;
		QLineEdit *newGroup;
		Q3VBox *groupsBox;

		QMap <QString, QLabel *> pixmapLabels;

		void keyPressEvent(QKeyEvent *);

	protected:
		/**
			\fn void setupTab1()
			Tworzy pierwsz� zak�adk�.
		**/
		void setupTab1();

		/**
			\fn void setupTab2()
			Tworzy drug� zak�adk�.
		**/
		void setupTab2();

		/**
			\fn void setupTab3()
			Tworzy trzeci� zak�adk�.
		**/
		void setupTab3();

	public slots:
		/**
			\fn void updateUserlist()
			Wci�ni�cie przycisku Dodaj/Uaktualnij
		**/
		void updateUserlist();

	private slots:
		/**
			\fn void resultsReady()
			Ustawia warto�� pola DNS na znalezion� nazw� domeny.
		**/
		void resultsReady();

		/**
			\fn void newGroupClicked()
			wci�ni�to przycisk tworzenia nowej grupy
		**/
		void newGroupClicked();

		/**
			\fn void scrollToBottom()
			po dodaniu nowej grupy przewija list� grup aby dodana by�a widoczna
		**/
		void scrollToBottom();
		
		/**
			void selectIcon()
			wci�ni�to przycisk dodania ikony dla grupy
		**/
		void selectIcon();

		/**
			void deleteIcon()
			wci�ni�to przycisk usuni�cia ikony grupy
		**/
		void deleteIcon();
	signals:
		void updateClicked(UserInfo *);
};

#endif
