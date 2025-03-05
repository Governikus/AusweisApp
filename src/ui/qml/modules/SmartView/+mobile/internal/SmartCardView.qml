/**
 * Copyright (c) 2021-2025 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.Animations
import Governikus.Global
import Governikus.Style
import Governikus.Type

Item {
	id: root

	readonly property real padding: Style.dimens.groupbox_spacing
	readonly property int smartState: SmartModel.state

	implicitHeight: cardIcon.implicitHeight + 2 * padding
	implicitWidth: cardIcon.implicitWidth + 2 * padding

	TintableIcon {
		id: cardIcon

		anchors.fill: parent
		anchors.margins: root.padding
		desaturate: true
		source: "qrc:///images/mobile/generic_id_card.svg"
		sourceSize.height: Style.dimens.header_icon_size - 2 * root.padding
		tintEnabled: root.smartState !== SmartModel.State.READY
	}
	HourglassAnimation {
		anchors.bottom: parent.bottom
		anchors.right: parent.right
		sourceSize.height: Style.dimens.medium_icon_size
		tintColor: Style.color.image
		visible: root.smartState === SmartModel.State.UPDATING_STATUS
	}
}
