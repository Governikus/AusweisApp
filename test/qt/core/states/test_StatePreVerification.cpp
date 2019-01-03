/*!
 * \brief Unit tests for \ref StatePreVerification
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
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
			connect(this, &test_StatePreVerification::fireStateStart, mState.data(), &AbstractState::onEntry, Qt::ConnectionType::DirectConnection);
		}


		void cleanup()
		{
			mAuthContext.clear();
			mState.reset();
		}


		void testNoCertChain()
		{
			QSignalSpy spy(mState.data(), &StatePreVerification::fireAbort);
			Q_EMIT fireStateStart(nullptr);
			mAuthContext->setStateApproved();

			QCOMPARE(spy.count(), 1);
		}


		void testCvcaNotTrusted()
		{
			const_cast<QVector<QSharedPointer<const CVCertificate> >*>(&mState->mTrustedCvcas)->clear();

			QSignalSpy spy(mState.data(), &StatePreVerification::fireAbort);
			Q_EMIT fireStateStart(nullptr);
			mAuthContext->setStateApproved();

			QCOMPARE(spy.count(), 1);
		}


		void testSignatureInvalid()
		{
			const_cast<QDateTime*>(&mState->mValidationDateTime)->setDate(QDate(2013, 12, 1));
			auto signature = mAuthContext->mDIDAuthenticateEAC1->mEac1InputType.mCvCertificates.at(0)->getEcdsaSignature();
#if OPENSSL_VERSION_NUMBER < 0x10100000L || defined(LIBRESSL_VERSION_NUMBER)
			BIGNUM* signaturePart = signature->r;
#else
			const BIGNUM* signaturePart = nullptr;
			ECDSA_SIG_get0(signature.data(), &signaturePart, nullptr);
#endif
			BN_pseudo_rand(const_cast<BIGNUM*>(signaturePart), BN_num_bits(signaturePart), 0, 0);

			QSignalSpy spy(mState.data(), &StatePreVerification::fireAbort);
			Q_EMIT fireStateStart(nullptr);
			mAuthContext->setStateApproved();

			QCOMPARE(spy.count(), 1);
		}


		void testNotYetValid()
		{
			const_cast<QDateTime*>(&mState->mValidationDateTime)->setDate(QDate(2013, 11, 1));

			QSignalSpy spy(mState.data(), &StatePreVerification::fireAbort);
			Q_EMIT fireStateStart(nullptr);
			mAuthContext->setStateApproved();

			QCOMPARE(spy.count(), 1);
		}


		void testValid()
		{
			const_cast<QDateTime*>(&mState->mValidationDateTime)->setDate(QDate(2013, 12, 1));

			QSignalSpy spy(mState.data(), &StatePreVerification::fireContinue);
			Q_EMIT fireStateStart(nullptr);
			mAuthContext->setStateApproved();

			QCOMPARE(spy.count(), 1);
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

			const int expectedCvcaSize = 12;
			QCOMPARE(mState->mTrustedCvcas.size(), expectedCvcaSize);
			const_cast<QDateTime*>(&mState->mValidationDateTime)->setDate(QDate(2013, 12, 1));
			auto& trustedCvcas = const_cast<QVector<QSharedPointer<const CVCertificate> >&>(mState->mTrustedCvcas);

			remove(trustedCvcas, mAuthContext->mDIDAuthenticateEAC1->mEac1InputType.mCvCertificates.at(3));
			remove(trustedCvcas, mAuthContext->mDIDAuthenticateEAC1->mEac1InputType.mCvCertificates.at(2));

			QCOMPARE(mState->mTrustedCvcas.size(), expectedCvcaSize - 2);

			QSignalSpy spy(mState.data(), &StatePreVerification::fireContinue);
			Q_EMIT fireStateStart(nullptr);
			mAuthContext->setStateApproved();

			QCOMPARE(spy.count(), 1);
			QCOMPARE(settings.getLinkCertificates().size(), 2);
		}


};

QTEST_GUILESS_MAIN(test_StatePreVerification)
#include "test_StatePreVerification.moc"
