#!/usr/bin/perl

use strict;
use warnings;

if (@ARGV != 1){
  print "Incorrect number of arguments\n";
  print_usage();
}

if ($ARGV[0] eq "-h" || $ARGV[0] eq "--help") {
  print_usage();
}

my $file = $ARGV[0];

if (-e $file){
  open(PRT_FILE, $file) or die "Cannot open file $file\n";
}
else {
  die "File $file does not exist\n"
}

$file =~ s/\.prt$/.net/;
open(NET_FILE, ">$file") or die "Cannot create file $file\n";
print NET_FILE  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<application>\n";

$file =~ s/\.net$/.cpt/;
open(CPT_FILE, ">$file") or die "Cannot create file $file\n";
print CPT_FILE  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<computer_list>\n";

$file=~ s/\.cpt$/.prt/;

while (my $line = <PRT_FILE>) {
  chomp($line);
  if ($line =~ m/^\s*begin network/) {
    my %computers;

    print "Parsing Network section\n";
    print NET_FILE "\t<network>\n";

    while (!eof(PRT_FILE) && $line ne "end") {
      $line = <PRT_FILE>;
      chomp $line;

      if ($line =~ m/^\s*(\w+)\s+([\w\.]+)\s+(\d+)/) {
	my ($script_name, $computer_address, $port) = ($1, $2, $3);
	
	print "Found script/computer assignment: script name = $script_name, address = $computer_address, port = $port\n";
	print NET_FILE "\t\t<node>
\t\t\t<name>$script_name</name>
\t\t\t<computer>$computer_address</computer>
\t\t\t<cmd></cmd>
\t\t\t<options></options>
\t\t\t<path>
\t\t\t\t<script>$script_name/$script_name.symb</script>
\t\t\t\t<draw>$script_name/$script_name.draw</draw>
\t\t\t\t<res>$script_name/$script_name.res</res>
\t\t\t\t<config>$script_name/$script_name.config</config>
\t\t\t\t<dev>$script_name/$script_name.dev</dev>
\t\t\t\t<gcd>$script_name/$script_name.gcd</gcd>
\t\t\t\t<prt>$file</prt>
\t\t\t</path>
\t\t</node>\n";

	if (!exists $computers{$computer_address}) {
	  $computers{$computer_address} = 1;
	  print CPT_FILE "\t<computer>
\t\t<name>$computer_address</name>
\t\t<ip>$computer_address</ip>
\t\t<login></login>
\t\t<path></path>
\t\t<cpu></cpu>
\t\t<informations></informations>
\t</computer>\n";
	}
      }
    }

    print "end network \n";
    print NET_FILE "\t</network>\n";
  }

  if ($line =~ m/\s*begin link/) {

    print "Parsing Link section\n";
    print NET_FILE "\t<link_list>\n";

    while (!eof(PRT_FILE) && $line ne "end") {
      $line = <PRT_FILE>;
      chomp $line;

      if ($line =~ m/^\s*(\w+)\s+(\w+)\s+(\w+)/) {
	print "Found link: name = $1, input script = $2, output script = $3\n";
	print NET_FILE "\t\t<link>
\t\t\t<name>$1</name>
\t\t\t<input>$2</input>
\t\t\t<output>$3</output>
\t\t</link>\n";
      }
    }
    print "end link\n";
    print NET_FILE "\t</link_list>\n";
  }

}

print NET_FILE  "</application>\n";
print CPT_FILE  "</computer_list>\n";

close(PRT_FILE);
close(NET_FILE);
close(CPT_FILE);


sub print_usage {
  print <<"VERBATIM";
USAGE:

prt_to_cpt_net.pl application.prt

The script reads the prt input file and creates net and cpt files with the same name, which can be opened in metaleto.
It fills in all the information available in a prt file, the rest will have to be added manually in metaleto: 
  - network computers : both the ip and name of the machine in the cpt file will match the ip/dns name given in the prt file
  - script/computer associations: each script will be added to the application in the net file and linked to its corresponding computer
  - links: the links between the various scripts specified in the prt file will be created and added to the net file
  - promethe files paths: the default paths to the various promethe and leto files (.symb, .res etc.) will be set to [script_name]/[script_name].[extension], the default script file being a .symb

WARNING: all information about ports in the prt file will not be transfered to the metaleto files. Metaleto randomly generates port numbers when creating a prt file and thus does not store any port information.
VERBATIM
exit(0);
}
