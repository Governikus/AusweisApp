#include "VersionInfo.h"

#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVector>


using namespace governikus;


const QString NAME = QStringLiteral("Name");
const QString IMPL_TITLE = QStringLiteral("Implementation-Title");
const QString IMPL_VENDOR = QStringLiteral("Implementation-Vendor");
const QString IMPL_VERSION = QStringLiteral("Implementation-Version");
const QString SPEC_TITLE = QStringLiteral("Specification-Title");
const QString SPEC_VENDOR = QStringLiteral("Specification-Vendor");
const QString SPEC_VERSION = QStringLiteral("Specification-Version");


VersionInfo::VersionInfo(const QMap<QString, QString>& pInfo)
	: mInfo(pInfo)
{
}


VersionInfo::VersionInfo()
	: VersionInfo(QMap<QString, QString>())
{
}


VersionInfo VersionInfo::getInstance()
{
	return VersionInfo({
				{NAME, QCoreApplication::applicationName()},
				{IMPL_TITLE, QCoreApplication::applicationName()},
				{IMPL_VENDOR, QCoreApplication::organizationName()},
				{IMPL_VERSION, QCoreApplication::applicationVersion()},
				{SPEC_TITLE, QStringLiteral("TR-03124")},
				{SPEC_VENDOR, QStringLiteral("Federal Office for Information Security")},
				{SPEC_VERSION, QStringLiteral("1.2")}
			});
}


VersionInfo VersionInfo::fromText(const QString& pText)
{
	QMap<QString, QString> infos;
	const auto& header = pText.splitRef('\n');
	for (const auto& line : header)
	{
		const auto pair = line.split(':');
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
	return mInfo[NAME];
}


QString VersionInfo::getImplementationTitle() const
{
	return mInfo[IMPL_TITLE];
}


QString VersionInfo::getImplementationVendor() const
{
	return mInfo[IMPL_VENDOR];
}


QString VersionInfo::getImplementationVersion() const
{
	return mInfo[IMPL_VERSION];
}


QString VersionInfo::getSpecificationTitle() const
{
	return mInfo[SPEC_TITLE];
}


QString VersionInfo::getSpecificationVendor() const
{
	return mInfo[SPEC_VENDOR];
}


QString VersionInfo::getSpecificationVersion() const
{
	return mInfo[SPEC_VERSION];
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
	return list.join('\n');
}


QDebug operator<<(QDebug pDbg, const VersionInfo& pVersionInfo)
{
	QDebugStateSaver saver(pDbg);
	return pDbg.nospace().noquote() << "VersionInfo(" << pVersionInfo.toJson(QJsonDocument::JsonFormat::Compact) << ")";
}
