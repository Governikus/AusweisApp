/*!
 * VersionInfo.h
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#pragma once

#include <QDebug>
#include <QIODevice>
#include <QJsonDocument>
#include <QMap>
#include <QString>


namespace governikus
{

class VersionInfo
{
	QMap<QString, QString> mInfo;

	VersionInfo(const QMap<QString, QString>& pInfo);

	public:
		VersionInfo();
		static VersionInfo getInstance();
		static VersionInfo fromText(const QString& pText);

		bool isNull() const;

		QString getName() const;
		QString getImplementationTitle() const;
		QString getImplementationVendor() const;
		QString getImplementationVersion() const;

		QString getSpecificationTitle() const;
		QString getSpecificationVendor() const;
		QString getSpecificationVersion() const;

		QJsonObject toJsonObject() const;
		QByteArray toJson(QJsonDocument::JsonFormat pFormat = QJsonDocument::Indented) const;
		QString toText() const;
};


} /* namespace governikus */

QDebug operator<<(QDebug pDbg, const governikus::VersionInfo& pVersionInfo);
