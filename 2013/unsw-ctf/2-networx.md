Networx
-------

Our team managed to solve all the challenges in this category.  For this category, we were given network captures in .pcap format to analyse.

### Help the NSA ###

For this challenge, both @cyphar and @infosec-au sifted through the given `.pcap` using NetworkMiner. There were several documents sent between `Alice` and `woot`.

While most of them were a bunch of documents containing the lyrics for `Rick Astley's Never Gonna Give You Up` (well played, guys), there was one containing a love letter by Alice. It contained a password to a non-existant Netflix account. Key found. Boom!

### The remote ###

A Windows XP SP3 VM was remotely controlled from an Android phone using the [Unified Remote](https://play.google.com/store/apps/details?id=com.Relmtech.Remote) app.

After @auscompgeek had sifted through the `.pcap`, he discovered that the keys the Android app sent were sent as plain text. Going through every key, we discovered the sent message:

```
THE KEY IS WLAN RE<REDACTED>OOL!
```

### Mysterious messages ###

TODO: Something about the decoy Theora stream and using Cain and Abel to extract the Speex stream to an MP3 file.
