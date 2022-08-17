/*!
 * \brief Unit tests for \ref StateWriteHistory
 *
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateWriteHistory.h"

#include "AppSettings.h"
#include "Env.h"
#include "LanguageLoader.h"
#include "VolatileSettings.h"
#include "states/StateBuilder.h"

#include "MockCardConnectionWorker.h"
#include "TestAuthContext.h"

#include <QByteArrayList>
#include <QtTest>


using namespace governikus;

class test_StateWriteHistory
	: public QObject
{
	Q_OBJECT
	QSharedPointer<TestAuthContext> mContext;
	QSharedPointer<StateWriteHistory> mState;

	private Q_SLOTS:
		void init()
		{
			mContext.reset(new TestAuthContext(nullptr, ":/paos/DIDAuthenticateEAC1_2.xml"));
			mState.reset(StateBuilder::createState<StateWriteHistory>(mContext));
			mState->onEntry(nullptr);
		}


		void cleanup()
		{
			mContext.clear();
			mState.clear();
		}


		void test_RunHistoryDisabled()
		{
			QSignalSpy spyContinue(mState.data(), &StateWriteHistory::fireContinue);
			Env::getSingleton<AppSettings>()->getHistorySettings().setEnabled(false);

			QTest::ignoreMessage(QtDebugMsg, "History disabled");
			mContext->setStateApproved();
			QTRY_COMPARE(spyContinue.count(), 1); // clazy:exclude=qstring-allocations
		}


		void test_RunNoEffectiveAccessRights()
		{
			QSignalSpy spyAbort(mState.data(), &StateWriteHistory::fireAbort);
			Env::getSingleton<AppSettings>()->getHistorySettings().setEnabled(true);
			*mContext->getAccessRightManager() = {};

			QTest::ignoreMessage(QtWarningMsg, "No EAC1 structure or effective CHAT in model.");
			mContext->setStateApproved();
			QTRY_COMPARE(spyAbort.count(), 1); // clazy:exclude=qstring-allocations
		}


		void test_RunNoError()
		{
			SDK_MODE(false);
			auto& historySettings = Env::getSingleton<AppSettings>()->getHistorySettings();
			QSignalSpy spyContinue(mState.data(), &StateWriteHistory::fireContinue);
			historySettings.setEnabled(true);
			*mContext->getAccessRightManager() = {AccessRight::READ_DG01, AccessRight::READ_DG02};

			mContext->setStatus(GlobalStatus::Code::No_Error);
			mContext->setStateApproved();
			QTRY_COMPARE(spyContinue.count(), 1); // clazy:exclude=qstring-allocations
			QVERIFY(historySettings.getHistoryInfos().size() > 0);
			const auto historyInfo = historySettings.getHistoryInfos().at(0);
			QCOMPARE(historyInfo.getSubjectName(), mState->getContext()->getDidAuthenticateEac1()->getCertificateDescription()->getSubjectName());
			QCOMPARE(historyInfo.getSubjectUrl(), mState->getContext()->getDidAuthenticateEac1()->getCertificateDescription()->getSubjectUrl());
			QCOMPARE(historyInfo.getPurpose(), mState->getContext()->getDidAuthenticateEac1()->getCertificateDescription()->getPurpose());
			QVERIFY(historyInfo.getTermOfUsage().contains(mState->getContext()->getDidAuthenticateEac1()->getCertificateDescription()->getTermsOfUsage()));
			qDebug() << historyInfo.getTermOfUsage();
			const auto locale = LanguageLoader::getInstance().getUsedLocale();
			const auto effectiveDate = locale.toString(QDate(2014, 4, 7), QLocale::ShortFormat);
			const auto expirationDate = locale.toString(QDate(2014, 7, 6), QLocale::ShortFormat);
			QVERIFY(historyInfo.getTermOfUsage().endsWith(tr("Validity:\n%1 - %2").arg(effectiveDate, expirationDate)));
			QStringList list = {"DocumentType", "IssuingCountry"};
			qDebug() << historyInfo.getRequestedData();
			QCOMPARE(historyInfo.getRequestedData(), list);
			QCOMPARE(spyContinue.count(), 1);
		}


		void test_SetProgress()
		{
			mState->onEntry(nullptr);
			QCOMPARE(mContext->getProgressValue(), 100);
			QCOMPARE(mContext->getProgressMessage(), tr("Preparing results"));

			mContext->setStatus(GlobalStatus::Code::Workflow_Cancellation_By_User);
			mState->onEntry(nullptr);
			QCOMPARE(mContext->getProgressValue(), 100);
			QCOMPARE(mContext->getProgressMessage(), QString());
		}


		void test_RunCertificateValidity()
		{
			mContext.reset(new TestAuthContext(nullptr, ":/paos/DIDAuthenticateEAC1_ordered_certificates.xml"));
			mState.reset(StateBuilder::createState<StateWriteHistory>(mContext));
			mState->onEntry(nullptr);

			SDK_MODE(false);
			auto& historySettings = Env::getSingleton<AppSettings>()->getHistorySettings();
			QSignalSpy spyContinue(mState.data(), &StateWriteHistory::fireContinue);
			historySettings.setEnabled(true);

			mContext->setStatus(GlobalStatus::Code::No_Error);
			mContext->setStateApproved();
			QTRY_COMPARE(spyContinue.count(), 1); // clazy:exclude=qstring-allocations
			QVERIFY(historySettings.getHistoryInfos().size() > 0);
			const auto historyInfo = historySettings.getHistoryInfos().at(0);
			QCOMPARE(historyInfo.getSubjectName(), mState->getContext()->getDidAuthenticateEac1()->getCertificateDescription()->getSubjectName());
			QCOMPARE(historyInfo.getSubjectUrl(), mState->getContext()->getDidAuthenticateEac1()->getCertificateDescription()->getSubjectUrl());
			QCOMPARE(historyInfo.getPurpose(), mState->getContext()->getDidAuthenticateEac1()->getCertificateDescription()->getPurpose());
			QVERIFY(historyInfo.getTermOfUsage().contains(mState->getContext()->getDidAuthenticateEac1()->getCertificateDescription()->getTermsOfUsage()));
			qDebug() << historyInfo.getTermOfUsage();
			const auto locale = LanguageLoader::getInstance().getUsedLocale();
			const auto effectiveDate = locale.toString(QDate(2020, 5, 21), QLocale::ShortFormat);
			const auto expirationDate = locale.toString(QDate(2020, 6, 20), QLocale::ShortFormat);
			QVERIFY(historyInfo.getTermOfUsage().endsWith(tr("Validity:\n%1 - %2").arg(effectiveDate, expirationDate)));
			QCOMPARE(spyContinue.count(), 1);
		}


};

QTEST_GUILESS_MAIN(test_StateWriteHistory)
#include "test_StateWriteHistory.moc"
