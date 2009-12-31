/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "contacts/contact.h"
#include "contacts/contact-manager.h"
#include "misc/misc.h"
#include "protocols/protocol.h"

#include "buddy-personal-info-configuration-widget.h"

BuddyPersonalInfoConfigurationWidget::BuddyPersonalInfoConfigurationWidget(Buddy &buddy, QWidget *parent)
		: QWidget(parent), MyBuddy(buddy)
{
	setAttribute(Qt::WA_DeleteOnClose);

	createGui();
}

BuddyPersonalInfoConfigurationWidget::~BuddyPersonalInfoConfigurationWidget()
{
}

void BuddyPersonalInfoConfigurationWidget::createGui()
{
	QGridLayout *layout = new QGridLayout(this);
// 	layout->setColumnMinimumWidth(0, 10);
// 	layout->setColumnMinimumWidth(1, 10);
// 	layout->setColumnMinimumWidth(3, 10);
// 	layout->setColumnMinimumWidth(6, 20);

	int row = 0;
	
	layout->setRowStretch(row++, 1); 

	QLabel *usernameLabel = new QLabel(tr("Contact Username") + ":", this);
	layout->addWidget(usernameLabel, row, 3, 1, 1); 

	ContactIdCombo = new QComboBox(this);
	foreach (const Contact &data, MyBuddy.contacts())
	{
		QIcon icon = data.contactAccount().protocolHandler()
				? data.contactAccount().protocolHandler()->icon()
				: QIcon();
		ContactIdCombo->addItem(icon, data.id(), data.uuid().toString());
	}
	connect(ContactIdCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(accountSelectionChanged(int)));
	layout->addWidget(ContactIdCombo, row++, 4, 1, 1);

	QGroupBox *infoWidget = new QGroupBox(this);
	QGridLayout *infoLayout = new QGridLayout(infoWidget);
// 	infoLayout->setColumnMinimumWidth(0, 10);
// 	infoLayout->setColumnMinimumWidth(4, 10);
	infoLayout->setColumnStretch(1, 3);
	infoLayout->setColumnStretch(2, 3);
	infoLayout->setColumnStretch(3, 8);

	int subRow = 0;
	
	QLabel *firstNameLabel = new QLabel(tr("First Name") + ":",this);
	firstNameLabel->setAlignment(Qt::AlignRight);
	infoLayout->addWidget(firstNameLabel, subRow, 1, 1, 1);
	FirstNameText = new QLabel(this);
	infoLayout->addWidget(FirstNameText, subRow++, 2, 1, 1);

	QLabel *lastNameLabel = new QLabel(tr("Last Name") + ":", this);
	lastNameLabel->setAlignment(Qt::AlignRight);
	infoLayout->addWidget(lastNameLabel, subRow, 1, 1, 1);
	LastNameText = new QLabel(this);
	infoLayout->addWidget(LastNameText, subRow++, 2, 1, 1);

	QLabel *nicknameLabel = new QLabel(tr("Nickname") + ":", this);
	nicknameLabel->setAlignment(Qt::AlignRight);
	infoLayout->addWidget(nicknameLabel, subRow, 1, 1, 1);
	NicknameText = new QLabel(this);
	infoLayout->addWidget(NicknameText, subRow++, 2, 1, 1);

	QLabel *genderLabel = new QLabel(tr("Gender") + ":", this);
	genderLabel->setAlignment(Qt::AlignRight);
	infoLayout->addWidget(genderLabel, subRow, 1, 1, 1);
	GenderText = new QLabel(this);
	infoLayout->addWidget(GenderText, subRow++, 2, 1, 1);

	QLabel *birthdateLabel = new QLabel(tr("Birthdate") + ":",this);
	birthdateLabel->setAlignment(Qt::AlignRight);
	infoLayout->addWidget(birthdateLabel, subRow, 1, 1, 1);
	BirthdateText = new QLabel(this);
	infoLayout->addWidget(BirthdateText, subRow++, 2, 1, 1);

	QLabel *cityLabel = new QLabel(tr("City") + ":",this);
	cityLabel->setAlignment(Qt::AlignRight);
	infoLayout->addWidget(cityLabel, subRow, 1, 1, 1);
	CityText = new QLabel(this);
	infoLayout->addWidget(CityText, subRow++, 2, 1, 1);

	QLabel *stateProvinceLabel = new QLabel(tr("State/Province") + ":",this);
	stateProvinceLabel->setAlignment(Qt::AlignRight);
	infoLayout->addWidget(stateProvinceLabel, subRow, 1, 1, 1);
	StateProvinceText = new QLabel(this);
	infoLayout->addWidget(StateProvinceText, subRow++, 2, 1, 1);

	infoLayout->setRowStretch(subRow++, 20);

	QLabel *ipLabel = new QLabel(tr("IP Address") + ":",this);
	ipLabel->setAlignment(Qt::AlignRight);
	infoLayout->addWidget(ipLabel, subRow, 1, 1, 1);
	IpText = new QLabel(this);
	infoLayout->addWidget(IpText, subRow++, 2, 1, 1);

	QLabel *portLabel = new QLabel(tr("Port") + ":",this);
	portLabel->setAlignment(Qt::AlignRight);
	infoLayout->addWidget(portLabel, subRow, 1, 1, 1);
	PortText = new QLabel(this);
	infoLayout->addWidget(PortText, subRow++, 2, 1, 1);

	QLabel *dnsNameLabel = new QLabel(tr("DNS Name") + ":",this);
	dnsNameLabel->setAlignment(Qt::AlignRight);
	infoLayout->addWidget(dnsNameLabel, subRow, 1, 1, 1);
	DnsNameText = new QLabel(this);
	infoLayout->addWidget(DnsNameText, subRow++, 2, 1, 1);

	QLabel *protocolVerLabel = new QLabel(tr("Protocol Version") + ":",this);
	protocolVerLabel->setAlignment(Qt::AlignRight);
	infoLayout->addWidget(protocolVerLabel, 11, 1, 1, 1);
	ProtocolVerText = new QLabel(this);
	infoLayout->addWidget(ProtocolVerText, 11, 2, 1, 1);

	infoLayout->setRowStretch(12, 100);

	layout->addWidget(infoWidget, row++, 2, 1, 4);

	layout->setRowStretch(row, 100);
}

void BuddyPersonalInfoConfigurationWidget::accountSelectionChanged(int index)
{
	// TODO 0.6.6: is it needed?
	QString contactUuid = ContactIdCombo->itemData(index).toString();
	if (contactUuid.isEmpty())
		return;

	Contact contact = ContactManager::instance()->byUuid(QUuid(contactUuid));
	if (contact.isNull())
		return;

	//TODO 0.6.6 proper values
	FirstNameText->setText(MyBuddy.firstName());
	LastNameText->setText(MyBuddy.lastName());
	NicknameText->setText(MyBuddy.nickName());
	GenderText->setText(MyBuddy.firstName());
	BirthdateText->setText(MyBuddy.firstName());
	CityText->setText(MyBuddy.city());
	StateProvinceText->setText(MyBuddy.firstName());

	//
	IpText->setText(contact.address().toString());
	PortText->setText(QString::number(contact.port()));
	DnsNameText->setText(contact.dnsName());
	ProtocolVerText->setText(contact.protocolVersion());
}
