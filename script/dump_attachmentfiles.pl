#!/usr/bin/perl
use strict;
use warnings;
use File::Basename;
use DBI;

sub output_dir {"output";}

sub dump_files {
  my $dbname = shift;
  my $dbh = DBI->connect("dbi:SQLite:$dbname") || die "Cannot connect: $DBI::errstr";
  my $res = $dbh->selectall_arrayref(q(SELECT id, filename, content FROM element_file));

  foreach my $row (@$res) {
    my $filename = output_dir . '/' . $row->[0] . '_' . $row->[1];
    print "save to $filename.\n";
    open my $f, '>', $filename;
    print $f $row->[2];
    close $f;
  }
  $dbh->disconnect;
}

my $dirname = dirname($0);
mkdir(output_dir);
foreach my $db (glob("$dirname/../db/*.db")) {
  next if $db =~ m{1.db};
  dump_files($db);
}
