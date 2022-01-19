/*
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Layouts 1.12

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.NumberModel 1.0


Item {
	id: root

	property alias enableText: enableInfo.text
	property alias enableButtonText: enableButton.text
	property alias enableButtonVisible: enableButton.visible
	property alias titleText: title.text
	property alias subTitleText: subTitle.text

	signal enableClicked()
	signal enableLinkActivated()

	ColumnLayout {
		anchors.fill: parent

		spacing: 0

		GText {
			id: title

			visible: !enableInfo.visible && !enableButton.visible

			Layout.fillWidth: true

			Accessible.name: ApplicationModel.stripHtmlTags(text)

			horizontalAlignment: Text.AlignHCenter
			textStyle: Style.text.header_accent

			Behavior on text {
				SequentialAnimation {
					NumberAnimation { target: rotation; property: "angle"; to: 90; duration: Constants.animation_duration }
					PropertyAction { target: title; property: "text" }
					NumberAnimation { target: rotation; property: "angle"; to: 0; duration: Constants.animation_duration }
				}
			}

			transform: Rotation {
				id: rotation
				origin.x: 0
				origin.y: title.height/2
				axis.x: 1; axis.y: 0; axis.z: 0
				angle: 0
			}
		}

		GText {
			id: subTitle

			visible: !enableInfo.visible && !enableButton.visible

			Layout.fillWidth: true
			Layout.fillHeight: true
			Layout.topMargin: Constants.component_spacing

			Accessible.name: ApplicationModel.stripHtmlTags(text)

			horizontalAlignment: Text.AlignHCenter
			verticalAlignment: Text.AlignTop
			textStyle: Style.text.normal_secondary

			Behavior on text {
				SequentialAnimation {
					PropertyAnimation { target: subTitle; property: "opacity"; to: 0; duration: Constants.animation_duration }
					PropertyAction { target: subTitle; property: "text" }
					PropertyAnimation { target: subTitle; property: "opacity"; to: 1.0; duration: Constants.animation_duration }
					PropertyAction { target: subTitle; property: "color"; value: NumberModel.hasError ? Style.color.warning_text : Style.color.secondary_text }
				}
			}
		}

		GSpacer { Layout.fillHeight: true }

		GText {
			id: enableInfo

			visible: !!text

			Layout.fillWidth: true
			Layout.fillHeight: true
			Layout.bottomMargin: Constants.component_spacing

			Accessible.name: ApplicationModel.stripHtmlTags(text)

			verticalAlignment: Text.AlignBottom
			horizontalAlignment: Text.AlignHCenter
			textStyle: Style.text.normal_warning

			onLinkActivated: enableLinkActivated()

			Behavior on text {
				SequentialAnimation {
					PropertyAction { target: enableInfo; property: "opacity"; value: 0.0 }
					PropertyAction { target: enableInfo; property: "text" }
					PropertyAnimation { target: enableInfo; property: "opacity"; to: 1.0; duration: Constants.animation_duration }
				}
			}
		}

		GButton {
			id: enableButton

			Layout.alignment: Qt.AlignHCenter
			Layout.bottomMargin: Constants.component_spacing

			onClicked: enableClicked()
		}
	}
}
