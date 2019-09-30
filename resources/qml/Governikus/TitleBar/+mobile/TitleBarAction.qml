/*
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtGraphicalEffects 1.10

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.SettingsModel 1.0

Item {
	id: baseItem

	property string icon
	property string text
	property bool iconOnly: false
	signal clicked
	signal textUpdated

	height: Style.dimens.titlebar_height
	implicitWidth: imageItem.implicitWidth + textItem.implicitWidth
	width: imageItem.implicitWidth + (iconOnly ? 0 : textItem.implicitWidth)

	Accessible.role: Accessible.Button
	Accessible.name: text
	Accessible.onPressAction: if (Qt.platform.os === "ios") clicked()

	Image {
		id: imageItem

		sourceSize.width: source ? 18 : 0
		anchors {
			left: parent.left
			verticalCenter: parent.verticalCenter
		}

		fillMode: Image.PreserveAspectFit
		source: icon

		Behavior on source {
			SequentialAnimation {
				PropertyAnimation {
					target: imageItem
					property: "opacity"
					to: 0
					duration: Constants.animation_duration
					easing.type: Easing.InCubic
				}
				PropertyAction {
					target: imageItem
					property: "source"
				}
				PropertyAnimation {
					target: imageItem
					property: "opacity"
					to: 1
					duration: Constants.animation_duration
					easing.type: Easing.OutCubic
				}
			}
		}

		ColorOverlay {
			anchors.fill: imageItem
			source: imageItem
			color: Style.color.primary_text_inverse
		}
	}

	GText {
		id: textItem

		visible: !iconOnly
		anchors {
			left: imageItem.right
			verticalCenter: parent.verticalCenter
		}

		Accessible.ignored: true

		verticalAlignment: Text.AlignVCenter
		maximumLineCount: 1
		elide: Text.ElideRight
		wrapMode: Text.NoWrap
		textStyle: Style.text.normal_inverse
		text: baseItem.text

		onTextChanged: baseItem.textUpdated()

		onVisibleChanged: {
			if (visible) {
				fadeIn.start()
			} else {
				fadeOut.start()
			}
		}

		PropertyAnimation {
			id: fadeOut
			target: textItem
			property: "opacity"
			to: 0
			duration: Constants.animation_duration
			easing.type: Easing.InCubic
		}
		PropertyAnimation {
			id: fadeIn
			target: textItem
			property: "opacity"
			to: 1
			duration: Constants.animation_duration
			easing.type: Easing.OutCubic
		}

		Behavior on text {
			SequentialAnimation {
				PropertyAnimation {
					target: textItem
					property: "opacity"
					to: 0
					duration: Constants.animation_duration
					easing.type: Easing.InCubic
				}
				PropertyAction {
					target: textItem
					property: "text"
				}
				PropertyAnimation {
					target: textItem
					property: "opacity"
					to: 1
					duration: Constants.animation_duration
					easing.type: Easing.OutCubic
				}
			}
		}
	}

	MouseArea {
		id: mouseArea
		anchors.fill: parent
		onClicked: baseItem.clicked()
	}
}
