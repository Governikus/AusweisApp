import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.0

import "../"
import "../global"

Item {
	id: baseItem

	property bool showDetail: false
	property var listModel

	width: parent.width

	Rectangle {
		id: root

		anchors.left: parent.left
		anchors.leftMargin: Utils.dp(10)
		anchors.right: parent.right
		anchors.rightMargin: Utils.dp(10)

		height: parent.height

		layer.enabled: true
		layer.effect: DropShadow {
						radius: 4
						samples: 8
						source: root
						color: Constants.grey
						verticalOffset: 2
					}

		HistoryListViewDelegateContent {
			showDetail: baseItem.showDetail
			listModel: baseItem.listModel

			width: parent.width
			height: parent.height
		}
	}
}
