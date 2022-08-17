/*
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
*/

import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.HistoryModel 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.View 1.0

SectionPage {
	id: root

	ConfirmationPopup {
		id: deleteHistoryConfirmationPopup

		width: ApplicationModel.scaleFactor * 600

		//: INFO DESKTOP Header of the confirmation dialog to clear the entire authentication history.
		title: qsTr("Delete history?")
		//: INFO DESKTOP Content of the confirmation dialog to clear the entire authentication history.
		text: qsTr("All history entries will be deleted.")

		onConfirmed: {
			var timePeriod = removalPeriod.period
			var removedItemCount = SettingsModel.removeHistory(timePeriod)
			tabbedPane.currentIndex = tabbedPane.sectionCount > 0 ? 0 : -1
			//: INFO DESKTOP Feedback how many history entries were removed.
			ApplicationModel.showFeedback(qsTr("Deleted %1 entries from the history.").arg(removedItemCount))
		}

		HistoryRemovalTimePeriodControl {
			id: removalPeriod

			width: parent.width
		}
	}

	titleBarAction: TitleBarAction {
		//: LABEL DESKTOP
		text: qsTr("History")
		helpTopic: "history"
		customSubAction: SearchBar {
			id: searchBar

			visible: HistoryModel.rowCount() > 0

			anchors.verticalCenter: parent ? parent.verticalCenter : undefined

			onDisplayTextChanged: HistoryModel.searchFilter.setFilterString(displayText)

			//: LABEL DESKTOP
			placeholderText: qsTr("Search in history")
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
					Utils.isThisWeek(model.dateTime) ? model.dateTime.toLocaleString(Qt.locale(SettingsModel.language), "dddd") :
						model.dateTime.toLocaleString(Qt.locale(), qsTr("dd.MM.yyyy")) ) : "")
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

					icon.source: "qrc:///images/material_delete.svg"
					//: LABEL DESKTOP
					text: qsTr("Clear history")
					tintIcon: true
					onClicked: deleteHistoryConfirmationPopup.open()
				}

				GButton {
					id: saveHistoryToPdf

					Layout.fillWidth: true

					icon.source: "qrc:///images/desktop/material_save.svg"
					//: LABEL DESKTOP
					text: qsTr("Save as PDF...")
					tintIcon: true
					onClicked: {
						let now = new Date().toLocaleDateString(Qt.locale(), "yyyy-MM-dd")
						let filenameSuggestion = "%1.%2.%3.pdf".arg(Qt.application.name).arg(qsTr("History")).arg(now)
						fileDialog.selectFile(filenameSuggestion)
					}

					GFileDialog {
						id: fileDialog

						defaultSuffix: "pdf"
						//: LABEL DESKTOP
						nameFilters: qsTr("Portable Document Format (*.pdf)")

						onAccepted: HistoryModel.exportHistory(file)
					}
				}
			}
		}
	}

	GText {
		visible: tabbedPane.sectionCount === 0
		anchors.centerIn: parent

		activeFocusOnTab: true

		text: HistoryModel.rowCount() === 0
			   //: INFO DESKTOP No authentication history, placeholder text.
			   ? qsTr("Currently there are no history entries.")
			   //: INFO DESKTOP No authentication history entries match the search, placeholder text.
			   : qsTr("No history entries match your search term.")
		textStyle: Style.text.header

		FocusFrame {}
	}
}
