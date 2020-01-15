/*
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#include "CVCertificateChain.h"


using namespace governikus;


CVCertificateChain::CVCertificateChain(bool pProductive)
	: CVCertificateChain(QVector<QSharedPointer<const CVCertificate> >(), pProductive)
{
}


CVCertificateChain::CVCertificateChain(const QVector<QSharedPointer<const CVCertificate> >& pCvcs, bool pProductive)
	: QVector<QSharedPointer<const CVCertificate> >(pCvcs)
	, mProductive(pProductive)
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
	return mProductive;
}
