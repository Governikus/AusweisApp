/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Parse tcTokenURL from GET query.
 */

#include "StateParseTcTokenUrl.h"

#include <QUrlQuery>

using namespace governikus;

StateParseTcTokenUrl::StateParseTcTokenUrl(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
}


void StateParseTcTokenUrl::run()
{
	QUrlQuery query(getContext()->getActivationUrl());
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
			updateStatus(GlobalStatus::Code::Workflow_Wrong_Parameter_Invocation);
			Q_EMIT fireAbort(FailureCode::Reason::Parse_TcToken_Invalid_Url);
		}
		else
		{
			qCritical() << "No parameter tcTokenURL";
			updateStatus(GlobalStatus::Code::Workflow_Wrong_Parameter_Invocation);
			Q_EMIT fireAbort(FailureCode::Reason::Parse_TcToken_Missing_Url);
		}
	}
}
