Failure Codes
=============
The AusweisApp2 will send failure codes indicating what went wrong and where it happened as well as
how to solve it.

Helpful tips
------------
The following tips will help you to prevent many of the failures mentioned in the next section.
Also they assist you in receiving further guidance and help.

.. _failure_code_card_position:

Check ID card position
^^^^^^^^^^^^^^^^^^^^^^
Check the position of your ID card on the smartphone or card reader. Especially with smartphones,
the field strength for the power supply of the ID card is not always sufficient. If you place your
smartphone on your ID card, please also ensure that your surface is not electrically conductive,
as this can then disrupt or prevent communication with the ID card. If all of the above does not
work, please see :ref:`failure_code_replace_card_or_card_reader`.

.. _failure_code_contact_support:

Contact support
^^^^^^^^^^^^^^^
If the provided failure code did not help to resolve the issue, please contact the support
(https://www.ausweisapp.bund.de/en/help-and-support), including the error code, situation
description, and logfile, so that they can identify issues in your system configuration or
AusweisApp2. If you are using the AusweisApp2 you will find the logfile in the Help section.

.. _failure_code_inform_service_provider:

Inform service provider
^^^^^^^^^^^^^^^^^^^^^^^
Directly notify the service provider if the failure code contained an incorrect TLS or service
configuration. Usually the the service provider contact information are available on the website on
which you have started the authentication.

.. _failure_code_fix_connections_problems:

Fix connection problems
^^^^^^^^^^^^^^^^^^^^^^^
For any failure code that mentions connection issues in its cause, it is recommended to check your
current connection. Verify an active internet connection, by opening e.g.
https://www.ausweisapp.bund.de in the browser of your choice. This includes checking your firewall
and antivirus configuration as well as your local network hardware. Ultimately the problem may be
with your telecommunications provider, or the service provider. Please refer to the attached
\"Network_Error\" for details. If you are using the AusweisApp2, the diagnosis, which is located in
the help section, may assist you in finding issues.

.. _failure_code_replace_card_or_card_reader:

Replace ID card of card reader
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
It cannot be ruled out that your ID card is defective or, due to necessary updates, initially
requires more power than your current smartphone or card reader can supply. If possible, try other
card readers or smartphones.
If the ID card still does not work you might need to replace it with a new one at your responsible
authority.


.. _failure_codes:

Codes
-----

  - | **User_Cancelled**
    | The user canceled the workflow. In the SDK case, the user can also be a third-party
      application that has disconnected from the SDK.
    | **Possible Solutions:** Complete the workflow without canceling.

  - | **Card_Removed**
    | Possible causes for this failure are:
    |   1 Unstable NFC connection
    |   2 Removal of the ID card
    |   3 Removal of the card reader
    |   4 Cancellation of the remote access
    | **Possible Solutions:**
    |   1 :ref:`failure_code_card_position`
    |   2 The ID card has to be present on the reader during the whole workflow
    |   3 The card reader has to be attached during the whole workflow
    |   4 You must not cancel the remote access during the whole workflow

  - | **Processing_Send_Status_Failed**
    | Occurs if the browser could not be told to wait longer to prevent a timeout.
    | **Possible Solutions:** Change the browser. If the problem persists,
      :ref:`failure_code_contact_support`.

  - | **Parse_TcToken_Invalid_Url**
    | An authentication was started according to TR-03124-1 section 2.2.1.1. However, no valid
      tcTokenURL was transmitted.
    | **Possible Solutions:** :ref:`failure_code_inform_service_provider`.

  - | **Parse_TcToken_Missing_Url**
    | An authentication was started according to TR-03124-1 section 2.2.1.1. However, the query
      "tcTokenURL" is missing.
    | **Possible Solutions:** :ref:`failure_code_inform_service_provider`.

  - | **Get_TcToken_Invalid_Url**
    | An authentication was started according to TR-03124-1 section 2.2.1.1. However, no valid
      tcTokenURL using the https scheme was transmitted.
    | **Possible Solutions:** :ref:`failure_code_inform_service_provider`.

  - | **Get_TcToken_Invalid_Redirect_Url**
    | The tcTokenURL call was answered with a redirect. The URL provided there is invalid or does
      not use the https scheme.
    | **Possible Solutions:** :ref:`failure_code_inform_service_provider`.

  - | **Get_TcToken_Invalid_Certificate_Key_Length**
    | The TLS certificate transmitted by the server when retrieving the tcToken uses an insufficient
      key length.
    | **Possible Solutions:** :ref:`failure_code_inform_service_provider`.

  - | **Get_TcToken_Invalid_Ephemeral_Key_Length**
    | The ephemeral key length generated by the TLS handshake to get the tcToken is insufficient.
    | **Possible Solutions:** :ref:`failure_code_inform_service_provider`.

  - | **Get_TcToken_Invalid_Server_Reply**
    | The server responded to the request for the tcToken neither with content nor with a
      forwarding.
    | **Possible Solutions:** :ref:`failure_code_inform_service_provider`.

  - | **Get_TcToken_Empty_Data**
    | The server responded to the request for the tcToken with empty content.
    | **Possible Solutions:** :ref:`failure_code_inform_service_provider`.

  - | **Get_TcToken_Invalid_Data**
    | The server responded to the request for the tcToken with content that does not comply
      with TR-03124-1 section 2.6.
    | **Possible Solutions:** :ref:`failure_code_inform_service_provider`.

  - | **Get_TcToken_Network_Error**
    | A network error occurred while retrieving the tcToken.
    | **Possible Solutions:** :ref:`failure_code_fix_connections_problems`.

  - | **Certificate_Check_Failed_No_Description**
    | TR-03112-7 section 3.6.4.1 requires a description of the service provider certificate.
      However, this was not transmitted by the service provider in the EAC1InputType.
    | **Possible Solutions:** :ref:`failure_code_inform_service_provider`.

  - | **Certificate_Check_Failed_No_SubjectUrl_In_Description**
    | TR-03124-1 section 2.7.3 requires that the service provider's URL is included in the
      description of the certificate. The URL does not exist.
    | **Possible Solutions:** :ref:`failure_code_inform_service_provider`.

  - | **Certificate_Check_Failed_Hash_Mismatch**
    | TR-03124-1 section 2.7.3 requires that the hash of the certificate description matches that
      stored in the certificate. These don't match.
    | **Possible Solutions:** :ref:`failure_code_inform_service_provider`.

  - | **Certificate_Check_Failed_Same_Origin_Policy_Violation**
    | TR-03124-1 section 2.7.3 requires that the tcTokenUrl has the same origin as the service
      provider's URL from the certificate description. This condition is not met.
    | **Possible Solutions:** :ref:`failure_code_inform_service_provider`.

  - | **Certificate_Check_Failed_Hash_Missing_In_Description**
    | TR-03124-1 Section 2.7.3 requires that the hashes of all TLS certificates used are included in
      the description of the service provider certificate. This condition is not met.
    | **Possible Solutions:** :ref:`failure_code_inform_service_provider`.

  - | **Pre_Verfication_No_Test_Environment**
    | Occurs when the development mode of AusweisApp2 is activated and a genuine ID card is used.
    | **Possible Solutions:** Disable developer mode. The use of genuine ID cards is not permitted with
      activated developer mode, as this is only intended to facilitate the commissioning of services
      with test ID cards.

  - | **Pre_Verfication_Invalid_Certificate_Chain**
    | A certificate chain was sent from the server that is unknown to AusweisApp2.
    | **Possible Solutions:** :ref:`failure_code_inform_service_provider`.

  - | **Pre_Verfication_Invalid_Certificate_Signature**
    | At least one signature in the certificate chain used by the server is incorrect.
    | **Possible Solutions:** :ref:`failure_code_inform_service_provider`.

  - | **Pre_Verfication_Certificate_Expired**
    | The certificate chain used by the server is currently not valid.
    | **Possible Solutions:** Make sure your system time is set correctly. If the problem persists,
      see :ref:`failure_code_inform_service_provider`.

  - | **Extract_Cvcs_From_Eac1_No_Unique_At**
    | The server submitted a certificate chain that contained more than one terminal certificate.
    | **Possible Solutions:** :ref:`failure_code_inform_service_provider`.

  - | **Extract_Cvcs_From_Eac1_No_Unique_Dv**
    | The server transmitted a certificate chain containing more than one DV certificate.
    | **Possible Solutions:** :ref:`failure_code_inform_service_provider`.

  - | **Extract_Cvcs_From_Eac1_At_Missing**
    | The server transmitted a certificate chain that does not contain a terminal certificate.
    | **Possible Solutions:** :ref:`failure_code_inform_service_provider`.

  - | **Extract_Cvcs_From_Eac1_Dv_Missing**
    | The server transmitted a certificate chain that does not contain a DV certificate.
    | **Possible Solutions:** :ref:`failure_code_inform_service_provider`.

  - | **Connect_Card_Connection_Failed**
    | In order to communicate with the ID card, a connection must first be established. This process
      failed.
    | **Possible Solutions:** :ref:`failure_code_card_position`.

  - | **Connect_Card_Eid_Inactive**
    | The PIN of the card is deactivated. The card can currently only be used with the CAN for
      on-site reading.
    | **Possible Solutions:** When your ID card was issued, the online ID card function (the PIN) was not
      activated or you had the function deactivated afterwards. You can have the function activated
      at the citizens' office (Bürgeramt) or activate it with the CAN at
      https://www.pin-ruecksetzbrief-bestellen.de.

  - | **Prepace_Pace_Smart_Eid_Invalidated**
    | The attempt to establish a connection with a PIN to a Smart-eID failed, because all
      PIN-attempts have been used.
    | **Possible Solutions:** The PIN is permanently disabled after 3 failed attempts. Please set up
      your Smart-eID again.

  - | **Establish_Pace_Channel_No_Active_Pin**
    | An authentication was aborted by a card reader in order to replace the five-digit Transport PIN.
    | **Possible Solutions:** The AusweisApp2 automatically leads the user to the PIN change to set
      a six-digit PIN. If this error occurs in a third-party app, you have to start a PIN change on
      your own.

  - | **Establish_Pace_Channel_Basic_Reader_No_Pin**
    | An attempt was made to establish a PACE-channel with a basic reader. However the PIN, CAN, or
      PUK could not be taken over after the user-input.
    | **Possible Solutions:** :ref:`failure_code_contact_support`.

  - | **Establish_Pace_Channel_Puk_Inoperative**
    | An attempt was made to set up a PACE channel with the PUK to unlock the PIN. However, the PUK
      can no longer be used because it has already been used 10 times.
    | **Possible Solutions:** The PIN can be unlocked with the PUK after three incorrect entries.
      However, this is only possible ten times and you have reached that limit. However you can set
      a new PIN at the citizens' office (Bürgeramt) or let it be set with the CAN at
      https://www.pin-ruecksetzbrief-bestellen.de.

  - | **Establish_Pace_Channel_User_Cancelled**
    | The user canceled the workflow on a comfort USB reader or a smartphone as a card reader
      with keyboard mode enabled.
    | **Possible Solutions:** Complete the workflow without canceling.

  - | **Maintain_Card_Connection_Pace_Unrecoverable**
    | An error occurred while setting up the PACE channel that was not due to user error.
    | **Possible Solutions:** The connection to the ID card could not be established with the PIN,
      CAN, or PUK. The entered passwords have no influence on this. Please note
      :ref:`failure_code_card_position`.

  - | **Did_Authenticate_Eac1_Card_Command_Failed**
    | The 4th card command of the terminal authentication according to TR-0110-3 section B.3 failed.
    | **Possible Solutions:** :ref:`failure_code_card_position`.

  - | **Process_Certificates_From_Eac2_Cvc_Chain_Missing**
    | When setting up the PACE channel with PIN or CAN, the ID card communicated which certificate it
      knew. However, the server sent a certificate chain that does not contain this certificate.
    | **Possible Solutions:** :ref:`failure_code_inform_service_provider`.

  - | **Did_Authenticate_Eac2_Invalid_Cvc_Chain**
    | When setting up the PACE channel with PIN or CAN, the ID card communicated which certificate it
      knew. However, the server sent a certificate chain that does not contain this certificate.
    | **Possible Solutions:** :ref:`failure_code_inform_service_provider`.

  - | **Did_Authenticate_Eac2_Card_Command_Failed**
    | A terminal or chip authentication card command according to TR-0110-3 sections B.2 and B.3
      failed.
    | **Possible Solutions:** :ref:`failure_code_card_position`.

  - | **Generic_Send_Receive_Paos_Unhandled**
    | A message was sent by the server in the PAOS communication during authentication, that
      could be completely processed.
    | **Possible Solutions:** :ref:`failure_code_contact_support`.

  - | **Generic_Send_Receive_Network_Error**
    | A network error has occurred in the PAOS communication during authentication.
    | **Possible Solutions:** :ref:`failure_code_fix_connections_problems`.

  - | **Generic_Send_Receive_Ssl_Error**
    | An authentication error occurred in the PAOS communication during the TLS handshake. The TLS
      certificate is incorrect.
    | **Possible Solutions:** :ref:`failure_code_inform_service_provider`.

  - | **Generic_Send_Receive_Server_Error**
    | A server error 5xx occurred in the PAOS communication during authentication.
    | **Possible Solutions:** :ref:`failure_code_inform_service_provider`.

  - | **Generic_Send_Receive_Client_Error**
    | A client error 4xx occurred in the PAOS communication during authentication.
    | **Possible Solutions:** :ref:`failure_code_contact_support`.

  - | **Generic_Send_Receive_Paos_Unknown**
    | An unknown message was sent by the server in the PAOS communication during authentication.
    | **Possible Solutions:** :ref:`failure_code_inform_service_provider`.

  - | **Generic_Send_Receive_Paos_Unexpected**
    | An unexpected message was sent by the server in the PAOS communication during authentication.
    | **Possible Solutions:** :ref:`failure_code_inform_service_provider`.

  - | **Generic_Send_Receive_Invalid_Ephemeral_Key_Length**
    | The symmetric key generated by the TLS handshake for PAOS communication is not long enough.
    | **Possible Solutions:** :ref:`failure_code_inform_service_provider`.

  - | **Generic_Send_Receive_Certificate_Error**
    | The TLS certificate for PAOS communication uses key lengths that are too small or is not
      included in the description of the service provider certificate.
    | **Possible Solutions:** :ref:`failure_code_inform_service_provider`.

  - | **Generic_Send_Receive_Session_Resumption_Failed**
    | Failed to resume TLS session during PAOS communication.
    | **Possible Solutions:** :ref:`failure_code_contact_support`.

  - | **Transmit_Card_Command_Failed**
    | During authentication, card commands transmitted in PAOS communication could not be correctly
      transmitted to the card.
    | **Possible Solutions:** :ref:`failure_code_card_position`.

  - | **Start_Paos_Response_Missing**
    | The message "StartPaosResponse" from the server could not be evaluated because it does not
      exist.
    | **Possible Solutions:** :ref:`failure_code_contact_support`.

  - | **Start_Paos_Response_Error**
    | The "StartPaosResponse" message from the server returned an error. The AusweisApp2 or the ID card
      did not behave as expected by the server.
    | **Possible Solutions:** :ref:`failure_code_contact_support`.

  - | **Check_Refresh_Address_Fatal_Ssl_Error_Before_Reply**
    | An error occurred during the TLS handshake when checking the return address after a successful
      authentication. The TLS certificate is incorrect.
    | **Possible Solutions:** :ref:`failure_code_inform_service_provider`.

  - | **Check_Refresh_Address_Invalid_Ephemeral_Key_Length**
    | The symmetric key generated by the TLS handshake when calling the return address is not long
      enough.
    | **Possible Solutions:** :ref:`failure_code_inform_service_provider`.

  - | **Check_Refresh_Address_Service_Unavailable**
    | The return address cannot be reached.
    | **Possible Solutions:** :ref:`failure_code_inform_service_provider`.

  - | **Check_Refresh_Address_Service_Timeout**
    | The call to the return address did not provide an answer within 30 seconds.
    | **Possible Solutions:** :ref:`failure_code_fix_connections_problems`.

  - | **Check_Refresh_Address_Proxy_Error**
    | A proxy server was configured by the operating system or the settings of AusweisApp2. This
      didn't work for checking the return address.
    | **Possible Solutions:** :ref:`failure_code_fix_connections_problems`.

  - | **Check_Refresh_Address_Fatal_Ssl_Error_After_Reply**
    | When checking the return address after successful authentication, the TLS handshake could not
      be completed successfully.
    | **Possible Solutions:** :ref:`failure_code_fix_connections_problems`.

  - | **Check_Refresh_Address_Unknown_Network_Error**
    | A unknown error occurred when checking the return address after successful authentication.
    | **Possible Solutions:** :ref:`failure_code_fix_connections_problems`.

  - | **Check_Refresh_Address_Invalid_Http_Response**
    | The call to the return address did not result in forwarding.
    | **Possible Solutions:** :ref:`failure_code_inform_service_provider`.

  - | **Check_Refresh_Address_Empty**
    | The call to the return address led to a redirect but no URL was supplied.
    | **Possible Solutions:** :ref:`failure_code_inform_service_provider`.

  - | **Check_Refresh_Address_Invalid_Url**
    | The call to the return address led to a redirect, but no correct URL was supplied.
    | **Possible Solutions:** :ref:`failure_code_inform_service_provider`.

  - | **Check_Refresh_Address_No_Https_Scheme**
    | The call to the return address led to a redirect, but delivered an URL without https scheme.
    | **Possible Solutions:** :ref:`failure_code_inform_service_provider`.

  - | **Check_Refresh_Address_Fetch_Certificate_Error**
    | The server certificate could not be obtained after tracing all redirects.
    | **Possible Solutions:** :ref:`failure_code_fix_connections_problems`.

  - | **Check_Refresh_Address_Unsupported_Certificate**
    | The check of the return address after a successful authentication was interrupted because the
      server uses a TLS certificate with unsupported algorithms or key lengths.
    | **Possible Solutions:** :ref:`failure_code_inform_service_provider`.

  - | **Check_Refresh_Address_Hash_Missing_In_Certificate**
    | The server certificate of the return address is not included in the description of the service
      provider certificate.
    | **Possible Solutions:** :ref:`failure_code_inform_service_provider`.

  - | **Redirect_Browser_Send_Error_Page_Failed**
    | Like Redirect_Browser_Send_Redirect_Failed. However, this only applies to desktop systems, as
      the AusweisApp2 only generates an error page there if no error address is available for
      forwarding by the service provider. On mobile systems, the error details are displayed in the
      AusweisApp2.
    | **Possible Solutions:** If the problem occurs repeatedly and changing the browser does not
      help, please :ref:`failure_code_contact_support`.

  - | **Redirect_Browser_Send_Redirect_Failed**
    | On desktop systems, the web browser waits for a response from AusweisApp2 after starting
      authentication. However, for unknown reasons, the web browser did not wait long enough for the
      response to be sent. On mobile systems it was not possible to open the answer in a web
      browser.
    | **Possible Solutions:** If the problem occurs repeatedly and changing the browser does not
      help, please :ref:`failure_code_contact_support`.

  - | **Generic_Provider_Communication_Network_Error**
    | A network error occurred while communicating with a service provider. This only applies to
      services that are started from AusweisApp2, such as self-authentication.
    | **Possible Solutions:** :ref:`failure_code_fix_connections_problems`.

  - | **Generic_Provider_Communication_Invalid_Ephemeral_Key_Length**
    | When communicating with a service provider, the symmetric key generated by the TLS handshake
      is not long enough. This only applies to services that are started from AusweisApp2, such as
      self-authentication.
    | **Possible Solutions:** :ref:`failure_code_inform_service_provider`.

  - | **Generic_Provider_Communication_Certificate_Error**
    | When communicating with a service provider, the TLS certificate uses key lengths that are
      insufficient. This only applies to services that are started from AusweisApp2, such as
      self-authentication.
    | **Possible Solutions:** :ref:`failure_code_inform_service_provider`.

  - | **Generic_Provider_Communication_Ssl_Error**
    | An error occurred during the TLS handshake when communicating with a service provider. The TLS
      certificate is incorrect. This only applies to services that are started from AusweisApp2,
      such as self-authentication.
    | **Possible Solutions:** :ref:`failure_code_inform_service_provider`.

  - | **Get_SelfAuthData_Invalid_Or_Empty**
    | The authentication for the self-authentication was completed
      successfully, but the server then did not transmit the read data correctly.
    | **Possible Solutions:** :ref:`failure_code_inform_service_provider`.

  - | **Change_Pin_No_SetEidPinCommand_Response**
    | The AusweisApp2 sent a PIN change command to its core, but received an answer for a
      different command.
    | **Possible Solutions:** :ref:`failure_code_contact_support`.

  - | **Change_Pin_Input_Timeout**
    | When changing a PIN, the user took too long to set the new PIN. Timeouts are currently only
      known from card readers with a PIN pad, which also affects smartphones as card readers with
      activated keyboard mode.
    | **Possible Solutions:** Enter the PIN within 60 seconds.

  - | **Change_Pin_User_Cancelled**
    | The user canceled the PIN change after entering the current valid PIN. Can only occur with
      card readers with a PIN pad, which also affects smartphones as card readers with activated
      keyboard mode.
    | **Possible Solutions:** Carry out the PIN change without abortion.

  - | **Change_Pin_New_Pin_Mismatch**
    | When changing a PIN, the user entered an incorrect confirmation of the new PIN. Can only occur
      with USB card readers with a PIN pad. Smartphone as a card reader with activated keyboard mode
      does not allow this behavior.
    | **Possible Solutions:** Confirm the new PIN correctly.

  - | **Change_Pin_New_Pin_Invalid_Length**
    | When changing a PIN, the user entered a new PIN with an incorrect length. Can only occur with
      USB card readers with a PIN pad. However, there is no known device/case that allows this
      possibility. Smartphone as a card reader with activated keyboard mode does not allow this
      behavior.
    | **Possible Solutions:** :ref:`failure_code_contact_support`.

  - | **Change_Pin_Unexpected_Transmit_Status**
    | The command to change the PIN has been transmitted and answered. However, the answer is blank,
      unknown, or unexpected.
    | **Possible Solutions:** :ref:`failure_code_card_position`.

  - | **Change_Pin_Card_New_Pin_Mismatch**
    | Like Change_Pin_New_Pin_Mismatch but at a higher protocol level.
    | **Possible Solutions:** Confirm the new PIN correctly.

  - | **Change_Pin_Card_User_Cancelled**
    | Like Change_Pin_User_Cancelled but at a higher log level.
    | **Possible Solutions:** Carry out the PIN change without abortion.

  - | **Start_Ifd_Service_Failed**
    | The IFD service according to TR-03112-6 appendix "IFD Service" could not be started. Either no
      suitable TLS certificate could be found/generated or the start of the TLS server failed. This
      applies to both remote access and the local service of AusweisApp2 on Android that is used
      through the SDK.
    | **Possible Solutions:** :ref:`failure_code_contact_support`.

  - | **Prepare_Pace_Ifd_Unknown**
    | The establishment of a PACE channel was requested by the client on a smartphone as a card
      reader with activated keyboard mode. However, an unsupported password type was requested (PIN,
      CAN, PUK are supported).
    | **Possible Solutions:** :ref:`failure_code_contact_support`.

  - | **Establish_Pace_Ifd_Unknown**
    | The establishment of a PACE channel was requested by the client on a smartphone as a card
      reader with activated keyboard mode. However, an unsupported password type was requested (PIN,
      CAN, PUK are supported).
    | **Possible Solutions:** :ref:`failure_code_contact_support`.

  - | **Enter_Pace_Password_Ifd_User_Cancelled**
    | Occurs when the user canceled entering the PIN, CAN, or PUK on a smartphone acting as a card
      reader with keyboard mode enabled.
    | **Possible Solutions:** :ref:`failure_code_contact_support`.

  - | **Enter_New_Pace_Pin_Ifd_User_Cancelled**
    | Occurs when the user has canceled entering the new PIN during a PIN change on a smartphone
      acting as a card reader with keyboard mode enabled.
    | **Possible Solutions:** :ref:`failure_code_contact_support`.

  - | **Transmit_Personalization_Size_Mismatch**
    | Is not yet included in the product and will only be relevant with version 2.0.0.

  - | **Start_Paos_Response_Personalization_Empty**
    | Is not yet included in the product and will only be relevant with version 2.0.0.

  - | **Start_Paos_Response_Personalization_Invalid**
    | Is not yet included in the product and will only be relevant with version 2.0.0.

  - | **Prepare_Applet_User_Cancelled**
    | Is not yet included in the product and will only be relevant with version 2.0.0.

  - | **Prepare_Applet_Status_Call_Failed**
    | Is not yet included in the product and will only be relevant with version 2.0.0.

  - | **Prepare_Applet_Installation_Loop**
    | Is not yet included in the product and will only be relevant with version 2.0.0.

  - | **Prepare_Applet_Installation_Failed**
    | Is not yet included in the product and will only be relevant with version 2.0.0.

  - | **Prepare_Applet_Unavailable**
    | Is not yet included in the product and will only be relevant with version 2.0.0.

  - | **Prepare_Applet_Delete_Personalization_Failed**
    | Is not yet included in the product and will only be relevant with version 2.0.0.

  - | **Prepare_Applet_UpdateInfo_Call_Failed**
    | Is not yet included in the product and will only be relevant with version 2.0.0.

  - | **Prepare_Applet_Delete_Smart_Failed**
    | Is not yet included in the product and will only be relevant with version 2.0.0.

  - | **Insert_Card_No_SmartReader**
    | Is not yet included in the product and will only be relevant with version 2.0.0.

  - | **Insert_Card_Multiple_SmartReader**
    | Is not yet included in the product and will only be relevant with version 2.0.0.

  - | **Insert_Card_Unknown_Eid_Type**
    | Is not yet included in the product and will only be relevant with version 2.0.0.

  - | **Insert_Card_HW_Keystore**
    | Is not yet included in the product and will only be relevant with version 2.0.0.

  - | **Insert_Card_Invalid_SmartReader**
    | Is not yet included in the product and will only be relevant with version 2.0.0.

  - | **Insert_Card_Missing_Card**
    | Is not yet included in the product and will only be relevant with version 2.0.0.

  - | **Initialize_Personalization_Failed**
    | Is not yet included in the product and will only be relevant with version 2.0.0.

  - | **Get_Session_Id_Invalid**
    | Is not yet included in the product and will only be relevant with version 2.0.0.

  - | **Get_Challenge_Invalid**
    | Is not yet included in the product and will only be relevant with version 2.0.0.

  - | **Finalize_Personalization_Failed**
    | Is not yet included in the product and will only be relevant with version 2.0.0.

  - | **Change_Smart_Pin_Failed**
    | Is not yet included in the product and will only be relevant with version 2.0.0.

  - | **Check_Status_Unavailable**
    | Is not yet included in the product and will only be relevant with version 2.0.0.

  - | **Check_Applet_Error**
    | Is not yet included in the product and will only be relevant with version 2.0.0.

  - | **Check_Applet_Unavailable**
    | Is not yet included in the product and will only be relevant with version 2.0.0.

.. _failure_code_solutions:
