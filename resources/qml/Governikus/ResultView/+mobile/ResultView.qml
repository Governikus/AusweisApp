/*
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
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

	default property alias children: pane.paneData
	property alias header: pane.title
	property alias text: resultText.text
	property alias textFormat: resultText.textFormat
	property alias buttonText: buttonContinue.text
	property alias buttonIcon: buttonContinue.icon.source
	property alias hintText: hintItem.text
	property alias hintButtonText: hintItem.buttonText
	property int resultType: ResultView.Type.IsSuccess
	signal continueClicked()
	signal cancelClicked()
	signal hintClicked()

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Cancel
		onClicked: cancelClicked()
	}
	sectionPageFlickable: contentItem

	GFlickableColumnLayout {
		id: contentItem

		readonly property var minIconHeight: Style.dimens.large_icon_size
		readonly property var maxIconHeight: Style.dimens.header_icon_size

		anchors.fill: parent

		minimumContentHeight: implicitContentHeight - (maxIconHeight - minIconHeight)
		spacing: 0

		StatusIcon {
			id: resultIcon

			implicitWidth : height
			Layout.alignment: Qt.AlignHCenter
			Layout.fillHeight: true
			Layout.minimumHeight: contentItem.minIconHeight
			Layout.maximumHeight: contentItem.maxIconHeight
			Layout.preferredHeight: contentItem.maxIconHeight

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

		GSpacer { Layout.fillHeight: true }

		Hint {
			id: hintItem

			visible: text !== ""

			Layout.fillWidth: true
			Layout.maximumWidth: Style.dimens.max_text_width
			Layout.alignment: Qt.AlignHCenter
			Layout.topMargin: Constants.component_spacing

			onClicked: hintClicked()
		}

		GButton {
			id: buttonContinue

			visible: text !== ""
			Layout.alignment: Qt.AlignHCenter
			Layout.topMargin: Constants.component_spacing

			//: LABEL ANDROID IOS
			text: qsTr("OK")
			tintIcon: true

			onClicked: continueClicked()
		}
	}
}
