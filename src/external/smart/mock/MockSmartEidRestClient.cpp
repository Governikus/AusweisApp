/*
 * \copyright Copyright (c) 2022-2023 Governikus GmbH & Co. KG, Germany
 */

#include "MockSmartEidRestClient.h"

#include "HttpServerRequestor.h"
#include "LogHandler.h"
#include "NetworkManager.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLoggingCategory>
#include <http_parser.h>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(card_smart)
Q_DECLARE_LOGGING_CATEGORY(network)

const char* MockSmartEidRestClient::SMART_EID_MOCK_URL = "AUSWEISAPP2_SMART_EID_MOCK_URL";


QUuid MockSmartEidRestClient::parseSessionId(const QByteArray& pJsonData)
{
	QJsonParseError error {};
	const QJsonObject jsonObject = QJsonDocument::fromJson(pJsonData, &error).object();
	const auto& sessionId = jsonObject[QLatin1String("SessionId")];

	if (sessionId.isUndefined())
	{
		qCDebug(card_smart) << "Received invalid SessionId" << pJsonData << "parseError" << error.errorString();
		return QUuid();
	}

	return QUuid(sessionId.toString());
}


std::string MockSmartEidRestClient::parsePreparePersonalizationData(const QByteArray& pJsonData)
{
	QJsonParseError error {};
	const QJsonObject jsonObject = QJsonDocument::fromJson(pJsonData, &error).object();
	const auto& preparePersonalizationData = jsonObject[QLatin1String("preparePersonalizationData")];

	if (preparePersonalizationData.isString())
	{
		return preparePersonalizationData.toString().toStdString();
	}

	qCDebug(card_smart) << "Received invalid preparePersonalizationData" << pJsonData << "parseError" << error.errorString();
	return std::string();
}


QStringList MockSmartEidRestClient::parseCommandList(const QByteArray& pJsonData)
{
	QJsonParseError error {};
	const QJsonObject jsonObject = QJsonDocument::fromJson(pJsonData, &error).object();
	const auto& commandList = jsonObject[QLatin1String("commandList")];

	if (commandList.isUndefined() || !commandList.isArray())
	{
		qCDebug(card_smart) << "Received invalid commandList" << pJsonData << "parseError" << error.errorString();
		return QStringList();
	}

	return commandList.toVariant().toStringList();
}


MockSmartEidRestClient::MockSmartEidRestClient()
	: mBaseUrl(qEnvironmentVariable(SMART_EID_MOCK_URL))
	, mSessionId()
{

}


InitializeResult MockSmartEidRestClient::createSession()
{
	if (!isEnabled())
	{
		return InitializeResult {EidServiceResult::ERROR};
	}

	if (!mSessionId.isNull())
	{
		qCWarning(card_smart) << "Existing Session" << mSessionId.toString(QUuid::WithoutBraces) << "was not deleted!";
	}

	QUrl url = mBaseUrl;
	url.setPath(url.path() + QStringLiteral("/persotest"));

	const auto& reply = HttpServerRequestor(true).postRequest(url, QByteArray(), QStringLiteral("application/json"));

	if (!reply)
	{
		qCDebug(card_smart) << "Failed to create reply";
		return InitializeResult {EidServiceResult::ERROR};
	}

	const auto& httpStatusCode = NetworkManager::getLoggedStatusCode(reply, spawnMessageLogger(card_smart));
	if (reply->error() != QNetworkReply::NoError)
	{
		switch (httpStatusCode)
		{
			case HTTP_STATUS_TOO_MANY_REQUESTS:
				qCDebug(card_smart) << "createSession failed due to Too Many Simultaneous Requests";
				break;

			default:
				qCDebug(card_smart) << "createSession failed with HTTP status code:" << httpStatusCode;
		}
		return InitializeResult {EidServiceResult::ERROR};
	}

	const auto& jsonData = reply->readAll();
	const auto& sessionId = parseSessionId(jsonData);
	const auto& preparePersonalizationData = parsePreparePersonalizationData(jsonData);

	if (sessionId.isNull() || preparePersonalizationData.empty())
	{
		qCDebug(card_smart) << "SessionId or preparePersonalizationData empty";
		return InitializeResult {EidServiceResult::ERROR};
	}

	mSessionId = sessionId;
	return InitializeResult {EidServiceResult::SUCCESS, preparePersonalizationData};
}


GenericDataResult MockSmartEidRestClient::sendPersonalizationCommand(const std::string& pCommand) const
{
	if (!isEnabled() || mSessionId.isNull())
	{
		return GenericDataResult {EidServiceResult::ERROR};
	}

	QUrl url = mBaseUrl;
	url.setPath(url.path() + QStringLiteral("/persotest/") + mSessionId.toString(QUuid::WithoutBraces));

	QJsonObject requestBody {
		{
			QStringLiteral("commandList"), QJsonArray({QString::fromStdString(pCommand)})
		}
	};

	const auto& reply = HttpServerRequestor(true).postRequest(url, QJsonDocument(requestBody).toJson(QJsonDocument::Compact), QStringLiteral("application/json"));

	if (!reply)
	{
		return GenericDataResult {EidServiceResult::ERROR};
	}

	const auto& httpStatusCode = NetworkManager::getLoggedStatusCode(reply, spawnMessageLogger(card_smart));
	if (reply->error() != QNetworkReply::NoError)
	{
		switch (httpStatusCode)
		{
			case HTTP_STATUS_BAD_REQUEST:
				qCDebug(card_smart) << "sendPersonalizationCommand failed due to Malformed Input";
				break;

			case HTTP_STATUS_NOT_FOUND:
				qCDebug(card_smart) << "sendPersonalizationCommand failed due to Invalid Session Id";
				break;
		}
		return GenericDataResult {EidServiceResult::ERROR};
	}

	const auto& commandList = parseCommandList(reply->readAll());
	if (commandList.size() != 1)
	{
		qCDebug(card_smart) << "Received commandList does not have expected size of 1";
		return GenericDataResult {EidServiceResult::ERROR};
	}

	return GenericDataResult {EidServiceResult::SUCCESS, commandList[0].toStdString()};
}


PersonalizationResult MockSmartEidRestClient::deleteSession()
{
	if (!isEnabled() || mSessionId.isNull())
	{
		return PersonalizationResult {EidServiceResult::ERROR};
	}

	QUrl url = mBaseUrl;
	url.setPath(url.path() + QStringLiteral("/persotest/") + mSessionId.toString(QUuid::WithoutBraces));

	const auto& reply = HttpServerRequestor(true).deleteRequest(url);

	if (!reply)
	{
		return PersonalizationResult {EidServiceResult::ERROR};
	}

	const auto& httpStatusCode = NetworkManager::getLoggedStatusCode(reply, spawnMessageLogger(card_smart));
	if (reply->error() != QNetworkReply::NoError)
	{
		if (httpStatusCode == HTTP_STATUS_NOT_FOUND)
		{
			qCDebug(card_smart) << "deleteSession failed due to Invalid Session Id";
		}
		return PersonalizationResult {EidServiceResult::ERROR};
	}

	mSessionId = QUuid();
	return PersonalizationResult {EidServiceResult::SUCCESS};
}


bool MockSmartEidRestClient::isEnabled() const
{
	return !mBaseUrl.isEmpty();
}
