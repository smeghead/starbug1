#!/usr/bin/perl
use strict;
use warnings;

use Net::SMTP;
use Encode;
use HTTP::Date qw(time2str);

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
    my $opts = {
        from => $ENV{'SB_FROM'},
        to => $ENV{'SB_TO'},
        server => $ENV{'SB_SERVER'},
        port => $ENV{'SB_PORT'} || 25,
    };
    my $subject = $ENV{'SB_SUBJECT'};
    my $content = $ENV{'SB_CONTENT'};
    Encode::from_to($subject, 'utf8', 'iso-2022-jp');
    Encode::from_to($content, 'utf8', 'iso-2022-jp');
    mail_send($opts, $subject, $content);
}
main();

