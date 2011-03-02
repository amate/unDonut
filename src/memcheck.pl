while (<>) {
	if	(m/new[\s]*([a-f\d]+)[\s]*:[\s]*([\d]+)/){
		$dat{$1} = $2;
	} elsif (m/del[\s]*([a-f\d]+)/){
		delete $dat{$1};
	}
}

foreach (keys %dat) {
	print "$_ : $dat{$_}\n";
}