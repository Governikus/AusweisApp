/*!
 * StateParseTcTokenUrl.cpp
 *
 * \brief Parse tcTokenURL from GET query.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "ErrorMessage.h"
#include "Result.h"
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

		setResult(Result::createCommunicationError(ErrorMessage::toString(ErrorMessageId::WRONG_PARAMETER_INVOCATION)));
		Q_EMIT fireError();
	}
}
