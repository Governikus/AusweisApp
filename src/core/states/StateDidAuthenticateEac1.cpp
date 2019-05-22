/*!
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#include "asn1/CVCertificateChainBuilder.h"
#include "CardConnection.h"
#include "StateDidAuthenticateEac1.h"

#include <QMetaObject>


using namespace governikus;


StateDidAuthenticateEac1::StateDidAuthenticateEac1(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractGenericState(pContext)
{
}


void StateDidAuthenticateEac1::run()
{
	Q_ASSERT(getContext()->getPin().isEmpty() && "PACE passwords must be cleared as soon as possible.");
	Q_ASSERT(getContext()->getCan().isEmpty() && "PACE passwords must be cleared as soon as possible.");
	Q_ASSERT(getContext()->getPuk().isEmpty() && "PACE passwords must be cleared as soon as possible.");

	Q_ASSERT(!getContext()->getDidAuthenticateEac1().isNull());
	Q_ASSERT(getContext()->getPaceOutputData() != nullptr);
	Q_ASSERT(getContext()->getCardConnection());
	Q_ASSERT(!getContext()->encodeEffectiveChat().isEmpty());
	auto cardConnection = getContext()->getCardConnection();
	Q_ASSERT(cardConnection);

	mConnections += cardConnection->callDidAuthenticateEAC1Command(this, &StateDidAuthenticateEac1::onCardCommandDone);
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
		eac1Response->setIDPICC(paceOutput->getIDicc().toHex());
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
		Q_EMIT fireAbort();
	}
}
