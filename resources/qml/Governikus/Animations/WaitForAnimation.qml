/**
 * Copyright (c) 2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.Global
import Governikus.Style
import Governikus.Type.SettingsModel

TintableIcon {
	id: root

	property bool animated: true
	required property real horizontalQuestionmarkOffset
	required property real verticalQuestionmarkOffset

	sourceSize.height: Style.dimens.header_icon_size
	tintColor: Style.color.image

	TintableIcon {
		property real size: 0.202

		source: "qrc:///animations/wait_for_questionmark.svg"
		sourceSize.height: parent.height * size
		tintColor: root.tintColor
		tintEnabled: root.tintEnabled

		SequentialAnimation on size {
			loops: Animation.Infinite
			running: root.animated && SettingsModel.useAnimations

			NumberAnimation {
				duration: 1000
				easing.type: Easing.InOutQuad
				from: 0.202
				to: 0.141
			}
			NumberAnimation {
				duration: 1000
				easing.type: Easing.InOutQuad
				from: 0.141
				to: 0.202
			}
		}

		anchors {
			horizontalCenter: parent.left
			horizontalCenterOffset: root.width * root.horizontalQuestionmarkOffset
			verticalCenter: parent.top
			verticalCenterOffset: root.height * root.verticalQuestionmarkOffset
		}
	}
}
