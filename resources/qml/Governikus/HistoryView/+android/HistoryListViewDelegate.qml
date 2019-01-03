import QtQuick 2.10

import Governikus.Global 1.0

Rectangle {
	property alias showDetail: contentItem.showDetail
	property alias listModel: contentItem.listModel

	HistoryListViewDelegateContent {
		id: contentItem
		height: parent.height - borderLine.height
		width: parent.width
		anchors.top: parent.top
	}
	Rectangle {
		id: borderLine
		color: "black"
		height: Utils.dp(1)
		width: parent.width
		anchors.top: contentItem.bottom
		anchors.bottom: parent.bottom
	}
}
