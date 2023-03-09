/**
 * Copyright (c) 2020-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0

ConfirmationPopup {
	id: proxyCredentials

	property QtObject credentials

	//: LABEL DESKTOP Text of the button in the proxy credentials popup.
	okButtonText: qsTr("Sign in")

	//: LABEL DESKTOP Title of the proxy credentials popup.
	title: qsTr("Sign in")
	width: formGrid.width + 2 * Constants.pane_padding

	onClosed: {
		if (!credentials)
			return;
		credentials.confirmInput();
	}
	onConfirmed: {
		if (!credentials)
			return;
		credentials.user = userInput.text;
		credentials.password = passwordInput.text;
	}
	onCredentialsChanged: {
		//: LABEL DESKTOP Text of the proxy credentials popup. An example for %1 is http://proxy.example.com:1337.
		text = qsTr("The proxy %1 requires username and password.").arg(credentials ? credentials.url : "");
		userInput.text = credentials ? credentials.proposedUser : "";
		passwordInput.text = "";
	}

	Grid {
		id: formGrid
		columns: 2
		spacing: Constants.component_spacing
		verticalItemAlignment: Text.AlignVCenter

		GText {
			//: LABEL DESKTOP Accessible name.
			Accessible.name: qsTr("Proxy credential username")
			activeFocusOnTab: true

			//: LABEL DESKTOP Label of the textfield for the username.
			text: qsTr("Username")

			FocusFrame {
			}
		}
		GTextField {
			id: userInput
			textStyle: Style.text.normal
			width: 500 * ApplicationModel.scaleFactor
		}
		GText {
			//: LABEL DESKTOP Accessible name.
			Accessible.name: qsTr("Proxy credential password")
			activeFocusOnTab: true

			//: LABEL DESKTOP Label of the textfield for the password.
			text: qsTr("Password")

			FocusFrame {
			}
		}
		GTextField {
			id: passwordInput
			echoMode: TextInput.Password
			textStyle: Style.text.normal
			width: 500 * ApplicationModel.scaleFactor
		}
	}
}
