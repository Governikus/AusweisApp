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
	required property int starCount
	required property real starDistance
	required property real starSize
	required property real starX
	required property real starY

	sourceSize.height: Style.dimens.header_icon_size
	tintColor: Style.color.image

	Repeater {
		model: root.starCount

		Image {
			property real size: starSize

			fillMode: Image.PreserveAspectFit
			height: parent.height * size
			source: "qrc:///animations/enter_password_star.svg"
			sourceSize.height: parent.height * starSize

			SequentialAnimation on size {
				loops: Animation.Infinite
				running: root.animated && SettingsModel.useAnimations

				PropertyAction {
					value: 0
				}
				PauseAnimation {
					id: startTime

					duration: 200 * (index + 1)
				}
				NumberAnimation {
					duration: 500
					easing.type: Easing.Linear
					from: 0
					to: starSize
				}
				PauseAnimation {
					duration: 2500 - startTime.duration
				}
				NumberAnimation {
					duration: 500
					easing.type: Easing.Linear
					from: starSize
					to: 0
				}
			}

			anchors {
				horizontalCenter: parent.left
				horizontalCenterOffset: parent.width * (starX + starDistance * index)
				verticalCenter: parent.top
				verticalCenterOffset: parent.height * starY
			}
		}
	}
}
