/*
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

#include "DiagnosisModel.h"

#include "AppSettings.h"
#include "LanguageLoader.h"
#include "RemoteServiceSettings.h"
#include "ScopeGuard.h"

#include <QOperatingSystemVersion>
#include <QSslSocket>

using namespace governikus;


DiagnosisModel::DiagnosisModel(const QSharedPointer<DiagnosisContext>& pContext)
	: mContext(pContext)
	, mRootItem(new DiagnosisItem(tr("Diagnosis data")))
	, mAppVersionItem(new DiagnosisItem(QCoreApplication::applicationName()))
	, mOperatingSystemItem(new DiagnosisItem(tr("Operating system")))
	, mReaderItem(new DiagnosisItem(tr("Card reader")))
	, mPcScItem(new DiagnosisItem(tr("PC/SC")))
	, mPairedDevices(new DiagnosisItem(tr("Paired devices")))
	, mNetworkInterfaces(new DiagnosisItem(tr("Network interfaces")))
	, mNetworkConnectionTest(new DiagnosisItem(tr("Network connection test")))
	, mInstalledAntivirus(new DiagnosisItem(tr("Installed antivirus software")))
	, mWindowsFirewall(new DiagnosisItem(tr("Firewall")))
	, mTimestampItem(new DiagnosisItem(tr("Time of diagnosis")))
	, mAntivirusDetection()
	, mFirewallDetection()
	, mConnectionTest()
{
	mRootItem->addChild(mAppVersionItem);
	initAppVersionInfo();

	mRootItem->addChild(mOperatingSystemItem);
	mOperatingSystemItem->addChild(QSharedPointer<DiagnosisItem>::create(QSysInfo::prettyProductName()));
	mOperatingSystemItem->addChild(QSharedPointer<DiagnosisItem>::create(QSysInfo::kernelVersion()));
	mOperatingSystemItem->addChild(QSharedPointer<DiagnosisItem>::create(QSysInfo::currentCpuArchitecture()));

	mRootItem->addChild(mReaderItem);
	mReaderItem->addChild(QSharedPointer<DiagnosisItem>::create(tr("Diagnosis is running...")));
	connect(mContext.data(), &DiagnosisContext::readerInfosChanged, this, &DiagnosisModel::onReaderInfosChanged);

	mRootItem->addChild(mPcScItem);
	mPcScItem->addChild(QSharedPointer<DiagnosisItem>::create(tr("Diagnosis is running...")));
	connect(mContext.data(), &DiagnosisContext::pcscInfoChanged, this, &DiagnosisModel::onPcscInfoChanged);

	mRootItem->addChild(mPairedDevices);
	RemoteServiceSettings& settings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
	connect(&settings, &RemoteServiceSettings::fireTrustedRemoteInfosChanged, this, &DiagnosisModel::onRemoteInfosChanged);
	onRemoteInfosChanged();

	mRootItem->addChild(mNetworkInterfaces);
	connect(mContext.data(), &DiagnosisContext::fireNetworkInfoChanged, this, &DiagnosisModel::onNetworkInfoChanged);

	mRootItem->addChild(mNetworkConnectionTest);
	mNetworkConnectionTest->addChild(QSharedPointer<DiagnosisItem>::create(tr("Diagnosis is running...")));
	connect(&mConnectionTest, &DiagnosisConnectionTest::fireConnectionTestDone, this, &DiagnosisModel::onConnectionTestDone);
	mConnectionTest.startConnectionTest();

	mRootItem->addChild(mInstalledAntivirus);
	mRootItem->addChild(mWindowsFirewall);
#ifdef Q_OS_WIN
	connect(&mAntivirusDetection, &DiagnosisAntivirusDetection::fireAntivirusInformationChanged, this, &DiagnosisModel::onAntivirusInformationChanged);
	connect(&mAntivirusDetection, &DiagnosisAntivirusDetection::fireDetectionFailed, this, &DiagnosisModel::onAntivirusDetectionFailed);
	mAntivirusDetection.startInformationProcess();
	mInstalledAntivirus->addChild(QSharedPointer<DiagnosisItem>::create(tr("Diagnosis is running...")));

	connect(&mFirewallDetection, &DiagnosisFirewallDetection::fireFirewallInformationReady, this, &DiagnosisModel::onFirewallInformationReady);
	connect(&mFirewallDetection, &DiagnosisFirewallDetection::fireDetectionFailed, this, &DiagnosisModel::onFirewallInformationFailed);
	mFirewallDetection.startDetection();
	mWindowsFirewall->addChild(QSharedPointer<DiagnosisItem>::create(tr("Diagnosis is running...")));
#else
	mInstalledAntivirus->addChild(QSharedPointer<DiagnosisItem>::create(tr("No Antivirus information available on this platform.")));
	mWindowsFirewall->addChild(QSharedPointer<DiagnosisItem>::create(tr("No Firewall information available on this platform.")));
#endif

	mRootItem->addChild(mTimestampItem);
	connect(mContext.data(), &DiagnosisContext::timestampChanged, this, &DiagnosisModel::onTimestampChanged);
	onTimestampChanged();
}


void DiagnosisModel::initAppVersionInfo()
{
	const QStringList appVersion({
				QCoreApplication::applicationVersion(),
				QCoreApplication::organizationName(),
				QStringLiteral("Qt ") + QString::fromLatin1(qVersion()),
				QSslSocket::sslLibraryVersionString()
			});
	for (const auto& str : appVersion)
	{
		mAppVersionItem->addChild(QSharedPointer<DiagnosisItem>::create(str));
	}
}


void DiagnosisModel::insertPcScComponentList(const QVector<DiagnosisContext::ComponentInfo>& pComponents, const QSharedPointer<DiagnosisItem>& pParentItem)
{
	if (pComponents.isEmpty())
	{
		return;
	}

	for (const DiagnosisContext::ComponentInfo& info : pComponents)
	{
		auto descriptionItem = QSharedPointer<DiagnosisItem>::create(info.getDescription());
		pParentItem->addChild(descriptionItem);

		auto companyItem = QSharedPointer<DiagnosisItem>::create(tr("Vendor: %1").arg(info.getManufacturer()));
		pParentItem->addChild(companyItem);

		auto versionItem = QSharedPointer<DiagnosisItem>::create(tr("Version: %1").arg(info.getVersion()));
		pParentItem->addChild(versionItem);

		auto pathItem = QSharedPointer<DiagnosisItem>::create(tr("File path: %1").arg(info.getPath()));
		descriptionItem->addChild(pathItem);
	}
}


void DiagnosisModel::removeChildItems(QModelIndex pIndex, QSharedPointer<DiagnosisItem> pParentItem)
{
	if (pParentItem->childCount() <= 0)
	{
		return;
	}

	beginRemoveRows(pIndex, 0, pParentItem->childCount() - 1);
	pParentItem->clearChildren();
	endRemoveRows();
}


void DiagnosisModel::onReaderInfosChanged()
{
	auto itemModelIndex = index(2, 0);
	removeChildItems(itemModelIndex, mReaderItem);

	const auto& readerInfos = mContext->getReaderInfos();
	if (readerInfos.isEmpty())
	{
		beginInsertRows(itemModelIndex, 0, 0);
		mReaderItem->addChild(QSharedPointer<DiagnosisItem>::create(tr("Not recognised")));
		endInsertRows();
		return;
	}

	beginInsertRows(itemModelIndex, 0, readerInfos.size() - 1);
	for (const ReaderInfo& info : readerInfos)
	{
		auto readerName = QSharedPointer<DiagnosisItem>::create(info.getName());
		mReaderItem->addChild(readerName);

		QString readerTypeString = info.isBasicReader() ? tr("Basic card reader") : tr("Standard / comfort card reader");
		auto readerType = QSharedPointer<DiagnosisItem>::create(tr("Type: %1").arg(readerTypeString));
		readerName->addChild(readerType);

		QString cardTypeString = info.getCardTypeString();
		auto cardType = QSharedPointer<DiagnosisItem>::create(tr("Card: %1").arg(cardTypeString));
		readerName->addChild(cardType);

		if (info.hasEidCard())
		{
			auto retryCounter = QSharedPointer<DiagnosisItem>::create(tr("Retry counter: %1").arg(3 - info.getRetryCounter()));
			readerName->addChild(retryCounter);
		}
	}
	endInsertRows();
}


void DiagnosisModel::onPcscInfoChanged()
{
	auto itemModelIndex = index(3, 0);
	removeChildItems(itemModelIndex, mPcScItem);

	beginInsertRows(itemModelIndex, 0, 2);
	auto pcscVersion = QSharedPointer<DiagnosisItem>::create(tr("Version: %1").arg(mContext->getPcscVersion()));
	mPcScItem->addChild(pcscVersion);

	auto pcscComponents = QSharedPointer<DiagnosisItem>::create(tr("Components"));
	mPcScItem->addChild(pcscComponents);
	insertPcScComponentList(mContext->getPcscComponents(), pcscComponents);

	auto driverItem = QSharedPointer<DiagnosisItem>::create(tr("Driver"));
	mPcScItem->addChild(driverItem);
	insertPcScComponentList(mContext->getPcscDrivers(), driverItem);
	endInsertRows();
}


void DiagnosisModel::onRemoteInfosChanged()
{
	auto itemModelIndex = index(4, 0);
	removeChildItems(itemModelIndex, mPairedDevices);

	const RemoteServiceSettings& settings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
	const auto& trustedCertificates = settings.getTrustedCertificates();

	if (trustedCertificates.isEmpty())
	{
		beginInsertRows(itemModelIndex, 0, 0);
		mPairedDevices->addChild(QSharedPointer<DiagnosisItem>::create(tr("No devices paired")));
		endInsertRows();
		return;
	}

	beginInsertRows(itemModelIndex, 0, trustedCertificates.size() - 1);
	for (const auto& cert : trustedCertificates)
	{
		const auto& info = settings.getRemoteInfo(cert);

		if (!info.getFingerprint().isEmpty())
		{
			const auto& deviceName = QSharedPointer<DiagnosisItem>::create(info.getName());
			mPairedDevices->addChild(deviceName);

			const auto& deviceFingerprint = QSharedPointer<DiagnosisItem>::create((tr("Certificate fingerprint: %1").arg(info.getFingerprint())));
			deviceName->addChild(deviceFingerprint);

			const QString& timestamp = LanguageLoader::getInstance().getUsedLocale().toString(info.getLastConnected(), tr("dd.MM.yyyy, hh:mm:ss"));
			const auto& deviceLastConnected = QSharedPointer<DiagnosisItem>::create(tr("Last connection: %1").arg(timestamp));
			deviceName->addChild(deviceLastConnected);
		}
		else
		{
			const auto& deviceName = QSharedPointer<DiagnosisItem>::create(RemoteServiceSettings::generateFingerprint(cert));
			mPairedDevices->addChild(deviceName);

			const auto& deviceInfo = QSharedPointer<DiagnosisItem>::create(tr("No information found for this certificate"));
			deviceName->addChild(deviceInfo);
		}
	}
	endInsertRows();
}


void DiagnosisModel::onTimestampChanged()
{
	auto itemModelIndex = index(9, 0);
	removeChildItems(itemModelIndex, mTimestampItem);

	beginInsertRows(itemModelIndex, 0, 0);
	QDateTime timestampValue = mContext->getTimestamp();
	if (!timestampValue.isValid())
	{
		mTimestampItem->addChild(QSharedPointer<DiagnosisItem>::create(tr("Initial diagnosis running, please wait.")));
	}
	else
	{
		QString timestamp = LanguageLoader::getInstance().getUsedLocale().toString(timestampValue, tr("d. MMMM yyyy, hh:mm:ss AP"));
		mTimestampItem->addChild(QSharedPointer<DiagnosisItem>::create(timestamp));
	}
	endInsertRows();
}


void DiagnosisModel::onNetworkInfoChanged()
{
	auto itemModelIndex = index(5, 0);
	removeChildItems(itemModelIndex, mNetworkInterfaces);

	const auto& networkInterfaces = mContext->getNetworkInterfaces();
	beginInsertRows(itemModelIndex, 0, networkInterfaces.size() - 1);
	for (const auto& interface : networkInterfaces)
	{
		const auto& interfaceName = QSharedPointer<DiagnosisItem>::create(interface.humanReadableName());
		mNetworkInterfaces->addChild(interfaceName);

		QString hardwareAddress = interface.hardwareAddress().isEmpty() ? tr("<Not set>") : interface.hardwareAddress();
		interfaceName->addChild(QSharedPointer<DiagnosisItem>::create(tr("Hardware address: %1").arg(hardwareAddress)));

		const auto& addresses = interface.addressEntries();
		if (addresses.isEmpty())
		{
			interfaceName->addChild(QSharedPointer<DiagnosisItem>::create(tr("No IP addresses assigned")));
		}
		else
		{
			const auto& interfaceAddresses = QSharedPointer<DiagnosisItem>::create(tr("IP addresses"));
			interfaceName->addChild(interfaceAddresses);
			for (const auto& address : addresses)
			{
				const auto& ip = QSharedPointer<DiagnosisItem>::create(address.ip().toString());
				interfaceAddresses->addChild(ip);
			}
		}
	}
	endInsertRows();
}


void DiagnosisModel::onConnectionTestDone()
{
	auto itemModelIndex = index(6, 0);
	removeChildItems(itemModelIndex, mNetworkConnectionTest);

	if (mConnectionTest.getIsProxySet())
	{
		beginInsertRows(itemModelIndex, 0, 2);
		auto proxy = QSharedPointer<DiagnosisItem>::create(tr("Proxy"));
		mNetworkConnectionTest->addChild(proxy);

		proxy->addChild(QSharedPointer<DiagnosisItem>::create(tr("Hostname: %1").arg(mConnectionTest.getProxyHostName())));
		proxy->addChild(QSharedPointer<DiagnosisItem>::create(tr("Port: %1").arg(mConnectionTest.getProxyPort())));
		proxy->addChild(QSharedPointer<DiagnosisItem>::create(tr("Type: %1").arg(mConnectionTest.getProxyType())));
		proxy->addChild(QSharedPointer<DiagnosisItem>::create(tr("Capabilities: %1").arg(mConnectionTest.getProxyCapabilities())));

		if (mConnectionTest.getPingTestOnProxySuccessful())
		{
			proxy->addChild(QSharedPointer<DiagnosisItem>::create(tr("Ping test to proxy: Successful")));
		}
		else
		{
			proxy->addChild(QSharedPointer<DiagnosisItem>::create(tr("Ping test to proxy: Failed")));
		}

		if (mConnectionTest.getConnectionTestWithProxySuccessful())
		{
			mNetworkConnectionTest->addChild(QSharedPointer<DiagnosisItem>::create(tr("Connection test with proxy: Successful")));
		}
		else
		{
			mNetworkConnectionTest->addChild(QSharedPointer<DiagnosisItem>::create(tr("Connection test with proxy: Failed")));
		}
	}
	else
	{
		beginInsertRows(itemModelIndex, 0, 1);
		mNetworkConnectionTest->addChild(QSharedPointer<DiagnosisItem>::create(tr("No Proxy Found")));
	}

	if (mConnectionTest.getConnectionTestWithoutProxySuccessful())
	{
		mNetworkConnectionTest->addChild(QSharedPointer<DiagnosisItem>::create(tr("Connection test without proxy: Successful")));
	}
	else
	{
		mNetworkConnectionTest->addChild(QSharedPointer<DiagnosisItem>::create(tr("Connection test without proxy: Failed")));
	}
	endInsertRows();
}


void DiagnosisModel::onAntivirusInformationChanged()
{
	auto itemModelIndex = index(7, 0);
	removeChildItems(itemModelIndex, mInstalledAntivirus);

	const auto& antivirusInfos = mAntivirusDetection.getAntivirusInformations();
	if (antivirusInfos.isEmpty())
	{
		beginInsertRows(itemModelIndex, 0, 0);
		mInstalledAntivirus->addChild(QSharedPointer<DiagnosisItem>::create(tr("No Antivirus software detected.")));
	}
	else
	{
		beginInsertRows(itemModelIndex, 0, antivirusInfos.size() - 1);
		for (const auto& info : antivirusInfos)
		{
			auto antivirusName = QSharedPointer<DiagnosisItem>::create(info->getDisplayName());
			mInstalledAntivirus->addChild(antivirusName);

			if (!info->getLastUpdate().isEmpty())
			{
				antivirusName->addChild(QSharedPointer<DiagnosisItem>::create(tr("Last updated: %1").arg(info->getLastUpdate())));
			}
			antivirusName->addChild(QSharedPointer<DiagnosisItem>::create(tr("Executable path: %1").arg(info->getExePath())));
		}
	}
	endInsertRows();
}


void DiagnosisModel::onAntivirusDetectionFailed()
{
	auto itemModelIndex = index(7, 0);
	removeChildItems(itemModelIndex, mInstalledAntivirus);

	beginInsertRows(itemModelIndex, 0, 0);
	mInstalledAntivirus->addChild(QSharedPointer<DiagnosisItem>::create(tr("Antivirus detection failed.")));
	endInsertRows();
}


const QString DiagnosisModel::boolToString(bool pBoolean)
{
	return pBoolean ? tr("Yes") : tr("No");
}


void DiagnosisModel::onFirewallInformationReady()
{
	auto itemModelIndex = index(8, 0);
	removeChildItems(itemModelIndex, mWindowsFirewall);

	beginInsertRows(itemModelIndex, 0, 2);
	auto installedFirewalls = mFirewallDetection.getDetectedFirewalls();
	if (installedFirewalls.isEmpty())
	{
#if defined(Q_OS_WIN)
		if (QOperatingSystemVersion::current() < QOperatingSystemVersion::Windows8)
		{
			mWindowsFirewall->addChild(QSharedPointer<DiagnosisItem>::create(tr("Third party firewalls cannot be detected on Windows 7.")));
		}
		else
		{
#else
		{
#endif
			mWindowsFirewall->addChild(QSharedPointer<DiagnosisItem>::create(tr("No third party firewalls detected")));
		}
	}
	else
	{
		auto thirdPartyFirewalls = QSharedPointer<DiagnosisItem>::create(tr("Firewalls from third party vendors"));
		mWindowsFirewall->addChild(thirdPartyFirewalls);
		for (const auto& firewall : installedFirewalls)
		{
			auto name = QSharedPointer<DiagnosisItem>::create(firewall->getName());
			thirdPartyFirewalls->addChild(name);

			QString enabled = boolToString(firewall->getEnabled());
			QString uptodate = boolToString(firewall->getUpToDate());
			name->addChild(QSharedPointer<DiagnosisItem>::create(tr("Enabled: %1").arg(enabled)));
			name->addChild(QSharedPointer<DiagnosisItem>::create(tr("Up to date: %1").arg(uptodate)));
		}
	}

	auto firewallRules = QSharedPointer<DiagnosisItem>::create(tr("Windows firewall rules"));
	mWindowsFirewall->addChild(firewallRules);

	QString firstRuleExists = boolToString(mFirewallDetection.getFirstRuleExists());
	QString firstRuleEnabled = boolToString(mFirewallDetection.getFirstRuleEnabled());
	auto outgoing = QSharedPointer<DiagnosisItem>::create(tr("Outgoing AusweisApp2 rule"));
	firewallRules->addChild(outgoing);
	outgoing->addChild(QSharedPointer<DiagnosisItem>::create(tr("Exists: %1").arg(firstRuleExists)));
	outgoing->addChild(QSharedPointer<DiagnosisItem>::create(tr("Enabled: %1").arg(firstRuleEnabled)));

	QString secondRuleExists = boolToString(mFirewallDetection.getSecondRuleExists());
	QString secondRuleEnabled = boolToString(mFirewallDetection.getSecondRuleEnabled());
	auto incoming = QSharedPointer<DiagnosisItem>::create(tr("Incoming AusweisApp2 rule"));
	firewallRules->addChild(incoming);
	incoming->addChild(QSharedPointer<DiagnosisItem>::create(tr("Exists: %1").arg(secondRuleExists)));
	incoming->addChild(QSharedPointer<DiagnosisItem>::create(tr("Enabled: %1").arg(secondRuleEnabled)));

	auto profiles = QSharedPointer<DiagnosisItem>::create(tr("Windows Firewall profiles"));
	mWindowsFirewall->addChild(profiles);

	auto firewallProfiles = mFirewallDetection.getFirewallProfiles();
	for (const auto& profile : firewallProfiles)
	{
		auto name = QSharedPointer<DiagnosisItem>::create(profile->getName());
		profiles->addChild(name);

		QString enabled = boolToString(profile->getEnabled());
		name->addChild(QSharedPointer<DiagnosisItem>::create(tr("Enabled: %1").arg(enabled)));
	}
	profiles->addChild(QSharedPointer<DiagnosisItem>::create(tr("Warning: The current firewall status can be obscured by additional Group Policies on your system, often set by system administrators.")));

	endInsertRows();
}


void DiagnosisModel::onFirewallInformationFailed()
{
	auto itemModelIndex = index(8, 0);
	removeChildItems(itemModelIndex, mWindowsFirewall);

	beginInsertRows(itemModelIndex, 0, 0);
	mWindowsFirewall->addChild(QSharedPointer<DiagnosisItem>::create(tr("An error occurred while trying to gather firewall information. Please check the log for more information.")));
	endInsertRows();
}


QVariant DiagnosisModel::data(const QModelIndex& pIndex, int pRole) const
{
	if (!pIndex.isValid())
	{
		return QVariant();
	}

	if (pRole != Qt::DisplayRole)
	{
		return QVariant();
	}

	DiagnosisItem* item = static_cast<DiagnosisItem*>(pIndex.internalPointer());

	return item->getText();
}


QModelIndex DiagnosisModel::index(int pRow, int pColumn, const QModelIndex& pParent) const
{
	if (!hasIndex(pRow, pColumn, pParent))
	{
		return QModelIndex();
	}

	DiagnosisItem* parentItem;

	if (pParent.isValid())
	{
		parentItem = static_cast<DiagnosisItem*>(pParent.internalPointer());
	}
	else
	{
		parentItem = mRootItem.data();
	}

	DiagnosisItem* childItem = parentItem->getChild(pRow).data();
	if (childItem)
	{
		return createIndex(pRow, pColumn, childItem);
	}
	else
	{
		return QModelIndex();
	}
}


QModelIndex DiagnosisModel::parent(const QModelIndex& pIndex) const
{
	if (!pIndex.isValid())
	{
		return QModelIndex();
	}

	DiagnosisItem* childItem = static_cast<DiagnosisItem*>(pIndex.internalPointer());
	if (childItem == nullptr)
	{
		return QModelIndex();
	}

	DiagnosisItem* parentItem = childItem->parentItem().data();

	if (parentItem == mRootItem.data())
	{
		return QModelIndex();
	}

	return createIndex(parentItem->row(), 0, parentItem);
}


int DiagnosisModel::rowCount(const QModelIndex& pParent) const
{
	DiagnosisItem* parentItem;
	if (pParent.column() > 0)
	{
		return 0;
	}

	if (pParent.isValid())
	{
		parentItem = static_cast<DiagnosisItem*>(pParent.internalPointer());
	}
	else
	{
		parentItem = mRootItem.data();
	}

	return parentItem->childCount();
}


int DiagnosisModel::columnCount(const QModelIndex& pParent) const
{
	Q_UNUSED(pParent);
	return 1;
}


QDateTime DiagnosisModel::getCreationTime() const
{
	return mContext->getTimestamp();
}


QString DiagnosisModel::getAsPlaintext() const
{
	QStringList modelPlaintext;
	mRootItem->appendPlaintextContent(modelPlaintext);
#ifdef Q_OS_WIN
	return modelPlaintext.join(QLatin1String("\r\n"));

#else
	return modelPlaintext.join(QLatin1String("\n"));

#endif
}


QVariant DiagnosisModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	Q_UNUSED(section);
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
	{
		return mRootItem->getText();
	}
	return QVariant();
}
