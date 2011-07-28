#!/usr/bin/perl
use strict;
use warnings;
use utf8;

use CGI;
my $q = CGI->new;

my $check_subs = {
    # execute cgi
    execute_cgi => sub {
        return '';
    },
    # able to write db.
    able_to_write_db => sub {
        mkdir './db', 755;
        my $check_file = './db/check.db';
        my $res = open my $f, '>>', $check_file;
        unless ($res) {
            return 'database directory is not writable. ./db/';
        }
        close $f;
        unlink $check_file;
        return '';
    },
    # db_initialized
    db_initialized => sub {
        if (!-f './db/1.db') {
            return 'not initialized';
        }
    },
};
my $sub = $check_subs->{$q->param('m')};

if (! $sub) {
    print "Status: 500 invalid request\n";
    print "Content-Type: text/html\n\n";
    print "invalid request";
    die "invalid request\n";
}

my $result = $sub->();
if ($result) {
    print "Status: 500 check error\n";
    print "Content-Type: text/html\n\n";
    print $result;
    die "check error: $result\n";
}

# ok
print "Content-Type: text/html\n\n";
print "ok";
