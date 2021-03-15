/*!
 * \copyright Copyright (c) 2014-2021 Governikus GmbH & Co. KG, Germany
 */

#include "PcscReaderManagerPlugIn.h"

#include "PcscReader.h"

#include <QLoggingCategory>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card_pcsc)


PcscReaderManagerPlugIn::PcscReaderManagerPlugIn()
	: ReaderManagerPlugIn(ReaderManagerPlugInType::PCSC, true)
	, mContextHandle(0)
	, mReaderState()
	, mTimerId(0)
	, mReaders()
{
	setObjectName(QStringLiteral("PcscReaderManager"));

#ifdef PCSCLITE_VERSION_NUMBER
	setPlugInValue(ReaderManagerPlugInInfo::Key::PCSC_LITE_VERSION, QStringLiteral(PCSCLITE_VERSION_NUMBER));
#endif

	initReaderState();
}


PcscReaderManagerPlugIn::~PcscReaderManagerPlugIn()
{
	Q_ASSERT(mTimerId == 0);
	Q_ASSERT(mContextHandle == 0);

	while (!mReaders.isEmpty())
	{
		removeReader(qAsConst(mReaders).first()->getName());
	}
}


QList<Reader*> PcscReaderManagerPlugIn::getReaders() const
{
	return mReaders.values();
}


void PcscReaderManagerPlugIn::startScan(bool pAutoConnect)
{
	Q_UNUSED(pAutoConnect)

	PCSC_RETURNCODE returnCode = SCardEstablishContext(SCARD_SCOPE_USER, nullptr, nullptr, &mContextHandle);
	setPlugInEnabled(returnCode == PcscUtils::Scard_S_Success);
	qCDebug(card_pcsc) << "SCardEstablishContext:" << PcscUtils::toString(returnCode);
	if (returnCode != PcscUtils::Scard_S_Success)
	{
		qCWarning(card_pcsc) << "Not started: Cannot establish context";
	}

	if (mTimerId == 0)
	{
		mTimerId = startTimer(500);
	}
	ReaderManagerPlugIn::startScan(pAutoConnect);
}


void PcscReaderManagerPlugIn::stopScan(const QString& pError)
{
	if (mTimerId)
	{
		killTimer(mTimerId);
		mTimerId = 0;
	}
	if (mContextHandle)
	{
		PCSC_RETURNCODE returnCode = SCardReleaseContext(mContextHandle);
		qCDebug(card_pcsc) << "SCardReleaseContext:" << PcscUtils::toString(returnCode);
		mContextHandle = 0;
		if (returnCode != PcscUtils::Scard_S_Success)
		{
			qCWarning(card_pcsc) << "Error releasing context";
		}
	}
	removeReaders(mReaders.keys());
	ReaderManagerPlugIn::stopScan(pError);
}


void PcscReaderManagerPlugIn::timerEvent(QTimerEvent* pEvent)
{
	if (pEvent->timerId() == mTimerId)
	{
		updateReaders();
	}
}


void PcscReaderManagerPlugIn::initReaderState()
{
	memset(&mReaderState, 0, sizeof(SCARD_READERSTATE));
#if defined(Q_OS_WIN) && defined(UNICODE)
	mReaderState.szReader = L"\\\\?PnP?\\Notification";
#else
	mReaderState.szReader = "\\\\?PnP?\\Notification";
#endif
}


void PcscReaderManagerPlugIn::updateReaders()
{
	QStringList readersToAdd;
	PCSC_RETURNCODE returnCode = readReaderNames(readersToAdd);
	if (returnCode != PcscUtils::Scard_S_Success && returnCode != PcscUtils::Scard_E_No_Readers_Available)
	{
		qCWarning(card_pcsc) << "Cannot update readers, returnCode:" << returnCode;

		if (returnCode == PcscUtils::Scard_E_No_Service && mTimerId != 0)
		{
			// Work around for an issue on Linux: Sometimes when unplugging a reader
			// the library seems to get confused and any further calls with existing
			// contexts fail with SCARD_E_NO_SERVICE. We try to restart the manager
			// in that case.
			qCDebug(card_pcsc) << "got SCARD_E_NO_SERVICE, trying to restart plugin";
			stopScan();
			startScan(true);
		}
		else if (returnCode == PcscUtils::Scard_E_Service_Stopped && mTimerId != 0)
		{
			// Work around for an issue on Windows 8.1: Sometimes when unplugging a reader
			// the library seems to get confused and any further calls with existing
			// contexts fail with SCARD_E_SERVICE_STOPPED. We try to restart the manager
			// in that case.
			qCDebug(card_pcsc) << "got SCARD_E_SERVICE_STOPPED, trying to restart plugin";
			stopScan();
			startScan(true);
		}
		else if (returnCode == PcscUtils::Scard_E_Invalid_Handle && mTimerId != 0)
		{
			// If the pc/sc daemon terminates on Linux, the handle is invalidated. We try
			// to restart the manager in this case.
			qCDebug(card_pcsc) << "got SCARD_E_INVALID_HANDLE, trying to restart plugin";
			stopScan();
			startScan(true);
		}
	}

	QStringList readersToRemove(mReaders.keys());
	for (QMutableListIterator<QString> it(readersToAdd); it.hasNext();)
	{
		QString readerName = it.next();
		if (readersToRemove.contains(readerName))
		{
			readersToRemove.removeOne(readerName);
			it.remove();
		}
	}

	removeReaders(readersToRemove);

	for (QMutableListIterator<QString> iterator(readersToAdd); iterator.hasNext();)
	{
		QString readerName = iterator.next();
		Reader* reader = new PcscReader(readerName);
		mReaders.insert(readerName, reader);

		connect(reader, &Reader::fireCardInserted, this, &PcscReaderManagerPlugIn::fireCardInserted);
		connect(reader, &Reader::fireCardRemoved, this, &PcscReaderManagerPlugIn::fireCardRemoved);
		connect(reader, &Reader::fireCardRetryCounterChanged, this, &PcscReaderManagerPlugIn::fireCardRetryCounterChanged);

		qCDebug(card_pcsc) << "fireReaderAdded:" << readerName << "(" << mReaders.size() << "reader in total )";
		Q_EMIT fireReaderAdded(reader->getReaderInfo());
	}
}


QString PcscReaderManagerPlugIn::extractReaderName(PCSC_CHAR_PTR pReaderPointer) const
{
#if defined(Q_OS_WIN) && defined(UNICODE)
	return QString::fromWCharArray(pReaderPointer);

#else
	return QString::fromUtf8(pReaderPointer);

#endif
}


void PcscReaderManagerPlugIn::removeReader(const QString& pReaderName)
{
	if (!mReaders.contains(pReaderName))
	{
		qCDebug(card_pcsc) << "Reader not present:" << pReaderName;
		Q_ASSERT(false);
	}

	auto* reader = mReaders.take(pReaderName);
	auto info = reader->getReaderInfo();
	delete reader;
	info.setCardInfo(CardInfo(CardType::NONE));

	qCDebug(card_pcsc) << "fireReaderRemoved:" << pReaderName;
	Q_EMIT fireReaderRemoved(info);
}


void PcscReaderManagerPlugIn::removeReaders(const QStringList& pReaderNames)
{
	for (const auto& readerName : pReaderNames)
	{
		removeReader(readerName);
	}
}


PCSC_RETURNCODE PcscReaderManagerPlugIn::readReaderNames(QStringList& pReaderNames)
{
	if (mContextHandle == 0)
	{
		return PcscUtils::Scard_E_Invalid_Handle;
	}

	QVarLengthArray<PCSC_CHAR, 8192> readers;
	auto maxReadersSize = static_cast<PCSC_INT>(readers.capacity());
	PCSC_RETURNCODE returnCode = SCardListReaders(mContextHandle, nullptr, readers.data(), &maxReadersSize);
	if (returnCode != PcscUtils::Scard_S_Success)
	{
		if (returnCode != PcscUtils::Scard_E_No_Readers_Available)
		{
			qCWarning(card_pcsc) << "SCardListReaders:" << PcscUtils::toString(returnCode);
			qCWarning(card_pcsc) << "Cannot read reader names";
		}
		return returnCode;
	}

	PCSC_CHAR_PTR pReader = readers.data();
	const PCSC_CHAR_PTR end = pReader + maxReadersSize - 1;
	while (pReader < end)
	{
		QString readerName = extractReaderName(pReader);
		pReaderNames += readerName;
		// Advance to the next value.
		pReader += readerName.size() + 1;
	}

	return returnCode;
}
