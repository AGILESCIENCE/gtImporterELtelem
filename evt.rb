Dir["/AGILE_PROC3/FM3.119_ASDCSTDk/EVT/ag10*.EVT.gz"].sort.each do | file |
	cmd = "./gtImporterELtelem 0 " + file.to_s
	puts cmd
	system cmd
end
