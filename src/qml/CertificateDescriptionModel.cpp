/*!
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "AppSettings.h"
#include "CertificateDescriptionModel.h"
#include "asn1/CertificateDescription.h"
#include "context/AuthContext.h"
#include "context/SelfAuthenticationContext.h"


using namespace governikus;


QSharedPointer<CertificateDescription> CertificateDescriptionModel::getCertificateDescription() const
{
	if (mContext && mContext->getDidAuthenticateEac1())
	{
		return mContext->getDidAuthenticateEac1()->getCertificateDescription();
	}

	const bool useTestUri = AppSettings::getInstance().getGeneralSettings().useSelfauthenticationTestUri();
	const auto& rawCertDescr = AppSettings::getInstance().getSecureStorage().getSelfAuthenticationCertDescr(useTestUri);
	QSharedPointer<CertificateDescription> selfAuthCertificateDescription(CertificateDescription::fromHex(rawCertDescr));
	Q_ASSERT(selfAuthCertificateDescription);

	return selfAuthCertificateDescription;
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


void CertificateDescriptionModel::initModelData(const QSharedPointer<CertificateDescription>& pCertDescription)
{
	const QString& serviceProviderAddress = pCertDescription->getServiceProviderAddress();
	const QString& purpose = getPurpose();
	const QString& dataSecurityOfficer = pCertDescription->getDataSecurityOfficer();
	const QString& termsOfUsage = pCertDescription->getTermsOfUsage();
	const bool showDetailedProviderInfo = !(serviceProviderAddress.isEmpty() || purpose.isEmpty() || dataSecurityOfficer.isEmpty());

	mData += QPair<QString, QString>(tr("Service provider"), getSubjectName() + '\n' + getSubjectUrl());
	mData += QPair<QString, QString>(tr("Certificate issuer"), pCertDescription->getIssuerName() + '\n' + pCertDescription->getIssuerUrl());
	if (showDetailedProviderInfo)
	{
		mData += QPair<QString, QString>(tr("Name, address and mail address of the service provider"), serviceProviderAddress);
		mData += QPair<QString, QString>(tr("Purpose"), purpose);
		mData += QPair<QString, QString>(tr("Indication of the bodies responsible for the service provider, "
											"that verify the compliance with data security regulations"), dataSecurityOfficer);
	}
	else if (!termsOfUsage.isEmpty())
	{
		mData += QPair<QString, QString>(tr("Service provider information"), termsOfUsage);
	}
	if (!getValidity().isEmpty())
	{
		mData += QPair<QString, QString>(tr("Validity"), getValidity());
	}
}


CertificateDescriptionModel::CertificateDescriptionModel(QObject* pParent)
	: QAbstractListModel(pParent)
	, mData()
	, mContext()
{
	resetContext();
	connect(&AppSettings::getInstance(), &AppSettings::fireSettingsChanged, this, &CertificateDescriptionModel::onDidAuthenticateEac1Changed);
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
		const QString effectiveDate = body.getCertificateEffectiveDate().toString(Qt::DefaultLocaleShortDate);
		const QString expirationDate = body.getCertificateExpirationDate().toString(Qt::DefaultLocaleShortDate);

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
