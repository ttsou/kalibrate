About Kalibrate
===============

Kalibrate is a GSM signal detector and clock frequency offset calculator.
Clock accuracy and frequency offset is an important aspect of GSM and cellular
networks. Out-of-specification clock accuracy causes a number of network issues
such as inconsistent network detection by the handset, handover failure, and
poor data and voice call performance.

Original version of kalibrate (for use with USRP1):
  * http://ttsou.github.com/kalibrate-uhd/kal-v0.4.1.tar.bz2

Universal Hardware Driver (UHD):
  * http://uhd.ettus.com

OpenBTS Clock Wiki:
  * http://gnuradio.org/redmine/projects/gnuradio/wiki/OpenBTSClocks

Release Notes

The USRP2/N200/N210 is clocked at 100MHz and does not output fractional sample rates.

For USRP1, the original, non-UHD version of kalibrate is recommended.
Download

Build
=====

```
$ ./bootstrap
$ ./configure
$ make
```

Examples
========

USRP2 with internal reference:

```
$ ./kal -f 1941.6e6
linux; GNU C++ version 4.4.4 20100630 (Red Hat 4.4.4-10); Boost_104100; UHD_20101116.195923.c5043c6

Current recv sock buff size: 50000000 bytes

Warning:
    The hardware does not support the requested RX sample rate:
    Target sample rate: 0.270833 MSps
    Actual sample rate: 0.271739 MSps

kal: Calculating clock frequency offset.
Using PCS-1900 channel 569 (1941.6MHz)
average         [min, max]      (range, stddev)
+ 22.221kHz     [21574, 22791]  (1217, 352.328674)
overruns: 0
not found: 0
```

USRP2 with external 10MHz reference - Agilent E4438C (OCXO):

```
$ ./kal -f 1941.6e6 -x
linux; GNU C++ version 4.4.4 20100630 (Red Hat 4.4.4-10); Boost_104100; UHD_20101116.195923.c5043c6

Current recv sock buff size: 50000000 bytes

Warning:
    The hardware does not support the requested RX sample rate:
    Target sample rate: 0.270833 MSps
    Actual sample rate: 0.271739 MSps

kal: Calculating clock frequency offset.
Using PCS-1900 channel 569 (1941.6MHz)
average         [min, max]      (range, stddev)
+  13Hz         [-32, 86]       (118, 34.811478)
overruns: 0
not found: 0
```

Authors
=======

Kalibrate was originally written by Joshua Lackey <jl@thre.at> in 2010.
Subsequent UHD device support and other changes were added by
Tom Tsou <tom@tsou.cc>.
