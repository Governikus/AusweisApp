/*!
 * \brief Command to perform the DID Authenticate EAC2 process.
 *
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "asn1/CVCertificateChain.h"
#include "BaseCardCommand.h"

class test_CardConnection;
class test_DidAuthenticateEAC2Command;

namespace governikus
{

class DidAuthenticateEAC2Command
	: public BaseCardCommand
{
	Q_OBJECT

	private:
		friend class ::test_CardConnection;
		friend class ::test_DidAuthenticateEAC2Command;
		CVCertificateChain mCvcChain;
		QByteArray mEphemeralPublicKeyAsHex;
		QByteArray mSignatureAsHex;
		QByteArray mAuthenticatedAuxiliaryDataAsBinary;
		QByteArray mEfCardSecurityAsHex;
		QByteArray mNonceAsHex;
		QByteArray mAuthTokenAsHex;

		CardReturnCode putCertificateChain(const CVCertificateChain& pCvcChain);
		CardReturnCode performTerminalAuthentication(const QByteArray& taProtocol,
				const QByteArray& chr,
				const QByteArray& auxiliaryData,
				const QByteArray& compressedEphemeralPublicKey,
				const QByteArray& signature);
		CardReturnCode performChipAuthentication(QSharedPointer<const ChipAuthenticationInfo> pChipAuthInfo,
				const QByteArray& ephemeralPublicKey);

	protected:
		void internalExecute() override;
		~DidAuthenticateEAC2Command() override = default;

	public:
		explicit DidAuthenticateEAC2Command(QSharedPointer<CardConnectionWorker> pCardConnectionWorker,
				const CVCertificateChain& pCvcChain, const QByteArray& pEphemeralPublicKeyAsHex,
				const QByteArray& pSignatureAsHex, const QByteArray& pAuthenticatedAuxiliaryDataAsBinary);


		[[nodiscard]] const QByteArray& getEfCardSecurityAsHex() const
		{
			return mEfCardSecurityAsHex;
		}


		[[nodiscard]] const QByteArray& getNonceAsHex() const
		{
			return mNonceAsHex;
		}


		[[nodiscard]] const QByteArray& getAuthTokenAsHex() const
		{
			return mAuthTokenAsHex;
		}


};

} // namespace governikus
