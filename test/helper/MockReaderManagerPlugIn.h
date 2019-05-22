/*!
 * \brief Mock implementation of ReaderManagerPlugIn
 *
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
 */


#include "MockReader.h"
#include "ReaderManagerPlugIn.h"

#include <QMap>
#include <QString>


namespace governikus
{

class MockReaderManagerPlugIn
	: public ReaderManagerPlugIn
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "governikus.ReaderManagerPlugIn" FILE "MockReaderManagerPlugIn.metadata.json")
	Q_INTERFACES(governikus::ReaderManagerPlugIn)

	private:
		static MockReaderManagerPlugIn* mInstance;
		QMap<QString, MockReader*> mReaders;

	public:
		MockReaderManagerPlugIn();
		virtual ~MockReaderManagerPlugIn() override;

		static MockReaderManagerPlugIn& getInstance();

		virtual QList<Reader*> getReaders() const override;
		MockReader* addReader(const QString& pReaderName);
		void removeReader(const QString& pReaderName);
		void removeAllReader();
};


} // namespace governikus
