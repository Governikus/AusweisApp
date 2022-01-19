/*
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Layouts 1.12

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0


SectionPage {
	id: baseItem

	navigationAction: NavigationAction { state: "cancel"; onClicked: baseItem.clicked() }

	enum Type {
		IsSuccess,
		IsError,
		IsInfo
	}

	property alias header: pane.title
	property alias text: resultText.text
	property alias buttonText: buttonContinue.text
	property alias button: buttonContinue
	property alias contentButton: buttonContent
	property string errorCode
	property alias errorDescription: textErrorDescription.text
	property bool errorDetailsShown: false
	readonly property bool hasErrorDetails: errorCode !== "" || errorDescription !== ""
	property int resultType: ResultView.Type.IsSuccess
	property alias customContent: customContentContainer.data
	property alias hintText: hintItem.text
	property alias hintButtonText: hintItem.buttonText

	signal clicked
	signal hintClicked
	signal contentButtonClicked

	onVisibleChanged: errorDetailsShown = false

	content: Item {
		readonly property real elementHeight: layout.implicitHeight - resultIcon.Layout.maximumHeight + resultIcon.Layout.minimumHeight + 2 * Constants.component_spacing
		readonly property bool elementsFitOnScreen: elementHeight <= baseItem.height

		anchors {
			left: parent.left
			right: parent.right
			top: parent.top
		}
		height: elementsFitOnScreen ? baseItem.height : elementHeight

		ColumnLayout {
			id: layout

			anchors.fill: parent
			anchors.margins: Constants.component_spacing

			spacing: Constants.component_spacing

			StatusIcon {
				id: resultIcon

				implicitWidth : height
				Layout.alignment: Qt.AlignHCenter
				Layout.fillHeight: true
				Layout.minimumHeight: Style.dimens.large_icon_size
				Layout.maximumHeight: Style.dimens.header_icon_size
				Layout.preferredHeight: Style.dimens.header_icon_size

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

				visible: resultText.text !== ""
				Layout.alignment: Qt.AlignCenter
				Layout.fillWidth: true
				Layout.maximumWidth: Style.dimens.max_text_width

				GText {
					id: resultText

					anchors {
						left: parent.left
						right: parent.right
					}
				}

				GButton {
					visible: baseItem.hasErrorDetails

					anchors.horizontalCenter: parent.horizontalCenter

					Accessible.name: qsTr("Show error details")

					buttonColor: Style.color.transparent
					textStyle: Style.text.normal_accent
					text: qsTr("Details") + (baseItem.errorDetailsShown ? "▲" : "▼")
					onClicked: baseItem.errorDetailsShown = !baseItem.errorDetailsShown
				}

				GSeparator {
					visible: baseItem.errorDetailsShown

					anchors {
						left: parent.left
						right: parent.right
					}
				}

				GText {
					visible: baseItem.errorDetailsShown

					anchors {
						left: parent.left
						right: parent.right
					}

					text: qsTr("Error code: %1").arg(baseItem.errorCode)
				}

				GText {
					id: textErrorDescription

					visible: baseItem.errorDetailsShown

					anchors {
						left: parent.left
						right: parent.right
						topMargin: Constants.pane_spacing
					}
				}

				GButton {
					id: buttonContent

					visible: buttonContent.text !== ""
					anchors.horizontalCenter: parent.horizontalCenter

					tintIcon: true

					onClicked: baseItem.contentButtonClicked()
				}
			}

			Item {
				id: customContentContainer

				Layout.fillWidth: true
				Layout.fillHeight: true
				Layout.minimumHeight: childrenRect.height
			}

			Hint {
				id: hintItem

				visible: text !== ""

				Layout.fillWidth: true
				Layout.maximumWidth: Style.dimens.max_text_width
				Layout.alignment: Qt.AlignCenter

				onClicked: baseItem.hintClicked()
			}

			GButton {
				id: buttonContinue

				visible: buttonContinue.text !== ""
				Layout.alignment: Qt.AlignHCenter
				Layout.topMargin: Constants.component_spacing

				//: LABEL ANDROID IOS
				text: qsTr("OK")
				onClicked: baseItem.clicked()
			}
		}
	}
}
