/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "states/StateEACAdditionalInputType.h"

using namespace governikus;

StateEACAdditionalInputType::StateEACAdditionalInputType(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractGenericState(pContext)
{
}


void StateEACAdditionalInputType::run()
{
	Q_ASSERT(!getContext()->getDidAuthenticateEac2().isNull());
	if (!getContext()->getDidAuthenticateEac2()->getSignature().isEmpty())
	{
		Q_EMIT fireSuccess();
	}
	else
	{
		qDebug() << "Signature not present, request EACAdditionalInputType";
		QSharedPointer<DIDAuthenticateResponseEAC2> response(getContext()->getDidAuthenticateResponseEacAdditionalInputType());
		response->setChallenge(getContext()->getDidAuthenticateResponseEac1()->getChallenge());

		Q_EMIT fireSendDidAuthenticatResponse();
	}
}
