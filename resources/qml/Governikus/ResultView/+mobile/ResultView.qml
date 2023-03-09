/**
 * Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0

SectionPage {
	id: baseItem
	enum Type {
		IsSuccess,
		IsError,
		IsInfo
	}

	property alias buttonIcon: buttonContinue.icon.source
	property alias buttonText: buttonContinue.text
	default property alias children: pane.paneData
	property alias header: pane.title
	property alias hintButtonText: hintItem.buttonText
	property alias hintText: hintItem.text
	property int resultType: ResultView.Type.IsSuccess
	property alias text: resultText.text
	property alias textFormat: resultText.textFormat

	signal cancelClicked
	signal continueClicked
	signal hintClicked

	sectionPageFlickable: contentItem

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Cancel

		onClicked: cancelClicked()
	}

	GFlickableColumnLayout {
		id: contentItem

		readonly property var maxIconHeight: Style.dimens.header_icon_size
		readonly property var minIconHeight: Style.dimens.large_icon_size

		anchors.fill: parent
		minimumContentHeight: implicitContentHeight - (maxIconHeight - minIconHeight)
		spacing: 0

		StatusIcon {
			id: resultIcon
			Layout.alignment: Qt.AlignHCenter
			Layout.fillHeight: true
			Layout.maximumHeight: contentItem.maxIconHeight
			Layout.minimumHeight: contentItem.minIconHeight
			Layout.preferredHeight: contentItem.maxIconHeight
			implicitWidth: height
			source: {
				switch (resultType) {
				case ResultView.Type.IsSuccess:
					return "qrc:///images/status_ok.svg";
				case ResultView.Type.IsInfo:
					return "qrc:///images/status_info.svg";
				case ResultView.Type.IsError:
					return "qrc:///images/status_error.svg";
				}
			}
		}
		GPane {
			id: pane
			Layout.alignment: Qt.AlignCenter | Qt.AlignTop
			Layout.fillWidth: true
			Layout.maximumWidth: Style.dimens.max_text_width
			Layout.topMargin: Constants.component_spacing

			GText {
				id: resultText
				visible: text !== ""

				anchors {
					left: parent.left
					right: parent.right
				}
			}
		}
		GSpacer {
			Layout.fillHeight: true
		}
		Hint {
			id: hintItem
			Layout.alignment: Qt.AlignHCenter
			Layout.fillWidth: true
			Layout.maximumWidth: Style.dimens.max_text_width
			Layout.topMargin: Constants.component_spacing
			visible: text !== ""

			onClicked: hintClicked()
		}
		GButton {
			id: buttonContinue
			Layout.alignment: Qt.AlignHCenter
			Layout.topMargin: Constants.component_spacing

			//: LABEL ANDROID IOS
			text: qsTr("OK")
			tintIcon: true
			visible: text !== ""

			onClicked: continueClicked()
		}
	}
}
