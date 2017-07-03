/*!
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG
 */

#include "MetaTypeRegister.h"

#include "DatagramHandler.h"
#include "ReaderManagerWorker.h"

#include "command/BaseCardCommand.h"
#include "command/CreateCardConnectionCommand.h"

#if defined(Q_OS_LINUX) || defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
	#include "messages/BluetoothMessage.h"
#endif

#if defined(Q_OS_IOS)
#include <QBluetoothDeviceInfo>
#include <QLowEnergyCharacteristic>
#include <QLowEnergyDescriptor>
#endif

namespace governikus
{

namespace MetaTypes
{
void registerMetaTypes()
{
	DatagramHandler::registerMetaTypes();
	ReaderManagerWorker::registerMetaTypes();

	BaseCardCommand::registerMetaTypes();
	CreateCardConnectionCommand::registerMetaTypes();

#if defined(Q_OS_IOS)
	// Used in card/bluetooth/BluetoothReaderManagerPlugin_p_ios
	// TODO: These types should be registered by Qt but apparently they are not.
	qRegisterMetaType<QBluetoothDeviceInfo>("QBluetoothDeviceInfo");
	qRegisterMetaType<QLowEnergyCharacteristic>("QLowEnergyCharacteristic");
	qRegisterMetaType<QLowEnergyDescriptor>("QLowEnergyDescriptor");
#endif

#if defined(Q_OS_LINUX) || defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
	BluetoothMessage::registerMetaTypes();
#endif
}


}
}
