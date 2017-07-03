/*!
 * \brief Unit tests for \ref StatePreVerification
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "states/StatePreVerification.h"

#include "AppSettings.h"
#include "TestFileHelper.h"
#include "paos/retrieve/DidAuthenticateEac1.h"
#include "paos/retrieve/DidAuthenticateEac1Parser.h"

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
			AppSettings::getInstance().load();
			mAuthContext.reset(new AuthContext(nullptr));
			QByteArray eac1Content = TestFileHelper::readFile(":/paos/DIDAuthenticateEAC1.xml");
			QSharedPointer<DIDAuthenticateEAC1> didAuthEac1(dynamic_cast<DIDAuthenticateEAC1*>(DidAuthenticateEac1Parser().parse(eac1Content)));
			mAuthContext->setDidAuthenticateEac1(didAuthEac1);

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
			QSignalSpy spy(mState.data(), &StatePreVerification::fireError);
			Q_EMIT fireStateStart(nullptr);
			mAuthContext->setStateApproved();

			QCOMPARE(spy.count(), 1);
		}


		void testCvcaNotTrusted()
		{
			const_cast<QVector<QSharedPointer<CVCertificate> >*>(&mState->mTrustedCvcas)->clear();

			QSignalSpy spy(mState.data(), &StatePreVerification::fireError);
			Q_EMIT fireStateStart(nullptr);
			mAuthContext->setStateApproved();

			QCOMPARE(spy.count(), 1);
		}


		void testSignatureInvalid()
		{
			const_cast<QDateTime*>(&mState->mValidationDateTime)->setDate(QDate(2013, 12, 1));
			BIGNUM* signaturePart = mAuthContext->mDIDAuthenticateEAC1->mEac1InputType.mCvCertificates.at(0)->getEcdsaSignature()->r;
			BN_pseudo_rand(signaturePart, BN_num_bits(signaturePart), 0, 0);

			QSignalSpy spy(mState.data(), &StatePreVerification::fireError);
			Q_EMIT fireStateStart(nullptr);
			mAuthContext->setStateApproved();

			QCOMPARE(spy.count(), 1);
		}


		void testNotYetValid()
		{
			const_cast<QDateTime*>(&mState->mValidationDateTime)->setDate(QDate(2013, 11, 1));

			QSignalSpy spy(mState.data(), &StatePreVerification::fireError);
			Q_EMIT fireStateStart(nullptr);
			mAuthContext->setStateApproved();

			QCOMPARE(spy.count(), 1);
		}


		void testValid()
		{
			const_cast<QDateTime*>(&mState->mValidationDateTime)->setDate(QDate(2013, 12, 1));

			QSignalSpy spy(mState.data(), &StatePreVerification::fireSuccess);
			Q_EMIT fireStateStart(nullptr);
			mAuthContext->setStateApproved();

			QCOMPARE(spy.count(), 1);
		}


		void testSaveLinkCertificates()
		{
			PreVerificationSettings settings;
			settings.load();
			for (const QByteArray& cvc : settings.getLinkCertificates())
			{
				settings.removeLinkCertificate(cvc);
			}
			settings.save();
			const_cast<QDateTime*>(&mState->mValidationDateTime)->setDate(QDate(2013, 12, 1));
			// remove DETESTeID00002_DETESTeID00001
			const_cast<QVector<QSharedPointer<CVCertificate> >*>(&mState->mTrustedCvcas)->removeOne(mAuthContext->mDIDAuthenticateEAC1->mEac1InputType.mCvCertificates.at(3));
			// remove DETESTeID00004_DETESTeID00002
			const_cast<QVector<QSharedPointer<CVCertificate> >*>(&mState->mTrustedCvcas)->removeOne(mAuthContext->mDIDAuthenticateEAC1->mEac1InputType.mCvCertificates.at(2));

			QSignalSpy spy(mState.data(), &StatePreVerification::fireSuccess);
			Q_EMIT fireStateStart(nullptr);
			mAuthContext->setStateApproved();

			QCOMPARE(spy.count(), 1);
			settings.load();
			QCOMPARE(settings.getLinkCertificates().size(), 2);
		}


};

QTEST_GUILESS_MAIN(test_StatePreVerification)
#include "test_StatePreVerification.moc"
