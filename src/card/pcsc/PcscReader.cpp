/*!
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#include "PcscCard.h"

#include <QLoggingCategory>
#include <QStringList>

using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card_pcsc)

PcscReader::PcscReader(const QString& pReaderName)
	: Reader(ReaderManagerPlugInType::PCSC, pReaderName)
	, mReaderState()
	, mReaderFeatures(nullptr)
	, mPaceCapabilities(nullptr)
	, mPcscCard()
	, mContextHandle(0)
{
	qCDebug(card_pcsc) << pReaderName;
	setObjectName(pReaderName);

	PCSC_RETURNCODE returnCode = SCardEstablishContext(SCARD_SCOPE_USER, nullptr, nullptr, &mContextHandle);
	qCDebug(card_pcsc) << "SCardEstablishContext:" << PcscUtils::toString(returnCode);
	if (returnCode != PcscUtils::Scard_S_Success)
	{
		qCWarning(card_pcsc) << "Cannot establish context";
		return;
	}

	memset(&mReaderState, 0, sizeof(SCARD_READERSTATE));

#if defined(Q_OS_WIN) && defined(UNICODE)
	wchar_t* name = new wchar_t[static_cast<size_t>(pReaderName.size()) + 1]();
	pReaderName.toWCharArray(name);
	mReaderState.szReader = name;
#else
	mReaderState.szReader = qstrdup(pReaderName.toUtf8().data());
#endif

	returnCode = readReaderFeaturesAndPACECapabilities();
	if (returnCode != PcscUtils::Scard_S_Success)
	{
		qCWarning(card_pcsc) << "Features / Capabilities not successful:" << returnCode;
		return;
	}

	// For PersoSim we need to check for FeatureID::EXECUTE_PACE
	// The correct check would be for PaceCapabilityId::EID
	// https://github.com/PersoSim/de.persosim.simulator/issues/89
	// mReaderInfo.setBasicReader(!mPaceCapabilities.contains(PaceCapabilityId::EID));
	mReaderInfo.setBasicReader(!hasFeature(FeatureID::EXECUTE_PACE));
	mReaderInfo.setConnected(true);

	update();
	mTimerId = startTimer(500);
}


PcscReader::~PcscReader()
{
	qCDebug(card_pcsc) << mReaderInfo.getName();
	qCDebug(card_pcsc) << "SCardCancel:        " << PcscUtils::toString(SCardCancel(mContextHandle));
	qCDebug(card_pcsc) << "SCardReleaseContext:" << PcscUtils::toString(SCardReleaseContext(mContextHandle));
	mContextHandle = 0;

	delete[] mReaderState.szReader;
}


SCARD_READERSTATE PcscReader::getState()
{
	return mReaderState;
}


bool PcscReader::hasFeature(FeatureID pFeatureID) const
{
	return mReaderFeatures.contains(pFeatureID);
}


PCSC_INT PcscReader::getFeatureValue(FeatureID pFeatureID)
{
	return mReaderFeatures.getValue(pFeatureID);
}


static QString SCARD_STATE_toString(DWORD i)
{
	QStringList sb(QStringLiteral("(%1)").arg(static_cast<ulong>(i)));

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


/*!
 * Currently not used, but needed when implementing signature PIN handling.
 */
bool PcscReader::hasPaceCapability(PaceCapabilityId pPaceCapability)
{
	return mPaceCapabilities.contains(pPaceCapability);
}


Reader::CardEvent PcscReader::updateCard()
{
	PCSC_RETURNCODE returnCode = SCardGetStatusChange(mContextHandle, 0, &mReaderState, 1);
	if (returnCode == PcscUtils::Scard_E_Timeout)
	{
		return Reader::CardEvent::NONE;
	}
	else if (returnCode == PcscUtils::Scard_E_Unknown_Reader)
	{
		qCWarning(card_pcsc) << "SCardGetStatusChange:" << PcscUtils::toString(returnCode);
		qCWarning(card_pcsc) << "Reader unknown, stop updating reader information";
		if (mTimerId != 0)
		{
			killTimer(mTimerId);
			mTimerId = 0;
		}
		return Reader::CardEvent::NONE;
	}
	else if (returnCode != PcscUtils::Scard_S_Success)
	{
		qCWarning(card_pcsc) << "SCardGetStatusChange:" << PcscUtils::toString(returnCode);
		qCWarning(card_pcsc) << "Cannot update reader";
		return Reader::CardEvent::NONE;
	}
	else if ((mReaderState.dwEventState & SCARD_STATE_CHANGED) == 0)
	{
		return Reader::CardEvent::NONE;
	}
	else if ((mReaderState.dwEventState & (SCARD_STATE_UNKNOWN | SCARD_STATE_UNAVAILABLE)) != 0)
	{
		return Reader::CardEvent::NONE;
	}

	qCDebug(card_pcsc) << "old state:" << SCARD_STATE_toString(mReaderState.dwCurrentState)
					   << "| new state:" << SCARD_STATE_toString(mReaderState.dwEventState);

	bool newPresent = (mReaderState.dwEventState & SCARD_STATE_PRESENT) == SCARD_STATE_PRESENT;
	bool newExclusive = (mReaderState.dwEventState & SCARD_STATE_EXCLUSIVE) == SCARD_STATE_EXCLUSIVE;

	mReaderState.dwCurrentState = mReaderState.dwEventState;

	if (newPresent)
	{
		if (!mPcscCard && !newExclusive)
		{
			static const int MAX_TRY_COUNT = 5;
			for (int tryCount = 0; tryCount < MAX_TRY_COUNT; ++tryCount)
			{
				mPcscCard.reset(new PcscCard(this));
				QSharedPointer<CardConnectionWorker> cardConnection = createCardConnectionWorker();
				CardInfoFactory::create(cardConnection, mReaderInfo);
				qCDebug(card_pcsc) << "Card detected:" << mReaderInfo.getCardInfo();

				if (mReaderInfo.hasEidCard() || mReaderInfo.hasPassport())
				{
					break;
				}

				qCDebug(card_pcsc) << "Unknown card detected, retrying.";
			}

			return CardEvent::CARD_INSERTED;
		}
	}
	else if (!mPcscCard.isNull())
	{
		mPcscCard.reset();
		mReaderInfo.setCardInfo(CardInfo(CardType::NONE));
		return CardEvent::CARD_REMOVED;
	}

	return CardEvent::NONE;
}


PCSC_RETURNCODE PcscReader::readReaderFeaturesAndPACECapabilities()
{
#if defined(PCSCLITE_VERSION_NUMBER)
	PCSC_INT PROTOCOL = SCARD_PROTOCOL_T0;
#else
	PCSC_INT PROTOCOL = 0;
#endif
	SCARDHANDLE cardHandle = 0;
	PCSC_INT protocol = 0;
	QString str =
			QStringLiteral("SCardConnect(%1, %2, %3, %4, %5, %6)").arg(mContextHandle, 0, 16).arg(mReaderInfo.getName()).arg(SCARD_SHARE_DIRECT)
			.arg(PROTOCOL).arg(cardHandle, 0, 16).arg(protocol);

	qCDebug(card_pcsc) << str;
	PCSC_RETURNCODE returnCode = SCardConnect(mContextHandle, mReaderState.szReader, SCARD_SHARE_DIRECT, PROTOCOL, &cardHandle, &protocol);
	qCDebug(card_pcsc) << "SCardConnect for" << mReaderInfo.getName() << ':' << PcscUtils::toString(returnCode);
	if (returnCode != PcscUtils::Scard_S_Success)
	{
		return returnCode;
	}

	// control (get features)
#if defined(PCSCLITE_VERSION_NUMBER)
	PCSC_INT CM_IOCTL_GET_FEATURE_REQUEST = 0x42000d48;
#else
	PCSC_INT CM_IOCTL_GET_FEATURE_REQUEST = 0x00313520;
#endif

	char buffer[1024];
	const uchar inBuffer1[2] = {
		0, 0
	};

	PCSC_INT clen = 0;
	returnCode = SCardControl(cardHandle, CM_IOCTL_GET_FEATURE_REQUEST, inBuffer1, 2, buffer, sizeof(buffer), &clen);
	qCDebug(card_pcsc) << "SCardControl for" << mReaderInfo.getName() << ':' << PcscUtils::toString(returnCode);

	if (returnCode != PcscUtils::Scard_S_Success)
	{
		clen = 0;
	}
	if (sizeof(buffer) < clen)
	{
		qCCritical(card_pcsc) << "Buffer size smaller than read length";
		Q_ASSERT(sizeof(buffer) >= clen);
		return PcscUtils::Scard_F_Unknown_Error;
	}
	if (clen > INT_MAX)
	{
		qCCritical(card_pcsc) << "Read length > INT_MAX not supported";
		Q_ASSERT(clen <= INT_MAX);
		return PcscUtils::Scard_F_Unknown_Error;
	}

	qCDebug(card_pcsc) << "FEATURES:" << QByteArray(buffer, static_cast<int>(clen)).toHex();
	mReaderFeatures = PcscReaderFeature(buffer, clen);
	qCDebug(card_pcsc) << "FEATURES:" << mReaderFeatures;

	if (mReaderFeatures.contains(FeatureID::EXECUTE_PACE))
	{
		PCSC_INT cmdID = mReaderFeatures.getValue(FeatureID::EXECUTE_PACE);

		clen = 0;

		const uchar inBuffer2[3] = {
			1, 0, 0
		};           // idx for GetReaderPACECapabilities (0x01), length (0, 0)
		qCDebug(card_pcsc) << "SCardControl ...";
		returnCode = SCardControl(cardHandle, cmdID, inBuffer2, 3, buffer, sizeof(buffer), &clen);
		qCDebug(card_pcsc) << "SCardControl for" << mReaderInfo.getName() << ':' << PcscUtils::toString(returnCode);

		if (returnCode != PcscUtils::Scard_S_Success)
		{
			clen = 0;
		}
		if (sizeof(buffer) < clen)
		{
			qCCritical(card_pcsc) << "Buffer size smaller than read length";
			Q_ASSERT(sizeof(buffer) >= clen);
			return PcscUtils::Scard_F_Unknown_Error;
		}
		if (clen > INT_MAX)
		{
			qCCritical(card_pcsc) << "Read length bigger than INT_MAX";
			Q_ASSERT(clen <= INT_MAX);
			return PcscUtils::Scard_F_Unknown_Error;
		}

		qCDebug(card_pcsc) << "PACE_CAPABILITIES:" << QByteArray(buffer, static_cast<int>(clen)).toHex();
		mPaceCapabilities = PcscReaderPaceCapability(buffer, clen);
		qCDebug(card_pcsc) << "PACE_CAPABILITIES:" << mPaceCapabilities;
	}
	else
	{
		clen = 0;
	}

	// disconnect
	returnCode = SCardDisconnect(cardHandle, SCARD_LEAVE_CARD);
	qCDebug(card_pcsc) << "SCardDisconnect for" << mReaderInfo.getName() << ':' << PcscUtils::toString(returnCode);
	return returnCode;
}


Card* PcscReader::getCard() const
{
	return mPcscCard.data();
}
