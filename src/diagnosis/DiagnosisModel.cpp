/*
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

#include "DiagnosisModel.h"

#include "AppSettings.h"
#include "BuildHelper.h"
#include "Env.h"
#include "LanguageLoader.h"
#include "RemoteServiceSettings.h"

#include <QOperatingSystemVersion>


using namespace governikus;


DiagnosisModel::DiagnosisModel(const QSharedPointer<DiagnosisContext>& pContext)
	: mContext(pContext)
	, mAntivirusSectionRunning(false)
	, mFirewallSectionRunning(false)
	, mCardReaderSectionRunning(false)
	, mPcscSectionRunning(false)
	, mRemoteDeviceSectionRunning(false)
{
	reloadContent();
}


DiagnosisModel::~DiagnosisModel()
{
	disconnectSignals();
}


QSharedPointer<SectionModel> DiagnosisModel::createAusweisApp2Section()
{
	QSharedPointer<SectionModel> aa2Section(new SectionModel());

	BuildHelper::processInformationHeader([aa2Section](const QString& pKey, const QString& pValue){
			aa2Section->addItem(pKey, pValue);
		});

	//: LABEL DESKTOP
	const auto& title = tr("Time of diagnosis");
	//: LABEL DESKTOP
	const auto& content = tr("Initial diagnosis running, please wait.");

	mTimestampItem = QSharedPointer<ContentItem>::create(title, content);
	aa2Section->addItem(mTimestampItem);

	return aa2Section;
}


void DiagnosisModel::createNetworkSection()
{
	mNetworkInterfaceSection = QSharedPointer<SectionModel>::create();
	mNetworkConnectionSection = QSharedPointer<SectionModel>::create();
	mCombinedNetworkSection = QSharedPointer<SectionModel>::create();
}


void DiagnosisModel::createCardReaderSection()
{
	mCombinedReaderSection = QSharedPointer<SectionModel>::create();
	mPcscSection = QSharedPointer<SectionModel>::create();
	//: LABEL DESKTOP
	mPcscSection->addTitleWithoutContent(tr("PC/SC driver information"));
	//: LABEL DESKTOP
	mPcscSection->addItemWithoutTitle(tr("Diagnosis is running..."));
	mPcscSectionRunning = true;
	mCardReaderSection = QSharedPointer<SectionModel>::create();
	//: LABEL DESKTOP
	mCardReaderSection->addTitleWithoutContent(tr("Card reader"));
	//: LABEL DESKTOP
	mCardReaderSection->addItemWithoutTitle(tr("Diagnosis is running..."));
	mCardReaderSectionRunning = true;
	mRemoteDeviceSection = QSharedPointer<SectionModel>::create();
	//: LABEL DESKTOP
	mRemoteDeviceSection->addTitleWithoutContent(tr("Paired smartphones"));
	//: LABEL DESKTOP
	mRemoteDeviceSection->addItemWithoutTitle(tr("Diagnosis is running..."));
	mRemoteDeviceSectionRunning = true;
	Q_EMIT fireRunningChanged();
}


void DiagnosisModel::createAntiVirusAndFirewallSection()
{
	mAntivirusSection = QSharedPointer<SectionModel>::create();
	mFirewallSection = QSharedPointer<SectionModel>::create();
	mCombinedAntivirusFirewallSection = QSharedPointer<SectionModel>::create();

	//: LABEL DESKTOP
	mAntivirusSection->addTitleWithoutContent(tr("Antivirus information"));
#ifdef Q_OS_WIN
	//: LABEL DESKTOP
	mAntivirusSection->addItemWithoutTitle(tr("Diagnosis is running..."));
	mAntivirusSectionRunning = true;
#else
	//: LABEL DESKTOP
	mAntivirusSection->addItemWithoutTitle(tr("No Antivirus information available on this platform."));
#endif
	//: LABEL DESKTOP
	mFirewallSection->addTitleWithoutContent(tr("Firewall information"));
#ifdef Q_OS_WIN
	//: LABEL DESKTOP
	mFirewallSection->addItemWithoutTitle(tr("Diagnosis is running..."));
	mFirewallSectionRunning = true;
#else
	//: LABEL DESKTOP
	mFirewallSection->addItemWithoutTitle(tr("No Firewall information available on this platform."));
#endif

#ifdef Q_OS_WIN
	Q_EMIT fireRunningChanged();
#endif
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


QVariant DiagnosisModel::data(const QModelIndex& pIndex, int pRole) const
{
	const int row = pIndex.row();
	if (!pIndex.isValid() || row >= mSections.size())
	{
		return QVariant();
	}

	switch (pRole)
	{
		case Qt::DisplayRole:
			return mSections.at(row).first;

		case ContentRole:
			return QVariant::fromValue(mSections.at(row).second);

		default:
			return QVariant();

	}
}


int DiagnosisModel::rowCount(const QModelIndex& pParent) const
{
	Q_UNUSED(pParent)
	return mSections.size();
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


QString DiagnosisModel::getAsPlaintext() const
{

#ifdef Q_OS_WIN
	static const QString endl = QStringLiteral("\r\n");
#else
	static const QString endl(QLatin1Char('\n'));
#endif

	QStringList modelPlaintext;
	for (const auto& sectionPair : std::as_const(mSections))
	{
		modelPlaintext << sectionPair.first;
		modelPlaintext << sectionPair.second->getAsPlaintext();
		modelPlaintext << endl;
	}

	return modelPlaintext.join(endl);
}


bool DiagnosisModel::isRunning() const
{
	return mAntivirusSectionRunning || mFirewallSectionRunning || mCardReaderSectionRunning || mPcscSectionRunning || mRemoteDeviceSectionRunning;
}


void DiagnosisModel::onTimestampChanged()
{
	QDateTime timestampValue = mContext->getTimestamp();
	if (!timestampValue.isValid())
	{
		//: LABEL DESKTOP
		mTimestampItem->mContent = tr("Failed to retrieve date & time");
	}
	else
	{
		//: LABEL DESKTOP Timestamp, formatted according to the selected language
		QString timestamp = LanguageLoader::getInstance().getUsedLocale().toString(timestampValue, tr("d. MMMM yyyy, hh:mm:ss AP"));
		mTimestampItem->mContent = timestamp;
	}

	mSections.at(0).second->emitDataChangedForItem(mTimestampItem);
}


void DiagnosisModel::onNetworkInfoChanged()
{
	mNetworkInterfaceSection->removeAllItems();

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
			mNetworkInterfaceSection->addItem(iface.humanReadableName(), interfaceInfos.join(QLatin1Char('\n')));
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

		mNetworkInterfaceSection->addItem(iface.humanReadableName(), interfaceInfos.join(QLatin1Char('\n')));
	}

	mCombinedNetworkSection->replaceWithSections({mNetworkConnectionSection, mNetworkInterfaceSection});
	mContext->setTimestamp(QDateTime::currentDateTime());
}


void DiagnosisModel::onConnectionTestDone()
{
	mNetworkConnectionSection->removeAllItems();
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
	mNetworkConnectionSection->addItem(tr("Proxy information"), proxyInfo.join(QLatin1Char('\n')));
	mCombinedNetworkSection->replaceWithSections({mNetworkConnectionSection, mNetworkInterfaceSection});
	mContext->setTimestamp(QDateTime::currentDateTime());
}


void DiagnosisModel::onAntivirusInformationChanged()
{
	mAntivirusSection->removeAllItems();
	mAntivirusSectionRunning = false;

	const auto& antivirusInfos = mAntivirusDetection.getAntivirusInformations();
	if (antivirusInfos.isEmpty())
	{
		//: LABEL DESKTOP
		const auto& title = tr("Antivirus information");
		//: LABEL DESKTOP
		const auto& content = tr("No Antivirus software detected.");
		mAntivirusSection->addItem(title, content);
	}
	else
	{
		//: LABEL DESKTOP
		mAntivirusSection->addTitleWithoutContent(tr("Antivirus information"));

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
			mAntivirusSection->addItem(antivirusName, avInfo.join(QLatin1Char('\n')));
		}
	}

	mCombinedAntivirusFirewallSection->replaceWithSections({mAntivirusSection, mFirewallSection});
	mContext->setTimestamp(QDateTime::currentDateTime());
	Q_EMIT fireRunningChanged();
}


void DiagnosisModel::onAntivirusDetectionFailed()
{
	mAntivirusSection->removeAllItems();

	//: LABEL DESKTOP
	const auto& title = tr("Antivirus information");
	//: LABEL DESKTOP
	const auto& content = tr("Antivirus detection failed.");
	mAntivirusSection->addItem(title, content);
	mCombinedAntivirusFirewallSection->replaceWithSections({mAntivirusSection, mFirewallSection});
	mContext->setTimestamp(QDateTime::currentDateTime());
}


void DiagnosisModel::onFirewallInformationReady()
{
	mFirewallSection->removeAllItems();
	mFirewallSectionRunning = false;

	//: LABEL DESKTOP
	mFirewallSection->addTitleWithoutContent(tr("Firewall information"));
	auto installedFirewalls = mFirewallDetection.getDetectedFirewalls();
	if (installedFirewalls.isEmpty())
	{
#if defined(Q_OS_WIN)
		if (QOperatingSystemVersion::current() < QOperatingSystemVersion::Windows8)
		{
			//: LABEL DESKTOP
			mFirewallSection->addItemWithoutTitle(tr("Third party firewalls cannot be detected on Windows 7."));
		}
		else
		{
#else
		{
#endif
			//: LABEL DESKTOP
			mFirewallSection->addItemWithoutTitle(tr("No third party firewalls detected"));
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
		mFirewallSection->addItem(tr("Firewalls from third party vendors"), firewallInfos.join(QLatin1Char('\n')));
	}

	QStringList windowsFirewallSettings;
	QString firstRuleExists = boolToString(mFirewallDetection.getFirstRuleExists());
	QString firstRuleEnabled = boolToString(mFirewallDetection.getFirstRuleEnabled());
	//: LABEL DESKTOP
	windowsFirewallSettings << tr("Outgoing AusweisApp2 rule");
	//: LABEL DESKTOP
	windowsFirewallSettings << tr("Exists: %1").arg(firstRuleExists);
	//: LABEL DESKTOP
	windowsFirewallSettings << tr("Enabled: %1").arg(firstRuleEnabled);

	QString secondRuleExists = boolToString(mFirewallDetection.getSecondRuleExists());
	QString secondRuleEnabled = boolToString(mFirewallDetection.getSecondRuleEnabled());
	//: LABEL DESKTOP
	windowsFirewallSettings << tr("Incoming AusweisApp2 rule");
	//: LABEL DESKTOP
	windowsFirewallSettings << tr("Exists: %1").arg(secondRuleExists);
	//: LABEL DESKTOP
	windowsFirewallSettings << tr("Enabled: %1").arg(secondRuleEnabled);

	//: LABEL DESKTOP
	mFirewallSection->addItem(tr("Windows firewall rules"), windowsFirewallSettings.join(QLatin1Char('\n')));

	QStringList windowsFirewallProfiles;
	auto firewallProfiles = mFirewallDetection.getFirewallProfiles();
	for (const auto& profile : firewallProfiles)
	{
		windowsFirewallProfiles << profile->getName();
		QString enabled = boolToString(profile->getEnabled());
		//: LABEL DESKTOP
		windowsFirewallProfiles << tr("Enabled: %1").arg(enabled);
	}

	//: LABEL DESKTOP
	mFirewallSection->addItem(tr("Windows firewall profiles"), windowsFirewallProfiles.join(QLatin1Char('\n')));

	mCombinedAntivirusFirewallSection->replaceWithSections({mAntivirusSection, mFirewallSection});
	mContext->setTimestamp(QDateTime::currentDateTime());
	Q_EMIT fireRunningChanged();
}


void DiagnosisModel::onFirewallInformationFailed()
{
	mFirewallSection->removeAllItems();
	//: LABEL DESKTOP
	const auto& title = tr("Firewall information");
	//: LABEL DESKTOP
	const auto& content = tr("An error occurred while trying to gather firewall information. Please check the log for more information.");
	mFirewallSection->addItem(title, content);
	mCombinedAntivirusFirewallSection->replaceWithSections({mAntivirusSection, mFirewallSection});
	mContext->setTimestamp(QDateTime::currentDateTime());
}


void DiagnosisModel::onPcscInfoChanged()
{
	mPcscSection->removeAllItems();
	mPcscSectionRunning = false;

	//: LABEL DESKTOP
	mPcscSection->addTitleWithoutContent(tr("PC/SC information"));
	//: LABEL DESKTOP
	mPcscSection->addItem(tr("Version"), mContext->getPcscVersion());

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
		mPcscSection->addItem(tr("Components"), pcscInfo.join(QLatin1Char('\n')));
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
		mPcscSection->addItem(tr("Driver"), pcscInfo.join(QLatin1Char('\n')));
	}

	mCombinedReaderSection->replaceWithSections({mRemoteDeviceSection, mCardReaderSection, mPcscSection});
	mContext->setTimestamp(QDateTime::currentDateTime());
	Q_EMIT fireRunningChanged();
}


void DiagnosisModel::onRemoteInfosChanged()
{
	mRemoteDeviceSection->removeAllItems();
	mRemoteDeviceSectionRunning = false;

	const RemoteServiceSettings& settings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
	const auto& trustedCertificates = settings.getTrustedCertificates();

	//: LABEL DESKTOP
	mRemoteDeviceSection->addTitleWithoutContent(tr("Paired smartphones"));

	if (trustedCertificates.isEmpty())
	{
		//: LABEL DESKTOP
		mRemoteDeviceSection->addItemWithoutTitle(tr("No devices paired."));
	}

	for (const auto& cert : trustedCertificates)
	{
		QStringList certInfo;
		const auto& info = settings.getRemoteInfo(cert);

		if (!info.getFingerprint().isEmpty())
		{
			//: LABEL DESKTOP Timestamp
			const QString& timestamp = LanguageLoader::getInstance().getUsedLocale().toString(info.getLastConnected(), tr("dd.MM.yyyy, hh:mm:ss"));
			//: LABEL DESKTOP
			mRemoteDeviceSection->addItem(info.getNameEscaped(), tr("Last connection: %1").arg(timestamp));
		}
		else
		{
			//: LABEL DESKTOP
			mRemoteDeviceSection->addItem(RemoteServiceSettings::generateFingerprint(cert), tr("No information found for this certificate."));
		}
	}

	mCombinedReaderSection->replaceWithSections({mRemoteDeviceSection, mCardReaderSection, mPcscSection});
	mContext->setTimestamp(QDateTime::currentDateTime());
	Q_EMIT fireRunningChanged();
}


void DiagnosisModel::onReaderInfosChanged()
{
	mCardReaderSection->removeAllItems();
	mCardReaderSectionRunning = false;

	//: LABEL DESKTOP
	mCardReaderSection->addTitleWithoutContent(tr("Connected Card readers"));

	const auto& readerInfos = mContext->getReaderInfos();
	const auto& readerInfoNoDriver = mContext->getReaderInfosNoDriver();
	if (readerInfos.isEmpty() && readerInfoNoDriver.isEmpty())
	{
		//: LABEL DESKTOP
		mCardReaderSection->addItemWithoutTitle(tr("No supported reader found."));
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

		mCardReaderSection->addItem(info.getName(), infoList.join(QLatin1Char('\n')));
	}

	for (const auto& info : readerInfoNoDriver)
	{
		//: LABEL ALL_PLATFORMS
		mCardReaderSection->addItem(info.getName(), tr("No driver installed"));
	}

	mCombinedReaderSection->replaceWithSections({mRemoteDeviceSection, mCardReaderSection, mPcscSection});
	mContext->setTimestamp(QDateTime::currentDateTime());
	Q_EMIT fireRunningChanged();
}


void DiagnosisModel::reloadContent()
{
	disconnectSignals();

	beginResetModel();
	mSections.clear();

	mSections.append(qMakePair(QCoreApplication::applicationName(), createAusweisApp2Section()));

	createCardReaderSection();
	mCombinedReaderSection->replaceWithSections({mRemoteDeviceSection, mCardReaderSection, mPcscSection});
	//: LABEL DESKTOP
	mSections.append(qMakePair(tr("Card reader"), mCombinedReaderSection));

	createNetworkSection();
	mCombinedNetworkSection->replaceWithSections({mNetworkConnectionSection, mNetworkInterfaceSection});
	//: LABEL DESKTOP
	mSections.append(qMakePair(tr("Network"), mCombinedNetworkSection));

	createAntiVirusAndFirewallSection();
	mCombinedAntivirusFirewallSection->replaceWithSections({mAntivirusSection, mFirewallSection});
	//: LABEL DESKTOP
	mSections.append(qMakePair(tr("Antivirus and firewall"), mCombinedAntivirusFirewallSection));

	onRemoteInfosChanged();
	mConnectionTest.startConnectionTest();

#ifdef Q_OS_WIN
	mAntivirusDetection.startInformationProcess();
	mFirewallDetection.startDetection();
#endif

	connectSignals();

	endResetModel();
}
