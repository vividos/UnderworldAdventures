AUTOMAKE_OPTIONS = foreign

SUBDIRS = ..

uwadv.cfg: uwadv.cfg.in
	rm -f $@ $@.tmp $@.tmptmp
	sed 's,@UW1PATH\@,@UW1PATH@,g' $< >$@.tmp
	sed 's,@UADATAPATH\@,$(datadir)/games/uwadv/,g' $@.tmp >$@.tmptmp
	rm -f $@.tmp
	mv $@.tmptmp $@

data_DATA = uwadv.cfg
