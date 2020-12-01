/*
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQml.Models 2.12

import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.NumberModel  1.0
import Governikus.Type.RemoteServiceModel 1.0
import Governikus.View 1.0

SectionPage {
	id: root

	signal closeView()

	property alias rootEnabled: titleBarAction.rootEnabled
	property alias paneAnchors: tabbedPane.anchors
	property int lastReaderCount: 0
	readonly property int currentView: d.view
	readonly property int availableReader: ApplicationModel.availableReader

	enum SubView {
		None,
		ConnectSacView
	}

	QtObject {
		id: d

		property int view
		property int precedingView
	}

	onVisibleChanged: d.view = TabbedReaderView.SubView.None

	onAvailableReaderChanged: {
		if (visible && availableReader > lastReaderCount) {
			root.closeView();
		}
		lastReaderCount = availableReader;
	}

	titleBarAction: TitleBarAction {
		id: titleBarAction

		//: LABEL DESKTOP_QML
		text: qsTr("Card Readers")
		helpTopic: Utils.helpTopicOf(tabbedPane.currentContentItem, "settings")
		rootEnabled: false
		customSubAction: CancelAction {
			onClicked: closeView()
		}

		onClicked: d.view = TabbedReaderView.SubView.None
	}

	Component.onCompleted: lastReaderCount = availableReader

	TabbedPane {
		id: tabbedPane

		visible: d.view === TabbedReaderView.SubView.None

		anchors {
			top: parent.top
			left: parent.left
			right: parent.right
			bottom: parent.bottom
			margins: Constants.pane_padding
		}

		sectionsModel: [
			qsTr("Smartphone as card reader"),
			qsTr("USB card reader")
		]
		contentObjectModel: ObjectModel {
			Component {
				RemoteReaderView {
					width: parent.width
					height: Math.max(implicitHeight, tabbedPane.availableHeight)
					onPairDevice: {
						if (RemoteServiceModel.rememberServer(pDeviceId)) {
							d.view = TabbedReaderView.SubView.ConnectSacView
							appWindow.menuBar.updateActions()
						}
					}
					onUnpairDevice: RemoteServiceModel.forgetDevice(pDeviceId)
					onMoreInformation: {
						d.precedingView = d.view
						d.view = TabbedReaderView.SubView.ConnectSacView
						connectSacView.showPairingInformation();
						appWindow.menuBar.updateActions()
					}
				}
			}

			Component {
				CardReaderView {
					width: parent.width
					height: Math.max(implicitHeight, tabbedPane.availableHeight)
				}
			}
		}
	}

	NavigationButton {
		visible: tabbedPane.visible

		anchors {
			left: parent.left
			bottom: parent.bottom
			margins: Constants.pane_padding
		}

		buttonType: NavigationButton.Type.Back
		onClicked: root.closeView()
	}

	ConnectSacView {
		id: connectSacView

		visible: d.view === TabbedReaderView.SubView.ConnectSacView

		rootEnabled: root.rootEnabled

		onCloseView: {
			d.view = d.precedingView
			appWindow.menuBar.updateActions()
		}
	}
}
