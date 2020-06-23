/*!
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

#include "CertificateDescriptionModel.h"

#include "AppSettings.h"
#include "LanguageLoader.h"
#include "SecureStorage.h"


using namespace governikus;


QSharedPointer<const CertificateDescription> CertificateDescriptionModel::getCertificateDescription() const
{
	if (mContext && mContext->getDidAuthenticateEac1())
	{
		return mContext->getDidAuthenticateEac1()->getCertificateDescription();
	}

	return QSharedPointer<const CertificateDescription>();
}


void CertificateDescriptionModel::onDidAuthenticateEac1Changed()
{
	beginResetModel();
	mData.clear();
	if (const auto& certDescr = getCertificateDescription())
	{
		initModelData(certDescr);
	}
	endResetModel();

	Q_EMIT fireChanged();
}


void CertificateDescriptionModel::initModelData(const QSharedPointer<const CertificateDescription>& pCertDescription)
{
	const QString& serviceProviderAddress = pCertDescription->getServiceProviderAddress();
	const QString& purpose = getPurpose();
	const QString& dataSecurityOfficer = pCertDescription->getDataSecurityOfficer();
	const QString termsOfUsage = pCertDescription->getTermsOfUsage().remove(QLatin1Char('\r')).replace(QLatin1Char('\t'), QLatin1Char(' '));
	const bool showDetailedProviderInfo = !(serviceProviderAddress.isEmpty() || purpose.isEmpty() || dataSecurityOfficer.isEmpty());

	//: LABEL ALL_PLATFORMS
	mData += QPair<QString, QString>(tr("Provider"), getSubjectName() + QLatin1Char('\n') + getSubjectUrl());
	//: LABEL ALL_PLATFORMS
	mData += QPair<QString, QString>(tr("Certificate issuer"), pCertDescription->getIssuerName() + QLatin1Char('\n') + pCertDescription->getIssuerUrl());
	if (showDetailedProviderInfo)
	{
		//: LABEL ALL_PLATFORMS
		mData += QPair<QString, QString>(tr("Name, address and mail address of the provider"), serviceProviderAddress);
		//: LABEL ALL_PLATFORMS
		mData += QPair<QString, QString>(tr("Purpose"), purpose);
		//: LABEL ALL_PLATFORMS
		mData += QPair<QString, QString>(tr("Indication of the bodies responsible for the provider, "
											"that verify the compliance with data security regulations"), dataSecurityOfficer);
	}
	else if (!termsOfUsage.isEmpty())
	{
		//: LABEL ALL_PLATFORMS
		mData += QPair<QString, QString>(tr("Provider information"), termsOfUsage);
	}
	if (!getValidity().isEmpty())
	{
		//: LABEL ALL_PLATFORMS
		mData += QPair<QString, QString>(tr("Validity"), getValidity());
	}
}


CertificateDescriptionModel::CertificateDescriptionModel(QObject* pParent)
	: QAbstractListModel(pParent)
	, mData()
	, mContext()
{
	resetContext();
	connect(Env::getSingleton<AppSettings>(), &AppSettings::fireSettingsChanged, this, &CertificateDescriptionModel::onDidAuthenticateEac1Changed);
	connect(&Env::getSingleton<AppSettings>()->getGeneralSettings(), &GeneralSettings::fireSettingsChanged, this, [this]()
			{
				beginResetModel();
				onDidAuthenticateEac1Changed();
				endResetModel();
			});
}


void CertificateDescriptionModel::resetContext(const QSharedPointer<AuthContext>& pContext)
{
	mContext = pContext;
	if (mContext)
	{
		connect(mContext.data(), &AuthContext::fireDidAuthenticateEac1Changed, this, &CertificateDescriptionModel::onDidAuthenticateEac1Changed);
	}

	onDidAuthenticateEac1Changed();
}


QString CertificateDescriptionModel::getSubjectName() const
{
	const auto& certificateDescription = getCertificateDescription();
	return certificateDescription ? getCertificateDescription()->getSubjectName() : QString();
}


QString CertificateDescriptionModel::getSubjectUrl() const
{
	const auto& certificateDescription = getCertificateDescription();
	return certificateDescription ? getCertificateDescription()->getSubjectUrl() : QString();
}


QString CertificateDescriptionModel::getPurpose() const
{
	const auto& certificateDescription = getCertificateDescription();
	return certificateDescription ? getCertificateDescription()->getPurpose() : QString();
}


QString CertificateDescriptionModel::getValidity() const
{
	if (mContext && mContext->getDidAuthenticateEac1() && !mContext->getDidAuthenticateEac1()->getCvCertificates().isEmpty())
	{
		const CVCertificateBody body = mContext->getDidAuthenticateEac1()->getCvCertificates().at(0)->getBody();
		const auto locale = LanguageLoader::getInstance().getUsedLocale();
		const auto effectiveDate = locale.toString(body.getCertificateEffectiveDate(), QLocale::ShortFormat);
		const auto expirationDate = locale.toString(body.getCertificateExpirationDate(), QLocale::ShortFormat);

		return QStringLiteral("%1 - %2").arg(effectiveDate, expirationDate);
	}
	return QString();
}


int CertificateDescriptionModel::rowCount(const QModelIndex&) const
{
	return mData.size();
}


QVariant CertificateDescriptionModel::data(const QModelIndex& pIndex, int pRole) const
{
	if (pIndex.isValid() && pIndex.row() < rowCount())
	{
		auto entry = mData[pIndex.row()];
		if (pRole == LABEL)
		{
			return entry.first;
		}
		if (pRole == TEXT)
		{
			return entry.second;
		}
	}
	return QVariant();
}


QHash<int, QByteArray> CertificateDescriptionModel::roleNames() const
{
	QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
	roles.insert(LABEL, "label");
	roles.insert(TEXT, "text");
	return roles;
}
