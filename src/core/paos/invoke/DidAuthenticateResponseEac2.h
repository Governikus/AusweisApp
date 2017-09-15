/*!
 * \brief Generate information for DIDAuthenticateResponseEAC2.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "paos/ResponseType.h"
#include "PaosCreator.h"

#include <QString>

namespace governikus
{

class DIDAuthenticateResponseEAC2
	: public PaosCreator
	, public ResponseType
{
	private:
		QByteArray mEfCardSecurity;
		QByteArray mAuthenticationToken;
		QByteArray mNonce;
		QByteArray mChallenge;

		QDomElement createDIDAuthenticateResponseEAC2Element();
		QDomElement createAuthenticationProtocolDataElement();

		virtual QDomElement getDocumentStructure();

		Q_DISABLE_COPY(DIDAuthenticateResponseEAC2)

	public:
		DIDAuthenticateResponseEAC2();

		void setAuthenticationToken(const QByteArray& pAuthenticationToken);
		void setEfCardSecurity(const QByteArray& pEfCardSecurity);
		void setNonce(const QByteArray& pNonce);
		void setChallenge(const QByteArray& pChallenge);
};

} /* namespace governikus */
