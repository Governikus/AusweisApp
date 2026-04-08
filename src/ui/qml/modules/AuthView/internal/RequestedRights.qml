/**
 * Copyright (c) 2016-2026 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import Governikus.Global
import Governikus.Style
import Governikus.Type

GPane {
	color: Style.color.paneSublevel.background.basic_unchecked
	drawShadow: false
	visible: writeData.count > 0 || optionalData.count > 0 || requiredData.count > 0

	DataGroup {
		id: writeData

		chat: ChatModel.write

		//: ALL_PLATFORMS
		title: qsTr("Write access (update)")
	}
	DataGroup {
		id: optionalData

		chat: ChatModel.optional

		//: ALL_PLATFORMS
		title: qsTr("Read access (optional)")
	}
	DataGroup {
		id: requiredData

		chat: ChatModel.required

		//: ALL_PLATFORMS
		title: qsTr("Read access")
	}
}
