/*
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Layouts 1.12

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.LogModel 1.0


SectionPage {
	id: baseItem

	navigationAction: NavigationAction { state: "cancel"; onClicked: baseItem.clicked() }

	enum Type {
		IsSuccess,
		IsError,
		IsInfo
	}

	property alias text: resultText.text
	property alias buttonText: buttonLeft.text
	property alias buttonLeft: buttonLeft
	property alias showMailButton: buttonRight.visible
	property int resultType: ResultView.Type.IsSuccess
	property alias customContent: customContentContainer.data
	signal clicked

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

					textStyle: resultType !== ResultView.Type.IsError ? Style.text.normal : Style.text.normal_warning
					wrapMode: Text.WordWrap
				}
			}

			Item {
				id: customContentContainer

				Layout.fillWidth: true
				Layout.fillHeight: true
				Layout.minimumHeight: childrenRect.height
			}

			RowLayout {
				id: buttonRow

				Layout.alignment: Qt.AlignHCenter
				Layout.topMargin: Constants.component_spacing

				spacing: Constants.component_spacing

				GButton {
					id: buttonLeft
					visible: buttonLeft.text !== ""

					//: LABEL ANDROID IOS
					text: qsTr("OK")
					onClicked: baseItem.clicked()
				}

				GButton {
					id: buttonRight
					visible: false

					//: LABEL ANDROID IOS
					text: qsTr("Send log")
					onClicked: LogModel.mailLog()
				}
			}
		}
	}
}
