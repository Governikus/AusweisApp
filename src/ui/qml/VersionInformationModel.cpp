/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
 */

#include "VersionInformationModel.h"

#include "AppSettings.h"
#include "BuildHelper.h"

#include <QCoreApplication>
#include <QSslSocket>
#include <QString>
#include <QSysInfo>

#include <openssl/crypto.h>

using namespace governikus;


VersionInformationModel::VersionInformationModel()
	: QAbstractListModel()
	, mData()
{
	init();

	connect(&Env::getSingleton<AppSettings>()->getGeneralSettings(), &GeneralSettings::fireSettingsChanged, this, [this]()
			{
				beginResetModel();
				init();
				endResetModel();
			});
}


void VersionInformationModel::init()
{
	mData.clear();

	BuildHelper::processInformationHeader([this](const QString& pKey, const QString& pValue){
				mData += qMakePair(pKey, pValue);
			});
}


int VersionInformationModel::rowCount(const QModelIndex&) const
{
	return static_cast<int>(mData.size());
}


QVariant VersionInformationModel::data(const QModelIndex& pIndex, int pRole) const
{
	if (pIndex.isValid() && pIndex.row() < rowCount())
	{
		const auto& [key, value] = std::as_const(mData).at(pIndex.row());
		if (pRole == KEY)
		{
			return key;
		}
		if (pRole == VALUE)
		{
			return value;
		}
	}
	return QVariant();
}


QHash<int, QByteArray> VersionInformationModel::roleNames() const
{
	QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
	roles.insert(KEY, "key");
	roles.insert(VALUE, "value");
	return roles;
}
