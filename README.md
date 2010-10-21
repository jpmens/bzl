# Bind Zone List

Retrieve a list of zones configured in a [BIND](http://www.isc.org/software/bind) name server via its _statistics server_.

	acl "trusted" {
		192.168.1.0/24;         
		127.0.0.1;
	};

	statistics-channels { 
		inet *  port 8053 allow { trusted; }; 
	}; 

Running `bzl http://127.0.0.1:8053/` on this machine will print a list of zones with their SOA `serial` number, the `zone name`, the `class` and the `view` in which they're configured (if applicable).

	42401 temp.aa IN internal
	0 0.IN-ADDR.ARPA IN internal
	0 127.IN-ADDR.ARPA IN internal
	0 254.169.IN-ADDR.ARPA IN internal
	0 2.0.192.IN-ADDR.ARPA IN internal
	0 100.51.198.IN-ADDR.ARPA IN internal
	0 113.0.203.IN-ADDR.ARPA IN internal
	0 255.255.255.255.IN-ADDR.ARPA IN internal
	0 0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.IP6.ARPA IN internal
	0 1.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.IP6.ARPA IN internal
	0 8.B.D.0.1.0.0.2.IP6.ARPA IN internal
	0 D.F.IP6.ARPA IN internal
	0 8.E.F.IP6.ARPA IN internal
	0 9.E.F.IP6.ARPA IN internal
	0 A.E.F.IP6.ARPA IN internal
	0 B.E.F.IP6.ARPA IN internal
	- foo.bar IN internal
	1287682762 bzl IN internal
	17 example.net IN external
	2001013101 bind CH extern-chaos
	0 authors.bind CH
	0 hostname.bind CH
	0 version.bind CH
	0 id.server CH

The provided `makezonefile.pl` formats that into an RFC 1035 zone file.
