/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "BaseCardCommand.h"
#include "CardConnectionWorker.h"
#include "InputAPDUInfo.h"


class test_TransmitCommand;


namespace governikus
{

class TransmitCommand
	: public BaseCardCommand
{
	Q_OBJECT
	friend class ::test_TransmitCommand;

	private:
		const QList<InputAPDUInfo> mInputApduInfos;
		const QString mSlotHandle;
		QByteArrayList mOutputApduAsHex;
		bool mSecureMessagingStopped;

	protected:
		void internalExecute() override;
		~TransmitCommand() override = default;

	public:
		explicit TransmitCommand(QSharedPointer<CardConnectionWorker> pCardConnectionWorker,
				const QList<InputAPDUInfo>& pInputApduInfos,
				const QString& pSlotHandle);

		static bool isAcceptable(const InputAPDUInfo& pInputApduInfo, const ResponseApdu& pResponse);

		[[nodiscard]] const QByteArrayList& getOutputApduAsHex() const
		{
			return mOutputApduAsHex;
		}


		[[nodiscard]] const QString& getSlotHandle() const
		{
			return mSlotHandle;
		}


		[[nodiscard]] bool getSecureMessagingStopped() const
		{
			return mSecureMessagingStopped;
		}


};

} // namespace governikus
