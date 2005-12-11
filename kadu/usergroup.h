#ifndef KADU_USERGROUP_H
#define KADU_USERGROUP_H

#include <qmap.h>
#include <qobject.h>
#include <qshared.h>
#include <qstring.h>
#include <qvaluelist.h>
#include <qvariant.h>

#include "userlistelement.h"

class UserGroupData;
class UserListElements;

enum BehaviourForAnonymous {TrueForAnonymous, FalseForAnonymous};

/**
	\class UserGroup

	Ze wzgl�du na budow� tej klasy, parametr "last" w sygna�ach dost�pnych
	w tej klasie, mo�e by� u�ywany tylko gdy mamy absolutn� pewno��, �e �aden
	sygna� nie utknie gdzie� we wn�trzach Kadu. Dlatego te� bezpieczne jest
	u�ywanie tego parametru tylko w slotach pod��czonych do klasy UserList.
	Je�eli mimo to potrzebujemy skorzysta� z parametru massively w celu wykonania
	jakiej� czynno�ci dopiero po zako�czeniu zmian, mo�emy pos�u�y� si� QTimerem
	z czasem 0 (czyli po oddaniu sterowania p�tli zdarze� Qt) i jednokrotnym
	uruchomieniem. Przyk�ad:
	<code>
	MojaKlasa{
		...
		QTimer refreshTimer;
		...
	};
	MojaKlasa::MojaKlasa()
	{
		...
		connect(&refreshTimer, SIGNAL(timeout()), this, SLOT(refresh()));
		...
	}
	inline void MojaKlasa::refreshLater()
	{
		refreshTimer.start(0, true);
	}

	void MojaKlasa::refresh()
	{
		...//bardzo kosztowne od�wie�anie
	}

	void MojaKlasa::userAdded(UserListElement elem, bool massively, bool last)
	{
		if (massively)
		{
			akcja1; //np jakie� buforowanie
			...
			refreshLater();
		}
		else
		{
			akcja2;
			...
			refresh();
		}
	}
	</code>
**/
class UserGroup : public QObject
{
	Q_OBJECT

	public:
		typedef QValueListConstIterator<UserListElement> const_iterator;
		typedef QValueListIterator<UserListElement> iterator;
		typedef size_t size_type;

		/**
			\fn UserGroup(int size, const char *name = 0)
			\param size spodziewana ilo�� element�w * 2
			\param name nazwa grupy
		**/
		UserGroup(int size, const char *name = 0);

		/**
			\fn virtual ~UserGroup()
			Zwalnia pami�� zwi�zan� z grup�
		**/
		virtual ~UserGroup();

		/**
			\fn bool equals(UserListElements users) const
			\param users
			Z�o�ono�� O(users.count()).
		**/
		bool equals(UserListElements users) const;

		/**
			\fn bool equals(UserListElements users) const
			\param users
			Z�o�ono�� O(users.count()).
		**/
		bool equals(const UserGroup *group) const;

		/**
			\fn UserListElement byID(const QString &protocolName, const QString &id)
			\param protocolName identyfikator protoko�u
			\param id identyfikator kontaktu
			Zwraca element listy kontakt�w identyfikowany przez nazw�
			protoko�u i identyfikator w tym�e protokole.
			Je�eli nie znajdzie si� taki kontakt, to tworzony jest nowy
			anonimowy kontakt i dodawany do listy.
			Z�o�ono�� O(count()), ale w przysz�o�ci b�dzie optymalizowana.
		**/
		UserListElement byID(const QString &protocolName, const QString &id);

		/**
			\fn UserListElement byAltNick(const QString &altnick)
			\param altnick wy�wietlany pseudonim wg. kt�rego nast�puje wyszukiwanie.
			\return obiekt reprezentuj�cy kontakt o podanym pseudonimie wy�wietlanym.
			Wyszukuje kontakt po wy�wietlanym pseudonimie.
			Je�eli nie znajdzie si� taki kontakt, to tworzony jest nowy
			anonimowy kontakt i dodawany do listy.
			Z�o�ono�� O(count()), ale w przysz�o�ci b�dzie optymalizowana do O(1).
		**/
		UserListElement byAltNick(const QString &altnick);

		/**
			\fn bool contains(const QString &protocolName, const QString &id, BehaviourForAnonymous beh = TrueForAnonymous) const
			\param protocolName identyfikator protoko�u
			\param id identyfikator kontaktu
			\param beh spos�b zachowania dla anonim�w
			Sprawdza czy grupa zawiera kontakt o danym identyfikatorze we wskazanym protokole.
			W przypadku kontakt�w anonimowych zachowanie tej funkcji okre�la parametr beh. Gdy jest utawiony na:
				TrueForAnonymous (domy�lnie), to zwraca prawd�
				FalseForAnonymous, to zwraca fa�sz
			Z�o�ono�� O(count()), ale w przysz�o�ci b�dzie optymalizowana.
		**/
		bool contains(const QString &protocolName, const QString &id, BehaviourForAnonymous beh = TrueForAnonymous) const;

		/**
			\fn bool contains(UserListElement elem, BehaviourForAnonymous beh = TrueForAnonymous) const
			\param elem sprawdzany kontakt
			\param beh spos�b zachowania dla anonim�w
			Sprawdza czy grupa zawiera wskazany kontakt.
			W przypadku kontakt�w anonimowych zachowanie tej funkcji okre�la parametr beh. Gdy jest utawiony na:
				TrueForAnonymous (domy�lnie), to  zwraca prawd�
				FalseForAnonymous, to zwraca fa�sz
			Z�o�ono�� O(1).
		**/
		bool contains(UserListElement elem, BehaviourForAnonymous beh = TrueForAnonymous) const;

		/**
			\fn bool containsAltNick(const QString &altnick, BehaviourForAnonymous beh = TrueForAnonymous) const
			\param altnick wy�wietlany pseudonim
			\param beh spos�b zachowania dla anonim�w
			Sprawdza czy grupa zawiera kontakt o danym pseudonimie wy�wietlanym.
			W przypadku kontakt�w anonimowych zachowanie tej funkcji okre�la parametr beh. Gdy jest utawiony na:
				TrueForAnonymous (domy�lnie), to  zwraca prawd�
				FalseForAnonymous, to zwraca fa�sz
			Z�o�ono�� O(count()), ale w przysz�o�ci b�dzie optymalizowana do O(1).
		**/
		bool containsAltNick(const QString &altnick, BehaviourForAnonymous beh = TrueForAnonymous) const;

		/**
			\fn size_type count() const
			Zwraca ilo�� kontakt�w.
		**/
		size_type count() const;

		/**
			\fn const_iterator constBegin () const
			Zwraca iterator pocz�tkowy.
		**/
		const_iterator constBegin () const;

		/**
			\fn const_iterator constEnd () const
			Zwraca iterator ko�cowy.
		**/
		const_iterator constEnd () const;

		/**
			\fn const_iterator begin () const
			Zwraca iterator pocz�tkowy.
		**/
		iterator begin () const;

		/**
			\fn const_iterator end () const
			Zwraca iterator ko�cowy.
		**/
		iterator end () const;

		/**
			\fn UserListElements toUserListElements() const
			Zwraca list� kontakt�w w postaci obiektu klasy UserListElements
		**/
		UserListElements toUserListElements() const;

		/**
			\fn void resize(int size)
			\param size rozmiar bufora * 2
			Zmienia wewn�trzny rozmiar bufor�w dla klas s�ownikowych (Q*Dict).
		**/
		void resize(int size);

	public slots:
		/**
			\fn void addUser(UserListElement ule, bool massively = false, bool last = false)
			\param ule obiekt reprezentuj�cy kontakt, kt�ry chcemy doda�.
			\param massively true, gdy jest to cze�� wi�kszego dodawania
			\param last true, gdy massively == true i jest to ostatnie dodanie
			Dodaje do listy podany kontakt.
		**/
		void addUser(UserListElement ule, bool massively = false, bool last = false);

		/**
			\fn void removeUser(UserListElement ule, bool massively = false, bool last = false)
			\param ule obiekt reprezentuj�cy kontakt, kt�ry chcemy usun��.
			\param massively true, gdy jest to cze�� wi�kszego usuwania
			\param last true, gdy massively == true i jest to ostatnie usuni�cie
			Usuwa podany kontakt z listy.
		**/
		void removeUser(UserListElement ule, bool massively = false, bool last = false);

		/**
			\fn UserListElement addAnonymous(const QString &protocolName, const QString &id, bool massively = false, bool last = false)
			\param protocolName identyfikator protoko�u
			\param id identyfikator kontaktu
			\param massively true, gdy jest to cze�� wi�kszego dodawania
			\param last true, gdy massively == true i jest to ostatnie dodanie
			Dodaje do listy anonimowy kontakt i zwraca go.
		**/
		UserListElement addAnonymous(const QString &protocolName, const QString &id, bool massively = false, bool last = false);

		/**
			\fn void addUsers(const UserGroup *group)
			Dodaje do listy wszystkie kontakty nale��ce do grupy group
			\param group grupa kontakt�w
		**/
		void addUsers(const UserGroup *group);

		/**
			\fn void addUsers(QValueList<UserListElement> users)
			Dodaje do listy wszystkie kontakty nale��ce do listy users
			\param users kontakty
		**/
		void addUsers(QValueList<UserListElement> users);

		/**
			\fn void removeUsers(const UserGroup *group)
			Usuwa z listy wszystkie kontakty nale��ce do grupy group
			\param group grupa kontakt�w
		**/
		void removeUsers(const UserGroup *group);

		/**
			\fn void removeUsers(QValueList<UserListElement> users)
			Usuwa z listy wszystkie kontakty nale��ce do listy users
			\param users kontakty
		**/
		void removeUsers(QValueList<UserListElement> users);

	signals:
		/**
			\fn void modified()
			Sygna� generowany po zako�czeniu zmian w li�cie	kontakt�w.
			\todo sprawdzi� czy zawsze dzieje si� to po zako�czeniu zmian
		**/
		void modified();

		/**
			\fn void userDataChanged(UserListElement elem, QString name, QVariant oldValue, QVariant currentValue, bool massively, bool last)
			\param elem kontakt, kt�rego dane si� zmieni�y
			\param name nazwa w�asno�ci
			\param oldValue stara warto��
			\param currentValue nowa (bie��ca) warto��
			\param massively true, gdy jest to cze�� wi�kszych zmian
			\param last true, gdy massively == true i jest to ostatnia zmiana
			Sygna� generowany gdy dla kontaktu elem zmieni si� w�asno�� o nazwie name.
			Przekazuje tak�e star� i now� warto��.
		**/
		void userDataChanged(UserListElement elem, QString name, QVariant oldValue,
							QVariant currentValue, bool massively, bool last);

		/**
			\fn void protocolUserDataChanged(QString protocolName, UserListElement elem, QString name, QVariant oldValue, QVariant currentValue, bool massively, bool last)
			\param protocolName identyfikator protoko�u
			\param elem kontakt, kt�rego dane si� zmieni�y
			\param name nazwa w�asno�ci
			\param oldValue stara warto��
			\param currentValue nowa (bie��ca) warto��
			\param massively true, gdy jest to cze�� wi�kszych zmian
			\param last true, gdy massively == true i jest to ostatnia zmiana
			Sygna� generowany gdy dla kontaktu elem zmieni si� w�asno�� zwi�zana z protoko�em
			protocolName o nazwie name. Przekazuje tak�e star� i now� warto��.
		**/
		void protocolUserDataChanged(QString protocolName, UserListElement elem,
							QString name, QVariant oldValue, QVariant currentValue,
							bool massively, bool last);

		/**
			\fn void userAdded(UserListElement elem, bool massively, bool last)
			\param elem dodany kontakt
			\param massively czy jest to fragment wi�kszych zmian
			\param last je�eli massively == true, to last == true dla ostatniego kontaktu
			Sygna� generowany po dodaniu kontaktu elem do listy
		**/
		void userAdded(UserListElement elem, bool massively, bool last);

		/**
			\fn void addingUser(UserListElement elem, bool massively, bool last)
			\param elem dodany kontakt
			\param massively czy jest to fragment wi�kszych zmian
			\param last je�eli massively == true, to last == true dla ostatniego kontaktu
			Sygna� generowany przed dodaniem kontaktu elem do listy
		**/
		void addingUser(UserListElement elem, bool massively, bool last);

		/**
			\fn void removingUser(UserListElement elem, bool massively, bool last)
			\param elem usuwany kontakt
			\param massively czy jest to fragment wi�kszych zmian
			\param last je�eli massively == true, to last == true dla ostatniego kontaktu
			Sygna� generowany przed usuni�ciem kontaktu z listy.
		**/
		void removingUser(UserListElement elem, bool massively, bool last);

		/**
			\fn void userRemoved(UserListElement elem, bool massively, bool last)
			\param elem usuwany kontakt
			\param massively czy jest to fragment wi�kszych zmian
			\param last je�eli massively == true, to last == true dla ostatniego kontaktu
			Sygna� generowany po usuni�ciu kontaktu z listy.
		**/
		void userRemoved(UserListElement elem, bool massively, bool last);

		/**
			\fn void protocolAdded(UserListElement elem, QString protocolName, bool massively, bool last)
			\param elem kontakt, dla kt�rego dodany zosta� protok�
			\param protocolName identyfikator protoko�u
			\param massively czy jest to fragment wi�kszych zmian
			\param last je�eli massively == true, to last == true dla ostatniego kontaktu
			Sygna� generowany po dodaniu protoko�u protocolName kontaktowi elem.
		**/
		void protocolAdded(UserListElement elem, QString protocolName, bool massively, bool last);

		/**
			\fn void removingProtocol(UserListElement elem, QString protocolName, bool massively, bool last)
			\param elem kontakt, dla kt�rego usuni�ty zosta� protok�
			\param protocolName identyfikator protoko�u
			\param massively czy jest to fragment wi�kszych zmian
			\param last je�eli massively == true, to last == true dla ostatniego kontaktu
			Sygna� generowany przed usuni�ciem protoko�u protocolName kontaktu elem.
		**/
		void removingProtocol(UserListElement elem, QString protocolName, bool massively, bool last);

		/**
			\fn void statusChanged(UserListElement elem, QString protocolName, const UserStatus &oldStatus, bool massively, bool last)
			\param elem kontakt, dla kt�rego zmieni� si� status
			\param protocolName identyfikator protoko�u
			\param oldStatus poprzedni status
			\param massively czy jest to fragment wi�kszych zmian
			\param last je�eli massively == true, to last == true dla ostatniego kontaktu
		**/
		void statusChanged(UserListElement elem, QString protocolName,
							const UserStatus &oldStatus, bool massively, bool last);

	protected:
		UserGroupData *d;
		friend class UserListElement;
		friend class ULEPrivate;

		/**
			\fn UserListElement byKey(UserListKey key)
			\param key klucz
			Zwraca element listy kontakt�w identyfikowany przez klucz.
		**/
		UserListElement byKey(UserListKey key);

	private:
		/* standardowy konstruktor kopiuj�cy zepsu�by obiekt �r�d�owy */
		UserGroup(const UserGroup &) {}

		/* standardowy operator kopiowania zepsu�by obiekt �r�d�owy */
 		virtual void operator = (const UserGroup &) {}

		/* nie da si� por�wnywa� obiekt�w tej klasy */
		bool operator==(const UserGroup &) const {return false;}

};

/**
	\class UserListElements
	Prosta lista u�ytkownik�w z kilkoma u�atwiaczami.
**/
class UserListElements : public QValueList<UserListElement>
{
	public:
		/**
			\fn UserListElements(UserListElement)
			Konstruktor dodaj�cy od razu wskazany kontakt
		**/
		UserListElements(UserListElement);

		/**
			\fn UserListElements()
			Standardowy konstruktor.
		**/
		UserListElements();

		/**
			\fn bool equals(const UserListElements &elems) const
			Sprawdza czy bie��ca lista zaiwera te same elemnty co wskazana.
			\attention {Z�o�ono�� O(count()*elems.count()).}
		**/
		bool equals(const UserListElements &elems) const;

		/**
			\fn bool equals(const UserGroup *group) const
			Sprawdza czy bie��ca lista zaiwera te same elemnty co wskazana.
			\attention {Z�o�ono�� O(count()).}
		**/
		bool equals(const UserGroup *group) const;

		/**
			\fn void sort()
			Sortuje list� w spos�b jednoznaczny.
		**/
		void sort();

		/**
			\fn bool contains(QString protocol, QString id) const
			Zwraca informacj� o tym czy lista zawiera kontakt o wskazanym protokole i identyfiaktorze.
			(Z�o�ono�� O(n))
		**/
		bool contains(QString protocol, QString id) const;

		/**
			\fn bool contains(UserListElement e) const
			Zwraca informacj� o tym czy lista zawiera wskazany kontakt.
			(Z�o�ono�� O(n))
		**/
		bool contains(UserListElement e) const { return QValueList<UserListElement>::contains(e) > 0;}
};

#endif
