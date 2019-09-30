/*!
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#include "HelpAction.h"

#include "LanguageLoader.h"
#include "SingletonHelper.h"
#include "VersionNumber.h"

#include <QCoreApplication>
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QLoggingCategory>
#include <QStringBuilder>
#include <QUrl>


using namespace governikus;

defineSingleton(HelpAction)

Q_DECLARE_LOGGING_CATEGORY(gui)

//Mapping object name to help file, \see AppQtMainWidget::onContentActionClicked()
const QMap<QString, QString> HelpAction::mHelpMapping = {
	{QStringLiteral("setupAssistant"), QStringLiteral("wizard-info.html")},
	{QStringLiteral("setupAssistantSetupCompleted"), QString()},
	{QStringLiteral("ausweisenPage"), QStringLiteral("identify.html")},
	{QStringLiteral("providerPage"), QStringLiteral("provider.html")},
	{QStringLiteral("historyPage"), QStringLiteral("history.html")},
	{QStringLiteral("generalTab"), QStringLiteral("settings-general.html")},
	{QStringLiteral("pinTab"), QStringLiteral("settings-pin-management.html")},
	{QStringLiteral("readerDeviceTab"), QStringLiteral("settings-reader-detection.html")},
	{QStringLiteral("stepChooseCardGui"), QStringLiteral("settings-reader-detection.html")},
	{QStringLiteral("index"), QStringLiteral("index.html")}
};


HelpAction& HelpAction::getInstance()
{
	return *Instance;
}


QString HelpAction::getHelpPath(QLocale::Language pLang) const
{
	const QString langDir = QCoreApplication::applicationDirPath() % QStringLiteral("/help/") % QLocale(pLang).bcp47Name().mid(0, 2) % QLatin1Char('/');

	if (QDir(langDir).exists())
	{
		return langDir;
	}

	return QString();
}


QLocale::Language HelpAction::getExistingHelpLanguage() const
{
	QLocale::Language lang = LanguageLoader::getInstance().getUsedLocale().language();
	if (!getHelpPath(lang).isNull())
	{
		return lang;
	}

	lang = LanguageLoader::getInstance().getFallbackLanguage();
	if (!getHelpPath(lang).isNull())
	{
		return lang;
	}

	return QLocale::AnyLanguage;
}


QString HelpAction::getContextMapping(const QString& pObjectName) const
{
	if (mHelpMapping.contains(pObjectName))
	{
		return mHelpMapping.value(pObjectName);
	}
	else
	{
		qCWarning(gui) << "Cannot find help mapping:" << pObjectName;
	}

	return QStringLiteral("index.html");
}


QString HelpAction::getHelpUrl(const QString& pObjectName) const
{
	QLocale::Language lang = getExistingHelpLanguage();
	if (lang == QLocale::AnyLanguage)
	{
		return getOnlineUrl();
	}

	return QUrl::fromLocalFile(getHelpPath(lang)).toString() + getContextMapping(pObjectName);
}


QString HelpAction::getOnlineUrl(const QString& pObjectName)
{
#ifdef Q_OS_MACOS
	const QLatin1String osPath("macOS");
#else
	const QLatin1String osPath("Windows");
#endif

	const auto& appVersion = VersionNumber::getApplicationVersion().getVersionNumber();
	const QString ver = QString::number(appVersion.majorVersion()) % QLatin1Char('.') % QString::number(appVersion.minorVersion());
	const QString locale = QLocale(LanguageLoader::getInstance().getUsedLocale().language()).bcp47Name().mid(0, 2);
	const QString mapping = getInstance().getContextMapping(pObjectName);
	return QStringLiteral("https://www.ausweisapp.bund.de/ausweisapp2/handbuch/") % ver % QLatin1Char('/') % locale % QLatin1Char('/') % osPath % QLatin1Char('/') % mapping;
}


void HelpAction::openContextHelp(const QString& pObjectName)
{
	const auto& url = QUrl(getOnlineUrl(pObjectName));
	qCDebug(gui) << "Open online help:" << pObjectName << '|' << url;
	QDesktopServices::openUrl(url);
}
