/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Implementation of \ref ReaderManagerPlugIn for PCSC.
 */

#pragma once

#include "PcscUtils.h"
#include "Reader.h"
#include "ReaderManagerPlugIn.h"

#include <QMap>
#include <QStringList>
#include <QTimer>


class test_PcscReaderManagerPlugIn;


namespace governikus
{

class PcscReaderManagerPlugIn
	: public ReaderManagerPlugIn
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "governikus.ReaderManagerPlugIn" FILE "metadata.json")
	Q_INTERFACES(governikus::ReaderManagerPlugIn)
	friend class ::test_PcscReaderManagerPlugIn;

	private:
		SCARDCONTEXT mContextHandle;
		QTimer mTimer;
		QMap<QString, Reader*> mReaders;

	private:
		PCSC_RETURNCODE readReaderNames(QStringList& pReaderNames) const;
		void updateReaders();
		inline QString extractReaderName(const PCSC_CHAR_PTR pReaderPointer) const;
		void addReaders(const QStringList& pReaderNames);
		void removeReader(const QString& pReaderName);
		void removeReaders(const QStringList& pReaderNames);

	public:
		PcscReaderManagerPlugIn();
		~PcscReaderManagerPlugIn() override;

		[[nodiscard]] QList<Reader*> getReaders() const override;

		void startScan(bool pAutoConnect) override;
		void stopScan(const QString& pError = QString()) override;
};

} // namespace governikus
