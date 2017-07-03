import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import "../global"

Item {
	property bool showDetail: false

	property var listModel

	id: baseItem

	width: parent.width

	HistoryListViewDelegateContent {
		showDetail: baseItem.showDetail
		listModel: baseItem.listModel

		width: parent.width
		height: parent.height
	}
}
