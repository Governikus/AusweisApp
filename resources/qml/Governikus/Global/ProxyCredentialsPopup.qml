/*
 * \copyright Copyright (c) 2020-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0


ConfirmationPopup {
	id: proxyCredentials

	property QtObject credentials

	width: formGrid.width + 2 * Constants.pane_padding

	//: LABEL DESKTOP Title of the proxy credentials popup.
	title: qsTr("Sign in")
	//: LABEL DESKTOP Text of the button in the proxy credentials popup.
	okButtonText: qsTr("Sign in")

	onCredentialsChanged: {
		//: LABEL DESKTOP Text of the proxy credentials popup. An example for %1 is http://proxy.example.com:1337.
		text = qsTr("The proxy %1 requires username and password.").arg(credentials ? credentials.url : "")
		userInput.text = credentials ? credentials.proposedUser : ""
		passwordInput.text = ""
	}

	Grid {
		id: formGrid

		columns: 2
		spacing: Constants.component_spacing
		verticalItemAlignment: Text.AlignVCenter

		GText {
			activeFocusOnTab: true
			//: LABEL DESKTOP Accessible name.
			Accessible.name: qsTr("Proxy credential username")

			//: LABEL DESKTOP Label of the textfield for the username.
			text: qsTr("Username")

			FocusFrame {}
		}

		GTextField {
			id: userInput

			width: 500 * ApplicationModel.scaleFactor

			textStyle: Style.text.normal
		}

		GText {
			activeFocusOnTab: true
			//: LABEL DESKTOP Accessible name.
			Accessible.name: qsTr("Proxy credential password")

			//: LABEL DESKTOP Label of the textfield for the password.
			text: qsTr("Password")

			FocusFrame {}
		}

		GTextField {
			id: passwordInput

			width: 500 * ApplicationModel.scaleFactor

			textStyle: Style.text.normal
			echoMode: TextInput.Password
		}
	}

	onConfirmed: {
		if (!credentials) return;

		credentials.user = userInput.text
		credentials.password = passwordInput.text
	}

	onClosed: {
		if (!credentials) return;

		credentials.confirmInput()
	}
}
