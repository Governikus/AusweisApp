/*
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

#include "DiagnosisModel.h"

#include "AppSettings.h"
#include "BuildHelper.h"
#include "Env.h"
#include "GeneralSettings.h"
#include "LanguageLoader.h"
#include "RemoteServiceSettings.h"

#include <QOperatingSystemVersion>


using namespace governikus;


DiagnosisModel::DiagnosisModel(const QSharedPointer<DiagnosisContext>& pContext)
	: mContext(pContext)
{
	reloadContent();
}


DiagnosisModel::~DiagnosisModel()
{
	disconnectSignals();
}


QSharedPointer<SectionModel> DiagnosisModel::createAusweisApp2Section()
{
	QSharedPointer<SectionModel> aa2Section(new SectionModel);

	BuildHelper::processInformationHeader([aa2Section](const QString& pKey, const QString& pValue){
				aa2Section->addItem(pKey, pValue);
			});

	mTimestampItem = QSharedPointer<ContentItem>::create(tr("Time of diagnosis"), tr("Initial diagnosis running, please wait."));
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
	mPcscSection->addTitleWithoutContent(tr("PC/SC information"));
	mPcscSection->addItemWithoutTitle(tr("Diagnosis is running..."));
	mCardReaderSection = QSharedPointer<SectionModel>::create();
	mCardReaderSection->addTitleWithoutContent(tr("Card reader"));
	mCardReaderSection->addItemWithoutTitle(tr("Diagnosis is running..."));
	mRemoteDeviceSection = QSharedPointer<SectionModel>::create();
	mRemoteDeviceSection->addTitleWithoutContent(tr("Paired remote devices"));
	mRemoteDeviceSection->addItemWithoutTitle(tr("Diagnosis is running..."));
}


void DiagnosisModel::createAntiVirusAndFirewallSection()
{
	mAntivirusSection = QSharedPointer<SectionModel>::create();
	mFirewallSection = QSharedPointer<SectionModel>::create();
	mCombinedAntivirusFirewallSection = QSharedPointer<SectionModel>::create();

#ifdef Q_OS_WIN
	mAntivirusSection->addTitleWithoutContent(tr("Antivirus information"));
	mAntivirusSection->addItemWithoutTitle(tr("Diagnosis is running..."));

	mFirewallSection->addTitleWithoutContent(tr("Firewall information"));
	mFirewallSection->addItemWithoutTitle(tr("Diagnosis is running..."));
#else
	mAntivirusSection->addItemWithoutTitle(tr("No Antivirus information available on this platform."));
	mFirewallSection->addItemWithoutTitle(tr("No Firewall information available on this platform."));
#endif
}


void DiagnosisModel::emitDataChangedForSection(const QSharedPointer<ContentItem>& pItem) const
{
	QSharedPointer<SectionModel> sectionModel = pItem->mSection;
	if (sectionModel)
	{
		sectionModel->emitDataChangedForItem(pItem);
	}
}


void DiagnosisModel::connectSignals()
{
	const GeneralSettings& generalSettings = Env::getSingleton<AppSettings>()->getGeneralSettings();
	connect(&generalSettings, &GeneralSettings::fireLanguageChanged, this, &DiagnosisModel::reloadContent);
	connect(mContext.data(), &DiagnosisContext::timestampChanged, this, &DiagnosisModel::onTimestampChanged);
	connect(mContext.data(), &DiagnosisContext::fireNetworkInfoChanged, this, &DiagnosisModel::onNetworkInfoChanged);
	connect(&mConnectionTest, &DiagnosisConnectionTest::fireConnectionTestDone, this, &DiagnosisModel::onConnectionTestDone);
	connect(mContext.data(), &DiagnosisContext::pcscInfoChanged, this, &DiagnosisModel::onPcscInfoChanged);
	RemoteServiceSettings& settings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
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

	const GeneralSettings& generalSettings = Env::getSingleton<AppSettings>()->getGeneralSettings();
	disconnect(&generalSettings, &GeneralSettings::fireLanguageChanged, this, &DiagnosisModel::reloadContent);
	disconnect(mContext.data(), &DiagnosisContext::timestampChanged, this, &DiagnosisModel::onTimestampChanged);
	disconnect(mContext.data(), &DiagnosisContext::fireNetworkInfoChanged, this, &DiagnosisModel::onNetworkInfoChanged);
	disconnect(&mConnectionTest, &DiagnosisConnectionTest::fireConnectionTestDone, this, &DiagnosisModel::onConnectionTestDone);
	disconnect(mContext.data(), &DiagnosisContext::pcscInfoChanged, this, &DiagnosisModel::onPcscInfoChanged);
	RemoteServiceSettings& settings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
	disconnect(&settings, &RemoteServiceSettings::fireTrustedRemoteInfosChanged, this, &DiagnosisModel::onRemoteInfosChanged);
	disconnect(mContext.data(), &DiagnosisContext::readerInfosChanged, this, &DiagnosisModel::onReaderInfosChanged);

#ifdef Q_OS_WIN
	disconnect(&mAntivirusDetection, &DiagnosisAntivirusDetection::fireAntivirusInformationChanged, this, &DiagnosisModel::onAntivirusInformationChanged);
	disconnect(&mAntivirusDetection, &DiagnosisAntivirusDetection::fireDetectionFailed, this, &DiagnosisModel::onAntivirusDetectionFailed);
	disconnect(&mFirewallDetection, &DiagnosisFirewallDetection::fireFirewallInformationReady, this, &DiagnosisModel::onFirewallInformationReady);
	disconnect(&mFirewallDetection, &DiagnosisFirewallDetection::fireDetectionFailed, this, &DiagnosisModel::onFirewallInformationFailed);
#endif
}


QVariant DiagnosisModel::data(const QModelIndex& pIndex, int pRole) const
{
	Q_UNUSED(pRole)

	const int row = pIndex.row();
	if (!pIndex.isValid() || row >= mSections.size())
	{
		return QVariant();
	}

	return mSections.at(row).first;
}


int DiagnosisModel::rowCount(const QModelIndex& pParent) const
{
	Q_UNUSED(pParent)
	return mSections.size();
}


QString DiagnosisModel::getFirstSectionName() const
{
	return mSections.first().first;
}


QAbstractListModel* DiagnosisModel::getSectionContent(const QString& pSection)
{
	for (const auto& pair : qAsConst(mSections))
	{
		if (pair.first == pSection)
		{
			QSharedPointer<SectionModel> sectionModel = pair.second;
			return sectionModel.data();
		}
	}

	return nullptr;
}


QDateTime DiagnosisModel::getCreationTime() const
{
	return mContext->getTimestamp();
}


QString DiagnosisModel::getCreationTimeString() const
{
	return getCreationTime().toString(QStringLiteral("yyyy-MM-dd_HH-mm"));
}


QString DiagnosisModel::getAsPlaintext() const
{

#ifdef Q_OS_WIN
	static const QString endl = QStringLiteral("\r\n");
#else
	static const QString endl = QStringLiteral("\n");
#endif

	QStringList modelPlaintext;
	for (const auto& sectionPair : qAsConst(mSections))
	{
		modelPlaintext << sectionPair.first;
		modelPlaintext << sectionPair.second->getAsPlaintext(QStringLiteral("\t"));
		modelPlaintext << endl;
	}

	return modelPlaintext.join(endl);
}


QString DiagnosisModel::boolToString(bool pBoolean)
{
	return pBoolean ? tr("Yes") : tr("No");
}


void DiagnosisModel::onTimestampChanged()
{
	QDateTime timestampValue = mContext->getTimestamp();
	if (!timestampValue.isValid())
	{
		mTimestampItem->mContent = tr("Failed to retrieve date & time");
	}
	else
	{
		QString timestamp = LanguageLoader::getInstance().getUsedLocale().toString(timestampValue, tr("d. MMMM yyyy, hh:mm:ss AP"));
		mTimestampItem->mContent = timestamp;
	}

	emitDataChangedForSection(mTimestampItem);
}


void DiagnosisModel::onNetworkInfoChanged()
{
	mNetworkInterfaceSection->removeAllItems();

	const auto& networkInterfaces = mContext->getNetworkInterfaces();
	for (const auto& iface : networkInterfaces)
	{
		QStringList interfaceInfos;
		QString hardwareAddress = iface.hardwareAddress().isEmpty() ? tr("<Not set>") : iface.hardwareAddress();
		interfaceInfos << tr("Hardware address: %1").arg(hardwareAddress);

		const auto& addresses = iface.addressEntries();
		if (addresses.isEmpty())
		{
			interfaceInfos << tr("No IP addresses assigned");
		}
		else
		{
			for (const auto& address : addresses)
			{
				const auto& ip = address.ip();
				if (ip.protocol() == QAbstractSocket::NetworkLayerProtocol::IPv4Protocol)
				{
					interfaceInfos << tr("IPv4 address: %1").arg(ip.toString());
				}
				else if (ip.protocol() == QAbstractSocket::NetworkLayerProtocol::IPv6Protocol)
				{
					interfaceInfos << tr("IPv6 address: %1").arg(ip.toString());
				}
				else
				{
					interfaceInfos << tr("Unknown address: %1").arg(ip.toString());
				}
			}
		}

		mNetworkInterfaceSection->addItem(tr("Interface: \"%1\"").arg(iface.humanReadableName()), interfaceInfos.join(QStringLiteral("\n")));
	}

	mCombinedNetworkSection->replaceWithSections({mNetworkConnectionSection, mNetworkInterfaceSection});
}


void DiagnosisModel::onConnectionTestDone()
{
	mNetworkConnectionSection->removeAllItems();
	QStringList proxyInfo;

	if (mConnectionTest.getIsProxySet())
	{
		proxyInfo << tr("Hostname: %1").arg(mConnectionTest.getProxyHostName());
		proxyInfo << tr("Port: %1").arg(mConnectionTest.getProxyPort());
		proxyInfo << tr("Type: %1").arg(mConnectionTest.getProxyType());
		proxyInfo << tr("Capabilities: %1").arg(mConnectionTest.getProxyCapabilities());

		if (mConnectionTest.getPingTestOnProxySuccessful())
		{
			proxyInfo << tr("Ping test to proxy: Successful");
		}
		else
		{
			proxyInfo << tr("Ping test to proxy: Failed");
		}

		if (mConnectionTest.getConnectionTestWithProxySuccessful())
		{
			proxyInfo << tr("Connection test with proxy: Successful");
		}
		else
		{
			proxyInfo << tr("Connection test with proxy: Failed");
		}
	}
	else
	{
		proxyInfo << tr("No proxy found");
	}

	if (mConnectionTest.getConnectionTestWithoutProxySuccessful())
	{
		proxyInfo << tr("Connection test without proxy: Successful");
	}
	else
	{
		proxyInfo << tr("Connection test without proxy: Failed");
	}

	mNetworkConnectionSection->addItem(tr("Proxy information"), proxyInfo.join(QStringLiteral("\n")));
	mCombinedNetworkSection->replaceWithSections({mNetworkConnectionSection, mNetworkInterfaceSection});
}


void DiagnosisModel::onAntivirusInformationChanged()
{
	mAntivirusSection->removeAllItems();

	const auto& antivirusInfos = mAntivirusDetection.getAntivirusInformations();
	if (antivirusInfos.isEmpty())
	{
		mAntivirusSection->addItem(tr("Antivirus information"), tr("No Antivirus software detected."));
	}
	else
	{
		mAntivirusSection->addTitleWithoutContent(tr("Antivirus information"));

		for (const auto& info : antivirusInfos)
		{
			QStringList avInfo;
			if (!info->getLastUpdate().isEmpty())
			{
				avInfo << tr("Last updated: %1").arg(info->getLastUpdate());
			}
			avInfo << tr("Executable path: %1").arg(info->getExePath());
			auto antivirusName = info->getDisplayName();
			mAntivirusSection->addItem(antivirusName, avInfo.join(QStringLiteral("\n")));
		}
	}

	mCombinedAntivirusFirewallSection->replaceWithSections({mAntivirusSection, mFirewallSection});
}


void DiagnosisModel::onAntivirusDetectionFailed()
{
	mAntivirusSection->removeAllItems();
	mAntivirusSection->addItem(tr("Antivirus information"), tr("Antivirus detection failed."));
	mCombinedAntivirusFirewallSection->replaceWithSections({mAntivirusSection, mFirewallSection});
}


void DiagnosisModel::onFirewallInformationReady()
{
	mFirewallSection->removeAllItems();

	mFirewallSection->addTitleWithoutContent(tr("Firewall information"));
	auto installedFirewalls = mFirewallDetection.getDetectedFirewalls();
	if (installedFirewalls.isEmpty())
	{
#if defined(Q_OS_WIN)
		if (QOperatingSystemVersion::current() < QOperatingSystemVersion::Windows8)
		{
			mFirewallSection->addItemWithoutTitle(tr("Third party firewalls cannot be detected on Windows 7."));
		}
		else
		{
#else
		{
#endif
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
			firewallInfos << tr("Enabled: %1").arg(enabled);
			firewallInfos << tr("Up to date: %1").arg(uptodate);
		}
		mFirewallSection->addItem(tr("Firewalls from third party vendors"), firewallInfos.join(QStringLiteral("\n")));
	}

	QStringList windowsFirewallSettings;
	QString firstRuleExists = boolToString(mFirewallDetection.getFirstRuleExists());
	QString firstRuleEnabled = boolToString(mFirewallDetection.getFirstRuleEnabled());
	windowsFirewallSettings << tr("Outgoing AusweisApp2 rule");
	windowsFirewallSettings << tr("Exists: %1").arg(firstRuleExists);
	windowsFirewallSettings << tr("Enabled: %1").arg(firstRuleEnabled);

	QString secondRuleExists = boolToString(mFirewallDetection.getSecondRuleExists());
	QString secondRuleEnabled = boolToString(mFirewallDetection.getSecondRuleEnabled());
	windowsFirewallSettings << tr("Incoming AusweisApp2 rule");
	windowsFirewallSettings << tr("Exists: %1").arg(secondRuleExists);
	windowsFirewallSettings << tr("Enabled: %1").arg(secondRuleEnabled);

	mFirewallSection->addItem(tr("Windows firewall rules"), windowsFirewallSettings.join(QStringLiteral("\n")));

	QStringList windowsFirewallProfiles;
	auto firewallProfiles = mFirewallDetection.getFirewallProfiles();
	for (const auto& profile : firewallProfiles)
	{
		windowsFirewallProfiles << profile->getName();
		QString enabled = boolToString(profile->getEnabled());
		windowsFirewallProfiles << tr("Enabled: %1").arg(enabled);
	}

	mFirewallSection->addItem(tr("Windows firewall profiles"), windowsFirewallProfiles.join(QStringLiteral("\n")));
	mFirewallSection->addItemWithoutTitle(tr("Warning: The current firewall status can be obscured by additional Group Policies on your system, often set by system administrators."));

	mCombinedAntivirusFirewallSection->replaceWithSections({mAntivirusSection, mFirewallSection});
}


void DiagnosisModel::onFirewallInformationFailed()
{
	mFirewallSection->removeAllItems();
	mFirewallSection->addItem(tr("Firewall information"), tr("An error occurred while trying to gather firewall information. Please check the log for more information."));
	mCombinedAntivirusFirewallSection->replaceWithSections({mAntivirusSection, mFirewallSection});
}


void DiagnosisModel::onPcscInfoChanged()
{
	mPcscSection->removeAllItems();

	mPcscSection->addTitleWithoutContent(tr("PC/SC information"));
	mPcscSection->addItemWithoutTitle(tr("Version: %1").arg(mContext->getPcscVersion()));

	QStringList pcscInfo;
	for (const auto& info : mContext->getPcscComponents())
	{
		pcscInfo << info.getDescription();
		pcscInfo << tr("Vendor: %1").arg(info.getManufacturer());
		pcscInfo << tr("Version: %1").arg(info.getVersion());
		pcscInfo << tr("File path: %1").arg(info.getPath());
	}
	if (!pcscInfo.empty())
	{
		mPcscSection->addItem(tr("Components"), pcscInfo.join(QStringLiteral("\n")));
	}

	pcscInfo.clear();
	for (const auto& info : mContext->getPcscDrivers())
	{
		pcscInfo << info.getDescription();
		pcscInfo << tr("Vendor: %1").arg(info.getManufacturer());
		pcscInfo << tr("Version: %1").arg(info.getVersion());
		pcscInfo << tr("File path: %1").arg(info.getPath());
	}
	if (!pcscInfo.empty())
	{
		mPcscSection->addItem(tr("Driver"), pcscInfo.join(QStringLiteral("\n")));
	}

	mCombinedReaderSection->replaceWithSections({mCardReaderSection, mRemoteDeviceSection, mPcscSection});
}


void DiagnosisModel::onRemoteInfosChanged()
{
	mRemoteDeviceSection->removeAllItems();

	const RemoteServiceSettings& settings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
	const auto& trustedCertificates = settings.getTrustedCertificates();

	mRemoteDeviceSection->addTitleWithoutContent(tr("Paired remote devices"));

	if (trustedCertificates.isEmpty())
	{
		mRemoteDeviceSection->addItemWithoutTitle(tr("No devices paired."));
	}

	for (const auto& cert : trustedCertificates)
	{
		QStringList certInfo;
		const auto& info = settings.getRemoteInfo(cert);

		if (!info.getFingerprint().isEmpty())
		{
			certInfo << tr("Certificate fingerprint: %1").arg(info.getFingerprint());
			const QString& timestamp = LanguageLoader::getInstance().getUsedLocale().toString(info.getLastConnected(), tr("dd.MM.yyyy, hh:mm:ss"));
			certInfo << tr("Last connection: %1").arg(timestamp);

			mRemoteDeviceSection->addItem(info.getName(), certInfo.join(QStringLiteral("\n")));
		}
		else
		{
			mRemoteDeviceSection->addItem(RemoteServiceSettings::generateFingerprint(cert), tr("No information found for this certificate."));
		}
	}

	mCombinedReaderSection->replaceWithSections({mCardReaderSection, mRemoteDeviceSection, mPcscSection});
}


void DiagnosisModel::onReaderInfosChanged()
{
	mCardReaderSection->removeAllItems();

	mCardReaderSection->addTitleWithoutContent(tr("Connected Card reader"));

	const auto& readerInfos = mContext->getReaderInfos();
	if (readerInfos.isEmpty())
	{
		mCardReaderSection->addItemWithoutTitle(tr("No supported reader found."));
	}

	for (const auto& info : readerInfos)
	{
		QStringList infoList;

		QString readerType = info.isBasicReader() ? tr("Basic card reader") : tr("Standard / comfort card reader");
		infoList << tr("Type: %1").arg(readerType);
		infoList << tr("Card: %1").arg(info.getCardTypeString());

		if (info.hasEidCard())
		{
			infoList << tr("Retry counter: %1").arg(info.getRetryCounter());
		}

		mCardReaderSection->addItem(info.getName(), infoList.join(QStringLiteral("\n")));
	}

	mCombinedReaderSection->replaceWithSections({mCardReaderSection, mRemoteDeviceSection, mPcscSection});
}


void DiagnosisModel::reloadContent()
{
	disconnectSignals();

	beginResetModel();
	mSections.clear();

	mSections.append(qMakePair(QCoreApplication::applicationName(), createAusweisApp2Section()));

	createCardReaderSection();
	mCombinedReaderSection->replaceWithSections({mCardReaderSection, mRemoteDeviceSection, mPcscSection});
	mSections.append(qMakePair(tr("Card reader"), mCombinedReaderSection));

	createNetworkSection();
	mCombinedNetworkSection->replaceWithSections({mNetworkConnectionSection, mNetworkInterfaceSection});
	mSections.append(qMakePair(tr("Network"), mCombinedNetworkSection));

	createAntiVirusAndFirewallSection();
	mCombinedAntivirusFirewallSection->replaceWithSections({mAntivirusSection, mFirewallSection});
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


SectionModel::SectionModel(QObject* pParent)
	: QAbstractListModel(pParent)
{

}


QVariant SectionModel::data(const QModelIndex& pIndex, int pRole) const
{
	const int row = pIndex.row();
	if (!pIndex.isValid() || row >= mContentItems.size())
	{
		return QVariant();
	}

	static const QString endl = QStringLiteral("\n");

	QSharedPointer<ContentItem> item = mContentItems.at(row);
	switch (pRole)
	{
		case Qt::DisplayRole:
			if (item->mTitle.isEmpty())
			{
				return item->mContent;
			}
			else
			{
				return item->mTitle + endl + item->mContent;
			}

		case TitleRole:
			return item->mTitle;

		case ContentRole:
			return item->mContent;

		default:
			return QVariant();
	}
}


int SectionModel::rowCount(const QModelIndex& pParent) const
{
	Q_UNUSED(pParent)
	return mContentItems.size();
}


QHash<int, QByteArray> SectionModel::roleNames() const
{
	QHash<int, QByteArray> roles;
	roles.insert(Qt::DisplayRole, QByteArrayLiteral("display"));
	roles.insert(TitleRole, QByteArrayLiteral("title"));
	roles.insert(ContentRole, QByteArrayLiteral("content"));
	return roles;
}


void SectionModel::addItem(const QString& pTitle, const QString& pContent)
{
	addItem(QSharedPointer<ContentItem>::create(pTitle, pContent));
}


void SectionModel::addItem(const QSharedPointer<ContentItem>& pContentItem)
{
	beginInsertRows(index(0), mContentItems.size(), mContentItems.size());
	mContentItems.append(pContentItem);
	pContentItem->mSection = sharedFromThis();
	endInsertRows();
}


void SectionModel::addItemWithoutTitle(const QString& pContent)
{
	addItem(QString(), pContent);
}


void SectionModel::addTitleWithoutContent(const QString& pTitle)
{
	addItem(pTitle, QString());
}


void SectionModel::removeAllItems()
{
	if (!mContentItems.empty())
	{
		beginRemoveRows(index(0), 0, mContentItems.size() - 1);
		mContentItems.clear();
		endRemoveRows();
	}
}


void SectionModel::emitDataChangedForItem(const QSharedPointer<ContentItem>& pItem)
{
	QModelIndex itemIndex = index(mContentItems.indexOf(pItem));
	Q_EMIT dataChanged(itemIndex, itemIndex);
}


void SectionModel::replaceWithSections(QVector<QSharedPointer<SectionModel> > pSections)
{
	beginResetModel();
	removeAllItems();

	for (const auto& section : qAsConst(pSections))
	{
		const auto& sectionItems = section->mContentItems;
		for (const auto& item : sectionItems)
		{
			addItem(item);
		}
	}

	endResetModel();
}


QStringList SectionModel::getAsPlaintext(const QString& pPrependString) const
{
	QStringList sectionPlaintext;
	for (const auto& item : qAsConst(mContentItems))
	{
		if (!item->mTitle.isEmpty())
		{
			sectionPlaintext << pPrependString + item->mTitle;
		}

		if (!item->mContent.isEmpty())
		{
			sectionPlaintext << pPrependString + item->mContent;
		}
	}

	return sectionPlaintext;
}
