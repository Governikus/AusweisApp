/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.View
import Governikus.Global
import Governikus.Style
import Governikus.TitleBar
import Governikus.Type.ApplicationModel
import Governikus.Type.DiagnosisModel

SectionPage {
	id: sectionPage

	anchors.centerIn: parent

	titleBarAction: TitleBarAction {
		//: LABEL DESKTOP
		text: qsTr("System data")
	}

	DiagnosisModel {
		id: diagnosisModel

	}
	TabbedPane {
		id: sectionContent

		anchors.fill: parent
		contentDelegate: diagnosisContentDelegate
		footerItem: footerDelegate
		sectionsModel: diagnosisModel
	}
	Component {
		id: diagnosisContentDelegate

		GPane {
			Column {
				Layout.fillWidth: true
				spacing: Constants.pane_spacing

				Repeater {
					model: sectionContent.currentItemModel.content

					delegate: LabeledText {
						activeFocusOnTab: true
						label: title
						labelStyle: (title !== "" && content === "") ? Style.text.headline : Style.text.subline
						text: content
						width: parent.width

						onActiveFocusChanged: {
							if (activeFocus) {
								if (focusFrameMargins < 0)
									sectionContent.scrollYPositionIntoView(y + height - focusFrameMargins);
								else
									sectionContent.scrollYPositionIntoView(y + height);
							}
						}
					}
				}
			}
		}
	}
	Component {
		id: footerDelegate

		Item {
			height: saveToFile.height

			GButton {
				id: saveToFile

				Accessible.description: qsTr("Save system data to textfile")
				anchors.fill: parent
				anchors.rightMargin: Constants.groupbox_spacing
				//: LABEL DESKTOP
				disabledTooltipText: qsTr("Diagnosis is still running")
				enableButton: !diagnosisModel.running || !timeout.running
				//: LABEL DESKTOP
				enabledTooltipText: diagnosisModel.running ? qsTr("Diagnosis may be incomplete") : ""
				icon.source: "qrc:///images/desktop/save_icon.svg"
				//: LABEL DESKTOP
				text: qsTr("Save to file")
				tintIcon: true

				onClicked: {
					let filenameSuggestion = "%1.%2.%3.txt".arg(Qt.application.name).arg(qsTr("SystemData")).arg(diagnosisModel.getCreationTime());
					fileDialog.selectFile(filenameSuggestion);
				}

				GFileDialog {
					id: fileDialog

					defaultSuffix: "txt"
					//: LABEL DESKTOP
					nameFilters: qsTr("Textfiles (*.txt)")

					//: LABEL DESKTOP
					title: qsTr("Save system data")

					onAccepted: diagnosisModel.saveToFile(file)
				}
			}
			Timer {
				id: timeout

				interval: 10000
				repeat: false
				running: true
			}
		}
	}
}
