/*
 * \copyright Copyright (c) 2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

import Governikus.FeedbackView 1.0
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0


SectionPage {
	id: sectionPage

	enum SubViews {
		None,
		Diagnosis,
		ApplicationLog,
		VersionInformation
	}

	readonly property int cellHeight: (height - 2 * gridLayout.anchors.margins) / 4
	readonly property int informationViewIconWidth: width / 6
	readonly property int separatorHeight: Math.max(1, ApplicationModel.scaleFactor * 4)
	property int activeSubView

	Accessible.name: qsTr("Help section") + SettingsModel.translationTrigger
	Accessible.description: qsTr("This is the help section of the AusweisApp2.") + SettingsModel.translationTrigger
	Keys.onEscapePressed: {
		if (activeSubView === InformationView.SubViews.None) {
			event.accepted = false
			return
		}

		activeSubView = InformationView.SubViews.None
	}

	isAbstract: activeSubView !== InformationView.SubViews.None

	titleBarAction: TitleBarAction {
		//: LABEL DESKTOP_QML
		text: qsTr("Help") + SettingsModel.translationTrigger
		onClicked: activeSubView = InformationView.SubViews.None
	}
	onVisibleChanged: activeSubView = InformationView.SubViews.None

	GridLayout {
		id: gridLayout

		readonly property int iconAlignment: Qt.AlignTop | Qt.AlignHCenter
		readonly property int longestText: Math.max(textQuestionsFeedbackRating.width,
													textSetupManual.width,
													textDiagnosisApplicationLog.width,
													textVersionAndLicense.width)

		visible: activeSubView === InformationView.SubViews.None
		anchors.fill: sectionPage
		anchors.margins: Constants.pane_padding
		anchors.topMargin: anchors.margins + (sectionPage.cellHeight - iconDiagnosis.implicitHeight) / 2

		rowSpacing: 0
		columnSpacing: 0
		columns: 4
		rows: 4
		onVisibleChanged: if (visible) sectionPage.setActive()

		Column {
			Layout.fillWidth: true
			Layout.fillHeight: true

			topPadding: (implicitHeight - textQuestionsFeedbackRating.height - separatorHeight) / 2
			spacing: Constants.component_spacing

			GText {
				id: textQuestionsFeedbackRating

				Accessible.name: text
				activeFocusOnTab: true

				//: LABEL DESKTOP_QML
				text: qsTr("Questions, feedback, and rating") + SettingsModel.translationTrigger
				textStyle: Style.text.header

				FocusFrame {}
			}
			Rectangle {
				height: sectionPage.separatorHeight
				width: gridLayout.longestText * 1.2

				color: Constants.white
				opacity: 0.5
			}
		}
		InformationViewIcon{
			id: iconQuestions

			width: sectionPage.informationViewIconWidth

			Layout.alignment: gridLayout.iconAlignment
			Layout.minimumHeight: sectionPage.cellHeight

			activeFocusOnTab: true

			source: "qrc:/images/desktop/info_questions.svg"
			//: LABEL DESKTOP_QML
			text: qsTr("Questions") + SettingsModel.translationTrigger
			onClicked: Qt.openUrlExternally(qsTr("https://www.ausweisapp.bund.de/en/qa/frequently-asked-questions/")) + SettingsModel.translationTrigger
		}
		InformationViewIcon {
			id: iconReportError

			width: sectionPage.informationViewIconWidth

			Layout.alignment: gridLayout.iconAlignment
			Layout.minimumHeight: sectionPage.cellHeight

			activeFocusOnTab: true

			source: "qrc:/images/desktop/info_report_error.svg"
			//: LABEL DESKTOP_QML
			text: qsTr("Report error") + SettingsModel.translationTrigger
			onClicked: Qt.openUrlExternally(qsTr("https://www.ausweisapp.bund.de/en/qa/report-an-error/")) + SettingsModel.translationTrigger
		}
		InformationViewIcon {
			id: iconRateApplication

			width: sectionPage.informationViewIconWidth

			Layout.alignment: gridLayout.iconAlignment
			Layout.minimumHeight: sectionPage.cellHeight

			activeFocusOnTab: true

			source: "qrc:/images/desktop/info_rate_application.svg"
			//: LABEL DESKTOP_QML
			text: qsTr("Rate application") + SettingsModel.translationTrigger
			onClicked: Qt.openUrlExternally(qsTr("https://www.ausweisapp.bund.de/en/qa/evaluate-us/")) + SettingsModel.translationTrigger
		}

		Column {
			Layout.fillHeight: true
			Layout.fillWidth: true

			topPadding: (implicitHeight - textSetupManual.height - separatorHeight) / 2
			spacing: Constants.component_spacing

			GText {
				id: textSetupManual

				Accessible.name: text
				activeFocusOnTab: true

				//: LABEL DESKTOP_QML
				text: qsTr("Setup and manual") + SettingsModel.translationTrigger
				textStyle: Style.text.header

				FocusFrame {}
			}
			Rectangle {
				height: sectionPage.separatorHeight
				width: gridLayout.longestText * 1.2

				color: Constants.white
				opacity: 0.5
			}
		}
		Item {
			id: placeHolder1

			width: 1
			height: 1
		}
		InformationViewIcon {
			id: iconSetup

			width: sectionPage.informationViewIconWidth

			Layout.alignment: gridLayout.iconAlignment
			Layout.minimumHeight: sectionPage.cellHeight

			activeFocusOnTab: true

			source: "qrc:/images/desktop/info_setup.svg"
			//: LABEL DESKTOP_QML
			text: qsTr("Setup") + SettingsModel.translationTrigger
			onClicked: sectionPage.nextView(SectionPage.Views.SetupAssistant)
		}
		InformationViewIcon {
			id: iconManual

			width: sectionPage.informationViewIconWidth

			Layout.minimumHeight: sectionPage.cellHeight
			Layout.alignment: gridLayout.iconAlignment

			activeFocusOnTab: true

			//: LABEL DESKTOP_QML
			text: qsTr("Online help") + SettingsModel.translationTrigger
			source: "qrc:/images/desktop/info_manual.svg"
			onClicked: ApplicationModel.openOnlineHelp("index")
		}

		Column {
			Layout.fillHeight: true
			Layout.fillWidth: true

			topPadding: (implicitHeight - textDiagnosisApplicationLog.height - separatorHeight) / 2
			spacing: Constants.component_spacing

			GText {
				id: textDiagnosisApplicationLog

				Accessible.name: text
				activeFocusOnTab: true

				//: LABEL DESKTOP_QML
				text: qsTr("Diagnosis and application log") + SettingsModel.translationTrigger
				textStyle: Style.text.header

				FocusFrame {}
			}
			Rectangle {
				height: sectionPage.separatorHeight
				width: gridLayout.longestText * 1.2

				color: Constants.white
				opacity: 0.5
			}
		}
		Item {
			id: placeHolder2

			width: 1
			height: 1
		}
		InformationViewIcon {
			id: iconDiagnosis

			width: sectionPage.informationViewIconWidth

			Layout.alignment: gridLayout.iconAlignment
			Layout.minimumHeight: sectionPage.cellHeight

			activeFocusOnTab: true

			//: LABEL DESKTOP_QML
			text: qsTr("Diagnosis") + SettingsModel.translationTrigger
			source: "qrc:/images/desktop/info_diagnosis.svg"
			onClicked: activeSubView = InformationView.SubViews.Diagnosis
		}
		InformationViewIcon {
			id: iconApplicationLog

			width: sectionPage.informationViewIconWidth

			Layout.alignment: gridLayout.iconAlignment
			Layout.minimumHeight: sectionPage.cellHeight

			activeFocusOnTab: true

			//: LABEL DESKTOP_QML
			text: qsTr("Application log") + SettingsModel.translationTrigger
			source: "qrc:/images/desktop/info_application_log.svg"
			onClicked: activeSubView = InformationView.SubViews.ApplicationLog
		}

		Column {
			Layout.fillHeight: true
			Layout.fillWidth: true

			topPadding: (implicitHeight - textVersionAndLicense.height - separatorHeight) / 2
			spacing: Constants.component_spacing

			GText {
				id: textVersionAndLicense

				Accessible.name: text
				activeFocusOnTab: true

				//: LABEL DESKTOP_QML
				text: qsTr("Version and license information") + SettingsModel.translationTrigger
				textStyle: Style.text.header

				FocusFrame {}
			}
			Rectangle {
				height: sectionPage.separatorHeight
				width: gridLayout.longestText * 1.2

				color: Constants.white
				opacity: 0.5
			}
		}
		Item {
			id: placeHolder3

			width: 1
			height: 1
		}
		InformationViewIcon {
			id: iconVersionInformation

			width: sectionPage.informationViewIconWidth

			Layout.minimumHeight: sectionPage.cellHeight
			Layout.alignment: gridLayout.iconAlignment

			activeFocusOnTab: true

			//: LABEL DESKTOP_QML
			text: qsTr("Version information") + SettingsModel.translationTrigger
			source: "qrc:/images/desktop/info_version.svg"
			onClicked: activeSubView = InformationView.SubViews.VersionInformation
		}
		InformationViewIcon {
			id: iconLicense

			width: sectionPage.informationViewIconWidth

			Layout.minimumHeight: sectionPage.cellHeight
			Layout.alignment: gridLayout.iconAlignment

			activeFocusOnTab: true

			//: LABEL DESKTOP_QML
			text: qsTr("Software licenses") + SettingsModel.translationTrigger
			source: "qrc:/images/desktop/info_license.svg"
			onClicked: Qt.openUrlExternally(qsTr("https://www.ausweisapp.bund.de/en/download/windows-and-mac/"))
		}
	}

	Component {
		id: diagnosisView

		DiagnosisView {}
	}

	Component {
		id: versionInformation

		VersionInformation {}
	}

	Component {
		id: logFileView

		LogView {}
	}

	Loader {
		readonly property bool sectionPageTypeMarker: true
		property var titleBarAction: item ? item.titleBarAction : undefined

		visible: item // Otherwise onVisibleChildrenChanged and onVisibleChanged won't be triggered
		anchors.fill: parent

		sourceComponent: switch(sectionPage.activeSubView) {
			case InformationView.SubViews.Diagnosis: return diagnosisView
			case InformationView.SubViews.VersionInformation: return versionInformation
			case InformationView.SubViews.ApplicationLog: return logFileView
		}
	}
}
