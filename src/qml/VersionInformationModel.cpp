/*!
 * \brief Model implementation for version information.
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "BuildHelper.h"
#include "DeviceInfo.h"
#include "VersionInformationModel.h"

#include <QCoreApplication>
#include <QSslSocket>
#include <QString>
#include <QSysInfo>

#include <openssl/crypto.h>


using namespace governikus;


VersionInformationModel::VersionInformationModel(QObject* pParent)
	: QAbstractListModel(pParent)
	, mData()
{
	mData += QPair<QString, QString>(tr("Application Name"), QCoreApplication::applicationName());
	mData += QPair<QString, QString>(tr("Application Version"), QCoreApplication::applicationVersion());
	mData += QPair<QString, QString>(tr("Organization"), QCoreApplication::organizationName());
	mData += QPair<QString, QString>(tr("Organization domain"), QCoreApplication::organizationDomain());
	mData += QPair<QString, QString>(tr("Build date"), QString::fromLatin1(BuildHelper::getDateTime()));
#ifdef Q_OS_ANDROID
	mData += QPair<QString, QString>(tr("VersionCode"), QString::number(BuildHelper::getVersionCode()));
#endif
	mData += QPair<QString, QString>(tr("System version"), QSysInfo::prettyProductName());
	mData += QPair<QString, QString>(tr("Kernel"), QSysInfo::kernelVersion());
	mData += QPair<QString, QString>(tr("Architecture"), QSysInfo::currentCpuArchitecture());
#ifdef Q_OS_ANDROID
	mData += QPair<QString, QString>(tr("Compiled architecture"), QSysInfo::buildCpuArchitecture());
	mData += QPair<QString, QString>(tr("Device"), DeviceInfo::getPrettyInfo());
#endif
	mData += QPair<QString, QString>(tr("Qt Version"), QString::fromLatin1(qVersion()));
	mData += QPair<QString, QString>(tr("OpenSSL Version"), QSslSocket::sslLibraryVersionString());
}


int VersionInformationModel::rowCount(const QModelIndex&) const
{
	return mData.size();
}


QVariant VersionInformationModel::data(const QModelIndex& pIndex, int pRole) const
{
	if (pIndex.isValid() && pIndex.row() < rowCount())
	{
		auto entry = mData[pIndex.row()];
		if (pRole == LABEL)
		{
			return entry.first;
		}
		if (pRole == TEXT)
		{
			return entry.second;
		}
	}
	return QVariant();
}


QHash<int, QByteArray> VersionInformationModel::roleNames() const
{
	QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
	roles.insert(LABEL, "label");
	roles.insert(TEXT, "text");
	return roles;
}
