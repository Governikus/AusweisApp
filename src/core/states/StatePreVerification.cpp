/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "StatePreVerification.h"

#include "asn1/CVCertificateChainBuilder.h"
#include "asn1/SignatureChecker.h"
#include "AppSettings.h"
#include "EnumHelper.h"

#include <QVector>


using namespace governikus;


StatePreVerification::StatePreVerification(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractGenericState(pContext)
	, mTrustedCvcas(CVCertificate::fromHex(AppSettings::getInstance().getSecureStorage().getCVRootCertificates(true))
			+ CVCertificate::fromHex(AppSettings::getInstance().getSecureStorage().getCVRootCertificates(false)))
	, mValidationDateTime(QDateTime::currentDateTime())
{
}


void StatePreVerification::run()
{
	Q_ASSERT(getContext()->getDidAuthenticateEac1());

	qDebug() << "Try to build a cvc chain for one of the known trust points";
	CVCertificateChain certificateChain;
	for (const auto& trustPoint : mTrustedCvcas)
	{
		if (!trustPoint->isIssuedBy(*trustPoint))
		{
			// want a chain with self signed root, because the SignatureChecker needs it
			continue;
		}
		certificateChain = getContext()->getChainStartingWith(trustPoint);
		if (certificateChain.isValid())
		{
			break;
		}
	}

	if (AppSettings::getInstance().getGeneralSettings().isDeveloperMode())
	{
		if (certificateChain.isProductive())
		{
			qCritical() << "Using the developer mode is only allowed in a test environment";
			setStatus(GlobalStatus::Code::Workflow_Preverification_Developermode_Error);
			Q_EMIT fireError();
			return;
		}
	}

	if (!AppSettings::getInstance().getPreVerificationSettings().isEnabled())
	{
		qInfo() << "Pre-verification is disabled";
		Q_EMIT fireSuccess();
		return;
	}

	if (!certificateChain.isValid())
	{
		qCritical() << "Pre-verification failed: cannot build certificate chain";
		setStatus(GlobalStatus::Code::Workflow_Preverification_Error);
		Q_EMIT fireError();
		return;
	}
	else if (!SignatureChecker(certificateChain).check())
	{
		qCritical() << "Pre-verification failed: signature check failed";
		setStatus(GlobalStatus::Code::Workflow_Preverification_Error);
		Q_EMIT fireError();
		return;
	}
	else if (!isValid(certificateChain))
	{
		qCritical() << "Pre-verification failed: certificate not valid ";
		setStatus(GlobalStatus::Code::Workflow_Preverification_Error);
		Q_EMIT fireError();
		return;
	}

	saveCvcaLinkCertificates(certificateChain);

	Q_EMIT fireSuccess();
}


bool StatePreVerification::isValid(const QVector<QSharedPointer<CVCertificate> >& pCertificates)
{
	qDebug() << "Check certificate chain validity on" << mValidationDateTime.toString(Qt::ISODate);

	QVectorIterator<QSharedPointer<CVCertificate> > i(pCertificates);
	i.toBack();
	while (i.hasPrevious())
	{
		auto cert = i.previous();
		const QDate& expirationDate = cert->getBody().getCertificateExpirationDate();
		const QDate& effectiveDate = cert->getBody().getCertificateEffectiveDate();
		const QByteArray certInfo = cert->getBody().getCertificateHolderReference();

		qDebug() << "CVC" << certInfo
				 << cert->getBody().getCHAT().getAccessRole()
				 << "| valid from/to"
				 << '[' << effectiveDate.toString(Qt::ISODate) << ',' << expirationDate.toString(Qt::ISODate) << ']';

		if (!cert->isValidOn(mValidationDateTime))
		{
			return false;
		}

		if (cert->getBody().getCHAT().getAccessRole() == AccessRole::CVCA)
		{
			// only validate up to first CVCA
			break;
		}
	}
	return true;
}


void StatePreVerification::saveCvcaLinkCertificates(const QVector<QSharedPointer<CVCertificate> >& pCertificates)
{
	for (const auto& certificate : pCertificates)
	{
		if (certificate->getBody().getCHAT().getAccessRole() == AccessRole::CVCA && !mTrustedCvcas.contains(certificate))
		{
			qInfo() << "Save link certificate" << *certificate;
			AppSettings::getInstance().getPreVerificationSettings().addLinkCertificate(certificate->encode().toHex());
		}
	}
	AppSettings::getInstance().getPreVerificationSettings().save();
}
