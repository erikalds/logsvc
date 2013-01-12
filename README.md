logsvc
======

Log as a service.

The idea behind logsvc is that you have multiple applications which
shares one or several log files.  Each application that needs to log
to, say 'log.txt', uses the logsvc library to "open" that log file.
The logsvc library connects via standard sockets to a log
service/daemon and the log service handles all the file writing and
the multi-process intricacies that arise.  The logsvc API is as simple
as possible.
