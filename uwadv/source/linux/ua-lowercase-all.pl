#!/usr/bin/perl -w

#
# ua-lowercase-all.pl - converts all files and folders to lowercase
# Copyright (c) 2002 Cuneyt Cuneyitoglu
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
# 02111-1307, USA.
#
#  $Id$
#

use locale;
use File::Basename;

if ($ARGV[0]) {
        $input = $ARGV[0];
        $input =~ s/\/+$//;
        if (-d $input) {
                $base = basename($input);
                if ($base =~ /[A-Z]/) {
                        $dir  = dirname($input);
                        $base = lc($base);
                        $newinput = $dir . "/" . $base;
                        rename($input, $newinput) or die "Couldn't rename $input to $newinput: $!\n";
                        print "$input --> $newinput\n";
                        $input = $newinput;
                }
                gointodir($input);
        } else {
                print "$input does not exist or is not a directory!\n";
        }
} else {
        print "Usage: ua-lowercase-all.pl fullpath-to-dir\n";
}
exit;

sub gointodir {
        my ($dir, @list, $line, $newline);
        $dir = shift;
        opendir(MYDIR, $dir) or die "Can't open $dir : $!\n";
        @list = readdir(MYDIR);
        closedir(MYDIR);
        shift(@list);shift(@list);
        foreach (@list) {
                chomp;
                if (/[A-Z]/) {
                        $line = $dir . "/" . $_;
                        $newline = $dir . "/" . lc($_);
                        rename($line, $newline) or die "Couldn't rename $line to $newline: $!\n";
                        print "$line --> $newline\n";
                        if (-d $newline) {
                                gointodir($newline);
                        }
                } else {
                        $line = $dir . "/" . $_;
                        if (-d $line) {
                                gointodir($line);
                        }
                }
        }
}
