/**
 * Copyright (c) 2014-2026 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "CVCertificate.h"
#include "CVCertificateChain.h"
#include "pinpad/EstablishPaceChannelOutput.h"

class test_CVCertificateChainBuilder;

namespace governikus
{

class CVCertificateChainBuilder
{
	friend class ::test_CVCertificateChainBuilder;

	private:
		QList<QList<QSharedPointer<const CVCertificate>>> mChains;

		void buildChains(
			const QMultiMap<QByteArray, QSharedPointer<const CVCertificate>>& pPool,
			const QList<QSharedPointer<const CVCertificate>>& pChain);

		[[nodiscard]] CVCertificateChain getChainForCertificationAuthority(const QByteArray& pCar) const;

	public:
		explicit CVCertificateChainBuilder();

		/*!
		 * Creates a new instance. All chains are build using the CVCs passed in as parameter.
		 */
		explicit CVCertificateChainBuilder(const QList<QSharedPointer<const CVCertificate>>& pCvcPool);


		/*!
		 * Get a chain with root CVC passed in as parameter.
		 * The chain is returned top down, i.e. the chain root is at position 0 in the list,
		 * the end entity CVC (the terminal CVC) is the last element in the list.
		 *
		 * If no chain starting with the given root CVC could be found or any other error
		 * occurred ( e.g. the last element in the list is no terminal CVC), an invalid chain
		 * is returned (see CVCertificateChain::isValid()).
		 */
		[[nodiscard]] CVCertificateChain getChainStartingWith(const QSharedPointer<const CVCertificate>& pChainRoot) const;


		/*!
		 * Get a chain with root having Certification Authority Reference (CAR) equal to one of the
		 * CARs in the passed parameter EstablishPaceChannelOutput. The chain root will not be a self
		 * signed CVCA.
		 * The chain is returned top down, i.e. the chain root is at position 0 in the list,
		 * the end entity CVC (the terminal CVC) is the last element in the list.
		 *
		 * If no chain starting with an appropriate root CVC could be found or any other error
		 * occurred ( e.g. the last element in the list is no terminal CVC), an invalid chain is
		 * returned (see CVCertificateChain::isValid()).
		 */
		[[nodiscard]] CVCertificateChain getChainForCertificationAuthority(const EstablishPaceChannelOutput& pPaceOutput) const;
};

} // namespace governikus
