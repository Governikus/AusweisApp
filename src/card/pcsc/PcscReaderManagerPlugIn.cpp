/*!
 * PcscReaderManagerPlugIn.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "PcscReaderManagerPlugIn.h"

#include "PcscReader.h"
#include <QLoggingCategory>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card_pcsc)


PcscReaderManagerPlugIn::PcscReaderManagerPlugIn()
	: ReaderManagerPlugIn(ReaderManagerPlugInType::PCSC)
	, mContextHandle(0)
	, mReaderState()
	, mTimerId(0)
	, mReaders()
{
	setObjectName(QStringLiteral("PcscReaderManager"));

#ifdef PCSCLITE_VERSION_NUMBER
	setReaderInfoValue(ReaderManagerPlugInInfo::Key::PCSC_LITE_VERSION, QStringLiteral(PCSCLITE_VERSION_NUMBER));
#endif

	initReaderState();
}


PcscReaderManagerPlugIn::~PcscReaderManagerPlugIn()
{
	Q_ASSERT(mTimerId == 0);
	Q_ASSERT(mContextHandle == 0);

	while (!mReaders.isEmpty())
	{
		removeReader(mReaders.first()->getName());
	}
}


QList<Reader*> PcscReaderManagerPlugIn::getReader() const
{
	return mReaders.values();
}


void PcscReaderManagerPlugIn::init()
{
	ReaderManagerPlugIn::init();
	PCSC_RETURNCODE returnCode = SCardEstablishContext(SCARD_SCOPE_USER, nullptr, nullptr, &mContextHandle);
	setReaderInfoEnabled(returnCode == SCARD_S_SUCCESS);
	qCDebug(card_pcsc) << "SCardEstablishContext: " << PcscUtils::toString(returnCode);
	if (returnCode != SCARD_S_SUCCESS)
	{
		qCWarning(card_pcsc) << "Not started: Cannot establish context";
	}

	if (!mTimerId)
	{
		mTimerId = startTimer(500);
	}
}


void PcscReaderManagerPlugIn::shutdown()
{
	if (mTimerId)
	{
		killTimer(mTimerId);
		mTimerId = 0;
	}
	if (mContextHandle)
	{
		PCSC_RETURNCODE returnCode = SCardReleaseContext(mContextHandle);
		qCDebug(card_pcsc) << "SCardReleaseContext: " << PcscUtils::toString(returnCode);
		mContextHandle = 0;
		if (returnCode != SCARD_S_SUCCESS)
		{
			qCWarning(card_pcsc) << "Error releasing context";
		}
	}
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
	QStringList readersToRemove;
	QStringList readersToAdd;

	readersToRemove.reserve(mReaders.size());
	const auto& readerNames = mReaders.keys();
	for (const auto& readerName : readerNames)
	{
		readersToRemove += readerName;
	}

	PCSC_RETURNCODE returnCode = readReaderNames(readersToAdd);
	if (returnCode != SCARD_S_SUCCESS && returnCode != SCARD_E_NO_READERS_AVAILABLE)
	{
		qCWarning(card_pcsc) << "Cannot update readers";

		if (returnCode == SCARD_E_NO_SERVICE && mTimerId != 0)
		{
			// Work around for an issue on Linux: Sometimes when unplugging a reader
			// the library seems to get confused and any further calls with existing
			// contexts fail with SCARD_E_NO_SERVICE. We try to restart the manager
			// in that case.
			qCDebug(card_pcsc) << "got SCARD_E_NO_SERVICE, trying to restart plugin";
			shutdown();
			init();
		}
		else if (returnCode == SCARD_E_SERVICE_STOPPED && mTimerId != 0)
		{
			// Work around for an issue on Windows 8.1: Sometimes when unplugging a reader
			// the library seems to get confused and any further calls with existing
			// contexts fail with SCARD_E_SERVICE_STOPPED. We try to restart the manager
			// in that case.
			qCDebug(card_pcsc) << "got SCARD_E_SERVICE_STOPPED, trying to restart plugin";
			shutdown();
			init();
		}
		else if (returnCode == SCARD_E_INVALID_HANDLE && mTimerId != 0)
		{
			// If the pc/sc daemon terminates on Linux, the handle is invalidated. We try
			// to restart the manager in this case.
			qCDebug(card_pcsc) << "got SCARD_E_INVALID_HANDLE, trying to restart plugin";
			shutdown();
			init();
		}

	}

	for (QMutableListIterator<QString> it(readersToAdd); it.hasNext(); )
	{
		QString readerName = it.next();
		if (readersToRemove.contains(readerName))
		{
			readersToRemove.removeOne(readerName);
			it.remove();
		}
	}

	for (const auto& readerName : qAsConst(readersToRemove))
	{
		removeReader(readerName);
	}

	for (QMutableListIterator<QString> iterator(readersToAdd); iterator.hasNext(); )
	{
		QString readerName = iterator.next();
		Reader* reader = new PcscReader(readerName);
		mReaders.insert(readerName, reader);

		connect(reader, &Reader::fireCardInserted, this, &PcscReaderManagerPlugIn::fireCardInserted);
		connect(reader, &Reader::fireCardRemoved, this, &PcscReaderManagerPlugIn::fireCardRemoved);
		connect(reader, &Reader::fireCardRetryCounterChanged, this, &PcscReaderManagerPlugIn::fireCardRetryCounterChanged);

		qCDebug(card_pcsc) << "fireReaderAdded " << readerName;
		Q_EMIT fireReaderAdded(readerName);
	}
}


QString PcscReaderManagerPlugIn::extractReaderName(PCSC_CHAR_PTR pReaderPointer)
{
#if defined(Q_OS_WIN) && defined(UNICODE)
	return QString::fromWCharArray(pReaderPointer);

#else
	return QString::fromUtf8(pReaderPointer);

#endif
}


void PcscReaderManagerPlugIn::removeReader(const QString& pReaderName)
{
	delete mReaders.take(pReaderName);

	qCDebug(card_pcsc) << "fireReaderRemoved " << pReaderName;
	Q_EMIT fireReaderRemoved(pReaderName);
}


PCSC_RETURNCODE PcscReaderManagerPlugIn::readReaderNames(QStringList& pReaderNames)
{
	QVarLengthArray<PCSC_CHAR, 8192> readers;

	PCSC_INT maxReadersSize = readers.capacity();
	PCSC_RETURNCODE returnCode = SCardListReaders(mContextHandle, nullptr, readers.data(), &maxReadersSize);
	if (returnCode == SCARD_E_NO_READERS_AVAILABLE)
	{
		return returnCode;
	}
	else if (returnCode != SCARD_S_SUCCESS)
	{
		qCWarning(card_pcsc) << "SCardListReaders: " << PcscUtils::toString(returnCode);
		qCWarning(card_pcsc) << "Cannot read reader names";
		return returnCode;
	}

	PCSC_CHAR_PTR pReader = readers.data();
	PCSC_CHAR_PTR end = pReader + maxReadersSize - 1;
	while (pReader < end)
	{
		QString readerName = extractReaderName(pReader);
		pReaderNames.append(readerName);
		// Advance to the next value.
		pReader += readerName.size() + 1;
	}

	return returnCode;
}
