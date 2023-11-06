Container
=========
This chapter deals with the container specific properties of the |AppName| SDK.
The |AppName| core is reachable over a :ref:`websocket` like the :doc:`desktop`
variant and also enables the :ref:`automatic` mode by default.
Subsequent sections deal with the container itself and explain which steps are
necessary in order to communicate with the |AppName| SDK in the container.


.. note::
  The container SDK does not provide any graphical user interface.



Start
-----
In order to start up the container SDK you can use the usual docker commands.
It is recommended to use ``--rm`` as it not necessary to save the configuration.
If you need to restore the configuration you can add a
`VOLUME <https://docs.docker.com/storage/volumes/>`_ for ``/home/ausweisapp/.config``.


.. code-block:: sh

  docker run --rm -p 127.0.0.1:24727:24727 governikus/ausweisapp




Proxy
^^^^^
If you need a proxy in your network you can pass the necessary environment variables
by ``docker run -e https_proxy=IP:PORT -e http_proxy=IP:PORT``.



Behaviour
---------
The |AppName| uses the :ref:`automatic` mode with the default :doc:`simulator`
if there is no connected :ref:`websocket` client. That mode will be disabled
if a client is connected and enabled again after disconnection. If the client
disconnects during an active workflow the workflow will be canceled.


.. note::
  The usual eID activation via localhost is still possible with both options.
