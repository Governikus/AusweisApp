/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import Governikus.Global
import Governikus.Style
import Governikus.Type

TintableIcon {
	id: root

	property bool animated: true
	property int symbol: Symbol.Type.STAR
	required property int symbolCount
	required property real symbolDistance
	required property real symbolSize
	required property real symbolX
	required property real symbolY

	sourceSize.height: Style.dimens.header_icon_size
	tintColor: Style.color.image

	Repeater {
		model: root.symbolCount

		Symbol {
			id: symbolIcon

			required property int index
			property real size: root.symbolSize

			height: parent.height * size
			sourceSize.height: parent.height * root.symbolSize
			tintColor: root.tintColor
			tintEnabled: root.tintEnabled && (type === Symbol.Type.STAR || type === Symbol.Type.QUESTION)
			type: root.symbol

			SequentialAnimation on size {
				loops: Animation.Infinite
				running: root.animated && SettingsModel.useAnimations

				PropertyAction {
					value: 0
				}
				PauseAnimation {
					id: startTime

					duration: 200 * (symbolIcon.index + 1)
				}
				NumberAnimation {
					duration: 500
					easing.type: Easing.Linear
					from: 0
					to: root.symbolSize
				}
				PauseAnimation {
					duration: 2500 - startTime.duration
				}
				NumberAnimation {
					duration: 500
					easing.type: Easing.Linear
					from: root.symbolSize
					to: 0
				}
			}

			anchors {
				horizontalCenter: parent.left
				horizontalCenterOffset: parent.width * (root.symbolX + root.symbolDistance * index)
				verticalCenter: parent.top
				verticalCenterOffset: parent.height * root.symbolY
			}
		}
	}
}
