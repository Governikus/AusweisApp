/*
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.HistoryModel 1.0
import Governikus.Type.SettingsModel 1.0


SectionPage {
	id: rootPage
	property alias listViewModel: listView.model
	property alias historyItemCount: listView.count

	sectionPageFlickable: listView

	//: INFO ANDROID IOS
	title: qsTr("History") + SettingsModel.translationTrigger

	GText {
		anchors.centerIn: parent
		//: INFO ANDROID IOS No authentication history, placeholder text.
		text: qsTr("Currently there are no history entries.") + SettingsModel.translationTrigger
		textStyle: Style.text.normal_secondary
		visible: listView.count === 0
	}

	GListView {
		id: listView

		anchors.fill: parent

		model: HistoryModel

		remove: Transition {
			NumberAnimation { property: "opacity"; from: 1.0; to: 0; duration: Constants.animation_duration / 2 }
		}

		removeDisplaced: Transition {
			SequentialAnimation {
				PauseAnimation { duration: Constants.animation_duration / 2  }
				NumberAnimation { properties: "y"; duration: Constants.animation_duration / 2  }
			}
		}

		delegate: HistoryListViewDelegate {
			width: listView.width

			Accessible.onScrollDownAction: historyListView.positionViewAtIndex(index, ListView.Beginning)
			Accessible.onScrollUpAction: historyListView.positionViewAtIndex(index, ListView.End)

			historyModelItem: model
			showSeparator: index < listView.count - 1

			onClicked: firePushWithProperties(detailsHistoryView, {historyModelItem: historyModelItem})
		}

		footer: Component {
			GSeparator {
				width: parent.width
			}
		}
	}

	Component {
		id: detailsHistoryView

		HistoryViewDetails {}
	}
}
