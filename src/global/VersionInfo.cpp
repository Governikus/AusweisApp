/*
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

#include "VersionInfo.h"

#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStringView>
#include <QVector>


using namespace governikus;

namespace
{
#define VERSION_NAME(_name, _key)\
	QString _name(){\
		return QStringLiteral(_key);\
	}

VERSION_NAME(NAME, "Name")
VERSION_NAME(IMPL_TITLE, "Implementation-Title")
VERSION_NAME(IMPL_VENDOR, "Implementation-Vendor")
VERSION_NAME(IMPL_VERSION, "Implementation-Version")
VERSION_NAME(SPEC_TITLE, "Specification-Title")
VERSION_NAME(SPEC_VENDOR, "Specification-Vendor")
VERSION_NAME(SPEC_VERSION, "Specification-Version")
} // namespace


VersionInfo::VersionInfo(const QMap<QString, QString>& pInfo)
	: mInfo(pInfo)
{
	if (!mInfo.isEmpty() && !mInfo.contains(NAME()))
	{
		qDebug() << "VersionInfo invalid... clearing";
		mInfo.clear();
	}
}


VersionInfo::VersionInfo()
	: VersionInfo(QMap<QString, QString>())
{
}


VersionInfo VersionInfo::getInstance()
{
	return VersionInfo({
				{NAME(), QCoreApplication::applicationName()},
				{IMPL_TITLE(), QCoreApplication::applicationName()},
				{IMPL_VENDOR(), QCoreApplication::organizationName()},
				{IMPL_VERSION(), QCoreApplication::applicationVersion()},
				{SPEC_TITLE(), QStringLiteral("TR-03124")},
				{SPEC_VENDOR(), QStringLiteral("Federal Office for Information Security")},
				{SPEC_VERSION(), QStringLiteral("1.3")}
			});
}


VersionInfo VersionInfo::fromText(const QString& pText)
{
	QMap<QString, QString> infos;

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
	const auto& header = QStringView(pText).split(QLatin1Char('\n'));
#else
	const auto& header = pText.splitRef(QLatin1Char('\n'));
#endif

	for (const auto& line : header)
	{
		const auto pair = line.split(QLatin1Char(':'));
		if (pair.size() != 2 || pair[0].trimmed().isEmpty() || pair[1].trimmed().isEmpty())
		{
			qWarning() << "Cannot parse line:" << line;
			continue;
		}
		infos[pair[0].trimmed().toString()] = pair[1].trimmed().toString();
	}
	return VersionInfo(infos);
}


bool VersionInfo::isNull() const
{
	return mInfo.isEmpty();
}


QString VersionInfo::getName() const
{
	return mInfo[NAME()];
}


QString VersionInfo::getImplementationTitle() const
{
	return mInfo[IMPL_TITLE()];
}


QString VersionInfo::getImplementationVendor() const
{
	return mInfo[IMPL_VENDOR()];
}


QString VersionInfo::getImplementationVersion() const
{
	return mInfo[IMPL_VERSION()];
}


QString VersionInfo::getSpecificationTitle() const
{
	return mInfo[SPEC_TITLE()];
}


QString VersionInfo::getSpecificationVendor() const
{
	return mInfo[SPEC_VENDOR()];
}


QString VersionInfo::getSpecificationVersion() const
{
	return mInfo[SPEC_VERSION()];
}


QJsonObject VersionInfo::toJsonObject() const
{
	QJsonObject jsonObj;
	for (auto i = mInfo.constBegin(); i != mInfo.constEnd(); ++i)
	{
		jsonObj[i.key()] = i.value();
	}
	return jsonObj;
}


QByteArray VersionInfo::toJson(QJsonDocument::JsonFormat pFormat) const
{
	return QJsonDocument(toJsonObject()).toJson(pFormat);
}


QString VersionInfo::toText() const
{
	QStringList list;
	for (auto i = mInfo.constBegin(); i != mInfo.constEnd(); ++i)
	{
		list += QStringLiteral("%1: %2").arg(i.key(), i.value());
	}
	return list.join(QLatin1Char('\n'));
}


QDebug operator<<(QDebug pDbg, const VersionInfo& pVersionInfo)
{
	QDebugStateSaver saver(pDbg);
	return pDbg.nospace().noquote() << "VersionInfo(" << pVersionInfo.toJson(QJsonDocument::JsonFormat::Compact) << ")";
}
