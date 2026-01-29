/**
 * Copyright (c) 2014-2026 Governikus GmbH & Co. KG, Germany
 */

#include "CVCertificateChainBuilder.h"

#include <QLoggingCategory>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)

void CVCertificateChainBuilder::buildChains(
		const QMultiMap<QByteArray, QSharedPointer<const CVCertificate>>& pPool,
		const QList<QSharedPointer<const CVCertificate>>& pChain)
{
	Q_ASSERT(!pChain.isEmpty());

	if (pChain.size() > pPool.size() + 1)
	{
		qCWarning(card) << "Maximum chain length exceeded, potential circular reference";
		return;
	}

	const auto& firstCVC = pChain.first();
	const auto holder = firstCVC->getBody().getCertificateHolderReference();
	const auto authority = firstCVC->getBody().getCertificationAuthorityReference();

	const auto selfSigned = holder == authority;
	const auto parentCVCs = pPool.values(authority);

	if (selfSigned || parentCVCs.isEmpty())
	{
		if (!CVCertificateChain(pChain).isValid())
		{
			qCWarning(card) << "Ignored invalid chain" << pChain;
			return;
		}
		qCDebug(card) << "Found valid chain" << pChain;
		mChains << pChain;
		return;
	}

	for (const auto& parentCVC : std::as_const(parentCVCs))
	{
		auto copy = pChain;
		copy.prepend(parentCVC);
		buildChains(pPool, copy);
	}
}


CVCertificateChainBuilder::CVCertificateChainBuilder()
	: mChains()
{
}


CVCertificateChainBuilder::CVCertificateChainBuilder(const QList<QSharedPointer<const CVCertificate>>& pCvcPool)
	: mChains()
{
	QSet<QSharedPointer<const CVCertificate>> set;
	QMultiMap<QByteArray, QSharedPointer<const CVCertificate>> poolWithoutDuplicates;
	QSharedPointer<const CVCertificate> leafCertificate;
	for (const auto& cvc : pCvcPool)
	{
		if (set.contains(cvc))
		{
			qCDebug(card) << "Ignored duplicate certificate" << cvc;
			continue;
		}

		set.insert(cvc);

		if (cvc->getBody().getCHAT().getAccessRole() == AccessRole::AT)
		{
			if (leafCertificate)
			{
				qCWarning(card) << "More than one terminal certificate";
				return;
			}
			qCDebug(card) << "Identified terminal certificate" << cvc;
			leafCertificate = cvc;
			continue;
		}

		poolWithoutDuplicates.insert(cvc->getBody().getCertificateHolderReference(), cvc);
	}

	if (leafCertificate.isNull())
	{
		qCWarning(card) << "Missing terminal certificate";
		return;
	}

	QList<QSharedPointer<const CVCertificate>> initialChain({leafCertificate});
	buildChains(poolWithoutDuplicates, initialChain);
	if (mChains.isEmpty())
	{
		qCWarning(card) << "No valid chains could be built";
		return;
	}
}


CVCertificateChain CVCertificateChainBuilder::getChainForCertificationAuthority(const EstablishPaceChannelOutput& pPaceOutput) const
{
	CVCertificateChain chain = getChainForCertificationAuthority(pPaceOutput.getCarCurr());
	if (!chain.isValid())
	{
		qCWarning(card) << "Cannot get CVC chain for CAR current, try CAR previous";
		chain = getChainForCertificationAuthority(pPaceOutput.getCarPrev());
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
	for (const auto& chain : std::as_const(mChains))
	{
		for (int i = 0; i < chain.length(); ++i)
		{
			auto cert = chain.at(i);
			if (cert->getBody().getCertificationAuthorityReference() != pCar || cert->isIssuedBy(*cert))
			{
				continue;
			}

			CVCertificateChain subChain(chain.mid(i));
			if (subChain.isValid())
			{
				qCDebug(card) << "Found valid chain" << subChain;
				return subChain;
			}
		}
	}
	qCWarning(card) << "Cannot find a valid chain for authority" << pCar;
	return CVCertificateChain();
}


CVCertificateChain CVCertificateChainBuilder::getChainStartingWith(const QSharedPointer<const CVCertificate>& pChainRoot) const
{
	qCDebug(card) << "Get chain for root" << pChainRoot;
	for (const auto& chain : std::as_const(mChains))
	{
		for (int i = 0; i < chain.length(); ++i)
		{
			if (*chain.at(i) != *pChainRoot)
			{
				continue;
			}

			CVCertificateChain subChain(chain.mid(i));
			if (subChain.isValid())
			{
				qCDebug(card) << "Found valid chain" << subChain;
				return subChain;
			}
		}
	}
	qCWarning(card) << "Cannot find a valid chain for root" << pChainRoot;
	return CVCertificateChain();
}
