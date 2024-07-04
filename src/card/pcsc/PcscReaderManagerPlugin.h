/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Implementation of \ref ReaderManagerPlugin for PCSC.
 */

#pragma once

#include "PcscUtils.h"
#include "Reader.h"
#include "ReaderManagerPlugin.h"

#include <QMap>
#include <QStringList>
#include <QTimer>


class test_PcscReaderManagerPlugin;


namespace governikus
{

class PcscReaderManagerPlugin
	: public ReaderManagerPlugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "governikus.ReaderManagerPlugin" FILE "metadata.json")
	Q_INTERFACES(governikus::ReaderManagerPlugin)
	friend class ::test_PcscReaderManagerPlugin;

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
		PcscReaderManagerPlugin();
		~PcscReaderManagerPlugin() override;

		[[nodiscard]] QList<Reader*> getReaders() const override;

		void startScan(bool pAutoConnect) override;
		void stopScan(const QString& pError = QString()) override;
};

} // namespace governikus
