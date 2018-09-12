Dir["/AGILE_PROC3/DATA_ASDCSTDk/LOG/ag-[1-2]*.gz"].sort.each do | file |
	cmd = "./gtImporterELtelem 0 " + file.to_s
	puts cmd
	system cmd
end
