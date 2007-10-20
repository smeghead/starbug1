#!/usr/bin/perl
use strict;
use warnings;

use Getopt::Std;
use Net::SMTP;

sub get_options {
  my $o = {};
  getopts('x:s:f:t:p:u:h', $o);
  if ($o->{h}) {
    print <<EOF;
 usage: $0 -s <subject> -f <from_address> -t <to_address> -x <smtp_server> -p <smtp_port> -u <url>
EOF
    die;
  }
  die "ERROR: specify smtp_server."  if (!$o->{x});
  die "ERROR: specify to address."  if (!$o->{t});
  die "ERROR: specify from address."  if (!$o->{f});
  return {
    subject => $o->{s},
    from => $o->{f},
    to => $o->{t},
    server => $o->{x},
    port => $o->{p} || 25,
    url => $o->{u}
  };
}
sub mail_send {
  my ($opts, $content) = @_;
  my $smtp = new Net::SMTP(
    $opts->{server},
    Port => $opts->{port}
  ) or die "smtp error.";

  use Data::Dumper;
  print Dumper($smtp);

  $smtp->mail($opts->{from});
  $smtp->to($opts->{to});

  my $date = localtime();
  $smtp->data();
  $smtp->datasend("Date: " . $date . "\n");
  $smtp->datasend("To: $opts->{to}\n");
  $smtp->datasend("From: $opts->{from}\n");
  $smtp->datasend("Subject: $opts->{subject}\n");
  $smtp->datasend("\n");
  $smtp->datasend($content);
  $smtp->datasend("\n");
  $smtp->dataend();

  $smtp->quit;
}

sub main {
  my $opts = get_options();
  my $url = $opts->{url} || '';
  my $content = <<EOD;
ticket was upgraded. 
check this url.
 
EOD
  mail_send($opts, $content);
}
main();
