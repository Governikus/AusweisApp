import QtQuick 2.7

import Governikus.Global 1.0
import Governikus.TitleBar 1.0


Item {
	id: baseItem
	Column {
		readonly property int maxWidth: width - 2 * Constants.pane_padding
		width: baseItem.width

		id: root
		spacing: Constants.component_spacing
		padding: Constants.pane_padding

		Item {
			height: childrenRect.height
			width: root.maxWidth
			Image {
				id: useNpa
				anchors.left: parent.left
				width: parent.width * 0.4
				fillMode: Image.PreserveAspectFit
				source: "qrc:///images/siteWithLogo.png"
			}

			Text {
				color: Constants.secondary_text
				font.pixelSize: Constants.normal_font_size
				anchors.verticalCenter: useNpa.verticalCenter
				anchors.leftMargin: Constants.component_spacing
				wrapMode: Text.WordWrap
				anchors.left: useNpa.right
				anchors.right: parent.right
				text: qsTr("You can use your ID card anywhere you see this logo.") + settingsModel.translationTrigger
			}
		}

		Pane {
			anchors.leftMargin: Constants.pane_padding
			anchors.rightMargin: Constants.pane_padding
			Text {
				id: info
				color: Constants.secondary_text
				font.pixelSize: Constants.normal_font_size
				wrapMode: Text.WordWrap
				anchors.left: parent.left
				anchors.right: parent.right
				text: qsTr("Use the button 'See my personal data' to display the data stored on your ID card."
							+ " An Internet connection is required to display the data.")
							+ "<br><br><b>"
							+ qsTr("Your personal data is neither saved nor processed in any way. Please see our %1 for details on how your personal data is processed.")
								.arg('<a href="' + qsTr("https://www.ausweisapp.bund.de/datenschutz/") + '">' + qsTr("data privacy statement") + '</a>')
							+ "</b>"
							+ settingsModel.translationTrigger
				onLinkActivated: Qt.openUrlExternally(link)
			}
		}
	}

	GButton {
		iconSource: "qrc:///images/npa.svg"
		anchors.bottom: baseItem.bottom
		anchors.bottomMargin: Constants.pane_padding
		anchors.horizontalCenter: parent.horizontalCenter
		buttonColor: settingsModel.useSelfauthenticationTestUri ? Constants.red : Constants.blue
		text: qsTr("See my personal data") + settingsModel.translationTrigger

		enabled: applicationModel.currentWorkflow !== "authentication"
		onClicked: {
			selfAuthenticationModel.startWorkflow()
		}
	}
}
