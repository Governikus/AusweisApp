/*!
 * \brief Generate information for DIDAuthenticateResponseEAC2.
 *
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "PaosCreator.h"
#include "paos/ResponseType.h"

#include <QString>

namespace governikus
{

class DIDAuthenticateResponseEAC2
	: public PaosCreator
	, public ResponseType
{
	Q_DISABLE_COPY(DIDAuthenticateResponseEAC2)

	private:
		QByteArray mEfCardSecurity;
		QByteArray mAuthenticationToken;
		QByteArray mNonce;
		QByteArray mChallenge;

		void createDIDAuthenticateResponseEAC2Element();
		void createAuthenticationProtocolDataElement();

		void createBodyElement() override;

	public:
		DIDAuthenticateResponseEAC2();

		void setAuthenticationToken(const QByteArray& pAuthenticationToken);
		void setEfCardSecurity(const QByteArray& pEfCardSecurity);
		void setNonce(const QByteArray& pNonce);
		void setChallenge(const QByteArray& pChallenge);
};

} // namespace governikus
