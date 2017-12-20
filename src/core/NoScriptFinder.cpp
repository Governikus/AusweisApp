/*!
 * \copyright Copyright (c) 2015-2017 Governikus GmbH & Co. KG, Germany
 */

#include "NoScriptFinder.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QRegularExpression>
#include <QSettings>
#include <QStringList>


namespace governikus
{


class NoScriptFinderPrivate
{
	Q_DECLARE_PUBLIC(NoScriptFinder)

	NoScriptFinder * const q_ptr;
	bool mExtensionFound;


	NoScriptFinderPrivate(NoScriptFinder* pPublic)
		: q_ptr(pPublic)
		, mExtensionFound(false)
	{
		QStringList profileIniFiles;
#if defined(Q_OS_LINUX)
		profileIniFiles += QDir::homePath() + QStringLiteral("/.mozilla/firefox/profiles.ini");
#elif defined(Q_OS_WIN)
		profileIniFiles += QDir::homePath() + QStringLiteral("/AppData/Roaming/Mozilla/Firefox/profiles.ini");
#elif defined(Q_OS_MACOS)
		profileIniFiles += QDir::homePath() + QStringLiteral("/Library/Application Support/Firefox/profiles.ini");
		profileIniFiles += QDir::homePath() + QStringLiteral("/Library/Mozilla/Firefox/profiles.ini");
#endif
		init(profileIniFiles);
	}


	void init(const QStringList& pProfileIniFiles)
	{
		for (const QString& profileFileName : pProfileIniFiles)
		{
			QFileInfo profileFileInfo(profileFileName);
			if (!profileFileInfo.exists())
			{
				qWarning() << "profile file not found" << profileFileName;
				continue;
			}

			QSettings profileSettings(profileFileInfo.canonicalFilePath(), QSettings::Format::IniFormat);
			const auto& filtered = profileSettings.childGroups().filter(QRegularExpression(QStringLiteral("Profile[0-9]*")));
			for (const auto& profileGroup : filtered)
			{
				profileSettings.beginGroup(profileGroup);
				bool isRelative = profileSettings.value(QStringLiteral("IsRelative")).toBool();
				QString path = profileSettings.value(QStringLiteral("Path")).toString();

				if (!path.isEmpty())
				{
					QString extensionFile = isRelative ? profileFileInfo.canonicalPath() + QLatin1Char('/') + path : path;
					extensionFile += QStringLiteral("/extensions.json");

					if ((mExtensionFound = isNoScriptConfigured(extensionFile)))
					{
						qDebug() << "Extension NoScript found in extensions file" << extensionFile;
						return;
					}
					else
					{
						qDebug() << "Extension NoScript not found in extensions file" << extensionFile;
					}
				}

				profileSettings.endGroup();
			}
		}
	}


	bool isNoScriptConfigured(const QString& pExtensionsFile)
	{
		QFile file(pExtensionsFile);
		if (file.exists())
		{
			if (file.open(QIODevice::ReadOnly))
			{
				bool found = file.readAll().contains("\"name\":\"NoScript\"");
				file.close();
				return found;
			}

			qDebug() << "Cannot open extensions file" << pExtensionsFile;
		}
		else
		{
			qWarning() << "Extensions file not found" << pExtensionsFile;
		}
		return false;
	}


};


NoScriptFinder::NoScriptFinder(QObject* pParent)
	: QObject(pParent)
	, d_ptr(new NoScriptFinderPrivate(this))
{
}


NoScriptFinder::~NoScriptFinder()
{
	delete d_ptr;
}


bool NoScriptFinder::isExtensionFound()
{
	Q_D(NoScriptFinder);
	return d->mExtensionFound;
}


} /* namespace governikus */
