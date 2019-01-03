/*!
 * \copyright Copyright (c) 2018 Governikus GmbH & Co. KG, Germany
 */

#include "SurveyHandler.h"

#include "DeviceInfo.h"
#include "Env.h"
#include "NetworkManager.h"
#include "SecureStorage.h"

#include <QCoreApplication>
#include <QDebug>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QObject>

using namespace governikus;


void SurveyHandler::transmitSurvey(const Survey& pSurvey) const
{
	const QUrl whitelistServerBaseUrl = SecureStorage::getInstance().getWhitelistServerBaseUrl();
	const QUrl postSurveyUrl(whitelistServerBaseUrl.toString() + QStringLiteral("/new"));
	QNetworkRequest request(postSurveyUrl);
	request.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("application/json; charset=UTF-8"));

	const QByteArray jsonData = pSurvey.toJsonByteArray();
	QNetworkReply* reply = Env::getSingleton<NetworkManager>()->post(request, jsonData);
	QObject::connect(reply, &QNetworkReply::finished, reply, &QNetworkReply::deleteLater);

	qDebug().noquote() << "Sent survey to whitelist server:" << jsonData;
}


SurveyHandler::SurveyHandler()
{
}


void SurveyHandler::sendSurvey(int pMaximumNfcPackageLength) const
{
	const Survey survey = createSurvey(pMaximumNfcPackageLength);

	if (survey.isNull())
	{
		qCritical() << "Cannot send survey containing no information at all";

		return;
	}

	transmitSurvey(survey);
}


Survey SurveyHandler::createSurvey(int pMaximumNfcPackageLength) const
{
	return Survey(
			DeviceInfo::getOSBuildNumber(),
			DeviceInfo::getOSVersion(),
			DeviceInfo::getKernelVersion(),
			pMaximumNfcPackageLength,
			DeviceInfo::getVendor(),
			DeviceInfo::getModelNumber(),
			DeviceInfo::getModelName(),
			QCoreApplication::applicationVersion());
}
