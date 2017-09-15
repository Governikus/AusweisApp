/*!
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#include "context/SelfAuthenticationContext.h"
#include "SelfAuthenticationModel.h"

using namespace governikus;


void SelfAuthenticationModel::onSelfAuthenticationDataChanged()
{
	beginResetModel();
	mData.clear();

	if (mContext && mContext->getSelfAuthenticationData())
	{
		//fill layout with new data, see 18 Personalausweisgesetz (PAuswG)

		auto selfAuthenticationData = mContext->getSelfAuthenticationData();
		if (!selfAuthenticationData->getValue(SelfAuthData::FamilyNames).isNull())
		{
			mData += QPair<QString, QString>(tr("Family name"), selfAuthenticationData->getValue(SelfAuthData::FamilyNames));
		}
		if (!selfAuthenticationData->getValue(SelfAuthData::BirthName).isNull())
		{
			mData += QPair<QString, QString>(tr("Birth name"), selfAuthenticationData->getValue(SelfAuthData::BirthName));
		}
		if (!selfAuthenticationData->getValue(SelfAuthData::GivenNames).isNull())
		{
			mData += QPair<QString, QString>(tr("Given name(s)"), selfAuthenticationData->getValue(SelfAuthData::GivenNames));
		}
		if (!selfAuthenticationData->getValue(SelfAuthData::AcademicTitle).isNull())
		{
			mData += QPair<QString, QString>(tr("Doctoral degree"), selfAuthenticationData->getValue(SelfAuthData::AcademicTitle));
		}
		if (!selfAuthenticationData->getValue(SelfAuthData::DateOfBirth).isNull())
		{
			QDateTime dateTime = QDateTime::fromString(selfAuthenticationData->getValue(SelfAuthData::DateOfBirth), QStringLiteral("yyyy-MM-dd+hh:mm"));
			auto dateString = dateTime.toString(tr("MM/dd/yyyy"));
			mData += QPair<QString, QString>(tr("Date of birth"), dateString);
		}
		if (!selfAuthenticationData->getValue(SelfAuthData::PlaceOfBirth).isNull())
		{
			mData += QPair<QString, QString>(tr("Place of birth"), selfAuthenticationData->getValue(SelfAuthData::PlaceOfBirth));
		}

		QString address;
		if (!selfAuthenticationData->getValue(SelfAuthData::PlaceOfResidenceNoPlaceInfo).isNull())
		{
			address += selfAuthenticationData->getValue(SelfAuthData::PlaceOfResidenceNoPlaceInfo);
		}
		if (!selfAuthenticationData->getValue(SelfAuthData::PlaceOfResidenceStreet).isNull())
		{
			address += address.isNull() ? QString() : QLatin1String("<br>");
			address += selfAuthenticationData->getValue(SelfAuthData::PlaceOfResidenceStreet);
		}
		if (!selfAuthenticationData->getValue(SelfAuthData::PlaceOfResidenceZipCode).isNull() || !selfAuthenticationData->getValue(SelfAuthData::PlaceOfResidenceCity).isNull())
		{
			address += address.isNull() ? QString() : QLatin1String("<br>");
			address += selfAuthenticationData->getValue(SelfAuthData::PlaceOfResidenceZipCode) + QLatin1Char(' ') + selfAuthenticationData->getValue(SelfAuthData::PlaceOfResidenceCity);
		}
		if (!selfAuthenticationData->getValue(SelfAuthData::PlaceOfResidenceCountry).isNull())
		{
			address += address.isNull() ? QString() : QLatin1String("<br>");
			address += selfAuthenticationData->getValue(SelfAuthData::PlaceOfResidenceCountry);
		}
		if (!address.isNull())
		{
			mData += QPair<QString, QString>(tr("Address"), address);
		}

		auto documentType = selfAuthenticationData->getValue(SelfAuthData::DocumentType);
		if (!documentType.isNull())
		{
			mData += QPair<QString, QString>(tr("Document type"), documentType);
		}
		if (!selfAuthenticationData->getValue(SelfAuthData::Nationality).isNull())
		{
			mData += QPair<QString, QString>(tr("Nationality"), selfAuthenticationData->getValue(SelfAuthData::Nationality));
		}
		if (!selfAuthenticationData->getValue(SelfAuthData::ArtisticName).isNull())
		{
			mData += QPair<QString, QString>(tr("Religious / artistic name"), selfAuthenticationData->getValue(SelfAuthData::ArtisticName));
		}
		if (!selfAuthenticationData->getValue(SelfAuthData::IssuingState).isNull())
		{
			mData += QPair<QString, QString>(tr("Issuing country"), selfAuthenticationData->getValue(SelfAuthData::IssuingState));
		}

		// Show "Residence Permit" for eAT- and Test-Cards only
		// AR, AS, AF --> see TR-03127 (v1.16) chapter 3.2.3
		// TA --> Used by Test-Cards
		if (!selfAuthenticationData->getValue(SelfAuthData::ResidencePermitI).isNull() && (
					documentType == QLatin1String("AR") ||
					documentType == QLatin1String("AS") ||
					documentType == QLatin1String("AF") ||
					documentType == QLatin1String("TA")))
		{
			mData += QPair<QString, QString>(tr("Residence permit I"), selfAuthenticationData->getValue(SelfAuthData::ResidencePermitI));
		}
	}

	endResetModel();
}


SelfAuthenticationModel::SelfAuthenticationModel(QObject* pParent)
	: QAbstractListModel(pParent)
	, mContext()
{
	onSelfAuthenticationDataChanged();
}


void SelfAuthenticationModel::resetContext(const QSharedPointer<SelfAuthenticationContext>& pContext)
{
	mContext = pContext;
	if (mContext)
	{
		connect(mContext.data(), &SelfAuthenticationContext::fireSelfAuthenticationDataChanged, this, &SelfAuthenticationModel::onSelfAuthenticationDataChanged);
	}
	onSelfAuthenticationDataChanged();
}


void SelfAuthenticationModel::startWorkflow()
{
	Q_EMIT fireStartWorkflow();
}


void SelfAuthenticationModel::cancelWorkflow()
{
	if (mContext)
	{
		Q_EMIT mContext->fireCancelWorkflow();
	}
}


bool SelfAuthenticationModel::isBasicReader()
{
	if (mContext)
	{
		return mContext->getCardConnection()->getReaderInfo().isBasicReader();
	}

	return true;
}


int SelfAuthenticationModel::rowCount(const QModelIndex&) const
{
	return mData.size();
}


QVariant SelfAuthenticationModel::data(const QModelIndex& pIndex, int pRole) const
{
	if (pIndex.isValid() && pIndex.row() < rowCount())
	{
		auto pair = mData.at(pIndex.row());
		if (pRole == Qt::DisplayRole || pRole == NAME)
		{
			return pair.first;
		}
		if (pRole == VALUE)
		{
			return pair.second;
		}
	}
	return QVariant();
}


QHash<int, QByteArray> SelfAuthenticationModel::roleNames() const
{
	QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
	roles.insert(NAME, "name");
	roles.insert(VALUE, "value");
	return roles;
}
