/*!
 * \copyright Copyright (c) 2014-2021 Governikus GmbH & Co. KG, Germany
 */


#include "AppSettings.h"
#include "states/StateLoadTcTokenUrl.h"
#include "SecureStorage.h"

using namespace governikus;

StateLoadTcTokenUrl::StateLoadTcTokenUrl(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext, false)
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
