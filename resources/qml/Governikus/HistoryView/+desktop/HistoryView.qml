/**
 * Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
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
	titleBarAction: TitleBarAction {
		helpTopic: "history"
		//: LABEL DESKTOP
		text: qsTr("History")

		customSubAction: SearchBar {
			id: searchBar
			anchors.verticalCenter: parent ? parent.verticalCenter : undefined

			//: LABEL DESKTOP
			placeholderText: qsTr("Search in history")
			visible: HistoryModel.rowCount() > 0

			onDisplayTextChanged: HistoryModel.searchFilter.setFilterString(displayText)
		}
	}

	onVisibleChanged: if (!visible)
		deleteHistoryConfirmationPopup.close()

	ConfirmationPopup {
		id: deleteHistoryConfirmationPopup
		//: INFO DESKTOP Content of the confirmation dialog to clear the entire authentication history.
		text: qsTr("All history entries will be deleted.")

		//: INFO DESKTOP Header of the confirmation dialog to clear the entire authentication history.
		title: qsTr("Delete history?")
		width: ApplicationModel.scaleFactor * 600

		onConfirmed: {
			var timePeriod = removalPeriod.period;
			var removedItemCount = SettingsModel.removeHistory(timePeriod);
			tabbedPane.currentIndex = tabbedPane.sectionCount > 0 ? 0 : -1;
			//: INFO DESKTOP Feedback how many history entries were removed.
			ApplicationModel.showFeedback(qsTr("Deleted %1 entries from the history.").arg(removedItemCount));
		}

		HistoryRemovalTimePeriodControl {
			id: removalPeriod
			width: parent.width
		}
	}
	TabbedPane {
		id: tabbedPane
		anchors.fill: parent
		anchors.margins: Constants.pane_padding
		sectionsModel: HistoryModel.searchFilter
		visible: sectionCount > 0

		contentDelegate: HistoryViewDetails {
			activeFocusOnTab: true
			historyModelItem: tabbedPane.currentItemModel ? tabbedPane.currentItemModel.model : undefined
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
						let now = new Date().toLocaleDateString(Qt.locale(), "yyyy-MM-dd");
						let filenameSuggestion = "%1.%2.%3.pdf".arg(Qt.application.name).arg(qsTr("History")).arg(now);
						fileDialog.selectFile(filenameSuggestion);
					}

					GFileDialog {
						id: fileDialog
						defaultSuffix: "pdf"
						//: LABEL DESKTOP
						nameFilters: qsTr("Portable Document Format (*.pdf)")

						//: LABEL DESKTOP
						title: qsTr("Save history")

						onAccepted: HistoryModel.exportHistory(file)
					}
				}
			}
		}
		sectionDelegate: TabbedPaneDelegateIconAndThreeLineText {
			footerText: model ? model.purpose : ""
			headerText: model ? Utils.historyDateString(model.dateTime) : ""
			iconPath: model ? model.providerIcon : ""
			sectionName: model ? model.subject : ""
		}
	}
	GText {
		activeFocusOnTab: true
		anchors.centerIn: parent
		text: HistoryModel.rowCount() === 0 ?
		//: INFO DESKTOP No authentication history, placeholder text.
		qsTr("Currently there are no history entries.") :
		//: INFO DESKTOP No authentication history entries match the search, placeholder text.
		qsTr("No history entries match your search term.")
		textStyle: Style.text.header
		visible: tabbedPane.sectionCount === 0

		FocusFrame {
		}
	}
}
