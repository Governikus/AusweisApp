/*
 * \copyright Copyright (c) 2019 Governikus GmbH & Co. KG, Germany
*/

import QtQml 2.10
import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.HistoryModel 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.View 1.0

SectionPage {
	id: root

	Accessible.name: qsTr("History view") + SettingsModel.translationTrigger
	Accessible.description: qsTr("This is the history view of the AusweisApp2.") + SettingsModel.translationTrigger

	ConfirmationPopup {
		id: deleteHistoryConfirmationPopup

		//: INFO DESKTOP_QML Header of the confirmation dialog to clear the entire authentication history.
		title: qsTr("Delete history?") + SettingsModel.translationTrigger
		//: INFO DESKTOP_QML Content of the confirmation dialog to clear the entire authentication history.
		text: qsTr("Please confirm that you want to delete your history entries.") + SettingsModel.translationTrigger

		onConfirmed: {
			var timePeriod = removalPeriod.period
			var removedItemCount = SettingsModel.removeHistory(timePeriod)
			tabbedPane.currentIndex = tabbedPane.count > 0 ? 0 : -1
			//: INFO DESKTOP_QML Feedback how many history entries were removed.
			ApplicationModel.showFeedback(qsTr("Removed %1 entries from the history.").arg(removedItemCount))
		}

		HistoryRemovalTimePeriodControl {
			id: removalPeriod
			width: parent.width
		}
	}

	titleBarAction: TitleBarAction {
		//: LABEL DESKTOP_QML
		text: qsTr("History") + SettingsModel.translationTrigger
		customSubAction: SearchBar {
			id: searchBar

			onDisplayTextChanged: HistoryModel.searchFilter.setFilterString(displayText)
		}
	}

	onVisibleChanged: if (!visible) deleteHistoryConfirmationPopup.close()

	TabbedPane {
		id: tabbedPane

		visible: sectionCount > 0
		anchors.fill: parent
		anchors.margins: Constants.pane_padding

		sectionsModel: HistoryModel.searchFilter

		sectionDelegate: TabbedPaneDelegateIconAndThreeLineText {
			headerText: (model ? ( Utils.isToday(model.dateTime) ? qsTr("today") :
				Utils.isYesterday(model.dateTime) ? qsTr("yesterday") :
					Utils.isThisWeek(model.dateTime) ? model.dateTime.toLocaleString(Qt.locale(), qsTr("dddd")) :
						model.dateTime.toLocaleString(Qt.locale(), qsTr("dd.MM.yyyy")) ) : "") + SettingsModel.translationTrigger
			mainText: model ? model.subject : ""
			footerText: model ? model.purpose : ""
			iconPath: model ? model.providerIcon : ""
		}

		contentDelegate: HistoryViewDetails {
			historyModelItem: tabbedPane.currentItemModel ? tabbedPane.currentItemModel.model : undefined

			activeFocusOnTab: true
		}

		footerItem: Item {
			height: Math.max(clearHistoryButton.implicitHeight, saveHistoryToPdf.implicitHeight)

			GButton {
				id: clearHistoryButton

				anchors {
					top: parent.top
					bottom: parent.bottom
					left: parent.left
					right: parent.horizontalCenter
					rightMargin: (Constants.groupbox_spacing / 2)
				}

				Accessible.name: text
				activeFocusOnTab: true

				icon.source: "qrc:///images/trash_icon_white.svg"
				//: LABEL DESKTOP_QML
				text: qsTr("Clear history") + SettingsModel.translationTrigger
				onClicked: deleteHistoryConfirmationPopup.open()
			}

			GButton {
				id: saveHistoryToPdf

				anchors {
					top: parent.top
					bottom: parent.bottom
					left: parent.horizontalCenter
					right: parent.right
					rightMargin: Constants.groupbox_spacing
					leftMargin: Math.floor(Constants.groupbox_spacing / 2)
				}

				Accessible.name: text
				activeFocusOnTab: true

				icon.source: "qrc:///images/icon_save.svg"
				//: LABEL DESKTOP_QML
				text: qsTr("Save to pdf") + SettingsModel.translationTrigger
				onClicked: {
					var now = new Date().toLocaleDateString(Qt.locale(), "yyyy-MM-dd")
					var filenameSuggestion = "%1.%2.%3.pdf".arg(Qt.application.name).arg(qsTr("History")).arg(now)
					console.log("filenameSuggestion", filenameSuggestion)
					appWindow.openSaveFileDialog(HistoryModel.exportHistory, filenameSuggestion, "pdf")
				}
			}
		}
	}

	GText {
		id: textNoHistoryEntries

		visible: tabbedPane.sectionCount === 0
		anchors.centerIn: parent

		Accessible.name: text
		activeFocusOnTab: true

		//: INFO DESKTOP_QML No authentication history, placeholder text.
		text: qsTr("Currently there are no history entries.") + SettingsModel.translationTrigger
		textStyle: Style.text.header
	}
}
