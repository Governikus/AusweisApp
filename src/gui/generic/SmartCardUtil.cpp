/*!
 * SmartCardUtil.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "SmartCardUtil.h"

using namespace governikus;

static QString prefix(QStringLiteral(":/images/reader/"));

QString SmartCardUtil::getMultipleReaderIconPath()
{
	return prefix + "default_more_cardreader.png";
}


QString SmartCardUtil::getNoReaderFoundIconPath()
{
	return prefix + "default_no_cardreader_01.png";
}


QString SmartCardUtil::getReaderIconPath(ReaderType pReaderType)
{
	switch (pReaderType)
	{
		case ReaderType::REINER_cyberJack_RFID_komfort:
			return prefix + "img_Reiner_SCT_cyberjack_RFID_komfort_mit_ausweis.png";

		case ReaderType::REINER_cyberJack_RFID_standard:
			return prefix + "img_Reiner_SCT_cyberjack_RFID_standard_mit_ausweis.png";

		case ReaderType::REINER_cyberJack_RFID_basis:
			return prefix + "img_Reiner_SCT_cyberjack_RFID_basis_mit_ausweis.png";

		case ReaderType::REINER_cyberJack_wave:
			return prefix + "img_cyberjack_wave_mit_ausweis.png";

		case ReaderType::SCM_SCL011_Contactless_Reader:
			return prefix + "img_Identive_SCL011_mit_ausweis.png";

		case ReaderType::SCM_SDI010:
		case ReaderType::SCM_SDI011:
			return prefix + "img_Identive_SDI011_mit_ausweis.png";

		case ReaderType::KOBIL_IDToken:
			return prefix + "img_KOBIL_ID_Token_mit_ausweis.png";

		case ReaderType::ACS_ACR1281_PICC_Reader:
			return prefix + "img_ACS_ACR1281U_mit_ausweis.png";

		case ReaderType::OMNIKEY_CardMan_5x21_CL:
			return prefix + "img_HID_Global_OMNIKEY_5321_V2_mit_ausweis.png";

		case ReaderType::OMNIKEY_4121_CL:
			return prefix + "img_HID_Omnikey_Mobile_Reader_4121_CL_mit_ausweis.png";

		case ReaderType::FEIG_OBID_myAXXESS_basic:
			return prefix + "img_FEIG_myAXXES_basic_mit_ausweis.png";

		case ReaderType::Gemalto_Prox_SU:
			return prefix + "img_Gemalto_Prox_SU_mit_ausweis.png";

		case ReaderType::Gemalto_Prox_DU:
			return prefix + "img_Gemalto_Prox_DU_mit_ausweis.png";

		case ReaderType::UNKNOWN:
			return getNoReaderFoundIconPath();
	}
	return QString();
}


QString SmartCardUtil::getReaderEmptyIconPath(ReaderType pReaderType)
{
	switch (pReaderType)
	{
		case ReaderType::REINER_cyberJack_RFID_komfort:
			return prefix + "img_Reiner_SCT_cyberjack_RFID_komfort.png";

		case ReaderType::REINER_cyberJack_RFID_standard:
			return prefix + "img_Reiner_SCT_cyberjack_RFID_standard.png";

		case ReaderType::REINER_cyberJack_RFID_basis:
			return prefix + "img_Reiner_SCT_cyberjack_RFID_basis.png";

		case ReaderType::REINER_cyberJack_wave:
			return prefix + "img_cyberjack_wave.png";

		case ReaderType::SCM_SCL011_Contactless_Reader:
			return prefix + "img_Identive_SCL011.png";

		case ReaderType::SCM_SDI010:
		case ReaderType::SCM_SDI011:
			return prefix + "img_Identive_SDI011.png";

		case ReaderType::KOBIL_IDToken:
			return prefix + "img_KOBIL_ID_Token.png";

		case ReaderType::ACS_ACR1281_PICC_Reader:
			return prefix + "img_ACS_ACR1281U.png";

		case ReaderType::OMNIKEY_CardMan_5x21_CL:
			return prefix + "img_HID_Global_OMNIKEY_5321_V2.png";

		case ReaderType::OMNIKEY_4121_CL:
			return prefix + "img_HID_Omnikey_Mobile_Reader_4121_CL.png";

		case ReaderType::FEIG_OBID_myAXXESS_basic:
			return prefix + "img_FEIG_myAXXES_basic.png";

		case ReaderType::Gemalto_Prox_SU:
			return prefix + "img_Gemalto_Prox_SU.png";

		case ReaderType::Gemalto_Prox_DU:
			return prefix + "img_Gemalto_Prox_DU.png";

		case ReaderType::UNKNOWN:
			return prefix + "default_no_card_found.png";
	}
	return QString();
}


QString SmartCardUtil::getCardIconPath(CardType pCardType)
{
	switch (pCardType)
	{
		case CardType::NONE:
		case CardType::UNKNOWN:
			return prefix + "blank_card.096.png";

		case CardType::EID_CARD:
			return prefix + "card.npa.096.png";
	}
	return QString();
}
