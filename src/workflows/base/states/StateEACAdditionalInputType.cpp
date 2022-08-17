/*!
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateEACAdditionalInputType.h"

using namespace governikus;

StateEACAdditionalInputType::StateEACAdditionalInputType(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
	setAbortOnCardRemoved();
}


void StateEACAdditionalInputType::run()
{
	Q_ASSERT(!getContext()->getDidAuthenticateEac2().isNull());
	if (!getContext()->getDidAuthenticateEac2()->getSignature().isEmpty())
	{
		Q_EMIT fireContinue();
	}
	else
	{
		qDebug() << "Signature not present, request EACAdditionalInputType";
		QSharedPointer<DIDAuthenticateResponseEAC2> response(getContext()->getDidAuthenticateResponseEacAdditionalInputType());
		response->setChallenge(getContext()->getDidAuthenticateResponseEac1()->getChallenge());

		Q_EMIT fireSendDidAuthenticatResponse();
	}
}
