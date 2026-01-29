/**
 * Copyright (c) 2014-2026 Governikus GmbH & Co. KG, Germany
 */

#include "CVCertificateChain.h"

#include "SecureStorage.h"


using namespace governikus;


CVCertificateChain::CVCertificateChain()
	: CVCertificateChain(QList<QSharedPointer<const CVCertificate>>())
{
}


CVCertificateChain::CVCertificateChain(const QList<QSharedPointer<const CVCertificate>>& pCvcs)
	: QList<QSharedPointer<const CVCertificate>>(pCvcs)
{
	if (!isValid())
	{
		clear();
	}
}


QSharedPointer<const CVCertificate> CVCertificateChain::getTerminalCvc() const
{
	Q_ASSERT(length() > 0);
	return at(length() - 1);
}


QSharedPointer<const CVCertificate> CVCertificateChain::getDvCvc() const
{
	Q_ASSERT(length() > 1);
	return at(length() - 2);
}


bool CVCertificateChain::isValid() const
{
	if (length() < 2)
	{
		return false;
	}

	AccessRole terminalCvcRole = getTerminalCvc()->getBody().getCHAT().getAccessRole();
	AccessRole dvCvcRole = getDvCvc()->getBody().getCHAT().getAccessRole();
	return (terminalCvcRole == AccessRole::AT) && (dvCvcRole == AccessRole::DV_od || dvCvcRole == AccessRole::DV_no_f);
}


bool CVCertificateChain::isProductive() const
{
	if (isEmpty())
	{
		return false;
	}

	const auto& prod = CVCertificate::fromRaw(Env::getSingleton<SecureStorage>()->getCVRootCertificates(true));
	return prod.contains(at(0));
}
