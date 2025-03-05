/**
 * Copyright (c) 2021-2025 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.Global
import Governikus.Style

Item {
	id: root

	property alias disabled: icon.tintEnabled

	implicitHeight: Style.dimens.workflow_progress_indicator_size
	implicitWidth: icon.implicitWidth

	TintableIcon {
		id: icon

		anchors.centerIn: parent
		desaturate: true
		opacity: tintEnabled ? 0.7 : 1.0
		source: "qrc:///images/mobile/phone_smart.svg"
		sourceSize.height: Style.dimens.header_icon_size
	}
}
