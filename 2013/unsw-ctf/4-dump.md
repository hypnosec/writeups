Taking a dump
-------------

... is really fun.

Hehehe.  But seriously, taking a dump of memory is highly useful.

We were given a memory dump of an infected Windows XP SP3 VM.

Our team solved all the challenges in this category.  Hooray!

These challenges were solved using a mixture of Volatility and `strings`.

### Tracing ###

Literally just a `strings dump.vmem | grep 'http://'`.  Looking at this `grep`ped output, it becomes obvious that a certain `totallylegit.<redacted>` is littered all over the place.  So we submitted that.  :win:

### Migration ###

There was quite a bit of talk about this.  However, there were two main points that came up:

* `iexplore.exe` spawned a `cmd.exe`, which is odd behaviour for IE.
* `vol malfind` only showed entries for the offending `iexplore.exe` process.

### Kits ###

Dumping the image of the offending `iexplore.exe`, we:

* ran it through `strings`, noticing a mutex, which again is odd behaviour for IE; and
* scanned it using VirusTotal and MSE, searching the signature name.

We eventually figured out that it was Poison Ivy.

### Vectors ###

This took us a while.  Once we got to our last submission, we eventually, we figured out that we weren't supposed to look at the EXE, but rather the HTML page that was loaded.  I deobfuscated the JavaScript by running the script in Node.js until the `eval()`.  We found the JavaScript contained in the HTML matched up to the Aurora exploit.  Yay!
