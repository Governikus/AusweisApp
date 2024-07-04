/**
 * Copyright (c) 2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.Global
import Governikus.Style
import Governikus.Type

TintableIcon {
	id: root

	property bool animated: true
	required property real horizontalQuestionmarkOffset
	required property real verticalQuestionmarkOffset

	sourceSize.height: Style.dimens.header_icon_size
	tintColor: Style.color.image

	TintableIcon {
		id: innerIcon

		property real animatedSize
		readonly property real maxSize: 0.202
		readonly property real minSize: 0.141
		readonly property real size: animation.running ? animatedSize : maxSize

		source: "qrc:///animations/wait_for_questionmark.svg"
		sourceSize.height: parent.height * size
		tintColor: root.tintColor
		tintEnabled: root.tintEnabled

		SequentialAnimation on animatedSize {
			id: animation

			loops: Animation.Infinite
			running: root.animated && SettingsModel.useAnimations

			NumberAnimation {
				duration: 1000
				easing.type: Easing.InOutQuad
				from: innerIcon.maxSize
				to: innerIcon.minSize
			}
			NumberAnimation {
				duration: 1000
				easing.type: Easing.InOutQuad
				from: innerIcon.minSize
				to: innerIcon.maxSize
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
