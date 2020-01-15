/*!
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
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
const QMap<QString, QString> HelpAction::mWidgetHelpMapping = {
	{QStringLiteral("setupAssistant"), QStringLiteral("wizard-info.html")},
	{QStringLiteral("setupAssistantSetupCompleted"), QString()},
	{QStringLiteral("identifyPage"), QStringLiteral("identify.html")},
	{QStringLiteral("providerPage"), QStringLiteral("provider.html")},
	{QStringLiteral("historyPage"), QStringLiteral("history.html")},
	{QStringLiteral("generalTab"), QStringLiteral("settings-general.html")},
	{QStringLiteral("pinTab"), QStringLiteral("settings-pin-management.html")},
	{QStringLiteral("readerDeviceTab"), QStringLiteral("settings-reader-detection.html")},
	{QStringLiteral("stepChooseCardGui"), QStringLiteral("settings-reader-detection.html")},
	{QStringLiteral("index"), QStringLiteral("index.html")}
};

const QMap<QString, QString> HelpAction::mQmlHelpMapping = {
	{QStringLiteral("applicationOverview"), QStringLiteral("index.html")},
	{QStringLiteral("index"), QStringLiteral("index.html")},
	{QStringLiteral("selfauthentication"), QStringLiteral("selfauthentication.html")},
	{QStringLiteral("authentication"), QStringLiteral("provider-identify.html")},
	{QStringLiteral("provider"), QStringLiteral("provider.html")},
	{QStringLiteral("providerDetails"), QStringLiteral("provider-list.html#provider-details")},
	{QStringLiteral("history"), QStringLiteral("history.html")},
	{QStringLiteral("settings"), QStringLiteral("settings.html")},
	{QStringLiteral("settingsRemoteReader"), QStringLiteral("settings-remote-reader.html")},
	{QStringLiteral("settingsPcscReader"), QStringLiteral("settings-pcsc-reader.html")},
	{QStringLiteral("settingsSecurityPrivacy"), QStringLiteral("settings-security-privacy.html")},
	{QStringLiteral("settingsDeveloper"), QStringLiteral("settings-developer.html")},
	{QStringLiteral("pinManagement"), QStringLiteral("pin-management.html")},
	{QStringLiteral("helpSection"), QStringLiteral("help-section.html")},
	{QStringLiteral("diagnosis"), QStringLiteral("help-section-diagnosis.html")},
	{QStringLiteral("applicationLog"), QStringLiteral("help-section-application-log.html")},
	{QStringLiteral("helpVersioninformation"), QStringLiteral("help-section-versioninformation.html")},
	{QStringLiteral("setupAssistant"), QStringLiteral("setup-assistant.html")}
};

const QString HelpAction::mBaseUrl = QStringLiteral("https://www.ausweisapp.bund.de/ausweisapp2/");

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


QString HelpAction::getContextMapping(const QString& pObjectName, bool pWidgetUiHelp) const
{
	const auto& mapping = pWidgetUiHelp ? mWidgetHelpMapping : mQmlHelpMapping;
	if (mapping.contains(pObjectName))
	{
		return mapping.value(pObjectName);
	}
	else
	{
		qCWarning(gui) << "Cannot find help mapping:" << pObjectName;
	}

	return QStringLiteral("index.html");
}


QString HelpAction::getHelpUrl(const QString& pObjectName, bool pWidgetUiHelp) const
{
	QLocale::Language lang = getExistingHelpLanguage();
	if (lang == QLocale::AnyLanguage)
	{
		return getOnlineUrl(QString(), pWidgetUiHelp);
	}

	return QUrl::fromLocalFile(getHelpPath(lang)).toString() + getContextMapping(pObjectName, pWidgetUiHelp);
}


QString HelpAction::getOnlineUrl(const QString& pObjectName, bool pWidgetUiHelp)
{
#ifdef Q_OS_MACOS
	const QLatin1String osPath("macOS");
#else
	const QLatin1String osPath("Windows");
#endif

	const auto& appVersion = VersionNumber::getApplicationVersion().getVersionNumber();
	const QString ver = pWidgetUiHelp ? QStringLiteral("1.16") : QString::number(appVersion.majorVersion()) % QLatin1Char('.') % QString::number(appVersion.minorVersion());
	const QString locale = QLocale(LanguageLoader::getInstance().getUsedLocale().language()).bcp47Name().mid(0, 2);
	const QString mapping = getInstance().getContextMapping(pObjectName, pWidgetUiHelp);
	const QString directory = pWidgetUiHelp ? QStringLiteral("handbuch") : QStringLiteral("help");
	return mBaseUrl % directory % QLatin1Char('/') % ver % QLatin1Char('/') % locale % QLatin1Char('/') % osPath % QLatin1Char('/') % mapping;
}


void HelpAction::openContextHelp(const QString& pObjectName, bool pWidgetUiHelp)
{
	const auto& url = QUrl(getOnlineUrl(pObjectName, pWidgetUiHelp));
	qCDebug(gui) << "Open online help:" << pObjectName << '|' << url;
	QDesktopServices::openUrl(url);
}
