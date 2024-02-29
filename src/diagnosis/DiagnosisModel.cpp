/**
 * Copyright (c) 2018-2024 Governikus GmbH & Co. KG, Germany
 */

#include "DiagnosisModel.h"

#include "AppSettings.h"
#include "BuildHelper.h"
#include "Env.h"
#include "HttpServer.h"
#include "LanguageLoader.h"
#include "RemoteServiceSettings.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QFile>
#if defined(Q_OS_WIN)
	#include <QOperatingSystemVersion>
#endif
#include <QQmlEngine>
#include <QStringList>


using namespace governikus;


DiagnosisModel::DiagnosisModel()
	: mSections()
	, mContext(new DiagnosisContext())
	, mDiagnosisController(mContext)
	, mAusweisAppSection()
	, mTimestampSection()
	, mRemoteDeviceSectionRunning(false)
	, mRemoteDeviceSection()
	, mCardReaderSectionRunning(false)
	, mCardReaderSection()
	, mPcscSectionRunning(false)
	, mPcscSection()
	, mConnectionTest()
	, mNetworkConnectionSection()
	, mNetworkInterfaceSection()
	, mAntivirusSectionRunning(false)
	, mAntivirusDetection()
	, mAntivirusSection()
	, mFirewallSectionRunning(false)
	, mFirewallDetection()
	, mFirewallSection()
{
	mSections[Section::GENERAL].reset(new SectionModel());
	QQmlEngine::setObjectOwnership(mSections[Section::GENERAL].data(), QQmlEngine::CppOwnership);

	mSections[Section::READER].reset(new SectionModel());
	QQmlEngine::setObjectOwnership(mSections[Section::READER].data(), QQmlEngine::CppOwnership);

	mSections[Section::NETWORK].reset(new SectionModel());
	QQmlEngine::setObjectOwnership(mSections[Section::NETWORK].data(), QQmlEngine::CppOwnership);

	mSections[Section::SECURITY].reset(new SectionModel());
	QQmlEngine::setObjectOwnership(mSections[Section::SECURITY].data(), QQmlEngine::CppOwnership);


	initContent();
	mDiagnosisController.run();
}


DiagnosisModel::~DiagnosisModel()
{
	disconnectSignals();
}


QString DiagnosisModel::getSectionName(Section pSection) const
{
	switch (pSection)
	{
		case Section::GENERAL:
			return QCoreApplication::applicationName();

		case Section::READER:
			//: LABEL DESKTOP
			return tr("Card reader");

		case Section::NETWORK:
			//: LABEL DESKTOP
			return tr("Network");

		case Section::SECURITY:
			//: LABEL DESKTOP
			return tr("Antivirus and firewall");
	}

	Q_UNREACHABLE();
}


void DiagnosisModel::initGeneralSections()
{
	mAusweisAppSection.clear();
	BuildHelper::processInformationHeader([this](const QString& pKey, const QString& pValue){
			mAusweisAppSection << ContentItem(pKey, pValue);
		});

	//: LABEL DESKTOP
	const auto& title = tr("Time of diagnosis");
	//: LABEL DESKTOP
	const auto& content = tr("Initial diagnosis running, please wait.");
	mTimestampSection << ContentItem(title, content);
}


void DiagnosisModel::updateGeneralSection()
{
	mSections[Section::GENERAL]->removeAllItems();
	mSections[Section::GENERAL]->addContent(mAusweisAppSection);
	mSections[Section::GENERAL]->addContent(mTimestampSection);
}


void DiagnosisModel::initNetworkSections()
{
	mSections[Section::NETWORK]->removeAllItems();
	mNetworkInterfaceSection.clear();
	mNetworkConnectionSection.clear();
}


void DiagnosisModel::updateNetworkSection(bool pUpdateTimestamp)
{
	mSections[Section::NETWORK]->removeAllItems();
	mSections[Section::NETWORK]->addContent(mNetworkConnectionSection);
	mSections[Section::NETWORK]->addContent(mNetworkInterfaceSection);
	if (pUpdateTimestamp)
	{
		mContext->setTimestamp(QDateTime::currentDateTime());
	}
}


void DiagnosisModel::initCardReaderSections()
{
	mPcscSection.clear();
	//: LABEL DESKTOP
	mPcscSection << ContentItem(tr("PC/SC driver information"));
	//: LABEL DESKTOP
	mPcscSection << ContentItem(QString(), tr("Diagnosis is running..."));
	mPcscSectionRunning = true;
	mCardReaderSection.clear();
	//: LABEL DESKTOP
	mCardReaderSection << ContentItem(tr("Card reader"));
	//: LABEL DESKTOP
	mCardReaderSection << ContentItem(QString(), tr("Diagnosis is running..."));
	mCardReaderSectionRunning = true;
	mRemoteDeviceSection.clear();
	//: LABEL DESKTOP
	mRemoteDeviceSection << ContentItem(tr("Paired smartphones"));
	//: LABEL DESKTOP
	mRemoteDeviceSection << ContentItem(QString(), tr("Diagnosis is running..."));
	mRemoteDeviceSectionRunning = true;
	Q_EMIT fireRunningChanged();
}


void DiagnosisModel::updateCardReaderSection(bool pUpdateTimestamp)
{
	mSections[Section::READER]->removeAllItems();
	mSections[Section::READER]->addContent(mRemoteDeviceSection);
	mSections[Section::READER]->addContent(mCardReaderSection);
	mSections[Section::READER]->addContent(mPcscSection);
	if (pUpdateTimestamp)
	{
		mContext->setTimestamp(QDateTime::currentDateTime());
	}
}


void DiagnosisModel::initAntiVirusAndFirewallSection()
{
	mSections[Section::SECURITY]->removeAllItems();
	mAntivirusSection.clear();
	mFirewallSection.clear();

	//: LABEL DESKTOP
	mAntivirusSection << ContentItem(tr("Antivirus information"));
#ifdef Q_OS_WIN
	mAntivirusSection << ContentItem(QString(), tr("Diagnosis is running..."));
	mAntivirusSectionRunning = true;
#else
	//: LABEL DESKTOP
	mAntivirusSection << ContentItem(QString(), tr("No Antivirus information available on this platform."));
#endif
	//: LABEL DESKTOP
	mFirewallSection << ContentItem(tr("Firewall information"));
#ifdef Q_OS_WIN
	//: LABEL DESKTOP
	mFirewallSection << ContentItem(QString(), tr("Diagnosis is running..."));
	mFirewallSectionRunning = true;
#else
	//: LABEL DESKTOP
	mFirewallSection << ContentItem(QString(), tr("No Firewall information available on this platform."));
#endif

#ifdef Q_OS_WIN
	Q_EMIT fireRunningChanged();
#endif
}


void DiagnosisModel::updateAntiVirusAndFirewallSection(bool pUpdateTimestamp)
{
	mSections[Section::SECURITY]->removeAllItems();
	mSections[Section::SECURITY]->addContent(mAntivirusSection);
	mSections[Section::SECURITY]->addContent(mFirewallSection);
	if (pUpdateTimestamp)
	{
		mContext->setTimestamp(QDateTime::currentDateTime());
	}
}


void DiagnosisModel::connectSignals()
{
	connect(mContext.data(), &DiagnosisContext::timestampChanged, this, &DiagnosisModel::onTimestampChanged);
	connect(mContext.data(), &DiagnosisContext::fireNetworkInfoChanged, this, &DiagnosisModel::onNetworkInfoChanged);
	connect(&mConnectionTest, &DiagnosisConnectionTest::fireConnectionTestDone, this, &DiagnosisModel::onConnectionTestDone);
	connect(mContext.data(), &DiagnosisContext::pcscInfoChanged, this, &DiagnosisModel::onPcscInfoChanged);
	const RemoteServiceSettings& settings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
	connect(&settings, &RemoteServiceSettings::fireTrustedRemoteInfosChanged, this, &DiagnosisModel::onRemoteInfosChanged);
	connect(mContext.data(), &DiagnosisContext::readerInfosChanged, this, &DiagnosisModel::onReaderInfosChanged);

#ifdef Q_OS_WIN
	connect(&mAntivirusDetection, &DiagnosisAntivirusDetection::fireAntivirusInformationChanged, this, &DiagnosisModel::onAntivirusInformationChanged);
	connect(&mAntivirusDetection, &DiagnosisAntivirusDetection::fireDetectionFailed, this, &DiagnosisModel::onAntivirusDetectionFailed);
	connect(&mFirewallDetection, &DiagnosisFirewallDetection::fireFirewallInformationReady, this, &DiagnosisModel::onFirewallInformationReady);
	connect(&mFirewallDetection, &DiagnosisFirewallDetection::fireDetectionFailed, this, &DiagnosisModel::onFirewallInformationFailed);
#endif
}


void DiagnosisModel::disconnectSignals()
{
	disconnect(mContext.data(), &DiagnosisContext::timestampChanged, this, &DiagnosisModel::onTimestampChanged);
	disconnect(mContext.data(), &DiagnosisContext::fireNetworkInfoChanged, this, &DiagnosisModel::onNetworkInfoChanged);
	disconnect(&mConnectionTest, &DiagnosisConnectionTest::fireConnectionTestDone, this, &DiagnosisModel::onConnectionTestDone);
	disconnect(mContext.data(), &DiagnosisContext::pcscInfoChanged, this, &DiagnosisModel::onPcscInfoChanged);
	const RemoteServiceSettings& settings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
	disconnect(&settings, &RemoteServiceSettings::fireTrustedRemoteInfosChanged, this, &DiagnosisModel::onRemoteInfosChanged);
	disconnect(mContext.data(), &DiagnosisContext::readerInfosChanged, this, &DiagnosisModel::onReaderInfosChanged);

#ifdef Q_OS_WIN
	disconnect(&mAntivirusDetection, &DiagnosisAntivirusDetection::fireAntivirusInformationChanged, this, &DiagnosisModel::onAntivirusInformationChanged);
	disconnect(&mAntivirusDetection, &DiagnosisAntivirusDetection::fireDetectionFailed, this, &DiagnosisModel::onAntivirusDetectionFailed);
	disconnect(&mFirewallDetection, &DiagnosisFirewallDetection::fireFirewallInformationReady, this, &DiagnosisModel::onFirewallInformationReady);
	disconnect(&mFirewallDetection, &DiagnosisFirewallDetection::fireDetectionFailed, this, &DiagnosisModel::onFirewallInformationFailed);
#endif
}


QString DiagnosisModel::boolToString(bool pBoolean) const
{
	if (pBoolean)
	{
		//: LABEL DESKTOP
		return tr("Yes");
	}

	//: LABEL DESKTOP
	return tr("No");
}


QString DiagnosisModel::getAsPlaintext() const
{

#ifdef Q_OS_WIN
	static const QString endl = QStringLiteral("\r\n");
#else
	static const QString endl(QLatin1Char('\n'));
#endif

	QStringList modelPlaintext;

	const auto& sections = mSections.keys();
	for (const auto& section : sections)
	{
		modelPlaintext << getSectionName(section);
		modelPlaintext << mSections[section]->getAsPlaintext();
		modelPlaintext << endl;
	}

	return modelPlaintext.join(endl);
}


void DiagnosisModel::initContent()
{
	disconnectSignals();

	beginResetModel();

	initGeneralSections();
	updateGeneralSection();

	initCardReaderSections();
	updateCardReaderSection(false);

	initNetworkSections();
	updateNetworkSection(false);

	initAntiVirusAndFirewallSection();
	updateAntiVirusAndFirewallSection(false);

	onRemoteInfosChanged();

	endResetModel();

	connectSignals();

	mConnectionTest.startConnectionTest();

#ifdef Q_OS_WIN
	mAntivirusDetection.startInformationProcess();
	mFirewallDetection.startDetection();
#endif

}


QVariant DiagnosisModel::data(const QModelIndex& pIndex, int pRole) const
{
	const auto section = static_cast<Section>(pIndex.row());
	if (!pIndex.isValid() || !mSections.contains(section))
	{
		return QVariant();
	}

	switch (pRole)
	{
		case Qt::DisplayRole:
			return getSectionName(section);

		case ContentRole:
			return QVariant::fromValue(mSections[section]);

		default:
			return QVariant();

	}
}


int DiagnosisModel::rowCount(const QModelIndex& pParent) const
{
	Q_UNUSED(pParent)
	return static_cast<int>(mSections.size());
}


QHash<int, QByteArray> DiagnosisModel::roleNames() const
{
	QHash<int, QByteArray> roles;
	roles.insert(Qt::DisplayRole, QByteArrayLiteral("display"));
	roles.insert(ContentRole, QByteArrayLiteral("content"));
	return roles;
}


QString DiagnosisModel::getCreationTime() const
{
	return mContext->getTimestamp().toString(QStringLiteral("yyyy-MM-dd_HH-mm"));
}


void DiagnosisModel::saveToFile(const QUrl& pFilename) const
{
	QFile file(pFilename.toLocalFile());
	if (file.open(QIODevice::WriteOnly))
	{
		QString diagnosisLog = getAsPlaintext();
		file.write(diagnosisLog.toUtf8());
	}
}


bool DiagnosisModel::isRunning() const
{
	return mAntivirusSectionRunning || mFirewallSectionRunning || mCardReaderSectionRunning || mPcscSectionRunning || mRemoteDeviceSectionRunning;
}


void DiagnosisModel::onTimestampChanged()
{
	mTimestampSection.clear();

	QDateTime timestampValue = mContext->getTimestamp();
	if (timestampValue.isValid())
	{
		//: LABEL DESKTOP Datetime format according to https://doc.qt.io/qt/qdate.html#toString and https://doc.qt.io/qt/qtime.html#toString
		QString timestamp = LanguageLoader::getInstance().getUsedLocale().toString(timestampValue, tr("d. MMMM yyyy, hh:mm:ss AP"));
		//: LABEL DESKTOP
		mTimestampSection << ContentItem(tr("Time of diagnosis"), timestamp);
	}
	else
	{
		//: LABEL DESKTOP
		const auto& title = tr("Time of diagnosis");
		//: LABEL DESKTOP
		const auto& content = tr("Failed to retrieve date & time");
		mTimestampSection << ContentItem(title, content);
	}

	updateGeneralSection();
}


void DiagnosisModel::onNetworkInfoChanged()
{
	mNetworkInterfaceSection.clear();

	const auto& networkInterfaces = mContext->getNetworkInterfaces();
	for (const auto& iface : networkInterfaces)
	{
		QStringList interfaceInfos;
		//: LABEL DESKTOP Interface has no hardware address set
		QString hardwareAddress = iface.hardwareAddress().isEmpty() ? tr("<Not set>") : iface.hardwareAddress();
		//: LABEL DESKTOP
		interfaceInfos << tr("Hardware address: %1").arg(hardwareAddress);

		const auto& addresses = iface.addressEntries();
		if (addresses.isEmpty())
		{
			//: LABEL DESKTOP Interface has no IP addresses assigned
			interfaceInfos << tr("No IP addresses assigned");
			mNetworkInterfaceSection << ContentItem(iface.humanReadableName(), interfaceInfos.join(QLatin1Char('\n')));
			continue;
		}

		for (const auto& address : addresses)
		{
			const auto& ip = address.ip();
			if (ip.protocol() == QAbstractSocket::NetworkLayerProtocol::IPv4Protocol)
			{
				//: LABEL DESKTOP
				interfaceInfos << tr("IPv4 address: %1").arg(ip.toString());
			}
			else if (ip.protocol() == QAbstractSocket::NetworkLayerProtocol::IPv6Protocol)
			{
				//: LABEL DESKTOP
				interfaceInfos << tr("IPv6 address: %1").arg(ip.toString());
			}
			else
			{
				//: LABEL DESKTOP
				interfaceInfos << tr("Unknown address: %1").arg(ip.toString());
			}
		}
		mNetworkInterfaceSection << ContentItem(iface.humanReadableName(), interfaceInfos.join(QLatin1Char('\n')));
	}

	updateNetworkSection();
}


void DiagnosisModel::onConnectionTestDone()
{
	mNetworkConnectionSection.clear();

	const auto& server = Env::getShared<HttpServer>(false);

	mNetworkConnectionSection << ContentItem(
			//: LABEL DESKTOP
			tr("Service addresses"),
			//: LABEL DESKTOP
			server ? server->boundAddresses().join(QLatin1Char('\n')) : tr("Not bound"));

	QStringList proxyInfo;

	if (mConnectionTest.getIsProxySet())
	{
		//: LABEL DESKTOP
		proxyInfo << tr("Hostname: %1").arg(mConnectionTest.getProxyHostName());
		//: LABEL DESKTOP
		proxyInfo << tr("Port: %1").arg(mConnectionTest.getProxyPort());
		//: LABEL DESKTOP
		proxyInfo << tr("Type: %1").arg(mConnectionTest.getProxyType());
		//: LABEL DESKTOP list of the capabitlities of the proxy connection
		proxyInfo << tr("Capabilities: %1").arg(mConnectionTest.getProxyCapabilities());

		if (mConnectionTest.getPingTestOnProxySuccessful())
		{
			//: LABEL DESKTOP
			proxyInfo << tr("Ping test to proxy: Successful");
		}
		else
		{
			//: LABEL DESKTOP
			proxyInfo << tr("Ping test to proxy: Failed");
		}

		if (mConnectionTest.getConnectionTestWithProxySuccessful())
		{
			//: LABEL DESKTOP
			proxyInfo << tr("Connection test with proxy: Successful");
		}
		else
		{
			//: LABEL DESKTOP
			proxyInfo << tr("Connection test with proxy: Failed");
			//: LABEL DESKTOP
			proxyInfo << tr("Encountered error: %1").arg(mConnectionTest.getErrorOfConnectionTestWithProxy());
		}
	}
	else
	{
		//: LABEL DESKTOP
		proxyInfo << tr("No proxy found");
	}

	if (mConnectionTest.getConnectionTestWithoutProxySuccessful())
	{
		//: LABEL DESKTOP
		proxyInfo << tr("Connection test without proxy: Successful");
	}
	else
	{
		//: LABEL DESKTOP
		proxyInfo << tr("Connection test without proxy: Failed");
	}

	//: LABEL DESKTOP
	mNetworkConnectionSection << ContentItem(tr("Proxy information"), proxyInfo.join(QLatin1Char('\n')));

	updateNetworkSection();
}


void DiagnosisModel::onAntivirusInformationChanged()
{
	mAntivirusSection.clear();
	mAntivirusSectionRunning = false;

	const auto& antivirusInfos = mAntivirusDetection.getAntivirusInformations();
	if (antivirusInfos.isEmpty())
	{
		//: LABEL DESKTOP
		const auto& title = tr("Antivirus information");
		//: LABEL DESKTOP
		const auto& content = tr("No Antivirus software detected.");
		mAntivirusSection << ContentItem(title, content);
	}
	else
	{
		//: LABEL DESKTOP
		mAntivirusSection << ContentItem(tr("Antivirus information"));

		for (const auto& info : antivirusInfos)
		{
			QStringList avInfo;
			if (!info->getLastUpdate().isEmpty())
			{
				//: LABEL DESKTOP
				avInfo << tr("Last updated: %1").arg(info->getLastUpdate());
			}
			//: LABEL DESKTOP
			avInfo << tr("Executable path: %1").arg(info->getExePath());
			auto antivirusName = info->getDisplayName();
			mAntivirusSection << ContentItem(antivirusName, avInfo.join(QLatin1Char('\n')));
		}
	}

	updateAntiVirusAndFirewallSection();
	Q_EMIT fireRunningChanged();
}


void DiagnosisModel::onAntivirusDetectionFailed()
{
	mAntivirusSection.clear();

	//: LABEL DESKTOP
	const auto& title = tr("Antivirus information");
	//: LABEL DESKTOP
	const auto& content = tr("Antivirus detection failed.");
	mAntivirusSection << ContentItem(title, content);

	updateAntiVirusAndFirewallSection();
}


void DiagnosisModel::onFirewallInformationReady()
{
	mFirewallSection.clear();
	mFirewallSectionRunning = false;

	//: LABEL DESKTOP
	mFirewallSection << ContentItem(tr("Firewall information"));
	auto installedFirewalls = mFirewallDetection.getDetectedFirewalls();
	if (installedFirewalls.isEmpty())
	{
#if defined(Q_OS_WIN)
		if (QOperatingSystemVersion::current() < QOperatingSystemVersion::Windows8)
		{
			//: LABEL DESKTOP
			mFirewallSection << ContentItem(QString(), tr("3rd party firewalls cannot be detected on Windows 7."));
		}
		else
		{
#else
		{
#endif
			//: LABEL DESKTOP
			mFirewallSection << ContentItem(QString(), tr("No 3rd party firewalls detected"));
		}
	}
	else
	{
		QStringList firewallInfos;
		for (const auto& firewall : installedFirewalls)
		{
			firewallInfos << firewall->getName();

			QString enabled = boolToString(firewall->getEnabled());
			QString uptodate = boolToString(firewall->getUpToDate());
			//: LABEL DESKTOP
			firewallInfos << tr("Enabled: %1").arg(enabled);
			//: LABEL DESKTOP
			firewallInfos << tr("Up to date: %1").arg(uptodate);
		}
		//: LABEL DESKTOP
		mFirewallSection << ContentItem(tr("Firewalls from 3rd party vendors"), firewallInfos.join(QLatin1Char('\n')));
	}

	QStringList windowsFirewallSettings;
	QString firstRuleExists = boolToString(mFirewallDetection.getFirstRuleExists());
	QString firstRuleEnabled = boolToString(mFirewallDetection.getFirstRuleEnabled());
	//: LABEL DESKTOP
	windowsFirewallSettings << tr("Outgoing %1 rule").arg(QCoreApplication::applicationName());
	//: LABEL DESKTOP
	windowsFirewallSettings << tr("Exists: %1").arg(firstRuleExists);
	//: LABEL DESKTOP
	windowsFirewallSettings << tr("Enabled: %1").arg(firstRuleEnabled);

	QString secondRuleExists = boolToString(mFirewallDetection.getSecondRuleExists());
	QString secondRuleEnabled = boolToString(mFirewallDetection.getSecondRuleEnabled());
	//: LABEL DESKTOP
	windowsFirewallSettings << tr("Incoming %1 rule").arg(QCoreApplication::applicationName());
	//: LABEL DESKTOP
	windowsFirewallSettings << tr("Exists: %1").arg(secondRuleExists);
	//: LABEL DESKTOP
	windowsFirewallSettings << tr("Enabled: %1").arg(secondRuleEnabled);

	//: LABEL DESKTOP
	mFirewallSection << ContentItem(tr("Windows firewall rules"), windowsFirewallSettings.join(QLatin1Char('\n')));

	QStringList windowsFirewallProfiles;
	auto firewallProfiles = mFirewallDetection.getFirewallProfiles();
	for (const auto& profile : firewallProfiles)
	{
		windowsFirewallProfiles << profile->getName();
		QString enabled = boolToString(profile->getEnabled());
		windowsFirewallProfiles << tr("Enabled: %1").arg(enabled);
		//: LABEL DESKTOP
	}

	if (windowsFirewallProfiles.isEmpty())
	{
		//: LABEL DESKTOP
		mFirewallSection << ContentItem(tr("Windows firewall profiles"), tr("Diagnosis is running..."));
	}
	else
	{
		//: LABEL DESKTOP
		mFirewallSection << ContentItem(tr("Windows firewall profiles"), windowsFirewallProfiles.join(QLatin1Char('\n')));
	}

	updateAntiVirusAndFirewallSection();
	Q_EMIT fireRunningChanged();
}


void DiagnosisModel::onFirewallInformationFailed()
{
	mFirewallSection.clear();

	//: LABEL DESKTOP
	const auto& title = tr("Firewall information");
	//: LABEL DESKTOP
	const auto& content = tr("An error occurred while trying to gather firewall information. Please check the log for more information.");
	mFirewallSection << ContentItem(title, content);

	updateAntiVirusAndFirewallSection();
}


void DiagnosisModel::onPcscInfoChanged()
{
	mPcscSection.clear();
	mPcscSectionRunning = false;

	//: LABEL DESKTOP
	mPcscSection << ContentItem(tr("PC/SC information"));
	//: LABEL DESKTOP
	mPcscSection << ContentItem(tr("Version"), mContext->getPcscVersion());

	QStringList pcscInfo;
	for (const auto& info : mContext->getPcscComponents())
	{
		pcscInfo << info.getDescription();
		//: LABEL DESKTOP
		pcscInfo << tr("Vendor: %1").arg(info.getManufacturer());
		//: LABEL DESKTOP
		pcscInfo << tr("Version: %1").arg(info.getVersion());
		//: LABEL DESKTOP
		pcscInfo << tr("File path: %1").arg(info.getPath());
	}
	if (!pcscInfo.empty())
	{
		//: LABEL DESKTOP
		mPcscSection << ContentItem(tr("Components"), pcscInfo.join(QLatin1Char('\n')));
	}

	pcscInfo.clear();
	for (const auto& info : mContext->getPcscDrivers())
	{
		pcscInfo << info.getDescription();
		//: LABEL DESKTOP
		pcscInfo << tr("Vendor: %1").arg(info.getManufacturer());
		//: LABEL DESKTOP
		pcscInfo << tr("Version: %1").arg(info.getVersion());
		//: LABEL DESKTOP
		pcscInfo << tr("File path: %1").arg(info.getPath());
	}
	if (!pcscInfo.empty())
	{
		//: LABEL DESKTOP
		mPcscSection << ContentItem(tr("Driver"), pcscInfo.join(QLatin1Char('\n')));
	}

	updateCardReaderSection();
	Q_EMIT fireRunningChanged();
}


void DiagnosisModel::onRemoteInfosChanged()
{
	mRemoteDeviceSection.clear();
	mRemoteDeviceSectionRunning = false;

	const RemoteServiceSettings& settings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
	const auto& trustedCertificates = settings.getTrustedCertificates();

	//: LABEL DESKTOP
	mRemoteDeviceSection << ContentItem(tr("Paired smartphones"));

	if (trustedCertificates.isEmpty())
	{
		//: LABEL DESKTOP
		mRemoteDeviceSection << ContentItem(QString(), tr("No devices paired."));
	}

	for (const auto& cert : trustedCertificates)
	{
		const auto& info = settings.getRemoteInfo(cert);

		if (!info.getFingerprint().isEmpty())
		{
			//: LABEL DESKTOP Datetime format according to https://doc.qt.io/qt/qdate.html#toString and https://doc.qt.io/qt/qtime.html#toString
			const QString& timestamp = LanguageLoader::getInstance().getUsedLocale().toString(info.getLastConnected(), tr("dd.MM.yyyy, hh:mm:ss"));
			//: LABEL DESKTOP
			mRemoteDeviceSection << ContentItem(info.getNameEscaped(), tr("Last connection: %1").arg(timestamp));
		}
		else
		{
			//: LABEL DESKTOP
			mRemoteDeviceSection << ContentItem(RemoteServiceSettings::generateFingerprint(cert), tr("No information found for this certificate."));
		}
	}

	updateCardReaderSection();
	Q_EMIT fireRunningChanged();
}


void DiagnosisModel::onReaderInfosChanged()
{
	mCardReaderSection.clear();
	mCardReaderSectionRunning = false;

	//: LABEL DESKTOP
	mCardReaderSection << ContentItem(tr("Connected Card readers"));

	const auto& readerInfos = mContext->getReaderInfos();
	const auto& readerInfoNoDriver = mContext->getReaderInfosNoDriver();
	if (readerInfos.isEmpty() && readerInfoNoDriver.isEmpty())
	{
		//: LABEL DESKTOP
		mCardReaderSection << ContentItem(QString(), tr("No supported reader found."));
	}

	for (const auto& info : readerInfos)
	{
		QStringList infoList;

		//: LABEL DESKTOP
		const auto& basicReader = tr("Basic card reader");
		//: LABEL DESKTOP
		const auto& standardReader = tr("Standard / comfort card reader");
		QString readerType = info.isBasicReader() ? basicReader : standardReader;
		//: LABEL DESKTOP
		infoList << tr("Type: %1").arg(readerType);
		//: LABEL DESKTOP
		infoList << tr("Card: %1").arg(info.getCardTypeString());

		if (info.hasEid())
		{
			//: LABEL DESKTOP
			infoList << tr("Retry counter: %1").arg(info.getRetryCounter());
		}

		mCardReaderSection << ContentItem(info.getName(), infoList.join(QLatin1Char('\n')));
	}

	for (const auto& info : readerInfoNoDriver)
	{
		//: LABEL DESKTOP
		mCardReaderSection << ContentItem(info.getName(), tr("No driver installed"));
	}

	updateCardReaderSection();
	Q_EMIT fireRunningChanged();
}
