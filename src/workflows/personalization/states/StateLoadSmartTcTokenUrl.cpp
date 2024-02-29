/**
 * Copyright (c) 2021-2024 Governikus GmbH & Co. KG, Germany
 */

#include "StateLoadSmartTcTokenUrl.h"


using namespace governikus;


StateLoadSmartTcTokenUrl::StateLoadSmartTcTokenUrl(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
}


void StateLoadSmartTcTokenUrl::run()
{
	const QUrl url = getContext()->getAppletServiceUrl(QStringLiteral("tcToken"));
	qDebug() << "Loaded tcTokenUrl for Smart-eID personalization:" << url;
	getContext()->setTcTokenUrl(url);

	Q_EMIT fireContinue();
}
