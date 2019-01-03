import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.LogModel 1.0

SectionPage {
	id: baseItem
	leftTitleBarAction: TitleBarAction { state: "cancel"; onClicked: baseItem.clicked() }

	enum Type {
		IsSuccess,
		IsError,
		IsInfo
	}

	property alias text: resultText.text
	property alias buttonText: buttonLeft.text
	property alias showMailButton: buttonRight.visible
	property int resultType: Type.IsSuccess
	signal clicked

	Rectangle {
		anchors.fill: parent
		color: Constants.background_color
	}

	StatusIcon {
		id: resultIcon
		height: Utils.dp(100)
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.top: parent.top
		anchors.topMargin: Utils.dp(60)
		source: {
			switch (resultType) {
				case ResultView.Type.IsSuccess:
					return "qrc:///images/status_ok.svg"
				case ResultView.Type.IsInfo:
					return "qrc:///images/status_info.svg"
				case ResultView.Type.IsError:
					return "qrc:///images/status_error.svg"
			}
		}
	}

	Text {
		id: resultText
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.top: resultIcon.bottom
		anchors.bottom: buttonRow.top
		width: parent.width - (2 * Constants.pane_padding)

		font.pixelSize: Constants.header_font_size
		horizontalAlignment: Text.AlignHCenter
		verticalAlignment: Text.AlignVCenter
		wrapMode: Text.WordWrap
		color: {
			switch (resultType) {
				case ResultView.Type.IsSuccess:
				case ResultView.Type.IsInfo:
					return Constants.blue
				case ResultView.Type.IsError:
					return Constants.red
			}
		}
		onLinkActivated: Qt.openUrlExternally(link)
	}

	Row {
		id: buttonRow

		anchors.bottom: parent.bottom
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.bottomMargin: Utils.dp(30)
		spacing: Constants.component_spacing

		GButton {
			id: buttonLeft

			text: qsTr("OK") + settingsModel.translationTrigger
			onClicked: baseItem.clicked()
		}

		GButton {
			id: buttonRight
			visible: false

			text: qsTr("Send log file") + settingsModel.translationTrigger
			onClicked: LogModel.mailLog()
		}
	}
}
