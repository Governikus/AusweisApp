/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "asn1/CVCertificate.h"
#include "asn1/CVCertificateChainBuilder.h"
#include "CardConnection.h"
#include "StateDidAuthenticateEac2.h"

using namespace governikus;

StateDidAuthenticateEac2::StateDidAuthenticateEac2(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractGenericState(pContext)
{
}


void StateDidAuthenticateEac2::run()
{
	Q_ASSERT(!getContext()->getDidAuthenticateEac2().isNull());
	Q_ASSERT(!getContext()->getCardConnection().isNull());
	Q_ASSERT(getContext()->getPaceOutputData() != nullptr);
	auto cardConnection = getContext()->getCardConnection();
	QString ephemeralPublicKeyAsHex = getContext()->getDidAuthenticateEac2()->getEphemeralPublicKey();
	QByteArray authenticatedAuxiliaryDataAsBinary = getContext()->getDidAuthenticateEac1()->getAuthenticatedAuxiliaryDataAsBinary();

	QString signatureAsHex;
	if (!getContext()->getDidAuthenticateEac2()->getSignature().isEmpty())
	{
		signatureAsHex = getContext()->getDidAuthenticateEac2()->getSignature();
	}
	else if (getContext()->getDidAuthenticateEacAdditional())
	{
		signatureAsHex = getContext()->getDidAuthenticateEacAdditional()->getSignature();
	}

	auto cvcChain = getContext()->getChainForCertificationAuthority(*getContext()->getPaceOutputData());
	if (!cvcChain.isValid())
	{
		setStatus(GlobalStatus::Code::Workflow_No_Permission_Error);
		Q_EMIT fireError();
		return;
	}

	mConnections += cardConnection->callDidAuthenticateEAC2Command(this, &StateDidAuthenticateEac2::onCardCommandDone, cvcChain, ephemeralPublicKeyAsHex, signatureAsHex,
			authenticatedAuxiliaryDataAsBinary);
}


void StateDidAuthenticateEac2::onCardCommandDone(QSharedPointer<BaseCardCommand> pCommand)
{
	if (pCommand->getReturnCode() != CardReturnCode::OK)
	{
		setStatus(GlobalStatus::Code::Workflow_No_Permission_Error);
		Q_EMIT fireError();
	}

	auto eac2Command = pCommand.staticCast<DidAuthenticateEAC2Command>();
	QSharedPointer<DIDAuthenticateResponseEAC2> response = getContext()->getDidAuthenticateResponseEac2();
	response->setAuthenticationToken(eac2Command->getAuthTokenAsHex());
	response->setEfCardSecurity(eac2Command->getEfCardSecurityAsHex());
	response->setNonce(eac2Command->getNonceAsHex());

	Q_EMIT fireSuccess();
}
