/*!
 * \brief Parse tcTokenURL from GET query.
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "StateParseTcTokenUrl.h"

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
		Q_EMIT fireContinue();
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

		updateStatus(GlobalStatus::Code::Workflow_Wrong_Parameter_Invocation);
		Q_EMIT fireAbort();
	}
}
