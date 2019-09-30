/*!
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
 */

#include "AccessRoleAndRight.h"

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(card)


using namespace governikus;


QList<AccessRight> AccessRoleAndRightsUtil::mAllRights;
QList<AccessRight> AccessRoleAndRightsUtil::mAllDisplayedOrderedRights;


// TR-03124, 3.8: Access rights for data groups or functions not defined in [TR-03127] SHALL NOT be available.
const QList<AccessRight>& AccessRoleAndRightsUtil::allDisplayedOrderedRights()
{
	if (mAllDisplayedOrderedRights.isEmpty())
	{
		mAllDisplayedOrderedRights += AccessRight::READ_DG05;
		mAllDisplayedOrderedRights += AccessRight::READ_DG13;
		mAllDisplayedOrderedRights += AccessRight::READ_DG04;
		mAllDisplayedOrderedRights += AccessRight::READ_DG07;
		mAllDisplayedOrderedRights += AccessRight::READ_DG08;
		mAllDisplayedOrderedRights += AccessRight::READ_DG09;
		mAllDisplayedOrderedRights += AccessRight::READ_DG17;
		mAllDisplayedOrderedRights += AccessRight::READ_DG01;
		mAllDisplayedOrderedRights += AccessRight::READ_DG03;
		mAllDisplayedOrderedRights += AccessRight::RESTRICTED_IDENTIFICATION;
		mAllDisplayedOrderedRights += AccessRight::READ_DG10;
		mAllDisplayedOrderedRights += AccessRight::READ_DG06;
		mAllDisplayedOrderedRights += AccessRight::AGE_VERIFICATION;
		mAllDisplayedOrderedRights += AccessRight::COMMUNITY_ID_VERIFICATION;
		mAllDisplayedOrderedRights += AccessRight::READ_DG02;
		mAllDisplayedOrderedRights += AccessRight::READ_DG19;
		mAllDisplayedOrderedRights += AccessRight::INSTALL_QUAL_CERT;
		mAllDisplayedOrderedRights += AccessRight::INSTALL_CERT;
	}
	return mAllDisplayedOrderedRights;
}


// TR 03127 - 3.2.2
const QList<AccessRight>& AccessRoleAndRightsUtil::allRights()
{
	if (mAllRights.isEmpty())
	{
		mAllRights += AccessRight::WRITE_DG17;
		mAllRights += AccessRight::WRITE_DG18;
		mAllRights += AccessRight::WRITE_DG19;
		mAllRights += AccessRight::WRITE_DG20;
		mAllRights += AccessRight::WRITE_DG21;

		mAllRights += AccessRight::RFU_32;
		mAllRights += AccessRight::RFU_31;
		mAllRights += AccessRight::RFU_30;
		mAllRights += AccessRight::RFU_29;

		mAllRights += AccessRight::READ_DG21;
		mAllRights += AccessRight::READ_DG20;
		mAllRights += AccessRight::READ_DG19;
		mAllRights += AccessRight::READ_DG18;
		mAllRights += AccessRight::READ_DG17;
		mAllRights += AccessRight::READ_DG16;
		mAllRights += AccessRight::READ_DG15;
		mAllRights += AccessRight::READ_DG14;
		mAllRights += AccessRight::READ_DG13;
		mAllRights += AccessRight::READ_DG12;
		mAllRights += AccessRight::READ_DG11;
		mAllRights += AccessRight::READ_DG10;
		mAllRights += AccessRight::READ_DG09;
		mAllRights += AccessRight::READ_DG08;
		mAllRights += AccessRight::READ_DG07;
		mAllRights += AccessRight::READ_DG06;
		mAllRights += AccessRight::READ_DG05;
		mAllRights += AccessRight::READ_DG04;
		mAllRights += AccessRight::READ_DG03;
		mAllRights += AccessRight::READ_DG02;
		mAllRights += AccessRight::READ_DG01;

		mAllRights += AccessRight::INSTALL_QUAL_CERT;
		mAllRights += AccessRight::INSTALL_CERT;
		mAllRights += AccessRight::PIN_MANAGEMENT;
		mAllRights += AccessRight::CAN_ALLOWED;
		mAllRights += AccessRight::PRIVILEGED_TERMINAL;
		mAllRights += AccessRight::RESTRICTED_IDENTIFICATION;
		mAllRights += AccessRight::COMMUNITY_ID_VERIFICATION;
		mAllRights += AccessRight::AGE_VERIFICATION;
	}

	return mAllRights;
}


// TR-03127, Annex C
QString AccessRoleAndRightsUtil::toDisplayText(AccessRight pRight)
{
	switch (pRight)
	{
		case AccessRight::WRITE_DG17:
			//: LABEL ALL_PLATFORMS
			return tr("WRITE_DG17");

		case AccessRight::WRITE_DG18:
			//: LABEL ALL_PLATFORMS
			return tr("WRITE_DG18");

		case AccessRight::WRITE_DG19:
			//: LABEL ALL_PLATFORMS
			return tr("WRITE_DG19");

		case AccessRight::WRITE_DG20:
			//: LABEL ALL_PLATFORMS
			return tr("WRITE_DG20");

		case AccessRight::WRITE_DG21:
			//: LABEL ALL_PLATFORMS
			return tr("WRITE_DG21");

		/* 32-29: reserved for future use */
		case AccessRight::READ_DG21:
			//: LABEL ALL_PLATFORMS
			return tr("Optional data");

		case AccessRight::READ_DG20:
			//: LABEL ALL_PLATFORMS
			return tr("Residence permit II");

		case AccessRight::READ_DG19:
			// "Auxiliary conditions" are replaced with "Residence permit I" in agreement with the BMI
			//: LABEL ALL_PLATFORMS
			return tr("Residence permit I");

		case AccessRight::READ_DG18:
			//: LABEL ALL_PLATFORMS
			return tr("Community-ID");

		case AccessRight::READ_DG17:
			//: LABEL ALL_PLATFORMS
			return tr("Address");

		case AccessRight::READ_DG16:
			//: LABEL ALL_PLATFORMS
			return tr("RFU");

		case AccessRight::READ_DG15:
			//: LABEL ALL_PLATFORMS
			return tr("RFU");

		case AccessRight::READ_DG14:
			//: LABEL ALL_PLATFORMS
			return tr("RFU");

		case AccessRight::READ_DG13:
			//: LABEL ALL_PLATFORMS
			return tr("Birth name");

		case AccessRight::READ_DG12:
			//: LABEL ALL_PLATFORMS
			return tr("Optional data");

		case AccessRight::READ_DG11:
			//: LABEL ALL_PLATFORMS
			return tr("Gender");

		case AccessRight::READ_DG10:
			//: LABEL ALL_PLATFORMS
			return tr("Nationality");

		case AccessRight::READ_DG09:
			//: LABEL ALL_PLATFORMS
			return tr("Place of birth");

		case AccessRight::READ_DG08:
			//: LABEL ALL_PLATFORMS
			return tr("Date of birth");

		case AccessRight::READ_DG07:
			//: LABEL ALL_PLATFORMS
			return tr("Doctoral degree");

		case AccessRight::READ_DG06:
			//: LABEL ALL_PLATFORMS
			return tr("Religious / artistic name");

		case AccessRight::READ_DG05:
			//: LABEL ALL_PLATFORMS
			return tr("Family name");

		case AccessRight::READ_DG04:
			//: LABEL ALL_PLATFORMS
			return tr("Given name(s)");

		case AccessRight::READ_DG03:
			//: LABEL ALL_PLATFORMS
			return tr("Valid until");

		case AccessRight::READ_DG02:
			//: LABEL ALL_PLATFORMS
			return tr("Issuing country");

		case AccessRight::READ_DG01:
			//: LABEL ALL_PLATFORMS
			return tr("Document type");

		case AccessRight::INSTALL_QUAL_CERT:
			//: LABEL ALL_PLATFORMS
			return tr("Installation of qualified signature certificates");

		case AccessRight::INSTALL_CERT:
			//: LABEL ALL_PLATFORMS
			return tr("Installation of signature certificates");

		case AccessRight::PIN_MANAGEMENT:
			//: LABEL ALL_PLATFORMS
			return tr("PIN Management");

		case AccessRight::CAN_ALLOWED:
			//: LABEL ALL_PLATFORMS
			return tr("CAN allowed");

		case AccessRight::PRIVILEGED_TERMINAL:
			//: LABEL ALL_PLATFORMS
			return tr("Privileged terminal");

		case AccessRight::RESTRICTED_IDENTIFICATION:
			//: LABEL ALL_PLATFORMS
			return tr("Pseudonym");

		case AccessRight::COMMUNITY_ID_VERIFICATION:
			//: LABEL ALL_PLATFORMS
			return tr("Address verification");

		case AccessRight::AGE_VERIFICATION:
			//: LABEL ALL_PLATFORMS
			return tr("Age verification");

		case AccessRight::RFU_29:
		case AccessRight::RFU_30:
		case AccessRight::RFU_31:
		case AccessRight::RFU_32:
			//: LABEL ALL_PLATFORMS
			return tr("Unknown (reserved)");
	}

	//: LABEL ALL_PLATFORMS
	return tr("Unknown");
}


QLatin1String AccessRoleAndRightsUtil::toTechnicalName(AccessRight pRight)
{
	const auto name = getEnumName(static_cast<AccessRightNames>(pRight));
	if (name.isEmpty())
	{
		qCCritical(card) << "Requested AccessRight without mapping:" << pRight;
	}
	return name;
}


QString AccessRoleAndRightsUtil::joinFromTechnicalName(const QStringList& pStr, const QString& pJoin)
{
	return fromTechnicalName(pStr).join(pJoin);
}


QStringList AccessRoleAndRightsUtil::fromTechnicalName(const QStringList& pStr)
{
	QStringList result;
	for (auto entry : pStr)
	{
		fromTechnicalName(entry, [&entry](AccessRight pRight){
					entry = AccessRoleAndRightsUtil::toDisplayText(pRight);
				});
		result << entry;
	}
	return result;
}


bool AccessRoleAndRightsUtil::fromTechnicalName(const QString& pStr, const std::function<void(AccessRight)>& pFunc)
{
	return fromTechnicalName(pStr.toLatin1().constData(), pFunc);
}


bool AccessRoleAndRightsUtil::fromTechnicalName(const char* const pStr, const std::function<void(AccessRight)>& pFunc)
{
	const auto undefined = static_cast<AccessRightNames>(UINT_MAX);

	auto entry = Enum<AccessRightNames>::fromString(pStr, undefined);
	if (entry != undefined)
	{
		Q_ASSERT(Enum<AccessRight>::isValue(static_cast<int>(entry)));
		pFunc(static_cast<AccessRight>(entry));
		return true;
	}
	return false;
}


#include "moc_AccessRoleAndRight.cpp"
