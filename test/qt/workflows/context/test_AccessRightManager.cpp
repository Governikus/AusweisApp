/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
 */

#include "AppSettings.h"
#include "VolatileSettings.h"
#include "paos/retrieve/DidAuthenticateEac1Parser.h"

#include "TestAuthContext.h"
#include "TestFileHelper.h"

#include <QSharedPointer>
#include <QSignalSpy>
#include <QtTest>


using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class test_AccessRightManager
	: public QObject
{
	Q_OBJECT

	private:
		QScopedPointer<QSignalSpy> mEac1Changed;
		QScopedPointer<QSignalSpy> mARMCreated;
		QSharedPointer<TestAuthContext> mTestAuthContext;

		QSharedPointer<CHAT> getChat(const std::initializer_list<AccessRight>& pList)
		{
			auto chat = newObject<CHAT>();
			chat->setAccessRights(QSet<AccessRight>(pList));
			return chat;
		}


		void clearSignals()
		{
			mEac1Changed->clear();
			mARMCreated->clear();
		}


		void checkChatSizes(const QString& pTestNum, int pRequired, int pOptional, int pEffective)
		{
			qDebug() << "Checking test" << pTestNum;
			QCOMPARE(mTestAuthContext->getAccessRightManager()->getRequiredAccessRights().size(), pRequired);
			QCOMPARE(mTestAuthContext->getAccessRightManager()->getOptionalAccessRights().size(), pOptional);
			QCOMPARE(mTestAuthContext->getAccessRightManager()->getEffectiveAccessRights().size(), pEffective);
		}


		void checkAndClearSignals(const QString& pTestNum, int pEac1Data)
		{
			qDebug() << "Checking test" << pTestNum;
			QCOMPARE(mEac1Changed->count(), pEac1Data);
			QCOMPARE(mARMCreated->count(), pEac1Data);

			clearSignals();
		}

	private Q_SLOTS:
		void init()
		{
			Env::getSingleton<VolatileSettings>()->setUsedAsSDK(false);
			Env::getSingleton<AppSettings>()->getGeneralSettings().setEnableCanAllowed(false);

			mTestAuthContext.reset(new TestAuthContext(":/paos/DIDAuthenticateEAC1_accessRightsEmpty.xml"_L1));
			mEac1Changed.reset(new QSignalSpy(mTestAuthContext.data(), &AuthContext::fireDidAuthenticateEac1Changed));
			mARMCreated.reset(new QSignalSpy(mTestAuthContext.data(), &AuthContext::fireAccessRightManagerCreated));
		}


		void cleanup()
		{
			mEac1Changed.reset();
			mTestAuthContext.reset();
		}


		void test_canAllowed_data()
		{
			QTest::addColumn<bool>("requested");
			QTest::addColumn<bool>("isUsedAsSDK");
			QTest::addColumn<bool>("canAllowedEnabled");
			QTest::addColumn<bool>("officialDomestic");
			QTest::addColumn<bool>("canAllowed");

			QTest::newRow("Not requested - Not allowed") << false << false << false << false << false;
			QTest::newRow("Not requested - Allowed by: OD") << false << false << false << true << false;
			QTest::newRow("Not requested - Allowed by: Settings") << false << false << true << false << false;
			QTest::newRow("Not requested - Allowed by: Settings, OD") << false << false << true << true << false;
			QTest::newRow("Not requested - Allowed by: SDK") << false << true << false << false << false;
			QTest::newRow("Not requested - Allowed by: SDK, OD") << false << true << false << true << false;
			QTest::newRow("Not requested - Allowed by: SDK, Settings") << false << true << true << false << false;
			QTest::newRow("Not requested - Allowed by: SDK, Settings,  OD") << false << true << true << true << false;
			QTest::newRow("Requested - Not allowed") << true << false << false << false << false;
			QTest::newRow("Requested - Allowed by: OD") << true << false << false << true << true;
			QTest::newRow("Requested - Allowed by: Settings") << true << false << true << false << true;
			QTest::newRow("Requested - Allowed by: Settings, OD") << true << false << true << true << true;
			QTest::newRow("Requested - Allowed by: SDK") << true << true << false << false << true;
			QTest::newRow("Requested - Allowed by: SDK, OD") << true << true << false << true << true;
			QTest::newRow("Requested - Allowed by: SDK, Settings") << true << true << true << false << true;
			QTest::newRow("Requested - Allowed by: SDK, Settings,  OD") << true << true << true << true << true;
		}


		void test_canAllowed()
		{
			QFETCH(bool, requested);
			QFETCH(bool, isUsedAsSDK);
			QFETCH(bool, canAllowedEnabled);
			QFETCH(bool, officialDomestic);
			QFETCH(bool, canAllowed);

			Env::getSingleton<VolatileSettings>()->setUsedAsSDK(isUsedAsSDK);
			Env::getSingleton<AppSettings>()->getGeneralSettings().setEnableCanAllowed(canAllowedEnabled);

			QString file;
			if (requested)
			{
				if (officialDomestic)
				{
					file = ":/paos/DIDAuthenticateEAC1_official_domestic_can.xml"_L1;
				}
				else
				{
					file = ":/paos/DIDAuthenticateEAC1.xml"_L1;
				}
			}
			else
			{
				if (officialDomestic)
				{
					file = ":/paos/DIDAuthenticateEAC1_official_domestic.xml"_L1;
				}
				else
				{
					file = ":/paos/DIDAuthenticateEAC1_3.xml"_L1;
				}
			}
			const auto& content = TestFileHelper::readFile(file);
			QSharedPointer<DIDAuthenticateEAC1> eac1(static_cast<DIDAuthenticateEAC1*>(DidAuthenticateEac1Parser().parse(content)));

			const auto terminalCvc = eac1->getCvCertificates({AccessRole::AT}).at(0);
			QCOMPARE(terminalCvc->getBody().getCHAT().getAccessRights().contains(AccessRight::CAN_ALLOWED), requested);
			const auto dvCvc = eac1->getCvCertificates({AccessRole::DV_no_f, AccessRole::DV_od}).at(0);
			QCOMPARE(dvCvc->getBody().getCHAT().getAccessRole(), officialDomestic ? AccessRole::DV_od : AccessRole::DV_no_f);

			AccessRightManager manager(eac1, terminalCvc, dvCvc);
			QCOMPARE(manager.getOptionalAccessRights().contains(AccessRight::CAN_ALLOWED), canAllowed);
		}


		void test_AuthContext_00()
		{
			TestAuthContext context(":/paos/DIDAuthenticateEAC1_accessRightsMissing.xml"_L1);
			QCOMPARE(context.getAccessRightManager()->getRequiredAccessRights().size(), 0);
			QCOMPARE(context.getAccessRightManager()->getOptionalAccessRights().size(), 16);
			QCOMPARE(context.getAccessRightManager()->getEffectiveAccessRights().size(), 16);
		}


		void test_AuthContext_01()
		{
			// Empty after creation
			checkChatSizes("01-01"_L1, 0, 0, 0);
		}


		void test_AuthContext_02()
		{
			// Try to set required right that is not in cvc
			mTestAuthContext->setRequiredAccessRights({AccessRight::READ_DG21});
			checkChatSizes("02-01"_L1, 0, 0, 0);
			checkAndClearSignals("02-02"_L1, 1);

			// Try to set optional right that is not in cvc
			mTestAuthContext->setOptionalAccessRights({AccessRight::READ_DG21});
			checkChatSizes("02-03"_L1, 0, 0, 0);
			checkAndClearSignals("02-04"_L1, 1);

			// Try to set a right in required and optional
			mTestAuthContext->setOptionalAccessRights({AccessRight::READ_DG05});
			checkChatSizes("02-05"_L1, 0, 1, 1);
			checkAndClearSignals("02-06"_L1, 1);
			mTestAuthContext->setRequiredAccessRights({AccessRight::READ_DG05});
			checkChatSizes("02-07"_L1, 1, 0, 1);
			checkAndClearSignals("02-08"_L1, 1);
		}


		void test_AuthContext_03()
		{
			mTestAuthContext->setOptionalAccessRights({AccessRight::READ_DG05});
			mTestAuthContext->setRequiredAccessRights({AccessRight::READ_DG06});
			checkChatSizes("03-01"_L1, 1, 1, 2);
			checkAndClearSignals("03-02"_L1, 2);

			// Clear effective access rights
			*mTestAuthContext->getAccessRightManager() = {};
			checkChatSizes("03-03"_L1, 1, 1, 1);
			checkAndClearSignals("03-04"_L1, 0);
		}


		void test_AuthContext_04()
		{
			// Set only one access right as optional, required access rights remain empty
			mTestAuthContext->setOptionalAccessRights({AccessRight::READ_DG05});
			checkChatSizes("04-01"_L1, 0, 1, 1);
			QVERIFY(mTestAuthContext->getAccessRightManager()->getOptionalAccessRights().contains(AccessRight::READ_DG05));
			QVERIFY(mTestAuthContext->getAccessRightManager()->getEffectiveAccessRights().contains(AccessRight::READ_DG05));
			checkAndClearSignals("04-02"_L1, 1);

			// Try to set an optional access right that is already set
			*mTestAuthContext->getAccessRightManager() += AccessRight::READ_DG05;
			checkChatSizes("04-03"_L1, 0, 1, 1);
			QVERIFY(mTestAuthContext->getAccessRightManager()->getOptionalAccessRights().contains(AccessRight::READ_DG05));
			QVERIFY(mTestAuthContext->getAccessRightManager()->getEffectiveAccessRights().contains(AccessRight::READ_DG05));
			checkAndClearSignals("04-04"_L1, 0);

			// Try to remove an optional access right
			*mTestAuthContext->getAccessRightManager() -= AccessRight::READ_DG05;
			checkChatSizes("04-05"_L1, 0, 1, 0);
			QVERIFY(mTestAuthContext->getAccessRightManager()->getOptionalAccessRights().contains(AccessRight::READ_DG05));
			checkAndClearSignals("04-06"_L1, 0);

			// Try to remove an optional access right that is not set
			*mTestAuthContext->getAccessRightManager() -= AccessRight::READ_DG05;
			checkChatSizes("04-07"_L1, 0, 1, 0);
			QVERIFY(mTestAuthContext->getAccessRightManager()->getOptionalAccessRights().contains(AccessRight::READ_DG05));
			checkAndClearSignals("04-08"_L1, 0);

			// Try to set an optional access right
			*mTestAuthContext->getAccessRightManager() += AccessRight::READ_DG05;
			checkChatSizes("04-09"_L1, 0, 1, 1);
			QVERIFY(mTestAuthContext->getAccessRightManager()->getOptionalAccessRights().contains(AccessRight::READ_DG05));
			QVERIFY(mTestAuthContext->getAccessRightManager()->getEffectiveAccessRights().contains(AccessRight::READ_DG05));
			checkAndClearSignals("04-10"_L1, 0);

			// Try to set an access right that is not included in optional or required access rights
			QTest::ignoreMessage(QtWarningMsg, "Adding AGE_VERIFICATION to effective chat is not allowed");
			*mTestAuthContext->getAccessRightManager() += AccessRight::AGE_VERIFICATION;
			checkChatSizes("04-11"_L1, 0, 1, 1);
			QVERIFY(mTestAuthContext->getAccessRightManager()->getOptionalAccessRights().contains(AccessRight::READ_DG05));
			QVERIFY(mTestAuthContext->getAccessRightManager()->getEffectiveAccessRights().contains(AccessRight::READ_DG05));
			checkAndClearSignals("04-12"_L1, 0);
		}


		void test_AuthContext_05()
		{
			// Set only one access right as required, optional access rights remain empty
			mTestAuthContext->setRequiredAccessRights({AccessRight::READ_DG05});
			checkChatSizes("05-01"_L1, 1, 0, 1);
			QVERIFY(mTestAuthContext->getAccessRightManager()->getRequiredAccessRights().contains(AccessRight::READ_DG05));
			QVERIFY(mTestAuthContext->getAccessRightManager()->getEffectiveAccessRights().contains(AccessRight::READ_DG05));
			checkAndClearSignals("05-02"_L1, 1);

			// Try to set an required access right that is already set
			*mTestAuthContext->getAccessRightManager() += AccessRight::READ_DG05;
			checkChatSizes("05-03"_L1, 1, 0, 1);
			QVERIFY(mTestAuthContext->getAccessRightManager()->getRequiredAccessRights().contains(AccessRight::READ_DG05));
			QVERIFY(mTestAuthContext->getAccessRightManager()->getEffectiveAccessRights().contains(AccessRight::READ_DG05));
			checkAndClearSignals("05-04"_L1, 0);

			// Try to remove an required access right
			QTest::ignoreMessage(QtWarningMsg, "Removing READ_DG05 from effective chat is not allowed");
			*mTestAuthContext->getAccessRightManager() -= AccessRight::READ_DG05;
			checkChatSizes("05-05"_L1, 1, 0, 1);
			QVERIFY(mTestAuthContext->getAccessRightManager()->getRequiredAccessRights().contains(AccessRight::READ_DG05));
			QVERIFY(mTestAuthContext->getAccessRightManager()->getEffectiveAccessRights().contains(AccessRight::READ_DG05));
			checkAndClearSignals("05-06"_L1, 0);

			// Try to remove an access right that is not included in optional or required access rights
			*mTestAuthContext->getAccessRightManager() -= AccessRight::AGE_VERIFICATION;
			checkChatSizes("05-07"_L1, 1, 0, 1);
			QVERIFY(mTestAuthContext->getAccessRightManager()->getRequiredAccessRights().contains(AccessRight::READ_DG05));
			QVERIFY(mTestAuthContext->getAccessRightManager()->getEffectiveAccessRights().contains(AccessRight::READ_DG05));
			checkAndClearSignals("05-08"_L1, 0);
		}


		void test_Testbed_C2_01()
		{
			TestAuthContext context(":/testbed/DIDAuthenticateEAC1_C2_01.xml"_L1);
			*context.getAccessRightManager() -= AccessRight::CAN_ALLOWED;
			QVERIFY(context.encodeEffectiveChat().toHex().endsWith("3c0f13ffe4"));
		}


		void test_Testbed_C2_02()
		{
			TestAuthContext context(":/testbed/DIDAuthenticateEAC1_C2_02.xml"_L1);
			*context.getAccessRightManager() = {};
			QVERIFY(context.encodeEffectiveChat().toHex().endsWith("0000000000"));
		}


};

QTEST_GUILESS_MAIN(test_AccessRightManager)
#include "test_AccessRightManager.moc"
