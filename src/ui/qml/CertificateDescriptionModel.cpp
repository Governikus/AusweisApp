/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */

#include "CertificateDescriptionModel.h"

#include "LanguageLoader.h"
#include "context/AuthContext.h"
#include "context/IfdServiceContext.h"


using namespace governikus;


CertificateDescriptionModel::CertificateDescriptionModel()
	: QAbstractListModel()
	, mData()
	, mContext()
{
	resetContext();
}


QSharedPointer<const CertificateDescription> CertificateDescriptionModel::getCertificateDescription() const
{
	if (const auto authContext = mContext.objectCast<AuthContext>(); authContext && authContext->getDidAuthenticateEac1())
	{
		return authContext->getDidAuthenticateEac1()->getCertificateDescription();
	}

	if (const auto ifdContext = mContext.objectCast<IfdServiceContext>(); ifdContext && ifdContext->getCertificateDescription())
	{
		return ifdContext->getCertificateDescription();
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


void CertificateDescriptionModel::onTranslationChanged()
{
	onDidAuthenticateEac1Changed();
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


void CertificateDescriptionModel::resetContext(const QSharedPointer<WorkflowContext>& pContext)
{
	mContext = pContext;
	if (auto authContext = pContext.objectCast<AuthContext>())
	{
		connect(authContext.data(), &AuthContext::fireDidAuthenticateEac1Changed, this, &CertificateDescriptionModel::onDidAuthenticateEac1Changed);
		connect(authContext.data(), &AuthContext::fireAccessRightManagerCreated, this, &CertificateDescriptionModel::onDidAuthenticateEac1Changed);
	}
	else if (auto ifdContext = pContext.objectCast<IfdServiceContext>())
	{
		connect(ifdContext.data(), &IfdServiceContext::fireAccessRightManagerCreated, this, &CertificateDescriptionModel::onDidAuthenticateEac1Changed);
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
	if (const auto authContext = mContext.objectCast<AuthContext>(); authContext&& authContext->getAccessRightManager() && authContext->getAccessRightManager()->getTerminalCvc())
	{
		const CVCertificateBody body = authContext->getAccessRightManager()->getTerminalCvc()->getBody();
		const auto locale = LanguageLoader::getInstance().getUsedLocale();
		const auto effectiveDate = locale.toString(body.getCertificateEffectiveDate(), QLocale::ShortFormat);
		const auto expirationDate = locale.toString(body.getCertificateExpirationDate(), QLocale::ShortFormat);

		return QStringLiteral("%1 - %2").arg(effectiveDate, expirationDate);
	}
	return QString();
}


int CertificateDescriptionModel::rowCount(const QModelIndex&) const
{
	return static_cast<int>(mData.size());
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
