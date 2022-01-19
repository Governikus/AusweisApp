/*!
 * \brief Unit tests for \ref StatePreVerification
 *
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#include "states/StatePreVerification.h"

#include "AppSettings.h"

#include "TestAuthContext.h"

#include <openssl/bn.h>
#include <openssl/ecdsa.h>
#include <QtCore>
#include <QtTest>


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
		void init()
		{
			AbstractSettings::mTestDir.clear();
			mAuthContext.reset(new TestAuthContext(nullptr, ":/paos/DIDAuthenticateEAC1.xml"));

			mAuthContext->initCvcChainBuilder();

			mState.reset(new StatePreVerification(mAuthContext));
			mState->setStateName("StatePreVerification");
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
		}


		void testExpired()
		{
			const_cast<QDateTime*>(&mState->mValidationDateTime)->setDate(QDate(2020, 06, 22));

			QSignalSpy spy(mState.data(), &StatePreVerification::fireAbort);
			mAuthContext->setStateApproved();

			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
		}


		void testCvcaNotTrusted()
		{
			const_cast<QVector<QSharedPointer<const CVCertificate> >*>(&mState->mTrustedCvcas)->clear();

			QSignalSpy spy(mState.data(), &StatePreVerification::fireAbort);
			mAuthContext->setStateApproved();

			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
		}


		void testSignatureInvalid()
		{
			const_cast<QDateTime*>(&mState->mValidationDateTime)->setDate(QDate(2020, 05, 25));
			auto signature = mAuthContext->getDidAuthenticateEac1()->getCvCertificates().at(0)->getEcdsaSignature();
#if OPENSSL_VERSION_NUMBER < 0x10100000L || defined(LIBRESSL_VERSION_NUMBER)
			BIGNUM* signaturePart = signature->r;
#else
			const BIGNUM* signaturePart = nullptr;
			ECDSA_SIG_get0(signature, &signaturePart, nullptr);
#endif
			BN_rand(const_cast<BIGNUM*>(signaturePart), BN_num_bits(signaturePart), 0, 0);

			QSignalSpy spy(mState.data(), &StatePreVerification::fireAbort);
			mAuthContext->setStateApproved();

			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
		}


		void testNotYetValid()
		{
			const_cast<QDateTime*>(&mState->mValidationDateTime)->setDate(QDate(2020, 04, 25));

			QSignalSpy spy(mState.data(), &StatePreVerification::fireAbort);
			mAuthContext->setStateApproved();

			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
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
			const auto& remove = [](QVector<QSharedPointer<const CVCertificate> >& pVector, const QSharedPointer<const CVCertificate>& pCert)
					{
						QMutableVectorIterator<QSharedPointer<const CVCertificate> > iter(pVector);
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
			settings.save();

			const int expectedCvcaSize = 15;
			QCOMPARE(mState->mTrustedCvcas.size(), expectedCvcaSize);
			const_cast<QDateTime*>(&mState->mValidationDateTime)->setDate(QDate(2020, 05, 25));
			auto& trustedCvcas = const_cast<QVector<QSharedPointer<const CVCertificate> >&>(mState->mTrustedCvcas);

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
