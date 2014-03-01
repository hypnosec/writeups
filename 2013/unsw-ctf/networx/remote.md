### The remote ###
> Someone is controlling this computer remotely. Figure out what's going on.

**Tools used**: Wireshark; @auscompgeek's brain; text editor (as a scratchpad)

A Windows XP SP3 VM was remotely controlled from an Android phone using the [Unified Remote](https://play.google.com/store/apps/details?id=com.Relmtech.Remote) app.

After @auscompgeek had sifted through the `.pcap`, he discovered that the keys the Android app sent were sent as plain text. Going through every key, we discovered the sent message:

```
the key is: wlan remotes are sweet!
```
