#!/usr/bin/perl -w

use strict;
use IO::Handle;

my $version = "1.0";
my $OS = "linux";
my $gnuplotexe = "gnuplot";
my $inputfile = "spectrum_input.txt";

#Plot command variables
my $numx;
my $numy;

sub usage 
{
  print "RTgnuplotter - Real Time gnuplot'ter v $version\n";
  print "A script to plot data in real time with gnuplot\n";
  print "Author: Shantanu Goel (http://tech.shantnugoel.com/)\n";
  print "\nUsage: $0 <options>\n";
  print <<usage;

  -bands		(Optional) Number of freqency band (default 20)
  -center		(Optional) Center frequency (in Hz) to plot (default 8000 Hz)
  -span			(Optional) Span of the frequencies being plottted (default 16000 Hz)
  -h\\-help\\-v    	(Optional) Print usage and version info

usage
  exit(1);
}

sub main {
  my $arg_ctr = 0;
  my $bands = 100;
  my $center = 8000;
  my $span = 16000;
  my $minDB = -70;
  my $maxDB = 0;

  for($arg_ctr = 0; $arg_ctr <= $#ARGV; $arg_ctr++){

    if(($ARGV[$arg_ctr] eq "-h") or ($ARGV[$arg_ctr] eq "-help") or ($ARGV[$arg_ctr] eq "-v")){
      usage();
    }
    elsif($ARGV[$arg_ctr] eq "-bands"){
      $arg_ctr++;
      $bands = $ARGV[$arg_ctr];
    }
    elsif($ARGV[$arg_ctr] eq "-center"){
      $arg_ctr++;
      $center = $ARGV[$arg_ctr];
    }
    elsif($ARGV[$arg_ctr] eq "-span"){
      $arg_ctr++;
      $span = $ARGV[$arg_ctr];
    }
  }

  my $minFreq = $center - $span/2;
  my $maxFreq = $center + $span/2;

  local *PIPE;
  open PIPE, "|$gnuplotexe" || die "Can't initialize gnuplot number \n";
  PIPE->autoflush;
  print PIPE "set xtics\n";
  print PIPE "set ytics\n";
  print PIPE "set nokey\n";
  print PIPE "set autoscale\n";
  print PIPE "set style data linespoints\n";
  print PIPE "set xlabel \"Frequency (Hz)\"\n";
  print PIPE "set ylabel \"Magnitude (dB)\"\n";
  print PIPE "set xrange [$minFreq:$maxFreq]\n";
  print PIPE "set yrange [$minDB:$maxDB]\n";
  print PIPE "set terminal x11 noraise\n";
  print PIPE "set grid\n";

  STDOUT->autoflush(1);
  STDIN->blocking(0);
  open INPFILE, ">$inputfile";
  INPFILE->autoflush(1);
  my $flag =0;
  my $counter = 0;
  my $firstFreq = -1;
  my $looper = 0;

  $SIG{'INT'} = sub {system("")};

  while(1) 
  {
    $_=<STDIN>;
    if(defined)
    {
      chomp;
      ($numx, $numy) = split(/ /);
      if ($firstFreq eq -1)
      {
	$firstFreq = $numx;
      }
      if (($counter eq 0 and $firstFreq eq $numx))
      {

	close INPFILE;	
	open INPFILE, ">$inputfile";
	INPFILE->autoflush(1);

	print PIPE "set xtics\n";
 	print PIPE "set ytics\n";
	print PIPE "set autoscale\n";
	print PIPE "set grid\n";
	print PIPE "set style data linespoints\n";
	print PIPE "set nokey\n";
	print PIPE "set xrange [$minFreq:$maxFreq]\n";
	print PIPE "set yrange [$minDB:$maxDB]\n";
	print PIPE "set xlabel \"Frequency (Hz)\"\n";
	print PIPE "set ylabel \"Magnitude (dB)\"\n";
	
      	print INPFILE eval($numx)." ".eval($numy)."\n";
      	$counter++;
      }
      if ($counter ne 0){
	print INPFILE eval($numx)." ".eval($numy)."\n";
      	$counter++;
      }
      if ($counter eq $bands)
      {
	flush STDIN;
	$counter = 0;
      }
      if ($flag)
      {
        print PIPE "replot\n";
      }
      else
      {
        print PIPE "plot \"$inputfile\"\n";
        $flag = 1;
      }
    }
  }
  print PIPE "exit;\n";
  close PIPE;
  close INPFILE;
}

&main();
