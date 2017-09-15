/*!
 * \brief Builder for CVC chains.
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#pragma once

#include "ChainBuilder.h"
#include "CVCertificate.h"
#include "CVCertificateChain.h"
#include "EstablishPACEChannel.h"


namespace governikus
{


class CVCertificateChainBuilder
	: private ChainBuilder<QSharedPointer<CVCertificate> >
{
	private:
		bool mProductive;

		static bool isChild(const QSharedPointer<const CVCertificate>& pChild, const QSharedPointer<const CVCertificate>& pParent);

		void removeInvalidChains();

		CVCertificateChain getChainForCertificationAuthority(const QByteArray& pCar) const;

	public:
		CVCertificateChainBuilder(bool pProductive = true);

		/*!
		 * Creates a new instance. All chains are build using the CVCs passed in as parameter.
		 */
		CVCertificateChainBuilder(const QVector<QSharedPointer<CVCertificate> >& pCvcPool, bool pProductive);


		/*!
		 * Get a chain with root CVC passed in as parameter.
		 * The chain is returned top down, i.e. the chain root is at position 0 in the list,
		 * the end entity CVC (the terminal CVC) is the last element in the list.
		 *
		 * If no chain starting with the given root CVC could be found or any other error
		 * occurred ( e.g. the last element in the list is no terminal CVC), an invalid chain
		 * is returned (see CVCertificateChain::isValid()).
		 */
		CVCertificateChain getChainStartingWith(const QSharedPointer<const CVCertificate>& pChainRoot) const;


		/*!
		 * Get a chain with root having Certification Authority Reference (CAR) equal to one of the
		 * CARs in the passed parameter EstablishPACEChannelOutput. The chain root will not be a self
		 * signed CVCA.
		 * The chain is returned top down, i.e. the chain root is at position 0 in the list,
		 * the end entity CVC (the terminal CVC) is the last element in the list.
		 *
		 * If no chain starting with an appropriate root CVC could be found or any other error
		 * occurred ( e.g. the last element in the list is no terminal CVC), an invalid chain is
		 * returned (see CVCertificateChain::isValid()).
		 */
		CVCertificateChain getChainForCertificationAuthority(const EstablishPACEChannelOutput& pPaceOutput) const;
};

}
