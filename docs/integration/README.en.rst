English - Developer Options
===========================

AusweisApp2 features so-called developer options, available for the
supported operating systems of Windows and macOS. They facilitate
the integration of eID services.


Windows & macOS
---------------

Developer options are activated by the version number accessible via
"Help" -> "Version Information" 10 times. This is applicable for both
Windows and macOS. Once activated, the developer options are accessible
via "Settings".


Android & iOS
-------------

The mobile version of AusweisApp2 does not feature the developer mode. Solely
the test mode (Test-PKI) for self-authentication may be activated and
deactivated by clicking the magnifying glass on the start screen 10 times.


Settings
--------

Developer options allow to adjust two different settings:

Test mode for self-authentication (Test-PKI)
''''''''''''''''''''''''''''''''''''''''''''

In general, the self-authentication is a built-in service of AusweisApp2 and
can only be used with genuine ID cards. However, when in test mode, AusweisApp2
uses a test service allowing for self-authentication with a test ID card.

Developer mode
''''''''''''''

When the developer mode is activated, some safety measures during an
authentication process are ignored. Ignoring the safety measures with test
services usually employed in test scenarios, yields a successful authentication.
For example, secure TLS connections (https) as well as insecure connections
without TLS (http) are accepted. Invalid certificates will be ignored. Each
safety breach will be highlighted as an internal notification in AusweisApp2
or the operating system respectively.


**Please note:**
Developer mode can only be used for test services, usage with genuine provider
certificates is not possible.
