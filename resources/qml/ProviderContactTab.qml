import QtQuick 2.5
import QtQuick.Layouts 1.2

import "../"
import "global"

Item {
	id: baseItem
	property alias contactModel: repeater.model

	Pane {
		id: pane
		width: baseItem.width

		Repeater {
			id: repeater

			Item {
				height: info.height + Utils.dp(20)
				width: pane.width

				LabeledText {
					id: info
					label: model.label
					text: model.text
					anchors.verticalCenter: parent.verticalCenter
					width: parent.width
					margin: 0
					onLinkActivated: Qt.openUrlExternally(link)
				}
				Rectangle {
					visible: index != repeater.count - 1
					anchors.bottom: parent.bottom
					anchors.left: info.left
					anchors.right: parent.right
					height: 1
					color: Constants.grey
				}
			}
		}
	}
}
