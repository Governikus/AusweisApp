/*
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#include "CVCertificateChain.h"


using namespace governikus;


CVCertificateChain::CVCertificateChain(bool pProductive)
	: CVCertificateChain(QVector<QSharedPointer<CVCertificate> >(), pProductive)
{
}


CVCertificateChain::CVCertificateChain(const QVector<QSharedPointer<CVCertificate> >& pCvcs, bool pProductive)
	: QVector<QSharedPointer<CVCertificate> >(pCvcs)
	, mProductive(pProductive)
{
	if (!isValid())
	{
		clear();
	}
}


QSharedPointer<CVCertificate> CVCertificateChain::getTerminalCvc() const
{
	Q_ASSERT(length() > 0);
	return at(length() - 1);
}


QSharedPointer<CVCertificate> CVCertificateChain::getDvCvc() const
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
