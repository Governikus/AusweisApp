/*!
 * TransmitCommand.h
 *
 * \brief Command to transmit data to/from the card.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
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
		QByteArrayList mOutputApduAsHex;

	protected:
		virtual void internalExecute() override;
		virtual ~TransmitCommand();

	public:
		TransmitCommand(QSharedPointer<CardConnectionWorker> pCardConnectionWorker,
				const QVector<InputAPDUInfo>& pInputApduInfos);

		const QByteArrayList& getOutputApduAsHex() const
		{
			return mOutputApduAsHex;
		}


};

} /* namespace governikus */
