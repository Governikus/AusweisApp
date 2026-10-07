/**
 * Copyright (c) 2026 Governikus Service GmbH, Germany
 */

import QtQuick

import Governikus.ResultView
import Governikus.Type

ManualTest {
	id: root

	name: "InputSuccessView"
	variants: [
		{
			"label": "CAN",
			"props": {
				"passwordType": NumberModel.PasswordType.CAN
			}
		},
		{
			"label": "TRANSPORT_PIN",
			"props": {
				"passwordType": NumberModel.PasswordType.TRANSPORT_PIN
			}
		},
		{
			"label": "PUK",
			"props": {
				"passwordType": NumberModel.PasswordType.PUK
			}
		}
	]

	view: Component {
		InputSuccessView {
			isTransportPin: true
			title: root.name
		}
	}
}
