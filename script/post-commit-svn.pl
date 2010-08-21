#!/usr/bin/perl
use LWP::UserAgent;
use HTTP::Request::Common qw(POST);

my $starbug1_info = {
    submit_url => 'http://localhost/starbug1/index.cgi/Sand%20Box/register_submit',
    comment_field_id => 8
};

my $commit_info = {
    repos => "$1",
    rev => $2,
    log => `svnlook log -r $rev "$repos"`,
    author => `svnlook author -r $rev "$repos"`
};

sub change_status {
    my ($ticket_id, $status, $starbug1_info, $commit_info) = @_;
    my %params = (
        reply => 1,
        ticket_id => $ticket_id,
        field2 => $commit_info->{author},
        field3 => $status,
    );
    $params{'field' . $starbug1_info->{comment_field_id}} = $commit_info->{log};
    my $request = POST($starbug1_info->{submit_url}, [%params]);
    my $ua = LWP::UserAgent->new;
    my $response = $ua->request($request);

    print 'post-commit-svn: ', $response->status_line, "\n";
}

if ($commit_info->{log} =~ m{\#(\d+) (\S+)}m) {
    print "$1 $2\n";
    change_status($1, $2, $starbug1_info, $commit_info);
}
