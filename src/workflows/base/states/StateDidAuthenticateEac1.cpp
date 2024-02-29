/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

#include "StateDidAuthenticateEac1.h"

#include "CardConnection.h"

#include <QMetaObject>


using namespace governikus;


StateDidAuthenticateEac1::StateDidAuthenticateEac1(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
	setAbortOnCardRemoved();
}


void StateDidAuthenticateEac1::run()
{
	Q_ASSERT(!getContext()->getDidAuthenticateEac1().isNull());
	Q_ASSERT(getContext()->getPaceOutputData() != nullptr);
	Q_ASSERT(getContext()->getCardConnection());
	auto cardConnection = getContext()->getCardConnection();
	Q_ASSERT(cardConnection);

	*this << cardConnection->callDidAuthenticateEAC1Command(this, &StateDidAuthenticateEac1::onCardCommandDone);
}


void StateDidAuthenticateEac1::onCardCommandDone(QSharedPointer<BaseCardCommand> pCommand)
{
	CardReturnCode result = pCommand->getReturnCode();
	if (result == CardReturnCode::OK)
	{
		auto eac1Command = pCommand.staticCast<DidAuthenticateEAC1Command>();
		const QByteArray& challenge = eac1Command->getChallenge();

		QSharedPointer<DIDAuthenticateResponseEAC1> eac1Response = getContext()->getDidAuthenticateResponseEac1();
		eac1Response->setCertificateHolderAuthorizationTemplate(getContext()->encodeEffectiveChat().toHex());
		eac1Response->setChallenge(challenge.toHex());

		auto paceOutput = getContext()->getPaceOutputData();
		eac1Response->setEFCardAccess(paceOutput->getEfCardAccess().toHex());
		eac1Response->setIDPICC(paceOutput->getIdIcc().toHex());
		if (!getContext()->hasChainForCertificationAuthority(*paceOutput))
		{
			qDebug() << "No cvc chain determined, request new cvc list";
			eac1Response->setCertificationAuthorityReference(*paceOutput);
		}
		Q_EMIT fireContinue();
	}
	else
	{
		updateStatus(CardReturnCodeUtil::toGlobalStatus(result));
		Q_EMIT fireAbort({FailureCode::Reason::Did_Authenticate_Eac1_Card_Command_Failed,
						  {FailureCode::Info::Card_Return_Code, Enum<CardReturnCode>::getName(result)}
				});
	}
}


void StateDidAuthenticateEac1::onEntry(QEvent* pEvent)
{
	//: INFO ALL_PLATFORMS Status message after the PIN was entered, Terminal Authentication.
	getContext()->setProgress(20, tr("Provider is being verified"));
	AbstractState::onEntry(pEvent);
}
