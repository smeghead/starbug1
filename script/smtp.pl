#!/usr/bin/perl
use strict;
use warnings;

use Getopt::Std;
use Net::SMTP;
use Encode;
use HTTP::Date qw(time2str);

sub get_options {
    my $o = {};
    getopts('x:f:t:p:h', $o);
    if ($o->{h}) {
        print <<EOF;
        usage: $0 -f <from_address> -t <to_address> -x <smtp_server> -p <smtp_port>
EOF
        die;
    }
    die "ERROR: specify smtp_server."  if (!$o->{x});
    die "ERROR: specify to address."  if (!$o->{t});
    die "ERROR: specify from address."  if (!$o->{f});
    return {
        from => $o->{f},
        to => $o->{t},
        server => $o->{x},
        port => $o->{p} || 25,
    };
}
sub mail_send {
    my ($opts, $subject, $content) = @_;
    my $smtp = new Net::SMTP(
        $opts->{server},
        Port => $opts->{port}
    ) or die "smtp error.";

    use Data::Dumper;
    print Dumper($smtp);

    $smtp->mail($opts->{from});
    $smtp->to($opts->{to});

    $smtp->data();
    $smtp->datasend("Date: " . HTTP::Date::time2str() . "\n");
    $smtp->datasend("To: $opts->{to}\n");
    $smtp->datasend("From: $opts->{from}\n");
    $smtp->datasend("Subject: $subject\n");
    $smtp->datasend("Content-Type: text/plain; charset=iso-2022-jp\n");
    $smtp->datasend("Content-Transfer-Encoding: 7bit\n");
    $smtp->datasend("\n");
    $smtp->datasend($content);
    $smtp->datasend("\n");
    $smtp->dataend();

    $smtp->quit;
}

sub main {
    my $opts = get_options();
    my $url = $opts->{url} || '';
    my $subject = $ENV{'SB_SUBJECT'};
    my $content = $ENV{'SB_CONTENT'};
    Encode::from_to($subject, 'utf8', 'iso-2022-jp');
    Encode::from_to($content, 'utf8', 'iso-2022-jp');
    mail_send($opts,
    $subject,
    $content);
}
main();

