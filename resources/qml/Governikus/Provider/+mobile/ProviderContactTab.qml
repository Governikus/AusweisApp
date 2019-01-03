import QtQuick 2.10
import QtQuick.Layouts 1.2

import Governikus.Global 1.0

Item {
	id: baseItem
	property alias contactModel: infoList.model
	readonly property int contentHeight: infoList.contentHeight


	ListView {
		id: infoList
		anchors.fill: parent
		interactive: false

		delegate: Item {
			id: delegateItem
			width: parent.width
			height: Math.max(textItem.height, Utils.dp(50))

			Image {
				id: imageItem
				fillMode: Image.PreserveAspectFit
				height: Utils.dp(24)
				width: Utils.dp(24)
				anchors.left: parent.left
				anchors.leftMargin: Utils.dp(15)
				anchors.verticalCenter: parent.verticalCenter
				source: Qt.resolvedUrl(model.iconSource)
			}

			Text {
				id: textItem
				color: Constants.secondary_text
				text: !!model.text ? model.text : qsTr("Unknown") + settingsModel.translationTrigger
				verticalAlignment: Text.AlignVCenter
				anchors.left: imageItem.right
				anchors.leftMargin: Utils.dp(20)
				anchors.right: parent.right
				anchors.rightMargin: Utils.dp(10)
				anchors.verticalCenter: parent.verticalCenter
				font.pixelSize: Utils.dp(16)
				wrapMode: Text.WordWrap
				textFormat: Text.RichText
			}

			MouseArea {
				anchors.fill: delegateItem
				enabled: !!model.link
				onClicked: Qt.openUrlExternally(model.link)
			}

			Rectangle {
				anchors.bottom: parent.bottom
				anchors.left: parent.left
				anchors.right: parent.right
				height: 1
				color: Constants.grey
			}
		}
	}
}
