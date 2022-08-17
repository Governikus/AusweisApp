/*!
 * \brief Command to perform the DID Authenticate EAC2 process.
 *
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "BaseCardCommand.h"
#include "asn1/CVCertificateChain.h"

class test_CardConnection;
class test_DidAuthenticateEAC2Command;

namespace governikus
{

class DidAuthenticateEAC2Command
	: public BaseCardCommand
{
	Q_OBJECT
	friend class ::test_CardConnection;
	friend class ::test_DidAuthenticateEAC2Command;

	private:
		CVCertificateChain mCvcChain;
		QByteArray mEphemeralPublicKeyAsHex;
		QByteArray mSignatureAsHex;
		QByteArray mAuthenticatedAuxiliaryDataAsBinary;
		QByteArray mPin;
		QByteArray mEfCardSecurityAsHex;
		QByteArray mNonceAsHex;
		QByteArray mAuthTokenAsHex;

		CardReturnCode putCertificateChain(const CVCertificateChain& pCvcChain);
		CardReturnCode performTerminalAuthentication(const Oid& pTaProtocol,
				const QByteArray& pChr,
				const QByteArray& pAuxiliaryData,
				const QByteArray& pCompressedEphemeralPublicKey,
				const QByteArray& pSignature);
		CardReturnCode performChipAuthentication(QSharedPointer<const ChipAuthenticationInfo> pChipAuthInfo,
				const QByteArray& pEphemeralPublicKey);

		void internalExecuteSoftwareSmartCard();

	protected:
		void internalExecute() override;
		~DidAuthenticateEAC2Command() override = default;

	public:
		explicit DidAuthenticateEAC2Command(QSharedPointer<CardConnectionWorker> pCardConnectionWorker,
				const CVCertificateChain& pCvcChain, const QByteArray& pEphemeralPublicKeyAsHex,
				const QByteArray& pSignatureAsHex, const QByteArray& pAuthenticatedAuxiliaryDataAsBinary,
				const QByteArray& pPin);


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
