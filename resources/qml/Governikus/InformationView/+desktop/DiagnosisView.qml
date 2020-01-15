/*
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.View 1.0
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SelfDiagnosisModel 1.0
import Governikus.Type.SettingsModel 1.0


SectionPage {
	id: sectionPage

	anchors.centerIn: parent

	Accessible.name: qsTr("Diagnosis view") + SettingsModel.translationTrigger
	Accessible.description: qsTr("This is the diagnosis view of the AusweisApp2.") + SettingsModel.translationTrigger

	titleBarAction: TitleBarAction {
		//: LABEL DESKTOP_QML
		text: qsTr("Diagnosis") + SettingsModel.translationTrigger
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

				Accessible.description: qsTr("Save diagnosis to textfile") + SettingsModel.translationTrigger

				icon.source: "qrc:///images/icon_save.svg"
				//: LABEL DESKTOP_QML
				text: qsTr("Save to file") + SettingsModel.translationTrigger
				tintIcon: true
				onClicked: {
					var filenameSuggestion = "%1.%2.%3.txt".arg(Qt.application.name).arg(qsTr("Diagnosis")).arg(SelfDiagnosisModel.getCreationTimeString())
					appWindow.openSaveFileDialog(SelfDiagnosisModel.saveToFile, filenameSuggestion, "txt")
				}
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
