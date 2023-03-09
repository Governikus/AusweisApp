/**
 * Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import Governikus.View 1.0
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SelfDiagnosisModel 1.0

SectionPage {
	id: sectionPage
	anchors.centerIn: parent

	titleBarAction: TitleBarAction {
		helpTopic: "diagnosis"
		//: LABEL DESKTOP
		text: qsTr("Diagnosis")
	}

	Component.onCompleted: SelfDiagnosisModel.startController()
	Component.onDestruction: SelfDiagnosisModel.stopController()

	TabbedPane {
		id: sectionContent
		anchors.fill: parent
		anchors.margins: Constants.pane_padding
		contentDelegate: sectionDelegate
		footerItem: footerDelegate
		sectionsModel: SelfDiagnosisModel.sectionsModel
	}
	Component {
		id: sectionDelegate
		Column {
			height: implicitHeight
			spacing: Constants.pane_spacing

			Repeater {
				model: sectionContent.currentItemModel.content

				delegate: LabeledText {
					activeFocusOnTab: true
					label: title
					labelStyle: (title !== "" && content === "") ? Style.text.header_accent : Style.text.normal_accent
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
	Component {
		id: footerDelegate
		Item {
			height: saveToFile.height

			GButton {
				id: saveToFile
				Accessible.description: qsTr("Save diagnosis to textfile")
				anchors.fill: parent
				anchors.rightMargin: Constants.groupbox_spacing
				//: LABEL DESKTOP
				disabledTooltipText: qsTr("Diagnosis is still running")
				enableButton: !SelfDiagnosisModel.running || !timeout.running
				//: LABEL DESKTOP
				enabledTooltipText: SelfDiagnosisModel.running ? qsTr("Diagnosis may be incomplete") : ""
				icon.source: "qrc:///images/desktop/material_save.svg"
				//: LABEL DESKTOP
				text: qsTr("Save to file")
				tintIcon: true

				onClicked: {
					var filenameSuggestion = "%1.%2.%3.txt".arg(Qt.application.name).arg(qsTr("Diagnosis")).arg(SelfDiagnosisModel.getCreationTime());
					fileDialog.selectFile(filenameSuggestion);
				}

				GFileDialog {
					id: fileDialog
					defaultSuffix: "txt"
					//: LABEL DESKTOP
					nameFilters: qsTr("Textfiles (*.txt)")

					//: LABEL DESKTOP
					title: qsTr("Save diagnosis")

					onAccepted: SelfDiagnosisModel.saveToFile(file)
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
