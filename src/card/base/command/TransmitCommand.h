/*!
 * \brief Command to transmit data to/from the card.
 *
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "BaseCardCommand.h"
#include "InputAPDUInfo.h"

namespace governikus
{

class CardConnection;

class TransmitCommand
	: public BaseCardCommand
{
	Q_OBJECT

	private:
		const QVector<InputAPDUInfo> mInputApduInfos;
		const QString mReaderName;
		QByteArrayList mOutputApduAsHex;

	protected:
		virtual void internalExecute() override;
		virtual ~TransmitCommand() override;

	public:
		TransmitCommand(QSharedPointer<CardConnectionWorker> pCardConnectionWorker,
				const QVector<InputAPDUInfo>& pInputApduInfos);

		const QByteArrayList& getOutputApduAsHex() const
		{
			return mOutputApduAsHex;
		}


		const QString& getReaderName() const
		{
			return mReaderName;
		}


};

} /* namespace governikus */
