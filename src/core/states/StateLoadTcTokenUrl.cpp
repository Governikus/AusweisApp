/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */


#include "AppSettings.h"
#include "states/StateLoadTcTokenUrl.h"

using namespace governikus;

StateLoadTcTokenUrl::StateLoadTcTokenUrl(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractGenericState(pContext, false)
{
}


void StateLoadTcTokenUrl::run()
{
	const bool useTestUri = AppSettings::getInstance().getGeneralSettings().useSelfauthenticationTestUri();

	const QUrl& url = AppSettings::getInstance().getSecureStorage().getSelfAuthenticationUrl(useTestUri);
	qDebug() << "Loaded tcTokenUrl for self authentication from securestorage:" << url;
	getContext()->setTcTokenUrl(url);

	Q_EMIT fireSuccess();
}
