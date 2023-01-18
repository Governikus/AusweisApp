/*!
 * \copyright Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
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
	private:
		QMap<QString, QString> mInfo;

		explicit VersionInfo(const QMap<QString, QString>& pInfo);

	public:
		VersionInfo();
		static VersionInfo getInstance();
		static VersionInfo fromText(const QString& pText);

		[[nodiscard]] bool isNull() const;

		[[nodiscard]] QString getName() const;
		[[nodiscard]] QString getImplementationTitle() const;
		[[nodiscard]] QString getImplementationVendor() const;
		[[nodiscard]] QString getImplementationVersion() const;

		[[nodiscard]] QString getSpecificationTitle() const;
		[[nodiscard]] QString getSpecificationVendor() const;
		[[nodiscard]] QString getSpecificationVersion() const;

		[[nodiscard]] QJsonObject toJsonObject() const;
		[[nodiscard]] QByteArray toJson(QJsonDocument::JsonFormat pFormat = QJsonDocument::Indented) const;
		[[nodiscard]] QString toText() const;
};


} // namespace governikus

QDebug operator<<(QDebug pDbg, const governikus::VersionInfo& pVersionInfo);
