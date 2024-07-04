/**
 * Copyright (c) 2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.Global
import Governikus.Style
import Governikus.Type

TintableIcon {
	id: root

	enum Symbol {
		STAR,
		CHECK,
		WARNING,
		ERROR
	}

	property bool animated: true
	property int symbol: EnterPasswordAnimation.Symbol.STAR
	required property int symbolCount
	required property real symbolDistance
	required property real symbolSize
	required property real symbolX
	required property real symbolY

	sourceSize.height: Style.dimens.header_icon_size
	tintColor: Style.color.image

	Repeater {
		model: symbolCount

		TintableIcon {
			property real size: symbolSize

			height: parent.height * size
			source: {
				switch (symbol) {
				case EnterPasswordAnimation.Symbol.CHECK:
					return "qrc:///animations/enter_password_check.svg";
				case EnterPasswordAnimation.Symbol.WARNING:
					return "qrc:///animations/enter_password_warning.svg";
				case EnterPasswordAnimation.Symbol.ERROR:
					return "qrc:///animations/enter_password_error.svg";
				case EnterPasswordAnimation.Symbol.STAR:
					return "qrc:///animations/enter_password_star.svg";
				default:
					return "";
				}
			}
			sourceSize.height: parent.height * symbolSize
			tintColor: root.tintColor
			tintEnabled: symbol === EnterPasswordAnimation.Symbol.STAR

			SequentialAnimation on size {
				loops: Animation.Infinite
				running: animated && SettingsModel.useAnimations

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
					to: symbolSize
				}
				PauseAnimation {
					duration: 2500 - startTime.duration
				}
				NumberAnimation {
					duration: 500
					easing.type: Easing.Linear
					from: symbolSize
					to: 0
				}
			}

			anchors {
				horizontalCenter: parent.left
				horizontalCenterOffset: parent.width * (symbolX + symbolDistance * index)
				verticalCenter: parent.top
				verticalCenterOffset: parent.height * symbolY
			}
		}
	}
}
