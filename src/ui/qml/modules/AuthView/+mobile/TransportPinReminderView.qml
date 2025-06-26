/**
 * Copyright (c) 2020-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick

import Governikus.ChangePinView
import Governikus.TitleBar
import Governikus.MultiInfoView

BaseTransportPinReminderView {
	id: root

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Cancel

		onClicked: root.cancel()
	}

	onShowInfoView: pContentType => push(multiInfoView, {
			infoContent: infoData.createObject(root, {
				contentType: pContentType
			})
		})
	onShowTransportPinInfo: push(changeTransportInfoView)

	Component {
		id: infoData

		MultiInfoData {
		}
	}
	Component {
		id: multiInfoView

		MultiInfoView {
			navigationAction: NavigationAction {
				action: NavigationAction.Action.Back

				onClicked: root.pop()
			}
		}
	}
	Component {
		id: changeTransportInfoView

		ChangeTransportPinInfoView {
			title: root.title

			navigationAction: NavigationAction {
				action: NavigationAction.Action.Back

				onClicked: root.pop()
			}

			onContinueClicked: {
				root.pop();
				root.transportPinKnown();
			}
		}
	}
}
