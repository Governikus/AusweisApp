/**
 * Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style

Item {
	id: baseItem

	property alias icon: imageItem.source
	property alias iconTintColor: imageItem.tintColor
	property alias text: textItem.text

	signal clicked

	Accessible.focusable: true
	Accessible.ignored: imageItem.source == "" && textItem.text === ""
	Accessible.name: text
	Accessible.role: Accessible.Button
	implicitHeight: contentLayout.implicitHeight
	implicitWidth: contentLayout.implicitWidth

	Accessible.onPressAction: clicked()

	RowLayout {
		id: contentLayout

		anchors.fill: parent
		spacing: 0

		TintableIcon {
			id: imageItem

			Layout.preferredHeight: Layout.preferredWidth
			implicitWidth: Style.dimens.small_icon_size
			playAnimation: true
			sourceSize.width: width
			tintColor: textItem.color
			visible: imageItem.source != ""
		}
		GText {
			id: textItem

			Accessible.ignored: true
			color: Style.color.text
			elide: Text.ElideRight
			maximumLineCount: 1
			textStyle: Style.text.navigation
			visible: textItem.text !== ""
			wrapMode: Text.NoWrap

			Behavior on text {
				SequentialAnimation {
					PropertyAnimation {
						duration: Constants.animation_duration
						easing.type: Easing.InCubic
						property: "opacity"
						target: textItem
						to: 0
					}
					PropertyAction {
						property: "text"
						target: textItem
					}
					PropertyAnimation {
						duration: Constants.animation_duration
						easing.type: Easing.OutCubic
						property: "opacity"
						target: textItem
						to: 1
					}
				}
			}

			onVisibleChanged: {
				if (visible) {
					fadeIn.start();
				} else {
					fadeOut.start();
				}
			}
		}
		PropertyAnimation {
			id: fadeOut

			duration: Constants.animation_duration
			easing.type: Easing.InCubic
			property: "opacity"
			target: textItem
			to: 0
		}
		PropertyAnimation {
			id: fadeIn

			duration: Constants.animation_duration
			easing.type: Easing.OutCubic
			property: "opacity"
			target: textItem
			to: 1
		}
	}
	MouseArea {
		id: mouseArea

		anchors.fill: parent

		onClicked: baseItem.clicked()
	}
}
