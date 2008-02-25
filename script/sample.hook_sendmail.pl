#!/usr/local/bin/perl
use strict;
use warnings;

use Net::SMTP;
use Encode;
use HTTP::Date qw(time2str);
use JSON::Syck qw();

my $options = {
    #使用するSMTPホスト名
    smtp_host => 'localhost',
    #使用するSMTPポート番号
    smtp_port => 25,
    #送信するメールのFROMを指定してください。
    from => '',
    #メールの送信先を指定してください。通知したいメーリングリストのアドレスなど。
    to => ''
};

sub mail_send {
    my ($subject, $content) = @_;
    my $smtp = new Net::SMTP(
        $options->{smtp_host},
        Port => $options->{smtp_port}
    ) or die "smtp error.";

    use Data::Dumper;
    print Dumper($smtp);

    $smtp->mail($options->{from});
    $smtp->to($options->{to});

    $smtp->data();
    $smtp->datasend("Date: " . HTTP::Date::time2str() . "\n");
    $smtp->datasend("To: $options->{to}\n");
    $smtp->datasend("From: $options->{from}\n");
    $smtp->datasend("Subject: $subject\n");
    $smtp->datasend("Content-Type: text/plain; charset=iso-2022-jp\n");
    $smtp->datasend("Content-Transfer-Encoding: 7bit\n");
    $smtp->datasend("\n");
    $smtp->datasend($content);
    $smtp->datasend("\n");
    $smtp->dataend();

    $smtp->quit;
}

sub create_subject {
    my $info = shift;
    return "$info->{project}->{name} ID: $info->{ticket}->{id}";
}
sub create_content {
    my $info = shift;
    my $id = $info->{ticket}->{id};
    my $url = $info->{ticket}->{url};
    my $fields = $info->{ticket}->{fields};
    my $fields_data;
    foreach my $f (@$fields) {
        my $name = $f->{name};
        my $value = $f->{value};
        $value =~ s/([^\r])\n/$1\r\n/g;
        $fields_data .= " $name: $value\n";
    }
    return <<EOD;
チケットが更新されました。
以下のURLから確認してください。
 $url
----
$fields_data

EOD
}
sub main {
    my $info = JSON::Syck::Load($ENV{'STARBUG1_CONTENT'});
    my $subject = create_subject($info);
    my $content = create_content($info);
    Encode::from_to($subject, 'utf8', 'iso-2022-jp');
    Encode::from_to($content, 'utf8', 'iso-2022-jp');
    mail_send($subject, $content);
}
main();

