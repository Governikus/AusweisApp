/*!
 * \brief Unit tests for \ref MsgHandlerCertificate
 *
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

#include "messages/MsgHandlerCertificate.h"

#include "InternalActivationContext.h"
#include "MessageDispatcher.h"

#include "TestAuthContext.h"
#include <QtTest>

using namespace governikus;

class test_MsgHandlerCertificate
	: public QObject
{
	Q_OBJECT

	QSharedPointer<AuthContext> getContext()
	{
		QSharedPointer<ActivationContext> activationContext(new InternalActivationContext(QUrl("http://dummy")));
		QSharedPointer<TestAuthContext> context(new TestAuthContext(activationContext, ":/paos/DIDAuthenticateEAC1.xml"));
		context->setRequiredAccessRights({AccessRight::READ_DG01});
		context->setOptionalAccessRights({AccessRight::AGE_VERIFICATION});
		return context;
	}

	private Q_SLOTS:
		void getCertificateBadState()
		{
			MessageDispatcher dispatcher;

			QByteArray msg = QByteArray("{\"cmd\": \"GET_CERTIFICATE\"}");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"error\":\"GET_CERTIFICATE\",\"msg\":\"BAD_STATE\"}"));

			dispatcher.init(getContext());
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"error\":\"GET_CERTIFICATE\",\"msg\":\"BAD_STATE\"}"));
		}


		void getCertificate()
		{
			auto context = getContext();

			MessageDispatcher dispatcher;
			dispatcher.init(context);

			QVERIFY(!dispatcher.processStateChange("StateEditAccessRights").isEmpty());
			QByteArray msg = "{\"cmd\": \"GET_CERTIFICATE\"}";
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"description\":{\"issuerName\":\"Governikus Test DVCA\",\"issuerUrl\":\"http://www.governikus.de\",\"purpose\":\"\",\"subjectName\":\"Governikus GmbH & Co. KG\",\"subjectUrl\":\"https://test.governikus-eid.de\",\"termsOfUsage\":\"Name, Anschrift und E-Mail-Adresse des Diensteanbieters:\\r\\nGovernikus GmbH & Co. KG\\r\\nHochschulring 4\\r\\n28359 Bremen\\r\\nE-Mail: kontakt@governikus.de\\t\"},\"msg\":\"CERTIFICATE\",\"validity\":{\"effectiveDate\":\"2020-05-21\",\"expirationDate\":\"2020-06-20\"}}"));
		}


};

QTEST_GUILESS_MAIN(test_MsgHandlerCertificate)
#include "test_MsgHandlerCertificate.moc"
