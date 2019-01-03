import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0

SectionPage {
	id: baseItem

	property alias text: resultText.text
	property bool isError: false

	KeyNavigation.tab: button
	Accessible.role: Accessible.Grouping
	Accessible.name: qsTr("Result view") + settingsModel.translationTrigger
	Accessible.description: qsTr("This is the result of an authentication.") + settingsModel.translationTrigger

	StatusIcon {
		height: ApplicationModel.scaleFactor * 400
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.top
		anchors.verticalCenterOffset: baseItem.height / 4
		source: isError ? "qrc:///images/status_error.svg" : "qrc:///images/status_ok.svg"
	}

	Text {
		id: resultText
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.top: parent.verticalCenter
		width: parent.width - (2 * Constants.pane_padding)

		font.pixelSize: Constants.header_font_size
		horizontalAlignment: Text.AlignHCenter
		verticalAlignment: Text.AlignVCenter
		wrapMode: Text.WordWrap
		color: Constants.white
		onLinkActivated: Qt.openUrlExternally(link)
	}

	ContinueButton {
		id: button
		height: ApplicationModel.scaleFactor * 160
		anchors.verticalCenter: parent.bottom
		anchors.verticalCenterOffset: -baseItem.height / 6
		anchors.horizontalCenter: parent.horizontalCenter
		onClicked: baseItem.nextView(SectionPage.Views.Main)

		KeyNavigation.tab: baseItem.navSuccessor
	}
}
