import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.Type.NumberModel 1.0


Item {
	id: baseItem
	property alias enableText: enableInfo.text
	property alias enableButtonText: enableButton.text
	property alias enableButtonVisible: enableButton.visible
	property alias titleText: title.text
	property alias subTitleText: subTitle.text

	signal enableClicked()

	Text {
		id: enableInfo
		anchors.bottom: enableButton.top
		anchors.bottomMargin: Constants.component_spacing
		anchors.left: parent.left
		anchors.right: parent.right
		verticalAlignment: Text.AlignVCenter
		horizontalAlignment: Text.AlignHCenter
		font.pixelSize: Constants.normal_font_size
		color: Constants.red
		wrapMode: Text.WordWrap
		visible: !!text

		Behavior on text {
			SequentialAnimation {
				PropertyAction { target: enableInfo; property: "opacity"; value: 0.0 }
				PropertyAction { target: enableInfo; property: "text" }
				PropertyAnimation { target: enableInfo; property: "opacity"; to: 1.0; duration: 500}
			}
		}
	}

	GButton {
		id: enableButton
		anchors.bottom: parent.bottom
		anchors.bottomMargin: Constants.component_spacing
		anchors.horizontalCenter: parent.horizontalCenter
		onClicked: parent.enableClicked()
	}

	Text {
		id: title
		anchors.bottom: parent.verticalCenter
		anchors.bottomMargin: Constants.component_spacing
		anchors.horizontalCenter: parent.horizontalCenter
		font.pixelSize: Constants.header_font_size
		font.weight: Font.Bold
		color: Constants.blue
		visible: !enableInfo.visible && !enableButton.visible

		Behavior on text {
			SequentialAnimation {
				NumberAnimation { target: rotation; property: "angle"; to: 90; duration: 500 }
				PropertyAction { target: title; property: "text" }
				NumberAnimation { target: rotation; property: "angle"; to: 0; duration: 500 }
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

	Text {
		id: subTitle
		anchors.left: parent.left
		anchors.top: title.bottom
		anchors.right: parent.right
		anchors.bottom: enableButton.top
		horizontalAlignment: Text.AlignHCenter
		verticalAlignment: Text.AlignTop
		font.pixelSize: Constants.normal_font_size
		color: Constants.secondary_text
		wrapMode: Text.WordWrap
		visible: !enableInfo.visible && !enableButton.visible


		Behavior on text {
			SequentialAnimation {
				PropertyAnimation { target: subTitle; property: "opacity"; to: 0; duration: 500}
				PropertyAction { target: subTitle; property: "text" }
				PropertyAnimation { target: subTitle; property: "opacity"; to: 1.0; duration: 500 }
				PropertyAction { target: subTitle; property: "color"; value: NumberModel.hasError ? Constants.red : Constants.secondary_text }
			}
		}
	}
}
