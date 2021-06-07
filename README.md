# scmseed - Dump MiniSEED records from any RecordStream

## Content

- [About](#about)
- [Getting started](#getting-started)
- [Installation](#compiling-and-installation)
- [Issues](#issues)
- [Contributions](#contributions)
- [License](#license)

## About

`scmseed` is a simple utility module for [SeisComP](https://www.seiscomp.de/).
It connects to any
[RecordStream](https://www.seiscomp.de/doc/apps/global_recordstream.html#global-recordstream)
and writes [MiniSEED](http://www.fdsn.org/pdf/SEEDManual_V2.4.pdf) records to
`stdout`.

## Getting started

As `scmseed` is a standard SeisComP extension module a list of available
commandline options can be obtained with

```bash
$ scmseed -h
```

For a general more in-depth introduction on how to use SeisComP modules
including their particular configuration, please refer to the [SeisComP
documentation](https://www.seiscomp.de/doc/index.html).

### Stream configuration

`scmseed` provides the `--list` configuration parameter which allows specifying
streams according to the SeisComP
[scart](https://www.seiscomp.de/doc/apps/scart.html) extension module's *stream
list file*. The format is

```
startTime;endTime;streamID
```

e.g.

```
2019-07-17T02:00:00;2019-07-17T02:10:00;GR.CLL..BHZ
```

Note that comment lines (i.e. with a leading `#` character) are skipped.

### Examples:

- Dump records for all streams from a seedlink server

```
$ scmseed --record-url "slink://rtserve.iris.washington.edu" > ~/tmp/records.mseed
```

- Download data for a stream identified by `CH.GRIMS..HHZ` (including the time
  window as specified) via a [fdsnws dataselect web
  service](https://www.fdsn.org/webservices/fdsnws-dataselect-1.1.pdf) and
  resample the data on-the-fly to 20Hz:

```
$ echo "2020-01-01T00:00:00;2020-01-02T00:00:00;CH.GRIMS..HHZ" | \
  scmseed \
    --record-url "resample://fdsnws?rate=20/eida-federator.ethz.ch/fdsnws/dataselect/1/query" \
    --list - \
    > ~/tmp/records.mseed
```

## Compiling and Installation

Get a copy of
[SeisComP/seiscomp](https://github.com/SeisComP/seiscomp):

```bash
$ git clone https://github.com/SeisComP/seiscomp.git && cd seiscomp/src/extras/
```

Next, clone `scmseed`:

```bash
$ git clone https://github.com/damb/scmseed.git
```

For compiling SeisComP (including `scmseed`), please refer to
https://github.com/SeisComP/seiscomp#build.

## Issues

Please report bugs, issues, feature requests, etc on
[GitHub](https://github.com/damb/scmseed/issues).

## Contributions

Contributions are very welcome. Made with :two_hearts:.

## License

Licensed under the the [AGPLv3](https://www.gnu.org/licenses/agpl-3.0.en.html).
For more information see the
[LICENSE](https://github.com/damb/scmseed/tree/master/LICENSE) file.
