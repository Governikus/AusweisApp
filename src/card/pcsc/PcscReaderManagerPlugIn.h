/*!
 * \brief Implementation of \ref ReaderManagerPlugIn for PCSC.
 *
 * \copyright Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "PcscUtils.h"
#include "Reader.h"
#include "ReaderManagerPlugIn.h"

#include <QMap>
#include <QStringList>
#include <QTimer>


namespace governikus
{

class PcscReaderManagerPlugIn
	: public ReaderManagerPlugIn
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "governikus.ReaderManagerPlugIn" FILE "metadata.json")
	Q_INTERFACES(governikus::ReaderManagerPlugIn)

	private:
		SCARDCONTEXT mContextHandle;
		QTimer mTimer;
		QMap<QString, Reader*> mReaders;

	private:
		PCSC_RETURNCODE readReaderNames(QStringList& pReaderNames);
		void updateReaders();
		inline QString extractReaderName(const PCSC_CHAR_PTR pReaderPointer) const;
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
