/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#include "StateDidAuthenticateEac2.h"

#include "CardConnection.h"

using namespace governikus;

StateDidAuthenticateEac2::StateDidAuthenticateEac2(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
	setAbortOnCardRemoved();
}


void StateDidAuthenticateEac2::run()
{
	const auto& context = getContext();
	Q_ASSERT(!context->getDidAuthenticateEac2().isNull());
	Q_ASSERT(!context->getCardConnection().isNull());
	Q_ASSERT(context->getPaceOutputData() != nullptr);
	auto cardConnection = context->getCardConnection();
	const auto ephemeralPublicKeyAsHex = context->getDidAuthenticateEac2()->getEphemeralPublicKey().toLatin1();
	QByteArray authenticatedAuxiliaryDataAsBinary = context->getDidAuthenticateEac1()->getAuthenticatedAuxiliaryDataAsBinary();

	QByteArray signatureAsHex;
	if (!context->getDidAuthenticateEac2()->getSignature().isEmpty())
	{
		signatureAsHex = context->getDidAuthenticateEac2()->getSignature().toLatin1();
	}
	else if (context->getDidAuthenticateEacAdditional())
	{
		signatureAsHex = context->getDidAuthenticateEacAdditional()->getSignature().toLatin1();
	}

	auto cvcChain = context->getChainForCertificationAuthority(*context->getPaceOutputData());
	if (!cvcChain.isValid())
	{
		updateStatus(GlobalStatus::Code::Workflow_No_Permission_Error);
		Q_EMIT fireAbort(FailureCode::Reason::Did_Authenticate_Eac2_Invalid_Cvc_Chain);
		return;
	}

	mConnections += cardConnection->callDidAuthenticateEAC2Command(this,
			&StateDidAuthenticateEac2::onCardCommandDone, cvcChain, ephemeralPublicKeyAsHex,
			signatureAsHex, authenticatedAuxiliaryDataAsBinary, context->getPin().toLatin1());
}


void StateDidAuthenticateEac2::onCardCommandDone(QSharedPointer<BaseCardCommand> pCommand)
{
	const CardReturnCode returnCode = pCommand->getReturnCode();
	if (returnCode != CardReturnCode::OK)
	{
		GlobalStatus::Code newStatus;
		switch (returnCode)
		{
			case CardReturnCode::COMMAND_FAILED:
				newStatus = GlobalStatus::Code::Workflow_Card_Removed;
				break;

			case CardReturnCode::EXTENDED_LENGTH_MISSING:
				newStatus = GlobalStatus::Code::Workflow_No_Extended_Length_Error;
				break;

			default:
				newStatus = GlobalStatus::Code::Workflow_No_Permission_Error;
		}
		updateStatus(newStatus);
		Q_EMIT fireAbort({FailureCode::Reason::Did_Authenticate_Eac2_Card_Command_Failed,
						  {FailureCode::Info::Card_Return_Code, Enum<CardReturnCode>::getName(returnCode)}
				});
		return;
	}

	auto eac2Command = pCommand.staticCast<DidAuthenticateEAC2Command>();
	QSharedPointer<DIDAuthenticateResponseEAC2> response = getContext()->getDidAuthenticateResponseEac2();
	response->setAuthenticationToken(eac2Command->getAuthTokenAsHex());
	response->setEfCardSecurity(eac2Command->getEfCardSecurityAsHex());
	response->setNonce(eac2Command->getNonceAsHex());

	Q_EMIT fireContinue();
}


void StateDidAuthenticateEac2::onEntry(QEvent* pEvent)
{
	//: INFO ALL_PLATFORMS Status message after the PIN was entered, Card Authentication.
	getContext()->setProgress(40, tr("eID is being verified"));
	AbstractState::onEntry(pEvent);
}
