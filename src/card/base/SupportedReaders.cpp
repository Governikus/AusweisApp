/*!
 * SupportedReaders.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "SupportedReaders.h"

#include "SingletonHelper.h"

#define regex(X) QRegularExpression(QStringLiteral(X))

using namespace governikus;

defineSingleton(SupportedReaders)


SupportedReaders::SupportedReaders()
	: mWhiteList(
		{
			{ReaderType::REINER_cyberJack_RFID_komfort, regex("REINER SCT cyberJack RFID komfort")},
			{ReaderType::REINER_cyberJack_RFID_standard, regex("REINER SCT cyberJack RFID standard")},
			{ReaderType::REINER_cyberJack_RFID_basis, regex("REINER SCT cyberJack RFID basis")},
			{ReaderType::REINER_cyberJack_wave, regex("REINER SCT cyberJack wave")},
			{ReaderType::KOBIL_IDToken, regex("KOBIL (Systems )?IDToken")},
			{ReaderType::SCM_SDI010, regex("SDI010 (USB )?(Smart Card|Contactless) Reader")},
			{ReaderType::SCM_SDI011, regex("SDI011 (USB )?(Smart Card|Contactless) Reader")},
			{ReaderType::SCM_SCL011_Contactless_Reader, regex("(SCM Microsystems Inc. )?SCL011 Contactless Reader")},
			{ReaderType::ACS_ACR1281_PICC_Reader, regex("ACS ACR1281 PICC Reader")},
			{ReaderType::OMNIKEY_CardMan_5x21_CL, regex("OMNIKEY CardMan 5(x|3)21")},
			{ReaderType::OMNIKEY_4121_CL, regex("OMNIKEY 4121-CL")},
			{ReaderType::FEIG_OBID_myAXXESS_basic, regex("FEIG ELECTRONIC GmbH OBID myAXXESS basic")},
			{ReaderType::Gemalto_Prox_SU, regex("Gemalto Prox( |-)SU")},
			{ReaderType::Gemalto_Prox_DU, regex("Gemalto Prox(-DU| Dual)")}
		})
{
}


SupportedReaders::~SupportedReaders()
{
}


SupportedReaders& SupportedReaders::getInstance()
{
	return *Instance;
}


SupportedReaders::WhiteListMap::const_iterator SupportedReaders::getMapEntry(const QString& pReaderName) const
{
	for (auto iter = mWhiteList.constBegin(); iter != mWhiteList.constEnd(); ++iter)
	{
		if (pReaderName.contains(*iter))
		{
			return iter;
		}
	}

	return mWhiteList.constEnd();
}


ReaderType SupportedReaders::getReader(const QString& pReaderName) const
{
	auto iter = getMapEntry(pReaderName);
	if (iter == mWhiteList.constEnd())
	{
		return ReaderType::UNKNOWN;
	}

	return iter.key();
}


bool SupportedReaders::isOnWhiteList(const QString& pReaderName) const
{
	return getMapEntry(pReaderName) != mWhiteList.constEnd();
}


QString SupportedReaders::getPattern(ReaderType pReader) const
{
	if (mWhiteList.contains(pReader))
	{
		return mWhiteList.find(pReader)->pattern();
	}

	return QString();
}
