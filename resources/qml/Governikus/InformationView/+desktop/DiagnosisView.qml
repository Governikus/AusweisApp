/*
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Controls 2.12

import Governikus.View 1.0
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SelfDiagnosisModel 1.0

SectionPage {
	id: sectionPage

	anchors.centerIn: parent

	Accessible.name: qsTr("Diagnosis view")
	Accessible.description: qsTr("This is the diagnosis view of the AusweisApp2.")

	titleBarAction: TitleBarAction {
		//: LABEL DESKTOP_QML
		text: qsTr("Diagnosis")
		helpTopic: "diagnosis"
	}

	TabbedPane {
		id: sectionContent

		anchors.fill: parent
		anchors.margins: Constants.pane_padding

		sectionsModel: SelfDiagnosisModel.sectionsModel
		contentDelegate: sectionDelegate

		footerItem: footerDelegate
	}

	Component {
		id: sectionDelegate

		Column {
			height: implicitHeight

			spacing: Constants.pane_spacing

			Repeater {
				readonly property string currentSectionName: sectionContent.currentItemModel.display

				model: SelfDiagnosisModel.getSectionContentModel(currentSectionName)
				delegate: LabeledText {
					width: parent.width

					activeFocusOnTab: true

					label: title
					text: content
					labelStyle: (title !== "" && content === "") ? Style.text.header_accent : Style.text.normal_accent

					onActiveFocusChanged: {
						if (activeFocus) {
							if (focusFrameMargins < 0)
								sectionContent.scrollYPositionIntoView(y + height - focusFrameMargins)
							else
								sectionContent.scrollYPositionIntoView(y + height)
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

				anchors.fill: parent
				anchors.rightMargin: Constants.groupbox_spacing

				Accessible.description: qsTr("Save diagnosis to textfile")

				icon.source: "qrc:///images/desktop/material_save.svg"
				//: LABEL DESKTOP_QML
				text: qsTr("Save to file")
				tintIcon: true
				enableButton: !SelfDiagnosisModel.running || !timeout.running
				//: LABEL DESKTOP_QML
				disabledTooltipText: qsTr("Diagnosis is still running")
				//: LABEL DESKTOP_QML
				enabledTooltipText: SelfDiagnosisModel.running ? qsTr("Diagnosis may be incomplete") : ""
				onClicked: {
					var filenameSuggestion = "%1.%2.%3.txt".arg(Qt.application.name).arg(qsTr("Diagnosis")).arg(SelfDiagnosisModel.getCreationTimeString())
					appWindow.openSaveFileDialog(SelfDiagnosisModel.saveToFile, filenameSuggestion, qsTr("Textfiles"), "txt")
				}
			}

			Timer {
				id: timeout

				interval: 10000
				running: true
				repeat: false
			}
		}
	}

	onVisibleChanged: {
		if (visible) {
			SelfDiagnosisModel.startController()
			sectionContent.currentIndex = 0
		}
		else {
			SelfDiagnosisModel.stopController()
		}
	}
}
