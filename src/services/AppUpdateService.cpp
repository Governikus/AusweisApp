/*!
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "AppUpdateService.h"

#include "AppUpdateBackend.h"
#include "SingletonHelper.h"

#include <QLoggingCategory>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(update)

defineSingleton(AppUpdateService)


AppUpdateService::AppUpdateService()
	: UpdateService(QSharedPointer<AppUpdateBackend>(new AppUpdateBackend), QStringLiteral("app"))
{
	connect(getUpdateBackend<AppUpdateBackend>().data(), &AppUpdateBackend::fireAppUpdateCheckFinished, this, &AppUpdateService::fireAppUpdateCheckFinished);
}


AppUpdateService::~AppUpdateService()
{
}


AppUpdateService& AppUpdateService::getInstance()
{
	return *Instance;
}


void AppUpdateService::setTrustedUpdateCertificates(const QVector<QSslCertificate>& pTrustedUpdateCertificates)
{
	UpdateService::setTrustedUpdateCertificates(pTrustedUpdateCertificates);
	getUpdateBackend<AppUpdateBackend>()->setTrustedUpdateCertificates(pTrustedUpdateCertificates);
}


void AppUpdateService::skipVersion(const QString& pVersion)
{
	getUpdateBackend<AppUpdateBackend>()->skipVersion(pVersion);
}


const AppUpdateData& AppUpdateService::getUpdateData() const
{
	return getUpdateBackend<AppUpdateBackend>()->getUpdateData();
}
