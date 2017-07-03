/*!
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#include "NfcBridge.h"

#include "SingletonHelper.h"

#include <QLoggingCategory>

using namespace governikus;

defineSingleton(NfcBridge)

Q_DECLARE_LOGGING_CATEGORY(card_nfc)

#ifdef Q_OS_ANDROID
#include <QtAndroidExtras/QAndroidJniEnvironment>
#include <QtAndroidExtras/QtAndroid>
#endif


NfcBridge::NfcBridge()
#ifdef Q_OS_ANDROID
	: mJavaConnector(getJavaParent().callObjectMethod("getNfcConnector", "()Lcom/governikus/ausweisapp2/NFCConnector;"))
#endif
{
}


NfcBridge::~NfcBridge()
{
}


NfcBridge& NfcBridge::getInstance()
{
	return *Instance;
}


#ifdef Q_OS_ANDROID
QAndroidJniObject NfcBridge::getJavaParent()
{
	if (QtAndroid::androidActivity().isValid())
	{
		qCDebug(card_nfc) << "Parent is an activity.";
		return QtAndroid::androidActivity();
	}

	qCDebug(card_nfc) << "Parent is a service.";
	return QtAndroid::androidService();
}


QAndroidJniObject NfcBridge::getApplicationContext()
{
	QAndroidJniObject application = getJavaParent().callObjectMethod("getApplication", "()Landroid/app/Application;");
	if (application == nullptr)
	{
		qCCritical(card_nfc) << "Cannot get application";
		return nullptr;
	}

	return application.callObjectMethod("getApplicationContext", "()Landroid/content/Context;");
}


jbyteArray NfcBridge::convert(const QByteArray& pData)
{
	QAndroidJniEnvironment env;
	const int size = pData.size();
	const char* buffer = pData.constData();

	jbyteArray target = env->NewByteArray(size);
	jbyte* bytes = env->GetByteArrayElements(target, 0);
	for (int i = 0; i < size; ++i)
	{
		bytes[i] = buffer[i];
	}
	env->SetByteArrayRegion(target, 0, size, bytes);

	return target;
}


QByteArray NfcBridge::convert(const jbyteArray& pData)
{
	QAndroidJniEnvironment env;

	jsize size = env->GetArrayLength(pData);
	QVector<jbyte> buffer(size);
	env->GetByteArrayRegion(pData, 0, size, buffer.data());

	return QByteArray(reinterpret_cast<char*>(buffer.data()), buffer.size());
}


#endif

NfcCardCode NfcBridge::getCardStatus()
{
#ifdef Q_OS_ANDROID
	if (isValid())
	{
		return static_cast<NfcCardCode>(mJavaConnector.callMethod<jbyte>("isCardPresent"));
	}

#endif

	return NfcCardCode::NO_CARD;
}


ExtendedLengthApduSupportCode NfcBridge::getExtendedLengthApduSupportStatus()
{
#ifdef Q_OS_ANDROID
	if (isValid())
	{
		return static_cast<ExtendedLengthApduSupportCode>(mJavaConnector.callMethod<jbyte>("getExtendedLengthApduSupportStatus"));
	}

#endif

	return ExtendedLengthApduSupportCode::UNKNOWN;
}


bool NfcBridge::isNfcEnabled()
{
#ifdef Q_OS_ANDROID
	QAndroidJniObject context = getApplicationContext();
	if (context == nullptr)
	{
		qCCritical(card_nfc) << "Cannot get context";
		return false;
	}

	QAndroidJniObject nfcAdapter = QAndroidJniObject::callStaticObjectMethod("android/nfc/NfcAdapter", "getDefaultAdapter", "(Landroid/content/Context;)Landroid/nfc/NfcAdapter;", context.object());
	if (nfcAdapter == nullptr)
	{
		qCWarning(card_nfc) << "Cannot get default NfcAdapter";
		return false;
	}

	return static_cast<bool>(nfcAdapter.callMethod<jboolean>("isEnabled"));

#else
	return false;

#endif
}


bool NfcBridge::isValid()
{
#ifdef Q_OS_ANDROID
	return mJavaConnector != nullptr;

#else
	return false;

#endif
}


bool NfcBridge::start()
{
#ifdef Q_OS_ANDROID
	if (isValid())
	{
		return NfcConnectorCode::SUCCESS == static_cast<NfcConnectorCode>(mJavaConnector.callMethod<jbyte>("start"));
	}
#endif

	return false;
}


bool NfcBridge::stop()
{
#ifdef Q_OS_ANDROID
	if (isValid())
	{
		return NfcConnectorCode::SUCCESS == static_cast<NfcConnectorCode>(mJavaConnector.callMethod<jbyte>("stop"));
	}
#endif

	return false;
}


QByteArray NfcBridge::sendData(const QByteArray& pData)
{
	QByteArray ret;
#ifdef Q_OS_ANDROID
	if (isValid())
	{
		QAndroidJniEnvironment env;
		jbyteArray cmd = convert(pData);

		QAndroidJniObject arrayObj = mJavaConnector.callObjectMethod("sendData", "([B)[B", cmd);
		if (arrayObj == nullptr)
		{
			qCCritical(card_nfc) << "JavaConnector: sendData failed";
		}
		else
		{
			ret = convert(static_cast<jbyteArray>(arrayObj.object()));
		}

		env->DeleteLocalRef(cmd);
	}
#else
	Q_UNUSED(pData)
#endif

	return ret;
}


#include "moc_NfcBridge.cpp"
