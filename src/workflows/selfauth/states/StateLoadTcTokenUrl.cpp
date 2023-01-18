/*!
 * \copyright Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#include "StateLoadTcTokenUrl.h"

#include "AppSettings.h"
#include "SecureStorage.h"

using namespace governikus;

StateLoadTcTokenUrl::StateLoadTcTokenUrl(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
}


void StateLoadTcTokenUrl::run()
{
	const bool useTestUri = Env::getSingleton<AppSettings>()->getGeneralSettings().useSelfAuthTestUri();

	const QUrl& url = Env::getSingleton<SecureStorage>()->getSelfAuthenticationUrl(useTestUri);
	qDebug() << "Loaded tcTokenUrl for self-authentication from securestorage:" << url;
	getContext()->setTcTokenUrl(url);

	Q_EMIT fireContinue();
}
