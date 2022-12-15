/*
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.HistoryModel 1.0

SectionPage {
	id: rootPage

	property alias historyItemCount: listView.count
	property alias listViewModel: listView.model

	sectionPageFlickable: listView

	//: INFO ANDROID IOS
	title: qsTr("History")

	navigationAction: NavigationAction {
		action: enabled ? NavigationAction.Action.Back : NavigationAction.Action.None
		enabled: stackView && stackView.depth > 1

		onClicked: pop()
	}

	GText {
		anchors.centerIn: parent
		//: INFO ANDROID IOS No authentication history, placeholder text.
		text: qsTr("Currently there are no history entries.")
		textStyle: Style.text.normal_secondary
		visible: listView.count === 0
	}
	GListView {
		id: listView
		anchors.fill: parent
		model: HistoryModel

		delegate: HistoryListViewDelegate {
			historyModelItem: model
			showSeparator: index < listView.count - 1
			width: listView.width

			Accessible.onScrollDownAction: historyListView.positionViewAtIndex(index, ListView.Beginning)
			Accessible.onScrollUpAction: historyListView.positionViewAtIndex(index, ListView.End)
			onClicked: push(detailsHistoryView, {
					"historyModelItem": historyModelItem
				})
		}
		footer: Component {
			GSeparator {
				width: parent.width
			}
		}
		remove: Transition {
			NumberAnimation {
				duration: Constants.animation_duration / 2
				from: 1.0
				property: "opacity"
				to: 0
			}
		}
		removeDisplaced: Transition {
			SequentialAnimation {
				PauseAnimation {
					duration: Constants.animation_duration / 2
				}
				NumberAnimation {
					duration: Constants.animation_duration / 2
					properties: "y"
				}
			}
		}
	}
	Component {
		id: detailsHistoryView
		HistoryViewDetails {
		}
	}
}
