/**
 * Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0

Item {
	id: baseItem

	property alias disabled: icon.tintEnabled

	height: Style.dimens.workflow_progress_indicator_size

	TintableIcon {
		id: icon
		anchors.centerIn: parent
		desaturate: true
		opacity: tintEnabled ? 0.7 : 1.0
		source: "qrc:///images/mobile/phone_smart.svg"
		sourceSize.height: Style.dimens.header_icon_size
	}
}
