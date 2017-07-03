/*!
 * \brief Helper for bridge between C++ and Java or something else.
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#pragma once

#include "EnumHelper.h"
#include "ReaderInfo.h"

#ifdef Q_OS_ANDROID
#include <QtAndroidExtras/QAndroidJniObject>
#endif

namespace governikus
{
defineEnumType(NfcCardCode,
		NO_CARD = 0,
		SAME_CARD = 1,
		NEW_CARD = 2)

defineTypedEnumType(NfcConnectorCode, signed char,
		SUCCESS = 0,
		ERROR_NO_NFC = -1,
		ERROR_NO_TAG = -2,
		ERROR_CONNECT = -3,
		ERROR_TAG_LOST = -4,
		ERROR_TRANSMIT = -5,
		ERROR_UNKNOWN = -16)

class NfcBridge
{
	private:
#ifdef Q_OS_ANDROID
		QAndroidJniObject mJavaConnector;

		static QAndroidJniObject getJavaParent();
		static QAndroidJniObject getApplicationContext();
		static QByteArray convert(const jbyteArray& pData);
		static jbyteArray convert(const QByteArray& pData);
#endif

		Q_DISABLE_COPY(NfcBridge)

	protected:
		NfcBridge();
		~NfcBridge();

	public:
		static NfcBridge& getInstance();

		bool isValid();
		bool isNfcEnabled();
		NfcCardCode getCardStatus();
		ExtendedLengthApduSupportCode getExtendedLengthApduSupportStatus();
		bool start();
		bool stop();
		QByteArray sendData(const QByteArray& pData);
};

} /* namespace governikus */
