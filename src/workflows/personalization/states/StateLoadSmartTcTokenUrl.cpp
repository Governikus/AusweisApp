/*!
 * \copyright Copyright (c) 2021-2022 Governikus GmbH & Co. KG, Germany
 */

#include "StateLoadSmartTcTokenUrl.h"

#include "AppSettings.h"
#include "SecureStorage.h"

using namespace governikus;

StateLoadSmartTcTokenUrl::StateLoadSmartTcTokenUrl(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
}


void StateLoadSmartTcTokenUrl::run()
{
	const auto& context = qobject_cast<PersonalizationContext*>(getContext());
	Q_ASSERT(context);

	const QUrl url = context->getAppletServiceUrl(QStringLiteral("tcToken"));
	qDebug() << "Loaded tcTokenUrl for Smart-eID personalization:" << url;
	getContext()->setTcTokenUrl(url);

	Q_EMIT fireContinue();
}
