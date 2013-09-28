/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef OTR_OP_DATA_FACTORY_H
#define OTR_OP_DATA_FACTORY_H

#include <QtCore/QObject>
#include <QtCore/QWeakPointer>

class Contact;

class OtrAppOpsWrapper;
class OtrErrorMessageService;
class OtrFingerprintService;
class OtrInstanceTagService;
class OtrIsLoggedInService;
class OtrMessageService;
class OtrOpData;
class OtrPeerIdentityVerificationService;
class OtrPolicyService;
class OtrPrivateKeyService;
class OtrSessionService;
class OtrTimerService;
class OtrTrustLevelService;

class OtrOpDataFactory : public QObject
{
	Q_OBJECT

	QWeakPointer<OtrAppOpsWrapper> AppOpsWrapper;
	QWeakPointer<OtrErrorMessageService> ErrorMessageService;
	QWeakPointer<OtrFingerprintService> FingerprintService;
	QWeakPointer<OtrInstanceTagService> InstanceTagService;
	QWeakPointer<OtrIsLoggedInService> IsLoggedInService;
	QWeakPointer<OtrMessageService> MessageService;
	QWeakPointer<OtrPeerIdentityVerificationService> PeerIdentityVerificationService;
	QWeakPointer<OtrPolicyService> PolicyService;
	QWeakPointer<OtrPrivateKeyService> PrivateKeyService;
	QWeakPointer<OtrSessionService> SessionService;
	QWeakPointer<OtrTimerService> TimerService;
	QWeakPointer<OtrTrustLevelService> TrustLevelService;

public:
	explicit OtrOpDataFactory(QObject *parent = 0);
	virtual ~OtrOpDataFactory();

	void setAppOpsWrapper(OtrAppOpsWrapper *appOpsWrapper);
	void setErrorMessageService(OtrErrorMessageService *errorMessageService);
	void setFingerprintService(OtrFingerprintService *fingerprintService);
	void setInstanceTagService(OtrInstanceTagService *instanceTagService);
	void setIsLoggedInService(OtrIsLoggedInService *isLoggedInService);
	void setMessageService(OtrMessageService *messageService);
	void setPeerIdentityVerificationService(OtrPeerIdentityVerificationService *peerIdentityVerificationService);
	void setPolicyService(OtrPolicyService *policyService);
	void setPrivateKeyService(OtrPrivateKeyService *privateKeyService);
	void setSessionService(OtrSessionService *sessionService);
	void setTimerService(OtrTimerService *timerService);
	void setTrustLevelService(OtrTrustLevelService *trustLevelService);

	OtrOpData opData();
	OtrOpData opDataForContact(const Contact &contact);

};

#endif // OTR_OP_DATA_FACTORY_H
