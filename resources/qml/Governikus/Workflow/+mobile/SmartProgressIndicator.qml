/*
 * \copyright Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Global 1.0

Item {
	id: baseItem

	property alias disabled: icon.tintEnabled

	TintableIcon {
		id: icon
		anchors.centerIn: parent
		desaturate: true
		height: Math.ceil(parent.height * 0.25) * 2
		opacity: tintEnabled ? 0.7 : 1.0
		source: "qrc:///images/mobile/phone_smart.svg"
	}
}
