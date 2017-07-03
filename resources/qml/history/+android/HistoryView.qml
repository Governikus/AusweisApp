import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.2

import "../"
import "../../"
import "../../global"

SectionPage {
	property alias listViewModel: listView.model
	property var selectedIndices: []

	headerTitleBarAction: TitleBarAction { text: qsTr("History"); font.bold: true }

	HistoryViewBackground {
		visible: listView.count !== 0
	}

	Text {
		anchors.centerIn: parent
		text: qsTr("Currently there are no history entries.")
		wrapMode: Text.WordWrap
		font.pixelSize: Utils.sp(18)
		visible: listView.count === 0
	}

	ListView {
		id: listView
		anchors.fill: parent
		anchors.topMargin: Utils.dp(5)

		model: historyModel
		spacing: Utils.dp(10)

		delegate:
			HistoryListViewDelegate {
				id: historyDelegate
				anchors.left: parent.left
				anchors.right: parent.right
				height: Utils.dp(120)
				property var modelItem: model
				showDetail: false
		}
	}

	HistoryViewPage {
		id: providerHistoryView
		visible: false
	}

	HistoryViewDetails {
		id: detailsHistoryView
		visible: false
	}
}
