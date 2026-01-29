/**
 * Copyright (c) 2014-2026 Governikus GmbH & Co. KG, Germany
 */

#include "PcscReaderManagerPlugin.h"

#include <QLoggingCategory>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card_pcsc)


PcscReaderManagerPlugin::PcscReaderManagerPlugin()
	: ReaderManagerPlugin(ReaderManagerPluginType::PCSC, true)
	, mContextHandle(0)
	, mTimer()
	, mReaders()
{
	setObjectName(QStringLiteral("PcscReaderManager"));
	connect(&mTimer, &QTimer::timeout, this, &PcscReaderManagerPlugin::updateReaders);

#ifdef PCSCLITE_VERSION_NUMBER
	setPluginValue(ReaderManagerPluginInfo::Key::PCSC_LITE_VERSION, QStringLiteral(PCSCLITE_VERSION_NUMBER));
#endif
}


PcscReaderManagerPlugin::~PcscReaderManagerPlugin()
{
	Q_ASSERT(!mTimer.isActive());
	Q_ASSERT(mContextHandle == 0);

	while (!mReaders.isEmpty())
	{
		removeReader(std::as_const(mReaders).first()->getName());
	}
}


QPointer<Reader> PcscReaderManagerPlugin::getReader(const QString& pReaderName) const
{
	return mReaders.value(pReaderName).data();
}


void PcscReaderManagerPlugin::startScan(bool pAutoConnect)
{
	PCSC_RETURNCODE returnCode = SCardEstablishContext(SCARD_SCOPE_USER, nullptr, nullptr, &mContextHandle);
	setPluginEnabled(returnCode == pcsc::Scard_S_Success);
	qCDebug(card_pcsc) << "SCardEstablishContext:" << pcsc::toString(returnCode);
	if (!getInfo().isEnabled())
	{
		qCWarning(card_pcsc) << "Not started: Cannot establish context";
		setInitialScanState(ReaderManagerPluginInfo::InitialScan::FAILED);
		return;
	}

	mTimer.start(500);
	ReaderManagerPlugin::startScan(pAutoConnect);
}


void PcscReaderManagerPlugin::stopScan(const QString& pError)
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
	ReaderManagerPlugin::stopScan(pError);
}


void PcscReaderManagerPlugin::updateReaders()
{
	QStringList readersToAdd;
	PCSC_RETURNCODE returnCode = readReaderNames(readersToAdd);
	if (returnCode != pcsc::Scard_S_Success && returnCode != pcsc::Scard_E_No_Readers_Available)
	{
		qCWarning(card_pcsc) << "Cannot update readers, returnCode:" << returnCode;
		setInitialScanState(ReaderManagerPluginInfo::InitialScan::FAILED);

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
	erase_if(readersToAdd, [&readersToRemove](const auto& pReader){
				return readersToRemove.removeOne(pReader);
			});

	removeReaders(readersToRemove);
	addReaders(readersToAdd);
	setInitialScanState(ReaderManagerPluginInfo::InitialScan::SUCCEEDED);
}


QString PcscReaderManagerPlugin::extractReaderName(const PCSC_CHAR_PTR pReaderPointer) const
{
#if defined(Q_OS_WIN) && defined(UNICODE)
	return QString::fromWCharArray(pReaderPointer);

#else
	return QString::fromUtf8(pReaderPointer);

#endif
}


void PcscReaderManagerPlugin::addReaders(const QStringList& pReaderNames)
{
	for (const auto& readerName : pReaderNames)
	{
		const auto& reader = QSharedPointer<PcscReader>::create(readerName);
		if (reader->init() != pcsc::Scard_S_Success)
		{
			qCDebug(card_pcsc) << "Initialization of" << readerName << "failed";
			continue;
		}

		mReaders.insert(readerName, reader);

		connect(reader.data(), &Reader::fireCardInserted, this, &PcscReaderManagerPlugin::fireCardInserted);
		connect(reader.data(), &Reader::fireCardRemoved, this, &PcscReaderManagerPlugin::fireCardRemoved);
		connect(reader.data(), &Reader::fireCardInfoChanged, this, &PcscReaderManagerPlugin::fireCardInfoChanged);

		qCDebug(card_pcsc) << "fireReaderAdded:" << readerName << "(" << mReaders.size() << "reader in total )";
		Q_EMIT fireReaderAdded(reader->getReaderInfo());
	}
}


void PcscReaderManagerPlugin::removeReader(const QString& pReaderName)
{
	if (!mReaders.contains(pReaderName))
	{
		qCDebug(card_pcsc) << "Reader not present:" << pReaderName;
		Q_ASSERT(false);
	}

	const auto info = mReaders.take(pReaderName)->getReaderInfo();
	Q_EMIT fireReaderRemoved(info);
}


void PcscReaderManagerPlugin::removeReaders(const QStringList& pReaderNames)
{
	for (const auto& readerName : pReaderNames)
	{
		removeReader(readerName);
	}
}


PCSC_RETURNCODE PcscReaderManagerPlugin::readReaderNames(QStringList& pReaderNames) const
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


void PcscReaderManagerPlugin::shelveAll() const
{
	for (const auto& reader : mReaders)
	{
		shelve(reader.data());
	}
}
