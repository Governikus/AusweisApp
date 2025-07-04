/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#include "PcscCard.h"

#include <QLoggingCategory>
#include <QScopeGuard>
#include <QStringList>

#include <array>

#if defined(Q_OS_MACOS)
	#define CM_IOCTL_GET_FEATURE_REQUEST (0x42000000 + 3400)
#elif defined(PCSCLITE_VERSION_NUMBER)
	#include <reader.h>
#else
//  PC/SC Part 10 v2.02.09 November 2012 - 2.2 GET_FEATURE_REQUEST
	#define CM_IOCTL_GET_FEATURE_REQUEST SCARD_CTL_CODE(3400)
#endif


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card_pcsc)

PcscReader::PcscReader(const QString& pReaderName)
	: Reader(ReaderManagerPluginType::PCSC, pReaderName)
	, mReaderState()
	, mReaderFeatures(nullptr)
	, mPcscCard()
	, mContextHandle(0)
{
	qCDebug(card_pcsc) << pReaderName;
	setObjectName(pReaderName);

	memset(&mReaderState, 0, sizeof(SCARD_READERSTATE));
#if defined(Q_OS_WIN) && defined(UNICODE)
	wchar_t* name = new wchar_t[static_cast<size_t>(pReaderName.size()) + 1]();
	pReaderName.toWCharArray(name);
	mReaderState.szReader = name;
#else
	mReaderState.szReader = qstrdup(pReaderName.toUtf8().data());
#endif
}


PCSC_RETURNCODE PcscReader::init()
{
	PCSC_RETURNCODE returnCode = SCardEstablishContext(SCARD_SCOPE_USER, nullptr, nullptr, &mContextHandle);
	qCDebug(card_pcsc) << "SCardEstablishContext:" << pcsc::toString(returnCode);
	if (returnCode != pcsc::Scard_S_Success)
	{
		qCWarning(card_pcsc) << "Cannot establish context";
		return returnCode;
	}

	returnCode = readReaderFeatures();
	if (returnCode != pcsc::Scard_S_Success)
	{
		qCWarning(card_pcsc) << "Features / Capabilities not successful:" << returnCode;
		return returnCode;
	}

	setInfoBasicReader(!hasFeature(FeatureID::EXECUTE_PACE));

	PcscReader::updateCard();
	setTimerId(startTimer(500));
	return pcsc::Scard_S_Success;
}


PcscReader::~PcscReader()
{
	qCDebug(card_pcsc) << getReaderInfo().getName();

	if (mContextHandle != 0)
	{
		qCDebug(card_pcsc) << "SCardCancel:        " << pcsc::toString(SCardCancel(mContextHandle));
		qCDebug(card_pcsc) << "SCardReleaseContext:" << pcsc::toString(SCardReleaseContext(mContextHandle));
		mContextHandle = 0;
	}

	delete[] mReaderState.szReader;
}


SCARD_READERSTATE PcscReader::getState() const
{
	return mReaderState;
}


bool PcscReader::hasFeature(FeatureID pFeatureID) const
{
	return mReaderFeatures.contains(pFeatureID);
}


PCSC_INT PcscReader::getFeatureValue(FeatureID pFeatureID) const
{
	return mReaderFeatures.getValue(pFeatureID);
}


static QString SCARD_STATE_toString(DWORD i)
{
	QStringList sb(QStringLiteral("(%1)").arg(i));

	if (i == SCARD_STATE_UNAWARE)
	{
		sb += QStringLiteral("UNAWARE");
	}
	else
	{
		if ((i & SCARD_STATE_IGNORE) != 0)
		{
			sb += QStringLiteral("IGNORE");
		}
		if ((i & SCARD_STATE_CHANGED) != 0)
		{
			sb += QStringLiteral("CHANGED");
		}
		if ((i & SCARD_STATE_UNKNOWN) != 0)
		{
			sb += QStringLiteral("UNKNOWN");
		}
		if ((i & SCARD_STATE_UNAVAILABLE) != 0)
		{
			sb += QStringLiteral("UNAVAILABLE");
		}
		if ((i & SCARD_STATE_EMPTY) != 0)
		{
			sb += QStringLiteral("EMPTY");
		}
		if ((i & SCARD_STATE_PRESENT) != 0)
		{
			sb += QStringLiteral("PRESENT");
		}
		if ((i & SCARD_STATE_ATRMATCH) != 0)
		{
			sb += QStringLiteral("ATRMATCH");
		}
		if ((i & SCARD_STATE_EXCLUSIVE) != 0)
		{
			sb += QStringLiteral("EXCLUSIVE");
		}
		if ((i & SCARD_STATE_INUSE) != 0)
		{
			sb += QStringLiteral("INUSE");
		}
		if ((i & SCARD_STATE_MUTE) != 0)
		{
			sb += QStringLiteral("MUTE");
		}
		if ((i & SCARD_STATE_UNPOWERED) != 0)
		{
			sb += QStringLiteral("UNPOWERED");
		}
	}

	return sb.join(QLatin1Char(' '));
}


void PcscReader::updateCard()
{
	if (!readCardStatus())
	{
		return;
	}

	if ((mReaderState.dwEventState & SCARD_STATE_CHANGED) == 0)
	{
		return;
	}

	if (mReaderState.dwEventState & (SCARD_STATE_UNKNOWN | SCARD_STATE_UNAVAILABLE))
	{
		return;
	}

	qCDebug(card_pcsc) << "old state:" << SCARD_STATE_toString(mReaderState.dwCurrentState)
					   << "| new state:" << SCARD_STATE_toString(mReaderState.dwEventState);
	mReaderState.dwCurrentState = mReaderState.dwEventState;

	if ((mReaderState.dwCurrentState & SCARD_STATE_PRESENT) == 0)
	{
		if (!mPcscCard.isNull())
		{
			mPcscCard.reset();
			removeCardInfo();

			qCInfo(card_pcsc) << "Card removed";
			Q_EMIT fireCardRemoved(getReaderInfo());
		}
		return;
	}

	if (mPcscCard || (mReaderState.dwCurrentState & SCARD_STATE_EXCLUSIVE))
	{
		return;
	}

	qCDebug(card_pcsc) << "ATR:" << QByteArray(reinterpret_cast<char*>(mReaderState.rgbAtr), static_cast<int>(mReaderState.cbAtr)).toHex(' ');

#ifdef Q_OS_MACOS
	for (int tryCount = 0; tryCount < 3; ++tryCount)
#endif
	{
		mPcscCard.reset(new PcscCard(this));
		fetchCardInfo();

#ifdef Q_OS_MACOS
		if (getReaderInfo().getCardType() == CardType::UNKNOWN)
		{
			qCDebug(card_pcsc) << "Unknown card detected, retrying";
			continue;
		}

		break;
#endif
	}

	qCInfo(card_pcsc) << "Card inserted:" << getReaderInfo().getCardInfo();
	Q_EMIT fireCardInserted(getReaderInfo());
}


void PcscReader::printGetReaderInfo() const
{
	if (mPcscCard.isNull())
	{
		return;
	}

	// see TR-03119 chapter A 1.3
	// These commands have to be implemented for all interfaces.
	// However, they only have to be provided when a smart card is available at the respective interface.
	// The implementation of this functionality without a card being available is optional.

	const QByteArrayList apdus({"FF9A010100", "FF9A010300", "FF9A010600", "FF9A010700"});

	QByteArrayList result;
	for (const auto& apdu : apdus)
	{
		const CommandApdu cmd(QByteArray::fromHex(apdu));
		const auto& responseResult = mPcscCard->transmit(cmd);
		if (responseResult.mReturnCode == CardReturnCode::OK)
		{
			result += responseResult.mResponseApdu.getData();
		}
	}

	if (!result.isEmpty())
	{
		qCDebug(card_pcsc) << "GetReaderInfo:" << result.join(QByteArrayLiteral(", "));
	}
}


PCSC_RETURNCODE PcscReader::readReaderFeatures()
{
#if defined(PCSCLITE_VERSION_NUMBER)
	PCSC_INT PROTOCOL = SCARD_PROTOCOL_T0;
#else
	PCSC_INT PROTOCOL = 0;
#endif

	SCARDHANDLE cardHandle = 0;
	PCSC_INT protocol = 0;
	const auto& readerName = getReaderInfo().getName();

	qCDebug(card_pcsc) << QStringLiteral("SCardConnect(%1, %2, %3, %4, %5, %6)").arg(mContextHandle, 0, 16).arg(readerName).arg(SCARD_SHARE_DIRECT)
		.arg(PROTOCOL).arg(cardHandle, 0, 16).arg(protocol);

	PCSC_RETURNCODE returnCode = SCardConnect(mContextHandle, mReaderState.szReader, SCARD_SHARE_DIRECT, PROTOCOL, &cardHandle, &protocol);
	qCDebug(card_pcsc) << "SCardConnect for" << readerName << ':' << pcsc::toString(returnCode);
	if (returnCode != pcsc::Scard_S_Success)
	{
		return returnCode;
	}

	const auto guard = qScopeGuard([cardHandle, readerName] {
				PCSC_RETURNCODE disconnectCode = SCardDisconnect(cardHandle, SCARD_LEAVE_CARD);
				qCDebug(card_pcsc) << "SCardDisconnect for" << readerName << ':' << pcsc::toString(disconnectCode);
			});

	QByteArray buffer(1024, '\0');
	const std::array<uchar, 2> inBuffer({0, 0});

	PCSC_INT length = 0;
	returnCode = SCardControl(cardHandle, CM_IOCTL_GET_FEATURE_REQUEST,
			inBuffer.data(), static_cast<int>(inBuffer.size()),
			buffer.data(), static_cast<DWORD>(buffer.size()),
			&length);
	buffer.resize(static_cast<int>(length));

	qCDebug(card_pcsc) << "SCardControl for" << readerName << ':' << pcsc::toString(returnCode);
	if (returnCode != pcsc::Scard_S_Success)
	{
		qCCritical(card_pcsc) << "Determining the reader features failed. Assuming this is a basic reader";
		return pcsc::Scard_S_Success;
	}

	qCDebug(card_pcsc) << "FEATURES:" << buffer.toHex();
	mReaderFeatures = PcscReaderFeature(buffer);
	qCDebug(card_pcsc) << "FEATURES:" << mReaderFeatures;

	return pcsc::Scard_S_Success;
}


bool PcscReader::readCardStatus()
{
	PCSC_RETURNCODE returnCode = SCardGetStatusChange(mContextHandle, 0, &mReaderState, 1);
	switch (returnCode)
	{
		case pcsc::Scard_S_Success:
			return true;

		case pcsc::Scard_E_Timeout:
			break;

		case pcsc::Scard_E_Unknown_Reader:
			qCWarning(card_pcsc) << "SCardGetStatusChange:" << pcsc::toString(returnCode);
			qCWarning(card_pcsc) << "Reader unknown, stop updating reader information";
			if (getTimerId() != 0)
			{
				killTimer(getTimerId());
				setTimerId(0);
			}
			break;

		default:
			qCWarning(card_pcsc) << "SCardGetStatusChange:" << pcsc::toString(returnCode);
			qCWarning(card_pcsc) << "Cannot update reader";
			break;
	}
	return false;
}


Card* PcscReader::getCard() const
{
	return mPcscCard.data();
}
