/*
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
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

		width: ApplicationModel.scaleFactor * 600

		//: INFO DESKTOP_QML Header of the confirmation dialog to clear the entire authentication history.
		title: qsTr("Delete history?") + SettingsModel.translationTrigger
		//: INFO DESKTOP_QML Content of the confirmation dialog to clear the entire authentication history.
		text: qsTr("All history entries will be deleted.") + SettingsModel.translationTrigger

		onConfirmed: {
			var timePeriod = removalPeriod.period
			var removedItemCount = SettingsModel.removeHistory(timePeriod)
			tabbedPane.currentIndex = tabbedPane.sectionCount > 0 ? 0 : -1
			//: INFO DESKTOP_QML Feedback how many history entries were removed.
			ApplicationModel.showFeedback(qsTr("Deleted %1 entries from the history.").arg(removedItemCount))
		}

		HistoryRemovalTimePeriodControl {
			id: removalPeriod

			width: parent.width
		}
	}

	titleBarAction: TitleBarAction {
		//: LABEL DESKTOP_QML
		text: qsTr("History") + SettingsModel.translationTrigger
		helpTopic: "history"
		customSubAction: SearchBar {
			id: searchBar

			anchors.verticalCenter: parent ? parent.verticalCenter : undefined

			onDisplayTextChanged: HistoryModel.searchFilter.setFilterString(displayText)

			//: LABEL DESKTOP_QML
			placeholderText: qsTr("Search in history") + SettingsModel.translationTrigger
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
			sectionName: model ? model.subject : ""
			footerText: model ? model.purpose : ""
			iconPath: model ? model.providerIcon : ""
		}

		contentDelegate: HistoryViewDetails {
			historyModelItem: tabbedPane.currentItemModel ? tabbedPane.currentItemModel.model : undefined

			activeFocusOnTab: true
		}

		footerItem: Item {
			height: buttonLayout.implicitHeight

			ColumnLayout {
				id: buttonLayout

				anchors.fill: parent
				anchors.rightMargin: Constants.groupbox_spacing
				spacing: Constants.groupbox_spacing

				GButton {
					id: clearHistoryButton

					Layout.fillWidth: true

					icon.source: "qrc:///images/trash_icon_white.svg"
					//: LABEL DESKTOP_QML
					text: qsTr("Clear history") + SettingsModel.translationTrigger
					tintIcon: true
					onClicked: deleteHistoryConfirmationPopup.open()
				}

				GButton {
					id: saveHistoryToPdf

					Layout.fillWidth: true

					icon.source: "qrc:///images/icon_save.svg"
					//: LABEL DESKTOP_QML
					text: qsTr("Save as PDF...") + SettingsModel.translationTrigger
					tintIcon: true
					onClicked: {
						let now = new Date().toLocaleDateString(Qt.locale(), "yyyy-MM-dd")
						let filenameSuggestion = "%1.%2.%3.pdf".arg(Qt.application.name).arg(qsTr("History")).arg(now)
						appWindow.openSaveFileDialog(HistoryModel.exportHistory, filenameSuggestion, "pdf")
					}
				}
			}
		}
	}

	GText {
		visible: tabbedPane.sectionCount === 0
		anchors.centerIn: parent

		activeFocusOnTab: true

		//: INFO DESKTOP_QML No authentication history, placeholder text.
		text: qsTr("Currently there are no history entries.") + SettingsModel.translationTrigger
		textStyle: Style.text.header_inverse
	}
}
