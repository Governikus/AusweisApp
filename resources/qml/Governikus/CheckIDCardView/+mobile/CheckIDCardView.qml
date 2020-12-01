/*
 * \copyright Copyright (c) 2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.CheckIDCardModel 1.0

SectionPage {
	id: root

	signal restartCheck()

	//: LABEL ANDROID IOS
	Accessible.name: qsTr("Check device and ID card")
	//: LABEL ANDROID IOS
	Accessible.description: qsTr("This is the device and ID card check of the AusweisApp2.")

	navigationAction: NavigationAction { state: "back"; onClicked: navBar.showMain() }

	//: LABEL ANDROID IOS
	title: qsTr("Check device and ID card")

	function startCheck() {
		firePush(checkIDCardWorkflow)
	}

	onRestartCheck: {
		firePopAll()
		startCheck()
	}

	Component {
		id: checkIDCardResultView

		CheckIDCardResultView {}
	}

	Component {
		id: checkIDCardWorkflow

		CheckIDCardWorkflow {
			onRestartCheck: root.restartCheck()
		}
	}

	ColumnLayout {
		anchors.fill: parent
		anchors.margins: Constants.component_spacing

		spacing: Constants.component_spacing

		TintableIcon {
			Layout.fillHeight: true
			Layout.maximumHeight: Style.dimens.header_icon_size
			Layout.alignment: Qt.AlignHCenter

			source: "qrc:///images/mobile/device.svg"
			tintColor: Style.color.accent
		}

		GPane {
			Layout.fillWidth: true
			Layout.maximumWidth: Style.dimens.max_text_width
			Layout.alignment: Qt.AlignHCenter

			GText {
				width: parent.width

				//: LABEL ANDROID IOS
				text: qsTr("To use the online identification function, your device must meet certain technical requirements. Furthermore, the online identification function must be activated.")
			}

			GText {
				width: parent.width

				//: LABEL ANDROID IOS
				text: qsTr("Check if your smartphone and ID card are ready for use.")
			}
		}

		GSpacer { Layout.fillHeight: true }

		GButton {
			Layout.alignment: Qt.AlignHCenter

			//: LABEL ANDROID IOS
			text: qsTr("Start check")
			icon.source: "qrc:///images/mobile/device.svg"

			onClicked: {
				if (ApplicationModel.nfcAvailable) {
					startCheck()
				} else {
					firePushWithProperties(checkIDCardResultView, { result: CheckIDCardModel.NO_NFC })
				}
			}
		}
	}
}
