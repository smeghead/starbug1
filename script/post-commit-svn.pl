#!/usr/bin/perl
use strict;
use warnings;
use utf8;
use LWP::UserAgent;
use HTTP::Request::Common qw(POST);
use Encode;

my $starbug1_info = {
    submit_url => 'http://localhost/starbug1/index.cgi/Sand%20Box/register_submit',
    comment_field_id => 8
};

print "post-commit: start\n";
$ENV{LANG} = 'ja_JP.UTF-8';
my $svnlook = '/usr/bin/svnlook';
my $repos = shift;
my $rev = shift;
my $log = `$svnlook log -r $rev "$repos"`;
my $author = `$svnlook author -r $rev "$repos"`;

chomp($author);
my $commit_info = {
    repos => "$repos",
    rev => $rev,
    log => $log,
    author => $author
};

sub change_status {
    my ($ticket_id, $status, $starbug1_info, $commit_info) = @_;
    my %params = (
        reply => 1,
        ticket_id => $ticket_id,
        field2 => decode('utf-8', $commit_info->{author}),
        field3 => decode('utf-8', $status),
    );
    $params{'field' . $starbug1_info->{comment_field_id}} = decode('utf-8', $commit_info->{log});
    my $header = {'Accept-charset' => "UTF-8"};
    my $request = POST($starbug1_info->{submit_url}, [%params], $header);
    my $ua = LWP::UserAgent->new;
    my $response = $ua->request($request);

    print 'post-commit-svn: ', $response->status_line, "\n";
}

print "post-commit: check log...\n";
if ($commit_info->{log} =~ m{\#(\d+) (\S+)}s) {
    print "post-commit: ticket_id:$1 status:$2\n";
    change_status($1, $2, $starbug1_info, $commit_info);
}
print "post-commit: complete\n";
