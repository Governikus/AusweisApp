/**
 * Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.Global
import Governikus.Style
import Governikus.Type.SmartModel

Item {
	id: root

	readonly property real padding: Constants.groupbox_spacing
	readonly property int smartState: SmartModel.smartState

	implicitHeight: cardIcon.implicitHeight + 2 * padding
	implicitWidth: cardIcon.implicitWidth + 2 * padding

	TintableIcon {
		id: cardIcon

		anchors.fill: parent
		anchors.margins: root.padding
		desaturate: true
		source: "qrc:///images/mobile/generic_id_card.svg"
		sourceSize.height: Style.dimens.header_icon_size - 2 * root.padding
		tintEnabled: root.smartState !== SmartModel.SMART_READY
	}
	TintableAnimation {
		anchors.bottom: parent.bottom
		anchors.right: parent.right
		height: Style.dimens.medium_icon_size
		source: "qrc:///images/sandglass.webp"
		tintColor: Style.color.control
		visible: root.smartState === SmartModel.SMART_UPDATING_STATUS
	}
}
