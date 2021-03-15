/*!
 * \copyright Copyright (c) 2014-2021 Governikus GmbH & Co. KG, Germany
 */

#include "StatePreVerification.h"

#include "asn1/CVCertificateChainBuilder.h"
#include "asn1/SignatureChecker.h"
#include "AppSettings.h"
#include "EnumHelper.h"
#include "SecureStorage.h"

#include <QVector>

Q_DECLARE_LOGGING_CATEGORY(developermode)

using namespace governikus;


StatePreVerification::StatePreVerification(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext, false)
	, GenericContextContainer(pContext)
	, mTrustedCvcas(CVCertificate::fromHex(Env::getSingleton<SecureStorage>()->getCVRootCertificates(true))
			+ CVCertificate::fromHex(Env::getSingleton<SecureStorage>()->getCVRootCertificates(false)))
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

	const bool developerMode = Env::getSingleton<AppSettings>()->getGeneralSettings().isDeveloperMode();
	if (developerMode && certificateChain.isProductive())
	{
		qCritical() << "Using the developer mode is only allowed in a test environment";
		updateStatus(GlobalStatus::Code::Workflow_Preverification_Developermode_Error);
		Q_EMIT fireAbort();
		return;
	}

	if (!Env::getSingleton<AppSettings>()->getPreVerificationSettings().isEnabled())
	{
		qInfo() << "Pre-verification is disabled";
		Q_EMIT fireContinue();
		return;
	}

	if (!certificateChain.isValid())
	{
		qCritical() << "Pre-verification failed: cannot build certificate chain";
		updateStatus(GlobalStatus::Code::Workflow_Preverification_Error);
		Q_EMIT fireAbort();
		return;
	}
	else if (!SignatureChecker(certificateChain).check())
	{
		qCritical() << "Pre-verification failed: signature check failed";
		updateStatus(GlobalStatus::Code::Workflow_Preverification_Error);
		Q_EMIT fireAbort();
		return;
	}
	else if (!isValid(certificateChain))
	{
		if (developerMode)
		{
			qCCritical(developermode) << "Pre-verification failed: certificate not valid";
		}
		else
		{
			qCritical() << "Pre-verification failed: certificate not valid";
			updateStatus(GlobalStatus::Code::Workflow_Preverification_Error);
			Q_EMIT fireAbort();
			return;
		}
	}

	saveCvcaLinkCertificates(certificateChain);

	Q_EMIT fireContinue();
}


bool StatePreVerification::isValid(const QVector<QSharedPointer<const CVCertificate> >& pCertificates)
{
	qDebug() << "Check certificate chain validity on" << mValidationDateTime.toString(Qt::ISODate);

	QVectorIterator<QSharedPointer<const CVCertificate> > i(pCertificates);
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


void StatePreVerification::saveCvcaLinkCertificates(const QVector<QSharedPointer<const CVCertificate> >& pCertificates)
{
	const auto& contains = [](const QVector<QSharedPointer<const CVCertificate> >& pStore, const CVCertificate& pCert)
			{
				for (const auto& pCertInStore : pStore)
				{
					if (*pCertInStore == pCert)
					{
						return true;
					}
				}
				return false;
			};

	auto& settings = Env::getSingleton<AppSettings>()->getPreVerificationSettings();
	for (const auto& certificate : pCertificates)
	{
		if (certificate->getBody().getCHAT().getAccessRole() == AccessRole::CVCA && !contains(mTrustedCvcas, *certificate))
		{
			qInfo() << "Save link certificate" << *certificate;
			settings.addLinkCertificate(certificate->encode().toHex());
		}
	}
	settings.save();
}
