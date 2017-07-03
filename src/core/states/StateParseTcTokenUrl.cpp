/*!
 * StateParseTcTokenUrl.cpp
 *
 * \brief Parse tcTokenURL from GET query.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "StateParseTcTokenUrl.h"

#include "Result.h"


#include <QUrlQuery>

using namespace governikus;

StateParseTcTokenUrl::StateParseTcTokenUrl(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractGenericState(pContext)
{
}


void StateParseTcTokenUrl::run()
{
	QUrlQuery query(getContext()->getActivationContext()->getActivationURL());
	QUrl tcTokenURL(query.queryItemValue(QStringLiteral("tcTokenURL"), QUrl::FullyDecoded));
	if (tcTokenURL.isValid())
	{
		getContext()->setTcTokenUrl(tcTokenURL);
		Q_EMIT fireSuccess();
	}
	else
	{
		if (query.hasQueryItem(QStringLiteral("tcTokenURL")))
		{
			qCritical() << "TcTokenUrl is not valid:" << tcTokenURL.toString();
		}
		else
		{
			qCritical() << "No parameter tcTokenURL";
		}

		setStatus(GlobalStatus::Code::Workflow_Wrong_Parameter_Invocation);
		Q_EMIT fireError();
	}
}
