/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

#include "PcscReaderManagerPlugIn.h"

#include "PcscReader.h"

#include <QLoggingCategory>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card_pcsc)


PcscReaderManagerPlugIn::PcscReaderManagerPlugIn()
	: ReaderManagerPlugIn(ReaderManagerPlugInType::PCSC, true)
	, mContextHandle(0)
	, mTimer()
	, mReaders()
{
	setObjectName(QStringLiteral("PcscReaderManager"));
	connect(&mTimer, &QTimer::timeout, this, &PcscReaderManagerPlugIn::updateReaders);

#ifdef PCSCLITE_VERSION_NUMBER
	setPlugInValue(ReaderManagerPlugInInfo::Key::PCSC_LITE_VERSION, QStringLiteral(PCSCLITE_VERSION_NUMBER));
#endif
}


PcscReaderManagerPlugIn::~PcscReaderManagerPlugIn()
{
	Q_ASSERT(!mTimer.isActive());
	Q_ASSERT(mContextHandle == 0);

	while (!mReaders.isEmpty())
	{
		removeReader(std::as_const(mReaders).first()->getName());
	}
}


QList<Reader*> PcscReaderManagerPlugIn::getReaders() const
{
	return mReaders.values();
}


void PcscReaderManagerPlugIn::startScan(bool pAutoConnect)
{
	PCSC_RETURNCODE returnCode = SCardEstablishContext(SCARD_SCOPE_USER, nullptr, nullptr, &mContextHandle);
	setPlugInEnabled(returnCode == pcsc::Scard_S_Success);
	qCDebug(card_pcsc) << "SCardEstablishContext:" << pcsc::toString(returnCode);
	if (!getInfo().isEnabled())
	{
		qCWarning(card_pcsc) << "Not started: Cannot establish context";
		return;
	}

	mTimer.start(500);
	ReaderManagerPlugIn::startScan(pAutoConnect);
}


void PcscReaderManagerPlugIn::stopScan(const QString& pError)
{
	mTimer.stop();

	if (mContextHandle)
	{
		PCSC_RETURNCODE returnCode = SCardReleaseContext(mContextHandle);
		qCDebug(card_pcsc) << "SCardReleaseContext:" << pcsc::toString(returnCode);
		mContextHandle = 0;
		if (returnCode != pcsc::Scard_S_Success)
		{
			qCWarning(card_pcsc) << "Error releasing context";
		}
	}
	removeReaders(mReaders.keys());
	ReaderManagerPlugIn::stopScan(pError);
}


void PcscReaderManagerPlugIn::updateReaders()
{
	QStringList readersToAdd;
	PCSC_RETURNCODE returnCode = readReaderNames(readersToAdd);
	if (returnCode != pcsc::Scard_S_Success && returnCode != pcsc::Scard_E_No_Readers_Available)
	{
		qCWarning(card_pcsc) << "Cannot update readers, returnCode:" << returnCode;

		if (returnCode == pcsc::Scard_E_No_Service && mTimer.isActive())
		{
			// Work around for an issue on Linux: Sometimes when unplugging a reader
			// the library seems to get confused and any further calls with existing
			// contexts fail with SCARD_E_NO_SERVICE. We try to restart the manager
			// in that case.
			qCDebug(card_pcsc) << "got SCARD_E_NO_SERVICE, trying to restart plugin";
			stopScan();
			startScan(true);
		}
		else if (returnCode == pcsc::Scard_E_Service_Stopped && mTimer.isActive())
		{
			// Work around for an issue on Windows 8.1: Sometimes when unplugging a reader
			// the library seems to get confused and any further calls with existing
			// contexts fail with SCARD_E_SERVICE_STOPPED. We try to restart the manager
			// in that case.
			qCDebug(card_pcsc) << "got SCARD_E_SERVICE_STOPPED, trying to restart plugin";
			stopScan();
			startScan(true);
		}
		else if (returnCode == pcsc::Scard_E_Invalid_Handle && mTimer.isActive())
		{
			// If the pc/sc daemon terminates on Linux, the handle is invalidated. We try
			// to restart the manager in this case.
			qCDebug(card_pcsc) << "got SCARD_E_INVALID_HANDLE, trying to restart plugin";
			stopScan();
			startScan(true);
		}
	}

	QStringList readersToRemove(mReaders.keys());
	for (QMutableListIterator it(readersToAdd); it.hasNext();)
	{
		QString readerName = it.next();
		if (readersToRemove.contains(readerName))
		{
			readersToRemove.removeOne(readerName);
			it.remove();
		}
	}

	removeReaders(readersToRemove);
	addReaders(readersToAdd);
}


QString PcscReaderManagerPlugIn::extractReaderName(const PCSC_CHAR_PTR pReaderPointer) const
{
#if defined(Q_OS_WIN) && defined(UNICODE)
	return QString::fromWCharArray(pReaderPointer);

#else
	return QString::fromUtf8(pReaderPointer);

#endif
}


void PcscReaderManagerPlugIn::addReaders(const QStringList& pReaderNames)
{
	for (const auto& readerName : pReaderNames)
	{
		auto pcscReader = std::make_unique<PcscReader>(readerName);
		if (pcscReader->init() != pcsc::Scard_S_Success)
		{
			qCDebug(card_pcsc) << "Initialization of" << readerName << "failed";
			continue;
		}

		Reader* reader = pcscReader.release();
		mReaders.insert(readerName, reader);

		connect(reader, &Reader::fireCardInserted, this, &PcscReaderManagerPlugIn::fireCardInserted);
		connect(reader, &Reader::fireCardRemoved, this, &PcscReaderManagerPlugIn::fireCardRemoved);
		connect(reader, &Reader::fireCardInfoChanged, this, &PcscReaderManagerPlugIn::fireCardInfoChanged);

		qCDebug(card_pcsc) << "fireReaderAdded:" << readerName << "(" << mReaders.size() << "reader in total )";
		Q_EMIT fireReaderAdded(reader->getReaderInfo());
	}
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

	Q_EMIT fireReaderRemoved(info);
}


void PcscReaderManagerPlugIn::removeReaders(const QStringList& pReaderNames)
{
	for (const auto& readerName : pReaderNames)
	{
		removeReader(readerName);
	}
}


PCSC_RETURNCODE PcscReaderManagerPlugIn::readReaderNames(QStringList& pReaderNames) const
{
	if (mContextHandle == 0)
	{
		return pcsc::Scard_E_Invalid_Handle;
	}

	QVarLengthArray<PCSC_CHAR, 8192> readers;
	auto maxReadersSize = static_cast<PCSC_INT>(readers.capacity());
	PCSC_RETURNCODE returnCode = SCardListReaders(mContextHandle, nullptr, readers.data(), &maxReadersSize);
	if (returnCode != pcsc::Scard_S_Success)
	{
		if (returnCode != pcsc::Scard_E_No_Readers_Available)
		{
			qCWarning(card_pcsc) << "SCardListReaders:" << pcsc::toString(returnCode);
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
