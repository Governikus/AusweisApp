/*
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "CVCertificateChainBuilder.h"

#include <QLoggingCategory>

using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)


bool CVCertificateChainBuilder::isChild(const QSharedPointer<const CVCertificate>& pChild, const QSharedPointer<const CVCertificate>& pParent)
{
	// self signed CVCs are the root of a chain, no other parent possible.
	return !pChild->isIssuedBy(*pChild) && pChild->isIssuedBy(*pParent);
}


CVCertificateChainBuilder::CVCertificateChainBuilder(bool pProductive)
	: CVCertificateChainBuilder(QVector<QSharedPointer<const CVCertificate> >(), pProductive)
{
}


CVCertificateChainBuilder::CVCertificateChainBuilder(const QVector<QSharedPointer<const CVCertificate> >& pCvcPool, bool pProductive)
	: ChainBuilder(pCvcPool, &CVCertificateChainBuilder::isChild)
	, mProductive(pProductive)
{
	removeInvalidChains();

	for (const auto& cvc : pCvcPool)
	{
		qCDebug(card) << "CVC in pool" << cvc;
	}

	if (getChains().isEmpty())
	{
		qCWarning(card) << "No valid chains could be built";
	}
	else
	{
		for (const auto& chain : getChains())
		{
			qCDebug(card) << "Found valid chain" << chain;
		}
	}
}


void CVCertificateChainBuilder::removeInvalidChains()
{
	QMutableVectorIterator<QVector<QSharedPointer<const CVCertificate> > > chainIter(mChains);
	while (chainIter.hasNext())
	{
		if (!CVCertificateChain(chainIter.next(), mProductive).isValid())
		{
			chainIter.remove();
		}
	}
}


CVCertificateChain CVCertificateChainBuilder::getChainForCertificationAuthority(const EstablishPaceChannelOutput& pPaceOutput) const
{
	CVCertificateChain chain = getChainForCertificationAuthority(pPaceOutput.getCARcurr());
	if (!chain.isValid())
	{
		qCWarning(card) << "Cannot get CVC chain for CAR current, try CAR previous";
		chain = getChainForCertificationAuthority(pPaceOutput.getCARprev());
	}
	if (!chain.isValid())
	{
		qCWarning(card) << "Cannot get CVC chain for CAR current and CAR previous";
	}
	return chain;
}


CVCertificateChain CVCertificateChainBuilder::getChainForCertificationAuthority(const QByteArray& pCar) const
{
	qCDebug(card) << "Get chain for authority" << pCar;
	for (const auto& chain : getChains())
	{
		for (int i = 0; i < chain.length(); ++i)
		{
			auto cert = chain.at(i);
			if (cert->getBody().getCertificationAuthorityReference() == pCar && !cert->isIssuedBy(*cert))
			{
				CVCertificateChain subChain(chain.mid(i), mProductive);
				if (subChain.isValid())
				{
					qCDebug(card) << "Found valid chain" << subChain;
					return subChain;
				}
			}
		}
	}
	qCWarning(card) << "Cannot find a valid chain for authority" << pCar;
	return CVCertificateChain(mProductive);
}


CVCertificateChain CVCertificateChainBuilder::getChainStartingWith(const QSharedPointer<const CVCertificate>& pChainRoot) const
{
	qCDebug(card) << "Get chain for root" << pChainRoot;
	for (const auto& chain : getChains())
	{
		for (int i = 0; i < chain.length(); ++i)
		{
			if (*chain.at(i) == *pChainRoot)
			{
				CVCertificateChain subChain(chain.mid(i), mProductive);
				if (subChain.isValid())
				{
					qCDebug(card) << "Found valid chain" << subChain;
					return subChain;
				}
			}
		}
	}
	qCWarning(card) << "Cannot find a valid chain for root" << pChainRoot;
	return CVCertificateChain(mProductive);
}
