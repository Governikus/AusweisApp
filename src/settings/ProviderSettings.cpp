/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */


#include "ProviderSettings.h"

#include "AppSettings.h"
#include "VersionNumber.h"

#include <QRegularExpression>
#include <QSet>

using namespace governikus;


const QLatin1String SETTINGS_GROUP_NAME_PROVIDERS("providers");
const QLatin1String SETTINGS_DEFAULT_NAME("default");
const QLatin1String SETTINGS_NAME_ISSUEDATE("issueDate");
const QLatin1String SETTINGS_NAME_PROVIDERS("list");
const QLatin1String SETTINGS_NAME_SHORTNAME("shortName");
const QLatin1String SETTINGS_NAME_LONGNAME("longName");
const QLatin1String SETTINGS_NAME_SHORTDESCRIPTION("shortDescription");
const QLatin1String SETTINGS_NAME_LONGDESCRIPTION("longDescription");
const QLatin1String SETTINGS_NAME_ADDRESS("address");
const QLatin1String SETTINGS_NAME_HOMEPAGE("homepage");
const QLatin1String SETTINGS_NAME_CATEGORY("category");
const QLatin1String SETTINGS_NAME_PHONE("phone");
const QLatin1String SETTINGS_NAME_EMAIL("email");
const QLatin1String SETTINGS_NAME_POSTALADDRESS("postalAddress");
const QLatin1String SETTINGS_NAME_ICON("icon");
const QLatin1String SETTINGS_NAME_IMAGE("image");
const QLatin1String SETTINGS_NAME_TCTOKENURL("tcTokenUrl");
const QLatin1String SETTINGS_NAME_CLIENTURL("clientUrl");
const QLatin1String SETTINGS_NAME_SUBJECTURLS("subjectUrls");
const QLatin1String SETTINGS_NAME_CALLCOSTS("callcosts");
const QLatin1String SETTINGS_NAME_CALLCOST_PREFIX("prefix");
const QLatin1String SETTINGS_NAME_CALLCOST_FREESECS("free-seconds");
const QLatin1String SETTINGS_NAME_CALLCOST_LANDLINE_PER_MIN("landline-per-minute");
const QLatin1String SETTINGS_NAME_CALLCOST_LANDLINE_PER_CALL("landline-per-call");
const QLatin1String SETTINGS_NAME_CALLCOST_MOB_PER_MIN("mobile-per-minute");
const QLatin1String SETTINGS_NAME_CALLCOST_MOB_PER_CALL("mobile-per-call");


Provider::Provider(const LanguageString& pShortName,
		const LanguageString& pLongName,
		const LanguageString& pShortDescription,
		const LanguageString& pLongDescription,
		const QString& pAddress,
		const QString& pHomepage,
		const QString& pCategory,
		const QString& pPhone,
		const QString& pEmail,
		const QString& pPostalAddress,
		const QString& pIcon,
		const QString& pImage,
		const QString& pTcTokenUrl,
		const QString& pClientUrl,
		const QStringList& pSubjectUrls)
	: mShortName(pShortName)
	, mLongName(pLongName)
	, mShortDescription(pShortDescription)
	, mLongDescription(pLongDescription)
	, mAddress(pAddress)
	, mHomepage(pHomepage)
	, mCategory(pCategory)
	, mPhone(pPhone)
	, mEmail(pEmail)
	, mPostalAddress(pPostalAddress)
	, mIcon(pIcon)
	, mImage(pImage)
	, mLocalIconUrl()
	, mLocalImageUrl()
	, mTcTokenUrl(pTcTokenUrl)
	, mClientUrl(pClientUrl)
	, mSubjectUrls(pSubjectUrls)
{
}


const LanguageString& Provider::getShortName() const
{
	return mShortName;
}


const LanguageString& Provider::getLongName() const
{
	return mLongName;
}


const LanguageString& Provider::getShortDescription() const
{
	return mShortDescription;
}


const LanguageString& Provider::getLongDescription() const
{
	return mLongDescription;
}


const QString& Provider::getAddress() const
{
	return mAddress;
}


QString Provider::getAddressDomain() const
{
	return QUrl::fromUserInput(mAddress).host();
}


const QString& Provider::getHomepage() const
{
	return mHomepage;
}


QString Provider::getHomepageBase() const
{
	return QUrl::fromUserInput(mHomepage).host();
}


const QString& Provider::getCategory() const
{
	return mCategory;
}


const QString& Provider::getPhone() const
{
	return mPhone;
}


const QString& Provider::getEMail() const
{
	return mEmail;
}


const QString& Provider::getPostalAddress() const
{
	return mPostalAddress;
}


const QString& Provider::getIcon() const
{
	return mIcon;
}


const QString& Provider::getImage() const
{
	return mImage;
}


const QString& Provider::getIconUrl() const
{
	return mLocalIconUrl;
}


const QString& Provider::getImageUrl() const
{
	return mLocalImageUrl;
}


const QString& Provider::getTcTokenUrl() const
{
	return mTcTokenUrl;
}


const QString& Provider::getClientUrl() const
{
	return mClientUrl;
}


const QStringList& Provider::getSubjectUrls() const
{
	return mSubjectUrls;
}


void Provider::setTcTokenUrl(const QString& pTcTokenUrl)
{
	mTcTokenUrl = pTcTokenUrl;
}


void Provider::applyMap(const QMap<QString, QString>& pMap)
{
	if (!mIcon.isEmpty())
	{
		mLocalIconUrl = pMap.value(mIcon, QString());
	}

	if (!mImage.isEmpty())
	{
		mLocalImageUrl = pMap.value(mImage, QString());
	}
}


void ProviderSettings::applyMap()
{
	for (Provider& provider : mProviders)
	{
		provider.applyMap(mIconMap);
	}
}


ProviderSettings::ProviderSettings()
	: AbstractSettings()
	, mIssueDate()
	, mProviders()
	, mIconMap()
	, mCallCosts()
{
	connect(this, &ProviderSettings::fireIssueDateChanged, this, &ProviderSettings::fireSettingsChanged);
	connect(this, &ProviderSettings::fireProvidersChanged, this, &ProviderSettings::fireSettingsChanged);
	connect(this, &ProviderSettings::fireCallCostsChanged, this, &ProviderSettings::fireSettingsChanged);
}


QStringList ProviderSettings::getRequiredIcons() const
{
	QSet<QString> icons;

	for (const Provider& provider : mProviders)
	{
		// Request icons / images whose names are not empty.
		const QString icon = provider.getIcon();
		const QString image = provider.getImage();
		if (!icon.isEmpty())
		{
			icons += icon;
		}

		if (!image.isEmpty())
		{
			icons += image;
		}
	}

	return icons.toList();
}


#ifndef QT_NO_DEBUG


ProviderSettings::ProviderSettings(const QDateTime& pIssueDate, const QVector<Provider>& pProviders)
	: AbstractSettings()
	, mIssueDate(pIssueDate)
	, mProviders(pProviders)
	, mIconMap()
	, mCallCosts()
{
}


#endif


ProviderSettings::~ProviderSettings()
{
}


void ProviderSettings::load()
{
	auto settings = getStore();

	static const VersionNumber FIRST_JSON_VERSION = VersionNumber(QStringLiteral("1.9.0"));
	const QString& settingsVersionString = AppSettings::getInstance().getGeneralSettings().getPersistentSettingsVersion();
	if (!settingsVersionString.isEmpty() && VersionNumber(settingsVersionString) < FIRST_JSON_VERSION)
	{
		settings->beginGroup(SETTINGS_GROUP_NAME_PROVIDERS);
		settings->remove(QString());
		settings->endGroup();

		return;
	}

	settings->beginGroup(SETTINGS_GROUP_NAME_PROVIDERS);
	const int listSize = settings->beginReadArray(SETTINGS_NAME_PROVIDERS);
	QVector<Provider> providers;
	providers.reserve(listSize);
	for (int i = 0; i < listSize; ++i)
	{
		settings->setArrayIndex(i);
		const LanguageString shortName = load(settings, SETTINGS_NAME_SHORTNAME);
		const LanguageString longName = load(settings, SETTINGS_NAME_LONGNAME);
		const LanguageString shortDescription = load(settings, SETTINGS_NAME_SHORTDESCRIPTION);
		const LanguageString longDescription = load(settings, SETTINGS_NAME_LONGDESCRIPTION);
		const QString address = getSettingsValue(settings, SETTINGS_NAME_ADDRESS).toString();
		const QString homepage = getSettingsValue(settings, SETTINGS_NAME_HOMEPAGE).toString();
		const QString category = getSettingsValue(settings, SETTINGS_NAME_CATEGORY).toString();
		const QString phone = getSettingsValue(settings, SETTINGS_NAME_PHONE).toString();
		const QString email = getSettingsValue(settings, SETTINGS_NAME_EMAIL).toString();
		const QString postalAddress = getSettingsValue(settings, SETTINGS_NAME_POSTALADDRESS).toString();
		const QString icon = getSettingsValue(settings, SETTINGS_NAME_ICON).toString();
		const QString image = getSettingsValue(settings, SETTINGS_NAME_IMAGE).toString();
		const QString tcTokenUrl = getSettingsValue(settings, SETTINGS_NAME_TCTOKENURL).toString();
		const QString clientUrl = getSettingsValue(settings, SETTINGS_NAME_CLIENTURL).toString();
		const QStringList subjectUrls = getSettingsValue(settings, SETTINGS_NAME_SUBJECTURLS).toStringList();

		providers += Provider(shortName,
				longName,
				shortDescription,
				longDescription,
				address,
				homepage,
				category,
				phone,
				email,
				postalAddress,
				icon,
				image,
				tcTokenUrl,
				clientUrl,
				subjectUrls);
	}
	settings->endArray();

	setProviders(providers);
	setIssueDate(settings->value(SETTINGS_NAME_ISSUEDATE).toDateTime());

	const int callCostListSize = settings->beginReadArray(SETTINGS_NAME_CALLCOSTS);
	QMap<QString, CallCost> callCosts;
	providers.reserve(listSize);
	for (int i = 0; i < callCostListSize; ++i)
	{
		settings->setArrayIndex(i);
		const int freeSeconds = getSettingsValue(settings, SETTINGS_NAME_CALLCOST_FREESECS).toInt();
		const double landlinePerMin = getSettingsValue(settings, SETTINGS_NAME_CALLCOST_LANDLINE_PER_MIN).toDouble();
		const double landlinePerCall = getSettingsValue(settings, SETTINGS_NAME_CALLCOST_LANDLINE_PER_CALL).toDouble();
		const double mobilePerMin = getSettingsValue(settings, SETTINGS_NAME_CALLCOST_MOB_PER_MIN).toDouble();
		const double mobilePerCall = getSettingsValue(settings, SETTINGS_NAME_CALLCOST_MOB_PER_CALL).toDouble();
		const QString prefix = getSettingsValue(settings, SETTINGS_NAME_CALLCOST_PREFIX).toString();
		callCosts.insert(prefix, CallCost(freeSeconds, landlinePerMin, landlinePerCall, mobilePerMin, mobilePerCall));
	}
	settings->endArray();
	setCallCosts(callCosts);

	settings->endGroup();
}


bool ProviderSettings::isUnsaved() const
{
	ProviderSettings oldSettings;
	oldSettings.load();
	return oldSettings != *this;
}


void ProviderSettings::save(QSharedPointer<QSettings>& pSettings, const QString& pGroupName, const LanguageString& pLanguageString)
{
	pSettings->beginGroup(pGroupName);
	for (auto i = pLanguageString.begin(); i != pLanguageString.end(); i++)
	{
		QString key = i.key();
		if (key.isEmpty())
		{
			key = SETTINGS_DEFAULT_NAME;    // QSettings ignores empty keys
		}

		pSettings->setValue(key, i.value());
	}
	pSettings->endGroup();
}


LanguageString ProviderSettings::load(const QSharedPointer<QSettings>& pSettings, const QString& pGroupName)
{
	pSettings->beginGroup(pGroupName);
	const QStringList& keys = pSettings->childKeys();
	QMap<QString, QString> map;
	for (QString key : keys)
	{
		const QString& value = pSettings->value(key).toString();
		if (key == SETTINGS_DEFAULT_NAME)
		{
			key = QString();
		}
		map[key] = value;
	}
	pSettings->endGroup();

	return LanguageString(map);
}


void ProviderSettings::save()
{
	auto settings = getStore();
	settings->beginGroup(SETTINGS_GROUP_NAME_PROVIDERS);
	settings->remove(QString()); // remove the whole group first

	settings->beginWriteArray(SETTINGS_NAME_PROVIDERS);
	for (int i = 0; i < mProviders.size(); ++i)
	{
		settings->setArrayIndex(i);
		auto& provider = mProviders.at(i);

		save(settings, SETTINGS_NAME_SHORTNAME, provider.getShortName());
		save(settings, SETTINGS_NAME_LONGNAME, provider.getLongName());
		save(settings, SETTINGS_NAME_SHORTDESCRIPTION, provider.getShortDescription());
		save(settings, SETTINGS_NAME_LONGDESCRIPTION, provider.getLongDescription());
		settings->setValue(SETTINGS_NAME_ADDRESS, provider.getAddress());
		settings->setValue(SETTINGS_NAME_HOMEPAGE, provider.getHomepage());
		settings->setValue(SETTINGS_NAME_CATEGORY, provider.getCategory());
		settings->setValue(SETTINGS_NAME_PHONE, provider.getPhone());
		settings->setValue(SETTINGS_NAME_EMAIL, provider.getEMail());
		settings->setValue(SETTINGS_NAME_POSTALADDRESS, provider.getPostalAddress());
		settings->setValue(SETTINGS_NAME_ICON, provider.getIcon());
		settings->setValue(SETTINGS_NAME_IMAGE, provider.getImage());
		settings->setValue(SETTINGS_NAME_TCTOKENURL, provider.getTcTokenUrl());
		settings->setValue(SETTINGS_NAME_CLIENTURL, provider.getClientUrl());
		settings->setValue(SETTINGS_NAME_SUBJECTURLS, provider.getSubjectUrls());
	}
	settings->endArray();

	settings->beginWriteArray(SETTINGS_NAME_CALLCOSTS);
	const auto& prefixes = mCallCosts.keys();
	for (int i = 0; i < prefixes.size(); ++i)
	{
		settings->setArrayIndex(i);
		const auto& prefix = prefixes.at(i);
		const auto callCost = mCallCosts.value(prefix);

		settings->setValue(SETTINGS_NAME_CALLCOST_FREESECS, callCost.getFreeSeconds());
		settings->setValue(SETTINGS_NAME_CALLCOST_LANDLINE_PER_MIN, callCost.getLandlineCentsPerMinute());
		settings->setValue(SETTINGS_NAME_CALLCOST_LANDLINE_PER_CALL, callCost.getLandlineCentsPerCall());
		settings->setValue(SETTINGS_NAME_CALLCOST_MOB_PER_MIN, callCost.getMobileCentsPerMinute());
		settings->setValue(SETTINGS_NAME_CALLCOST_MOB_PER_CALL, callCost.getMobileCentsPerCall());
		settings->setValue(SETTINGS_NAME_CALLCOST_PREFIX, prefix);
	}
	settings->endArray();


	settings->setValue(SETTINGS_NAME_ISSUEDATE, mIssueDate);

	settings->endGroup();
	settings->sync();
}


void ProviderSettings::update(const AbstractSettings& pOther)
{
	const ProviderSettings* const other = qobject_cast<const ProviderSettings* const>(&pOther);
	if (other != nullptr)
	{
		mIssueDate = other->getIssueDate();
		setProviders(other->getProviders());
		setCallCosts(other->getCallCosts());
	}
}


const QDateTime& ProviderSettings::getIssueDate() const
{
	return mIssueDate;
}


void ProviderSettings::setIssueDate(const QDateTime& pIssueDate)
{
	if (mIssueDate != pIssueDate)
	{
		mIssueDate = pIssueDate;
		Q_EMIT fireIssueDateChanged();
	}
}


const QVector<Provider>& ProviderSettings::getProviders() const
{
	return mProviders;
}


void ProviderSettings::setProviders(const QVector<Provider>& pProviders)
{
	if (mProviders != pProviders)
	{
		mProviders = pProviders;

		Q_EMIT fireProvidersChanged();

		Q_EMIT fireRequiredIcons(getRequiredIcons());
	}
}


void ProviderSettings::setCallCosts(const QMap<QString, CallCost>& pCallCosts)
{
	if (mCallCosts != pCallCosts)
	{
		mCallCosts = pCallCosts;
		Q_EMIT fireCallCostsChanged();
	}
}


const QMap<QString, CallCost>& ProviderSettings::getCallCosts() const
{
	return mCallCosts;
}


CallCost ProviderSettings::getCallCost(const Provider& pProvider) const
{
	return getCallCost(pProvider.getPhone());
}


CallCost ProviderSettings::getCallCost(const QString& pProviderPhone) const
{
	QString standardisedPhoneNumber = pProviderPhone;
	standardisedPhoneNumber = standardisedPhoneNumber.remove(QLatin1String("+49"));
	standardisedPhoneNumber = standardisedPhoneNumber.remove(QRegularExpression("[^\\d]"));

	if (!standardisedPhoneNumber.isEmpty())
	{
		const auto& prefixes = mCallCosts.keys();
		for (const auto& prefix : prefixes)
		{
			if (standardisedPhoneNumber.startsWith(prefix))
			{
				return mCallCosts[prefix];
			}
		}
	}
	return CallCost();
}


void ProviderSettings::requestIconMap()
{
	Q_EMIT fireRequiredIcons(getRequiredIcons());
}


void ProviderSettings::onIconMapChanged(const QMap<QString, QString>& pIconMap)
{
	if (mIconMap != pIconMap)
	{
		mIconMap = pIconMap;
		applyMap();

		Q_EMIT fireProvidersChanged();
	}
}
