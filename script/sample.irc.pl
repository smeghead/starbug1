#!/usr/bin/perl
use strict;
use warnings;
use utf8;

use JSON::Syck qw();

package ExampleBot;
use Encode;
use base 'Bot::BasicBot';
use Data::Dumper;

my $irc_accounts = {
    server1 => {
        nick => 'starbug1',
        username => 'starbug1',
        server => 'SERVERNAME',
        port => 6667,
        password => '',
        channels => ['#dev'],
        ssl => 0,
        charset => 'utf-8',
    },
};

my $tos = {
    'Sand Box' => 'server1',
};

sub new {
    my ($class, $config, $message) = @_;
    print Dumper($config);
    my $self = $class->SUPER::new(%$config);
    $self->{message} = $message;
    return $self;
}
### methods ###
# when connected
sub connected {
    my $self = shift;
    #コネクト時に、通知する
    $self->say( (who => $self->{nick}, channel => $self->{channels}[0], body => $self->{message}) );
}
sub tick {
    my $self = shift;

    #書き込みが完了するのを待つ。
    if ($self->{tick_times}++ > 3) {
        exit;
    }
    return 5;
}

### subs ###
sub which_for_send {
    my $project_name = shift;
    $project_name = decode_utf8($project_name);
    return $tos->{$project_name};
}

sub get_conf {
    my ($info) = @_;
    my $irc_name = which_for_send($info->{project}->{name});
    return unless $irc_name;
    my $conf = $irc_accounts->{$irc_name};
    return $conf;
}

sub create_message {
    my $info = shift;
    my $ticket_subject = '';
    my $ticket_status = '';
    my $fields = $info->{ticket}->{fields};
    foreach my $f (@$fields) {
        my $name = $f->{name};
        $name = decode_utf8($name);
        my $value = $f->{value};
        $value = decode_utf8($value);
        if ($name eq '件名' || $name eq 'Title') {
            $ticket_subject = $value;
        }
        if ($name eq '状態' || $name eq 'Status') {
            $ticket_status = $value;
        }
    }
    my $project_name = decode_utf8($info->{project}->{name});
    return "$project_name ID: $info->{ticket}->{id} $ticket_subject $ticket_status";
}

my $json;
if (@ARGV > -1) {
    open my $f, '<:utf8', $ARGV[0] or die 'failed to open file.' . $@;
    $json = do {local $/; <$f>};
    close $f;
} else {
    $json = $ENV{'STARBUG1_CONTENT'};
}
my $info = JSON::Syck::Load($json);
my $message = create_message($info);
my $config = get_conf($info);

__PACKAGE__->new($config, $message)->run();

