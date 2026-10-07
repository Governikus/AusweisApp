/**
 * Copyright (c) 2016-2026 Governikus Service GmbH, Germany
 */

#include "MsgHandlerAuth.h"

#include "UiLoader.h"
#include "UiPluginJson.h"
#include "controller/AuthController.h"

#include <QSharedPointer>
#include <QUrlQuery>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;


MsgHandlerAuth::MsgHandlerAuth()
	: MsgHandlerWorkflows(MsgType::AUTH)
{
}


MsgHandlerAuth::MsgHandlerAuth(const QJsonObject& pObj, MsgContext& pContext)
	: MsgHandlerAuth()
{
	const auto& jsonTcTokenUrl = pObj[QLatin1String("tcTokenURL")];
	if (jsonTcTokenUrl.isUndefined())
	{
		setError(QLatin1String("tcTokenURL cannot be undefined"));
	}
	else if (!jsonTcTokenUrl.isString())
	{
		setError(QLatin1String("Invalid tcTokenURL"));
	}
	else
	{
		if (const auto& url = createUrl(jsonTcTokenUrl.toString()); url.isValid())
		{
			handleWorkflowProperties(pObj, pContext);
			if (const auto& header = createMap(pObj[QLatin1String("header")]); !hasError())
			{
				initAuth(url, header);
				setVoid();
				return;
			}
		}
		Q_ASSERT(isString(QLatin1String("error")));
	}
}


MsgHandlerAuth::MsgHandlerAuth(const QSharedPointer<AuthContext>& pContext)
	: MsgHandlerAuth()
{
	Q_ASSERT(pContext);

	setValue(QLatin1String("result"), ECardApiResult(pContext->getStatus(), pContext->getFailureCode()).toJson());

	QString url;
	if (pContext->getRefreshUrl().isEmpty())
	{
		const auto& token = pContext->getTcToken();
		if (!token.isNull() && pContext->getTcToken()->getCommunicationErrorAddress().isValid())
		{
			url = pContext->getTcToken()->getCommunicationErrorAddress().toString();
		}
	}
	else
	{
		url = pContext->getRefreshUrl().toString();
	}

	setValue(QLatin1String("url"), url);
}


AuthContext::HeaderMap MsgHandlerAuth::createMap(const QJsonValue& pCustomHeader)
{
	AuthContext::HeaderMap header;

	if (pCustomHeader.isUndefined())
	{
		return header;
	}

	if (!pCustomHeader.isObject())
	{
		setError("Header is not an object"_L1);
		return header;
	}

	const auto obj = pCustomHeader.toObject();
#if (QT_VERSION >= QT_VERSION_CHECK(6, 10, 0))
	for (auto [key, value] : obj.asKeyValueRange())
#else
	for (const auto& key : obj.keys())
#endif
	{
#if (QT_VERSION >= QT_VERSION_CHECK(6, 10, 0))
		const auto keyStr = key.toString().trimmed();
#else
		const auto keyStr = key.trimmed();
		QJsonValue value = obj.value(key);
#endif

		if (keyStr.isEmpty())
		{
			setError("Key of header is empty"_L1);
			return header;
		}

		if (!value.isString())
		{
			setError("Value of header is not a string"_L1);
			return header;
		}

		const auto valueStr = value.toString().trimmed();
		if (valueStr.isEmpty())
		{
			setError("Value of header is empty"_L1);
			return header;
		}

		header.insert(keyStr.toUtf8(), valueStr.toUtf8());
	}

	return header;
}


QUrl MsgHandlerAuth::createUrl(const QString& pUrl)
{
	if (const QUrl parsedUrl(pUrl); parsedUrl.isValid() && !parsedUrl.host().isEmpty())
	{
		QUrlQuery query;
		query.addQueryItem(QStringLiteral("tcTokenURL"), QString::fromLatin1(QUrl::toPercentEncoding(pUrl)));

		QUrl url(QStringLiteral("http://localhost/")); // just a dummy for StateParseTcTokenUrl
		url.setQuery(query);

		return url;
	}

	setError(QLatin1String("Validation of tcTokenURL failed"));
	return QUrl();
}


void MsgHandlerAuth::initAuth(const QUrl& pTcTokenUrl, const AuthContext::HeaderMap& pCustomHeader) const
{
	auto* ui = Env::getSingleton<UiLoader>()->getLoaded<UiPluginJson>();
	Q_ASSERT(ui);
	Q_EMIT ui->fireWorkflowRequested(AuthController::createWorkflowRequest(pTcTokenUrl, QVariant(), AuthContext::BrowserHandler(), pCustomHeader));
}
