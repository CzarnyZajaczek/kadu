#ifndef PERSONAL_INFO_H
#define PERSONAL_INFO_H

#include <qdialog.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qsocketnotifier.h>
#include "libgadu.h"

/**
	Dialog umo�liwiaj�cy zarz�dzanie w�asnymi danymi w katalogu
	publicznym
**/
class PersonalInfoDialog : public QDialog
{
	Q_OBJECT

	private:
		QLineEdit* NicknameEdit;
		QLineEdit* NameEdit;
		QLineEdit* SurnameEdit;
		QComboBox* GenderCombo;
		QLineEdit* BirthyearEdit;
		QLineEdit* CityEdit;
		QLineEdit* FamilyNameEdit;
		QLineEdit* FamilyCityEdit;
		QSocketNotifier* SocketReadNotifier;
		QSocketNotifier* SocketWriteNotifier;
		struct gg_http* http;
		enum DialogState {READY, READING, WRITTING};
		DialogState State;

	private slots:
		void OkButtonClicked();

	protected:
		virtual void closeEvent(QCloseEvent * e);

	public:
		PersonalInfoDialog(QDialog *parent=0, const char *name=0);
		void fillFields(gg_pubdir50_t res);
};

#endif
