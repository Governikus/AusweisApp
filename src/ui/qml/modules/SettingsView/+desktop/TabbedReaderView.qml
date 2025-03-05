/**
 * Copyright (c) 2019-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQml.Models

import Governikus.Animations
import Governikus.Global
import Governikus.MultiInfoView
import Governikus.ResultView
import Governikus.Style
import Governikus.TitleBar
import Governikus.Type
import Governikus.View

SectionPage {
	id: root

	property int lastReaderCount: 0

	//: LABEL DESKTOP
	title: qsTr("Card Readers")

	titleBarSettings: TitleBarSettings {
		navigationAction: NavigationAction.Back

		onNavigationActionClicked: root.leaveView()
	}

	Component.onCompleted: lastReaderCount = ApplicationModel.availablePcscReader
	Keys.onPressed: event => {
		tabbedPane.handleKeyPress(event.key);
	}

	Connections {
		function onFireAvailableReaderChanged() {
			if (ApplicationModel.availablePcscReader > root.lastReaderCount) {
				root.push(pcscReaderFound);
			}
			root.lastReaderCount = ApplicationModel.availablePcscReader;
		}

		target: ApplicationModel
	}
	Component {
		id: pcscReaderFound

		ResultView {
			animationSymbol: Symbol.Type.CHECK
			animationType: AnimationLoader.Type.WAIT_FOR_READER
			text: qsTr("Found new USB card reader that is suitable for the ID card. The workflow may now be continued.")
			title: root.title

			titleBarSettings: TitleBarSettings {
				navigationAction: NavigationAction.Back

				onNavigationActionClicked: {
					root.pop();
					root.leaveView();
				}
			}

			onLeaveView: {
				pop();
				root.leaveView();
			}
		}
	}
	TabbedPane {
		id: tabbedPane

		sectionsModel: [qsTr("Smartphone as card reader"), qsTr("USB card reader")]

		contentObjectModel: ObjectModel {
			Component {
				RemoteReaderView {
					height: Math.max(implicitHeight, tabbedPane.availableHeight)
					width: parent.width

					onPairDevice: pDeviceId => {
						if (RemoteServiceModel.rememberServer(pDeviceId)) {
							root.push(connectSacView);
						}
					}
					onShowNoSacFoundInfo: root.push(noSacFoundInfo)

					Component {
						id: noSacFoundInfo

						MultiInfoView {
							progress: root.progress

							infoContent: MultiInfoData {
								contentType: MultiInfoData.NO_SAC_FOUND
							}
							titleBarSettings: TitleBarSettings {
								navigationAction: NavigationAction.Back

								onNavigationActionClicked: root.pop()
							}
						}
					}
					Component {
						id: connectSacView

						ConnectSacView {
							onPairingFailed: pop()
							onPairingSuccessful: {
								pop();
								root.leaveView();
							}
						}
					}
				}
			}
			Component {
				CardReaderView {
					height: Math.max(implicitHeight, tabbedPane.availableHeight)
					width: parent.width
				}
			}
		}

		anchors {
			bottom: parent.bottom
			left: parent.left
			margins: Style.dimens.pane_padding
			right: parent.right
			top: parent.top
		}
	}
}
