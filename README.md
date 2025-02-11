# Steam Privilege Escalation
Emulates the behavior of first time installation of a game, running an application as admin (does not bypass UAC)

# Weaponizing
Start an legitimate steam/steamservice process, so when requested, it will appear steam wants admin privileges. Use the returned process handle and suspend it, then hollow or hijack a thread to perform whatever routine you may wish.