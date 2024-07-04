/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Animations
import Governikus.Global
import Governikus.Style

ColumnLayout {
	id: baseItem

	property bool foundSelectedReader: false

	Layout.maximumHeight: Style.dimens.workflow_progress_indicator_size
	Layout.minimumHeight: Style.dimens.workflow_progress_indicator_size
	Layout.preferredHeight: Style.dimens.workflow_progress_indicator_size
	spacing: 0

	GSpacer {
		Layout.fillHeight: true
	}
	TintableIcon {
		id: phone

		Layout.alignment: Qt.AlignHCenter
		source: "qrc:///images/mobile/phone_remote_info.svg"
		sourceSize.height: Style.dimens.header_icon_size
		tintColor: Style.color.image
		visible: !baseItem.foundSelectedReader

		Image {
			id: x

			anchors.horizontalCenter: phone.right
			anchors.horizontalCenterOffset: -phone.width * 0.19
			anchors.verticalCenter: phone.bottom
			anchors.verticalCenterOffset: -phone.height * 0.36
			asynchronous: true
			fillMode: Image.PreserveAspectFit
			source: "qrc:///images/mobile/x.svg"
			sourceSize.height: phone.height * 0.2
		}
	}
	WaitForCardSacAnimation {
		Layout.alignment: Qt.AlignHCenter
		sourceSize.height: Style.dimens.medium_icon_size
		visible: baseItem.foundSelectedReader
	}
	GSpacer {
		Layout.fillHeight: true
	}
	ProgressCircle {
		id: pCircle

		Layout.alignment: Qt.AlignHCenter
		state: baseItem.foundSelectedReader ? "two" : "off"
		visible: baseItem.foundSelectedReader
	}
}
