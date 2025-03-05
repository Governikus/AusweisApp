/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#include "states/StatePreVerification.h"

#include "AppSettings.h"
#include "Randomizer.h"
#include "ResourceLoader.h"

#include "TestAuthContext.h"

#include <QtCore>
#include <QtTest>


using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class test_StatePreVerification
	: public QObject
{
	Q_OBJECT
	QScopedPointer<StatePreVerification> mState;
	QSharedPointer<AuthContext> mAuthContext;

	Q_SIGNALS:
		void fireStateStart(QEvent* pEvent);

	private Q_SLOTS:
		void initTestCase()
		{
			ResourceLoader::getInstance().init();
		}


		void init()
		{
			AbstractSettings::mTestDir.clear();
			mAuthContext.reset(new TestAuthContext(":/paos/DIDAuthenticateEAC1.xml"_L1));

			mAuthContext->initCvcChainBuilder();

			mState.reset(StateBuilder::createState<StatePreVerification>(mAuthContext));
			mState->onEntry(nullptr);
		}


		void cleanup()
		{
			mAuthContext.clear();
			mState.reset();
		}


		void testTimeZone_data()
		{
			QTest::addColumn<bool>("isValid");
			QTest::addColumn<QDateTime>("dateTime");

			QTest::newRow("") << false << QDateTime(QDate(2020, 05, 20), QTime(23, 30), QTimeZone(0));
			QTest::newRow("") << true << QDateTime(QDate(2020, 05, 20), QTime(23, 30), QTimeZone(-3600));
			QTest::newRow("") << false << QDateTime(QDate(2020, 05, 21), QTime(0, 30), QTimeZone(3600));
			QTest::newRow("") << true << QDateTime(QDate(2020, 05, 21), QTime(0, 30), QTimeZone(0));

			QTest::newRow("") << true << QDateTime(QDate(2020, 06, 20), QTime(23, 30), QTimeZone(0));
			QTest::newRow("") << false << QDateTime(QDate(2020, 06, 20), QTime(23, 30), QTimeZone(-3600));
			QTest::newRow("") << true << QDateTime(QDate(2020, 06, 21), QTime(0, 30), QTimeZone(3600));
			QTest::newRow("") << false << QDateTime(QDate(2020, 06, 21), QTime(0, 30), QTimeZone(0));
		}


		void testTimeZone()
		{
			QFETCH(bool, isValid);
			QFETCH(QDateTime, dateTime);

			*const_cast<QDateTime*>(&mState->mValidationDateTime) = dateTime;

			QSignalSpy spyContinue(mState.data(), &StatePreVerification::fireContinue);
			QSignalSpy spyAbort(mState.data(), &StatePreVerification::fireAbort);
			mAuthContext->setStateApproved();

			QTRY_COMPARE(spyContinue.count(), isValid ? 1 : 0); // clazy:exclude=qstring-allocations
			QTRY_COMPARE(spyAbort.count(), isValid ? 0 : 1); // clazy:exclude=qstring-allocations
			if (!isValid)
			{
				QCOMPARE(mAuthContext->getFailureCode(), FailureCode::Reason::Pre_Verification_Certificate_Expired);
			}
		}


		void testExpired()
		{
			const_cast<QDateTime*>(&mState->mValidationDateTime)->setDate(QDate(2020, 06, 22));

			QSignalSpy spy(mState.data(), &StatePreVerification::fireAbort);
			mAuthContext->setStateApproved();

			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(mAuthContext->getFailureCode(), FailureCode::Reason::Pre_Verification_Certificate_Expired);
		}


		void testCvcaNotTrusted()
		{
			const_cast<QList<QSharedPointer<const CVCertificate>>*>(&mState->mTrustedCvcas)->clear();

			QSignalSpy spy(mState.data(), &StatePreVerification::fireAbort);
			mAuthContext->setStateApproved();

			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(mAuthContext->getFailureCode(), FailureCode::Reason::Pre_Verification_Invalid_Certificate_Chain);
		}


		void testSignatureInvalid()
		{
			const_cast<QDateTime*>(&mState->mValidationDateTime)->setDate(QDate(2020, 05, 25));
			auto* signature = mAuthContext->getDidAuthenticateEac1()->getCvCertificates().at(0)->mSignature;

			QCOMPARE(Asn1OctetStringUtil::getValue(signature).size(), 64);
			Asn1OctetStringUtil::setValue(Randomizer::getInstance().createBytes(64), signature);

			QSignalSpy spy(mState.data(), &StatePreVerification::fireAbort);
			mAuthContext->setStateApproved();

			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(mAuthContext->getFailureCode(), FailureCode::Reason::Pre_Verification_Invalid_Certificate_Signature);
		}


		void testNotYetValid()
		{
			const_cast<QDateTime*>(&mState->mValidationDateTime)->setDate(QDate(2020, 04, 25));

			QSignalSpy spy(mState.data(), &StatePreVerification::fireAbort);
			mAuthContext->setStateApproved();

			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(mAuthContext->getFailureCode(), FailureCode::Reason::Pre_Verification_Certificate_Expired);
		}


		void testValid()
		{
			const_cast<QDateTime*>(&mState->mValidationDateTime)->setDate(QDate(2020, 05, 25));

			QSignalSpy spy(mState.data(), &StatePreVerification::fireContinue);
			mAuthContext->setStateApproved();

			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
		}


		void testSaveLinkCertificates()
		{
			const auto& remove = [](QList<QSharedPointer<const CVCertificate>>& pVector, const QSharedPointer<const CVCertificate>& pCert)
					{
						QMutableListIterator<QSharedPointer<const CVCertificate>> iter(pVector);
						while (iter.hasNext())
						{
							iter.next();
							if (*iter.value() == *pCert)
							{
								iter.remove();
							}
						}
					};

			auto& settings = Env::getSingleton<AppSettings>()->getPreVerificationSettings();
			const auto& linkCerts = settings.getLinkCertificates();
			for (const QByteArray& cvc : linkCerts)
			{
				settings.removeLinkCertificate(cvc);
			}

			const int expectedCvcaSize = 19;
			QCOMPARE(mState->mTrustedCvcas.size(), expectedCvcaSize);
			const_cast<QDateTime*>(&mState->mValidationDateTime)->setDate(QDate(2020, 05, 25));
			auto& trustedCvcas = const_cast<QList<QSharedPointer<const CVCertificate>>&>(mState->mTrustedCvcas);

			remove(trustedCvcas, mAuthContext->getDidAuthenticateEac1()->getCvCertificates().at(3));
			remove(trustedCvcas, mAuthContext->getDidAuthenticateEac1()->getCvCertificates().at(2));

			QCOMPARE(mState->mTrustedCvcas.size(), expectedCvcaSize - 2);

			QSignalSpy spy(mState.data(), &StatePreVerification::fireContinue);
			mAuthContext->setStateApproved();

			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(settings.getLinkCertificates().size(), 2);
		}


};

QTEST_GUILESS_MAIN(test_StatePreVerification)
#include "test_StatePreVerification.moc"
